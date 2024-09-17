#include <ROOT/RDataFrame.hxx>
#include <ROOT/RVec.hxx>
#include <TFile.h>
#include <TTree.h>
#include <TH2F.h>
#include <TLorentzVector.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <tuple>
#include <sstream>
#include <cstddef>

// Get the substring before the underscore (_). The grouping is for 
// tracks. Positive tracks will have a 'p' and negative tracks a 'm'
std::string getPrefixBeforeLastUnderscore(const std::string& str) {
    size_t pos = str.rfind('_');
    if (pos != std::string::npos && pos > 0) {
        // Looking at only the character before the '_' will avoid the 
        // need to specify every possible particle. At the end, the main
        // reason to group them is to allow for index permutation
        return str.substr(pos-1, 1);
    }
    // Print error message if no underscore is found
    std::cerr << "Error: Column name \"" << str << 
                 "\" does not contain an underscore." << std::endl;
    return ""; // Return empty string if no underscore is found
}

// Check if a string starts with a specific character. This is a safety 
// check to make sure there are only photons (gammas) as showers
bool startsWith(const std::string& str, char prefix) {
    return !str.empty() && str[0] == prefix;
}

// Check if a string ends with a given suffix. Charge particles: 'trkid'
// and photons: 'showid'
bool endsWith(const std::string& str, const std::string& suffix) {
    if (suffix.size() > str.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

// Process columns and extract those ending with specific suffixes.
// This function could be broken to allow the  extraction of other 
// columns like the measured 4-momenta 'p4_meas'
void processColumns(ROOT::RDataFrame& df,
                    std::vector<std::string>& trkidCols,
                    std::vector<std::string>& showidCols) {

    auto columnNames = df.GetColumnNames();
    
    // Filter columns based on suffix
    for (const auto& colName : columnNames) {
        if (endsWith(colName, "trkid")) {
            trkidCols.push_back(colName);
        } else if (endsWith(colName, "showid")) {
            showidCols.push_back(colName);
        }
    }
}

// Hash function to compare the entries. Maybe this is safer if it is 
// written in its own namespace rather than the std. Right now, it works
// and since I don't know much about hash functions, I'll keep it this
// way until I can get more feedback about it
namespace std {
    template<typename... Ts>
    struct hash<std::tuple<Ts...>> {
        size_t operator()(const std::tuple<Ts...>& t) const {
            return hash_tuple_impl(t, std::index_sequence_for<Ts...>{});
        }
    private:
        template<std::size_t Index>
        struct hash_helper {
            static void hash_combine(size_t& seed, const std::tuple<Ts...>& t) {
                std::hash<typename std::tuple_element<Index, std::tuple<Ts...>>::type> hasher;
                size_t h = hasher(std::get<Index>(t));
                seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
        };

        // Base case: empty index sequence
        size_t hash_tuple_impl(const std::tuple<Ts...>& t, std::index_sequence<>) const {
            return 0;
        }

        // Recursive case: process elements and combine hashes
        template<std::size_t I, std::size_t... Is>
        size_t hash_tuple_impl(const std::tuple<Ts...>& t, std::index_sequence<I, Is...>) const {
            size_t seed = hash_tuple_impl(t, std::index_sequence<Is...>{});
            hash_helper<I>::hash_combine(seed, t);
            return seed;
        }
    };
}

// Struct to hold sortable columns with an index
template<typename T>
struct SortableColumn {
    T value;
    int index;

    bool operator<(const SortableColumn& other) const {
        return value < other.value;
    }
};

// Helper function to sort specific columns
template<typename T1, typename T2, typename T3, typename T4, typename... Ts>
std::tuple<T1, T2, T3, T4, Ts...> sortSelectedTuple(
    const T1& col1, const T2& col2, const T3& col3, const T4& col4, const Ts&... others) {
    
    // Create a vector of the sortable columns
    std::vector<SortableColumn<T1>> columns = {{col1, 0}, {col2, 1}, {col3, 2}, {col4, 3}};
    
    // Sort the columns based on the value
    std::sort(columns.begin(), columns.end());
    
    // Extract sorted columns
    T1 sortedCol1 = columns[0].value;
    T1 sortedCol2 = columns[1].value;
    T1 sortedCol3 = columns[2].value;
    T1 sortedCol4 = columns[3].value;

    // Return the sorted tuple along with the other columns
    return std::make_tuple(sortedCol1, sortedCol2, sortedCol3, sortedCol4, others...);
}

// Helper function to test if coloumn vectors are the same size in each tree
void doSizesMatch( const std::string& coloumn1Name, 
                   std::size_t coloumn1Size, 
                   const std::string& coloumn2Name, 
                   std::size_t coloumn2Size){
    if( coloumn1Size != coloumn2Size ) {
        std::stringstream errorMessage;
        errorMessage << "The size of " << coloumn1Name << " (" << coloumn1Size
            << ") does not match the size of " << coloumn2Name 
            << " (" << coloumn2Size << ")";
        throw std::runtime_error(errorMessage.str());
    }
}


void matchTrees(const std::string& file1, const std::string& tree1,
                const std::string& file2, const std::string& tree2,
                const std::vector<std::string>& testColumnNames) {

    // Open the ROOT files and access the trees
    ROOT::RDataFrame df1(tree1, file1);
    ROOT::RDataFrame df2(tree2, file2);

    //These are the columns that all trees will have
    std::vector<std::string> fixedColumnNames = {"event", "run", "beam_beamid"};
    // Maps to store fixed column data
    std::unordered_map<std::string, std::vector<unsigned long long>> 
      eventColumnData1;
    std::unordered_map<std::string, std::vector<unsigned int>> 
      runBeamcolumnData1;

    std::unordered_map<std::string, std::vector<unsigned long long>> 
      eventColumnData2;
    std::unordered_map<std::string, std::vector<unsigned int>> 
      runBeamcolumnData2;

    // Retrieve data for each fixed column based on its type
    for (const auto& colName : fixedColumnNames) {
        if (colName == "event") {
            auto branch1 = df1.Take<unsigned long long>(colName);
            auto branch2 = df2.Take<unsigned long long>(colName);
            eventColumnData1[colName] = *branch1;
            eventColumnData2[colName] = *branch2;
        } else {
            auto branch1 = df1.Take<unsigned int>(colName);
            auto branch2 = df2.Take<unsigned int>(colName);
            runBeamcolumnData1[colName] = *branch1;
            runBeamcolumnData2[colName] = *branch2;
        }
    }

    // Define the vectors to storage the names of the dynamic columns
    std::vector<std::string> trkidColumnNamesData1;
    std::vector<std::string> showidColumnNamesData1;
    // We need a second set of vectors since the number of particles
    // can be different in each tree
    std::vector<std::string> trkidColumnNamesData2;
    std::vector<std::string> showidColumnNamesData2;
    // Now, we will dynamically get the columns for the particles
    processColumns( df1, trkidColumnNamesData1, showidColumnNamesData1);
    processColumns( df1, trkidColumnNamesData2, showidColumnNamesData2);

    // Maps to store dynamically columns data
    std::unordered_map<std::string, std::vector<unsigned int>> 
      positiveTrkColumnData1;
    std::unordered_map<std::string, std::vector<unsigned int>> 
      negativeTrkColumnData1;
    // Note: I think we only have shower from photons
    std::unordered_map<std::string, std::vector<unsigned int>> 
      gShowColumnData1;

    // Repeat for the second data set
    std::unordered_map<std::string, std::vector<unsigned int>> 
      positiveTrkColumnData2;
    std::unordered_map<std::string, std::vector<unsigned int>> 
      negativeTrkColumnData2;
    std::unordered_map<std::string, std::vector<unsigned int>> 
      gShowColumnData2;

    for (const auto &colName : trkidColumnNamesData1){
        // Read the values from the coloumns
        auto branch1 = df1.Take<unsigned int>(colName);
        // Extract the letter before the last underscore
        std::string prefix = getPrefixBeforeLastUnderscore(colName);
        // Group based on the specific prefix
        if (prefix == "p"){
            positiveTrkColumnData1[colName] = *branch1;
        }
        else if (prefix == "m"){
            negativeTrkColumnData1[colName] = *branch1;
        }
    }

    for (const auto &colName : trkidColumnNamesData2){
        // Read the values from the coloumns
        auto branch2 = df2.Take<unsigned int>(colName);
        // Extract the letter before the last underscore
        std::string prefix = getPrefixBeforeLastUnderscore(colName);
        // Group based on the specific prefix
        if (prefix == "p"){
            positiveTrkColumnData2[colName] = *branch2;
        }
        else if (prefix == "m"){
            negativeTrkColumnData2[colName] = *branch2;
        }
    }

    for (const auto& colName : showidColumnNamesData1) {
        // Read the values from the coloumns
        auto branch1 = df1.Take<unsigned int>(colName);
        // A safe check to make sure we have only photons
        if (startsWith(colName, 'g')) {
            gShowColumnData1[colName] = *branch1;
        }
    }

    for (const auto& colName : showidColumnNamesData2) {
        // Read the values from the coloumns
        auto branch2 = df2.Take<unsigned int>(colName);
        if (startsWith(colName, 'g')) {
            gShowColumnData2[colName] = *branch2;
        }
    }


 
    // Safety check to see if the vectors have the same size
    for(const auto& columnNameIt : runBeamcolumnData1){
        const std::string& columnTest = columnNameIt.first;
        doSizesMatch( "event", eventColumnData1["event"].size(),
                 columnTest, runBeamcolumnData1[columnTest].size());
    }
    
    for(const auto& columnNameIt : negativeTrkColumnData1){
        const std::string& columnTest = columnNameIt.first;
        doSizesMatch( "event", eventColumnData1["event"].size(),
                 columnTest, negativeTrkColumnData1[columnTest].size());
    }

    for(const auto& columnNameIt : positiveTrkColumnData1){
        const std::string& columnTest = columnNameIt.first;
        doSizesMatch( "event", eventColumnData1["event"].size(),
                 columnTest, positiveTrkColumnData1[columnTest].size());
    }

    for(const auto& columnNameIt : gShowColumnData2){
        const std::string& columnTest = columnNameIt.first;
        doSizesMatch( "event", eventColumnData2["event"].size(),
                 columnTest, gShowColumnData2[columnTest].size());
    }
    for(const auto& columnNameIt : runBeamcolumnData2){
        const std::string& columnTest = columnNameIt.first;
        doSizesMatch( "event", eventColumnData2["event"].size(),
                 columnTest, runBeamcolumnData2[columnTest].size());
    }
    
    for(const auto& columnNameIt : negativeTrkColumnData2){
        const std::string& columnTest = columnNameIt.first;
        doSizesMatch( "event", eventColumnData2["event"].size(),
                 columnTest, negativeTrkColumnData2[columnTest].size());
    }

    for(const auto& columnNameIt : positiveTrkColumnData2){
        const std::string& columnTest = columnNameIt.first;
        doSizesMatch( "event", eventColumnData2["event"].size(),
                 columnTest, positiveTrkColumnData2[columnTest].size());
    }
    
    for(const auto& columnNameIt : gShowColumnData2){
        const std::string& columnTest = columnNameIt.first;
        doSizesMatch( "event", eventColumnData2["event"].size(),
                 columnTest, gShowColumnData2[columnTest].size());
    }

    // Create a hash map to store sorted tuples and their indices from the first tree
    std::unordered_map<
        std::tuple<unsigned long long, unsigned int, unsigned int, unsigned int, unsigned int, 
                   unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>,
        std::vector<size_t>
    > indexMap;

    // Populate col1Vec with sorted tuples for specific columns
    for (size_t i = 0; i < eventColumnData1["event"].size(); ++i) {
        auto key = sortSelectedTuple(
            gShowColumnData1["g1_showid"][i],
            gShowColumnData1["g2_showid"][i],
            gShowColumnData1["g3_showid"][i],
            gShowColumnData1["g4_showid"][i],
            eventColumnData1["event"][i],
            runBeamcolumnData1["run"][i],
            runBeamcolumnData1["beam_beamid"][i],
            positiveTrkColumnData1["pip_trkid"][i],
            negativeTrkColumnData1["pim_trkid"][i],
            positiveTrkColumnData1["p_trkid"][i]
        );
        indexMap[key].push_back(i);
    }

    // Retrieve TLorentzVector data for each test column
    std::unordered_map<std::string, std::vector<TLorentzVector>> additionalData1;
    std::unordered_map<std::string, std::vector<TLorentzVector>> additionalData2;

    for (const auto& testColumnName : testColumnNames) {
        additionalData1[testColumnName] = *df1.Take<TLorentzVector>(testColumnName);
        additionalData2[testColumnName] = *df2.Take<TLorentzVector>(testColumnName);
    }

    // Retrieve "chisq/ndf" 
    auto kinChiSq1 = df1.Take<float>("kin_chisq");
    auto kinChiSq2 = df2.Take<float>("kin_chisq");
    auto kinNdf1 = df1.Take<unsigned int>("kin_ndf");
    auto kinNdf2 = df2.Take<unsigned int>("kin_ndf");
    std::vector<float> kinChiSqVec1 = *kinChiSq1;
    std::vector<float> kinChiSqVec2 = *kinChiSq2;
    std::vector<unsigned int> kinNdfVec1 = *kinNdf1;    
    std::vector<unsigned int> kinNdfVec2 = *kinNdf2;

    TFile outFile("output.root", "RECREATE");
    TString histAxis =  "#chi^{2}/ndf Comparison;"\
                        "#chi^{2}/ndf " + tree1 +";"\
                        "#chi^{2}/ndf " + tree2;
    TH2F hist("h_chisq/ndf",histAxis, 100, 0, 1000, 100, 0, 1000);

    // Process matches using sorted tuples for specific columns
    for (size_t i = 0; i < eventColumnData2["event"].size(); ++i) {
        auto key = sortSelectedTuple(
            gShowColumnData2["g1_showid"][i],
            gShowColumnData2["g2_showid"][i],
            gShowColumnData2["g3_showid"][i],
            gShowColumnData2["g4_showid"][i],
            eventColumnData2["event"][i],
            runBeamcolumnData2["run"][i],
            runBeamcolumnData2["beam_beamid"][i],
            positiveTrkColumnData2["pip_trkid"][i],
            negativeTrkColumnData2["pim_trkid"][i],
            positiveTrkColumnData2["p_trkid"][i]
        );       
        auto it = indexMap.find(key);
        if (it != indexMap.end()) {
            std::cout << "Match found!" << std::endl;
            std::cout << "Indices in first tree: ";
            for (size_t index : it->second) {
                std::cout << index << " ";
            }
            std::cout << std::endl;
            std::cout << "Index in second tree: " << i << std::endl;

            // Print the matched values
            std::cout << "Matched values:" << std::endl;
            std::cout << "In first tree:" << std::endl;
            for (size_t index : it->second) {
                hist.Fill(kinChiSqVec1[index]/kinNdfVec1[index], 
                          kinChiSqVec2[i]/kinNdfVec2[i]);
                std::cout << "Index " << index << ": ";
                for (const auto& colName : fixedColumnNames) {
                    if (colName == "event") {
                        std::cout << eventColumnData1[colName][index] << " ";
                    } else {
                        std::cout << runBeamcolumnData1[colName][index] << " ";
                    }
                }
                for (const auto& colName : trkidColumnNamesData1) {
                    // Extract the letter before the last underscore
                    std::string prefix = getPrefixBeforeLastUnderscore(colName);
                    // Group based on the specific prefix
                    if (prefix == "p"){
                        std::cout << positiveTrkColumnData1[colName][index]  << " ";
                    }
                    else if (prefix == "m"){
                        std::cout << negativeTrkColumnData1[colName][index]  << " ";
                    }
                }
                for (const auto& colName : showidColumnNamesData1) {
                    std::cout << gShowColumnData1[colName][index]  << " ";
                    
                }
                std::cout << std::endl;
            }
            std::cout << "In second tree:" << std::endl;
            std::cout << "Index " << i << ": ";
            for (const auto& colName : fixedColumnNames) {
                    if (colName == "event") {
                        std::cout << eventColumnData2[colName][i] << " ";
                    } else {
                        std::cout << runBeamcolumnData2[colName][i] << " ";
                    }
                }
                for (const auto& colName : trkidColumnNamesData2) {
                    // Extract the letter before the last underscore
                    std::string prefix = getPrefixBeforeLastUnderscore(colName);
                    // Group based on the specific prefix
                    if (prefix == "p"){
                        std::cout << positiveTrkColumnData2[colName][i]  << " ";
                    }
                    else if (prefix == "m"){
                        std::cout << negativeTrkColumnData2[colName][i]  << " ";
                    }
                }
                for (const auto& colName : showidColumnNamesData2) {
                    std::cout << gShowColumnData2[colName][i]  << " ";
                    
                }
            std::cout << std::endl;

            // Print the TLorentzVector values for the matched indices for each test column
            for (const auto& testColumnName : testColumnNames) {
                const auto& vec1 = additionalData2[testColumnName][i];
                std::cout << "Value of " << testColumnName << " at index " << i << " in the second tree: "
                          << "Px: " << vec1.Px() << ", "
                          << "Py: " << vec1.Py() << ", "
                          << "Pz: " << vec1.Pz() << ", "
                          << "E: " << vec1.E() << std::endl;

                for (size_t index : it->second) {
                    const TLorentzVector& vecInFirstTree = additionalData1[testColumnName][index];
                    std::cout << "Value of " << testColumnName << " at index " << index << " in the first tree: "
                              << "Px: " << vecInFirstTree.Px() << ", "
                              << "Py: " << vecInFirstTree.Py() << ", "
                              << "Pz: " << vecInFirstTree.Pz() << ", "
                              << "E: " << vecInFirstTree.E() << std::endl;
                }
            }
        }
    }

    // Write the histogram to file and clean up
    outFile.Write();
    outFile.Close();
    std::cout << "end of code" << std::endl;
}

int main() {
    std::string file1 = "lorax/tree_pi0pippimeta__B4_030406_flat.root";
    std::string tree1 = "pi0pippimeta__B4";
    std::string file2 = "lorax/tree_pi0pippimeta__B4_M17_030406_flat.root";
    std::string tree2 = "pi0pippimeta__B4_M17";
    std::vector<std::string> dynamicColumnNames = { "pip_trkid", "pim_trkid",
         "p_trkid", "g1_showid", "g2_showid", "g3_showid", "g4_showid"};
    std::vector<std::string> testColumnNames = {"pip_p4_meas", "pim_p4_meas",
                            "beam_p4_meas", "p_p4_meas", "g1_p4_meas", 
                            "g2_p4_meas", "g3_p4_meas", "g4_p4_meas"};

    matchTrees(file1, tree1, file2, tree2, testColumnNames);

    return 0;
}

// This Could help to make the sorting of columns dyanmic
// // Definition of SortableColumn with type support for arbitrary types
// template<typename T>
// struct SortableColumn {
//     T value;
//     int index;

//     bool operator<(const SortableColumn& other) const {
//         return value < other.value;
//     }
// };

// // Helper function to create a vector of SortableColumn objects
// template<typename T>
// std::vector<SortableColumn<T>> createSortableColumns(const T& col, int index) {
//     return { SortableColumn<T>{col, index} };
// }

// // Base case for handling a single column
// template<typename T>
// std::vector<SortableColumn<T>> createSortableColumnsHelper(const T& col, int index) {
//     return createSortableColumns(col, index);
// }

// // Recursive case for handling multiple columns
// template<typename T1, typename T2, typename... Ts>
// std::vector<SortableColumn<typename std::common_type<T1, T2, Ts...>::type>>
// createSortableColumnsHelper(const T1& col1, const T2& col2, const Ts&... cols) {
//     using CommonType = typename std::common_type<T1, T2, Ts...>::type;
//     std::vector<SortableColumn<CommonType>> sortableColumns = {
//         SortableColumn<CommonType>{col1, 0},
//         SortableColumn<CommonType>{col2, 1}
//     };
//     int index = 2;
//     // Recursively add the remaining columns
//     std::vector<SortableColumn<CommonType>> remainingColumns = createSortableColumnsHelper(cols..., index);
//     sortableColumns.insert(sortableColumns.end(), remainingColumns.begin(), remainingColumns.end());
//     return sortableColumns;
// }

// // Forward declarations of the helper functions
// template<std::size_t Index, typename T, typename... Ts>
// typename std::enable_if<Index == sizeof...(Ts), void>::type
// assignValuesToTuple(std::tuple<Ts...>&, const std::vector<SortableColumn<T>>& );

// template<std::size_t Index, typename T, typename... Ts>
// typename std::enable_if<Index < sizeof...(Ts), void>::type
// assignValuesToTuple(std::tuple<Ts...>& t, const std::vector<SortableColumn<T>>& v);

// // Helper function to sort and return the tuple
// template<typename... Ts>
// std::tuple<Ts...> sortSelectedTuple(const Ts&... columns) {
//     static_assert(sizeof...(columns) >= 1, "At least one column is required.");

//     using CommonType = typename std::common_type<Ts...>::type;
//     std::vector<SortableColumn<CommonType>> sortableColumns = createSortableColumnsHelper(columns...);

//     if (sortableColumns.size() > 1) {
//         std::sort(sortableColumns.begin(), sortableColumns.end());
//     }

//     // Prepare to extract sorted columns into a tuple
//     std::tuple<Ts...> sortedTuple;
//     assignValuesToTuple<0, Ts...>(sortedTuple, sortableColumns);

//     return sortedTuple;
// }

// // Helper function to manually assign sorted values to a tuple
// template<std::size_t Index, typename T, typename... Ts>
// typename std::enable_if<Index == sizeof...(Ts), void>::type
// assignValuesToTuple(std::tuple<Ts...>&, const std::vector<SortableColumn<T>>& ) {
//     // Base case: do nothing
// }

// template<std::size_t Index, typename T, typename... Ts>
// typename std::enable_if<Index < sizeof...(Ts), void>::type
// assignValuesToTuple(std::tuple<Ts...>& t, const std::vector<SortableColumn<T>>& v) {
//     std::get<Index>(t) = v[Index].value;
//     assignValuesToTuple<Index + 1, Ts...>(t, v); // Recurse
// }

