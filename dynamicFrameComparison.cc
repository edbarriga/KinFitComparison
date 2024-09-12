// #include <ROOT/RDataFrame.hxx>
// #include <ROOT/RVec.hxx>
// #include <TFile.h>
// #include <TTree.h>
// #include <TH2F.h>
// #include <TLorentzVector.h>
// #include <iostream>
// #include <unordered_map>
// #include <vector>
// #include <tuple>
// #include <algorithm>

// // Hash function for std::tuple
// namespace std {
//     template<typename... Ts>
//     struct hash<std::tuple<Ts...>> {
//         size_t operator()(const std::tuple<Ts...>& t) const {
//             return hash_tuple_impl(t, std::index_sequence_for<Ts...>{});
//         }
//     private:
//         template<std::size_t... I>
//         size_t hash_tuple_impl(const std::tuple<Ts...>& t, std::index_sequence<I...>) const {
//             size_t seed = 0;
//             (..., (seed ^= std::hash<Ts>{}(std::get<I>(t)) + 0x9e3779b9 + (seed << 6) + (seed >> 2)));
//             return seed;
//         }
//     };
// }

// // Struct to hold sortable columns with an index
// template<typename T>
// struct SortableColumn {
//     T value;
//     int index;

//     bool operator<(const SortableColumn& other) const {
//         return value < other.value;
//     }
// };

// // Helper function to sort columns and create a tuple dynamically
// template<typename... Ts>
// auto sortSelectedTuple(const Ts&... columns) {
//     std::vector<SortableColumn<std::decay_t<Ts>>> sortableColumns;
//     size_t index = 0;
//     ((sortableColumns.emplace_back(SortableColumn<std::decay_t<Ts>>{columns, index++})), ...);

//     std::sort(sortableColumns.begin(), sortableColumns.end());

//     std::tuple<Ts...> sortedTuple;
//     index = 0;
//     ((std::get<index++>(sortedTuple) = sortableColumns[index - 1].value), ...);

//     return sortedTuple;
// }

// void matchTrees(const std::string& file1, const std::string& tree1,
//                 const std::string& file2, const std::string& tree2,
//                 const std::vector<std::string>& columnNames,
//                 const std::vector<std::string>& testColumnNames) {
//     // Open the ROOT files and access the trees
//     ROOT::RDataFrame df1(tree1, file1);
//     ROOT::RDataFrame df2(tree2, file2);

//     // Maps to store column data
//     std::unordered_map<std::string, std::vector<unsigned long long>> column1Data;
//     std::unordered_map<std::string, std::vector<unsigned long long>> column2Data;

//     std::unordered_map<std::string, std::vector<unsigned int>> gShowColumnData1;
//     std::unordered_map<std::string, std::vector<unsigned int>> column2UintData;

//     // Retrieve data for each column based on its type
//     for (const auto& colName : columnNames) {
//         if (colName == "event") {
//             auto result1 = df1.Take<unsigned long long>(colName);
//             auto result2 = df2.Take<unsigned long long>(colName);
//             column1Data[colName] = *result1;
//             column2Data[colName] = *result2;
//         } else {
//             auto result1 = df1.Take<unsigned int>(colName);
//             auto result2 = df2.Take<unsigned int>(colName);
//             gShowColumnData1[colName] = *result1;
//             column2UintData[colName] = *result2;
//         }
//     }

//     // Convert vec to tuple dynamically
//     std::vector<std::tuple<unsigned long long, unsigned int, unsigned int,
//                      unsigned int, unsigned int, unsigned int, unsigned int, 
//                      unsigned int, unsigned int, unsigned int>> col1Vec;
//     std::vector<std::tuple<unsigned long long, unsigned int, unsigned int, 
//                      unsigned int, unsigned int, unsigned int, unsigned int, 
//                      unsigned int, unsigned int, unsigned int>> col2Vec;

//     // Populate col1Vec
//     for (size_t i = 0; i < column1Data["event"].size(); ++i) {
//         col1Vec.emplace_back(
//             column1Data["event"][i],
//             gShowColumnData1["run"][i],
//             gShowColumnData1["beam_beamid"][i],
//             gShowColumnData1["pip_trkid"][i],
//             gShowColumnData1["pim_trkid"][i],
//             gShowColumnData1["p_trkid"][i],
//             gShowColumnData1["g1_showid"][i],
//             gShowColumnData1["g2_showid"][i],
//             gShowColumnData1["g3_showid"][i],
//             gShowColumnData1["g4_showid"][i]
//         );
//     }

//     // Populate col2Vec
//     for (size_t i = 0; i < column2Data["event"].size(); ++i) {
//         col2Vec.emplace_back(
//             column2Data["event"][i],
//             column2UintData["run"][i],
//             column2UintData["beam_beamid"][i],
//             column2UintData["pip_trkid"][i],
//             column2UintData["pim_trkid"][i],
//             column2UintData["p_trkid"][i],
//             column2UintData["g1_showid"][i],
//             column2UintData["g2_showid"][i],
//             column2UintData["g3_showid"][i],
//             column2UintData["g4_showid"][i]
//         );
//     }

//     // Create a hash map to store sorted tuples and their indices from the first tree
//     std::unordered_map<
//         std::tuple<unsigned long long, unsigned int, unsigned int, unsigned int, unsigned int, 
//                    unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>,
//         std::vector<size_t>
//     > indexMap;

//     // Populate col1Vec with sorted tuples for specific columns
//     for (size_t i = 0; i < column1Data["event"].size(); ++i) {
//         auto key = sortSelectedTuple(
//             gShowColumnData1["g1_showid"][i],
//             gShowColumnData1["g2_showid"][i],
//             gShowColumnData1["g3_showid"][i],
//             gShowColumnData1["g4_showid"][i],
//             column1Data["event"][i],
//             gShowColumnData1["run"][i],
//             gShowColumnData1["beam_beamid"][i],
//             gShowColumnData1["pip_trkid"][i],
//             gShowColumnData1["pim_trkid"][i],
//             gShowColumnData1["p_trkid"][i]
//         );
//         indexMap[key].push_back(i);
//     }

//     // Retrieve TLorentzVector data for each test column
//     std::unordered_map<std::string, std::vector<TLorentzVector>> additionalData1;
//     std::unordered_map<std::string, std::vector<TLorentzVector>> additionalData2;

//     for (const auto& testColumnName : testColumnNames) {
//         additionalData1[testColumnName] = *df1.Take<TLorentzVector>(testColumnName);
//         additionalData2[testColumnName] = *df2.Take<TLorentzVector>(testColumnName);
//     }

//     // Retrieve "chisq/ndf" 
//     auto kinChiSq1 = df1.Take<float>("kin_chisq");
//     auto kinChiSq2 = df2.Take<float>("kin_chisq");
//     auto kinNdf1 = df1.Take<unsigned int>("kin_ndf");
//     auto kinNdf2 = df2.Take<unsigned int>("kin_ndf");
//     std::vector<float> kinChiSqVec1 = *kinChiSq1;
//     std::vector<float> kinChiSqVec2 = *kinChiSq2;
//     std::vector<unsigned int> kinNdfVec1 = *kinNdf1;    
//     std::vector<unsigned int> kinNdfVec2 = *kinNdf2;

//     TFile outFile("output.root", "RECREATE");
//     TString histAxis =  "#chi^{2}/ndf Comparison;"\
//                         "#chi^{2}/ndf " + tree1 +";"\
//                         "#chi^{2}/ndf " + tree2;
//     TH2F hist("h_chisq/ndf", histAxis, 100, 0, 1000, 100, 0, 1000);

//     // Process matches using sorted tuples for specific columns
//     for (size_t i = 0; i < col2Vec.size(); ++i) {
//         auto key = sortSelectedTuple(
//             column2UintData["g1_showid"][i],
//             column2UintData["g2_showid"][i],
//             column2UintData["g3_showid"][i],
//             column2UintData["g4_showid"][i],
//             column2Data["event"][i],
//             column2UintData["run"][i],
//             column2UintData["beam_beamid"][i],
//             column2UintData["pip_trkid"][i],
//             column2UintData["pim_trkid"][i],
//             column2UintData["p_trkid"][i]
//         );       

//         auto it = indexMap.find(key);
//         if (it != indexMap.end()) {
//             std::cout << "Match found!" << std::endl;
//             std::cout << "Indices in first tree: ";
//             for (size_t index : it->second) {
//                 std::cout << index << " ";
//             }
//             std::cout << std::endl;
//             std::cout << "Index in second tree: " << i << std::endl;

//             // Print the matched values
//             std::cout << "Matched values:" << std::endl;
//             std::cout << "In first tree:" << std::endl;
//             for (size_t index : it->second) {
//                 hist.Fill(kinChiSqVec1[index] / kinNdfVec1[index], 
//                           kinChiSqVec2[i] / kinNdfVec2[i]);
//                 std::cout << "Index " << index << ": ";
//                 for (const auto& colName : columnNames) {
//                     if (colName == "event") {
//                         std::cout << column1Data[colName][index] << " ";
//                     } else {
//                         std::cout << gShowColumnData1[colName][index] << " ";
//                     }
//                 }
//                 std::cout << std::endl;
//             }
//             std::cout << "In second tree:" << std::endl;
//             std::cout << "Index " << i << ": ";
//             for (const auto& colName : columnNames) {
//                 if (colName == "event") {
//                     std::cout << column2Data[colName][i] << " ";
//                 } else {
//                     std::cout << column2UintData[colName][i] << " ";
//                 }
//             }
//             std::cout << std::endl;

//             // Print the TLorentzVector values for the matched indices for each test column
//             for (const auto& testColumnName : testColumnNames) {
//                 const auto& vec1 = additionalData2[testColumnName][i];
//                 std::cout << "Value of " << testColumnName << " at index " << i << " in the second tree: "
//                           << "Px: " << vec1.Px() << ", "
//                           << "Py: " << vec1.Py() << ", "
//                           << "Pz: " << vec1.Pz() << ", "
//                           << "E: " << vec1.E() << std::endl;

//                 for (size_t index : it->second) {
//                     const TLorentzVector& vecInFirstTree = additionalData1[testColumnName][index];
//                     std::cout << "Value of " << testColumnName << " at index " << index << " in the first tree: "
//                               << "Px: " << vecInFirstTree.Px() << ", "
//                               << "Py: " << vecInFirstTree.Py() << ", "
//                               << "Pz: " << vecInFirstTree.Pz() << ", "
//                               << "E: " << vecInFirstTree.E() << std::endl;
//                 }
//             }
//         }
//     }

//     // Write the histogram to file and clean up
//     outFile.Write();
//     outFile.Close();
//     std::cout << "end of code" << std::endl;
// }

// int main() {
//     std::string file1 = "lorax/tree_pi0pippimeta__B4_030406_flat.root";
//     std::string tree1 = "pi0pippimeta__B4";
//     std::string file2 = "lorax/tree_pi0pippimeta__B4_M17_030406_flat.root";
//     std::string tree2 = "pi0pippimeta__B4_M17";
//     std::vector<std::string> columnNames = {"event", "run", "beam_beamid", 
//                             "pip_trkid", "pim_trkid", "p_trkid", "g1_showid", 
//                             "g2_showid", "g3_showid", "g4_showid"};
//     std::vector<std::string> testColumnNames = {"pip_p4_meas", "pim_p4_meas",
//                             "beam_p4_meas", "p_p4_meas", "g1_p4_meas", 
//                             "g2_p4_meas", "g3_p4_meas", "g4_p4_meas"};

//     matchTrees(file1, tree1, file2, tree2, columnNames, testColumnNames);

//     return 0;
// }



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


// Helper function to check if a string ends with a given suffix
bool endsWith(const std::string& str, const std::string& suffix) {
    if (suffix.size() > str.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

// Helper function to get the substring before the last underscore
// it helps to group +(p) and -(m) tracks
std::string getPrefixBeforeLastUnderscore(const std::string& str) {
    size_t pos = str.rfind('_');
    if (pos != std::string::npos && pos > 0) {
        return str.substr(0, pos);
    }
    // Print error message if no underscore is found
    std::cerr << "Error: Column name \"" << str << 
                 "\" does not contain an underscore." << std::endl;
    return ""; // Return empty string if no underscore is found
}

// Helper function to check if a string starts with a specific character
bool startsWith(const std::string& str, char prefix) {
    return !str.empty() && str[0] == prefix;
}

// Function to process columns and extract those ending with specific suffixes
void processColumns(ROOT::RDataFrame& df,
                    std::vector<std::string>& trkidCols,
                    std::vector<std::string>& showidCols) {
    // Retrieve column names
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


// Hash function for std::tuple
namespace std {
    template<typename... Ts>
    struct hash<std::tuple<Ts...>> {
        size_t operator()(const std::tuple<Ts...>& t) const {
            return hash_tuple_impl(t, std::index_sequence_for<Ts...>{});
        }
    private:
        template<std::size_t... I>
        size_t hash_tuple_impl(const std::tuple<Ts...>& t, std::index_sequence<I...>) const {
            size_t seed = 0;
            (..., (seed ^= std::hash<Ts>{}(std::get<I>(t)) + 0x9e3779b9 + (seed << 6) + (seed >> 2)));
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

void matchTrees(const std::string& file1, const std::string& tree1,
                const std::string& file2, const std::string& tree2,
                const std::vector<std::string>& testColumnNames) {

    // Open the ROOT files and access the trees
    ROOT::RDataFrame df1(tree1, file1);
    ROOT::RDataFrame df2(tree2, file2);

    //These are the columns that all tree will have
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
            auto result1 = df1.Take<unsigned long long>(colName);
            auto result2 = df2.Take<unsigned long long>(colName);
            eventColumnData1[colName] = *result1;
            eventColumnData2[colName] = *result2;
        } else {
            auto result1 = df1.Take<unsigned int>(colName);
            auto result2 = df2.Take<unsigned int>(colName);
            runBeamcolumnData1[colName] = *result1;
            runBeamcolumnData2[colName] = *result2;
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
    // Note, I think we only have shower from photons
    std::unordered_map<std::string, std::vector<unsigned int>> 
      gShowColumnData1;

    std::unordered_map<std::string, std::vector<unsigned int>> 
      positiveTrkColumnData2;
    std::unordered_map<std::string, std::vector<unsigned int>> 
      negativeTrkColumnData2;
    // Note, I think we only have shower from photons
    std::unordered_map<std::string, std::vector<unsigned int>> 
      gShowColumnData2;

    for (const auto &colName : trkidColumnNamesData1){
        // Read the values from the coloumns
        auto result1 = df1.Take<unsigned int>(colName);
        // Extract the letter before the last underscore
        std::string prefix = getPrefixBeforeLastUnderscore(colName);
        // Group based on the specific prefix
        if (prefix == "p" || prefix == "pip"){
            positiveTrkColumnData1[colName] = *result1;
        }
        else if (prefix == "pim"){
            negativeTrkColumnData1[colName] = *result1;
        }
    }

    for (const auto &colName : trkidColumnNamesData2){
        // Read the values from the coloumns
        auto result2 = df2.Take<unsigned int>(colName);
        // Extract the letter before the last underscore
        std::string prefix = getPrefixBeforeLastUnderscore(colName);
        // Group based on the specific prefix
        if (prefix == "p" || prefix == "pip"){
            positiveTrkColumnData2[colName] = *result2;
        }
        else if (prefix == "pim"){
            negativeTrkColumnData2[colName] = *result2;
        }
    }

    for (const auto& colName : showidColumnNamesData1) {
        // Read the values from the coloumns
        auto result1 = df1.Take<unsigned int>(colName);
        // Check if the prefix starts with 'g'
        if (startsWith(colName, 'g')) {
            gShowColumnData1[colName] = *result1;
        }
    }

    for (const auto& colName : showidColumnNamesData2) {
        // Read the values from the coloumns
        auto result2 = df2.Take<unsigned int>(colName);
        // Check if the prefix starts with 'g'
        if (startsWith(colName, 'g')) {
            gShowColumnData2[colName] = *result2;
        }
    }

    // Convert vec to tuple
    std::vector<std::tuple<unsigned long long, unsigned int, unsigned int,
                     unsigned int, unsigned int, unsigned int, unsigned int, 
                     unsigned int, unsigned int, unsigned int>> col1Vec;
    std::vector<std::tuple<unsigned long long, unsigned int, unsigned int, 
                     unsigned int, unsigned int, unsigned int, unsigned int, 
                     unsigned int, unsigned int, unsigned int>> col2Vec;




    if ( 
    eventColumnData1["event"].size() != runBeamcolumnData1["run"].size() ||
    eventColumnData1["event"].size() != runBeamcolumnData1["beam_beamid"].size() ||
    eventColumnData1["event"].size() != positiveTrkColumnData1["pip_trkid"].size() ||
    eventColumnData1["event"].size() != positiveTrkColumnData1["p_trkid"].size() ||
    eventColumnData1["event"].size() != gShowColumnData1["g1_showid"].size() ||
    eventColumnData1["event"].size() != gShowColumnData1["g2_showid"].size() ||
    eventColumnData1["event"].size() != gShowColumnData1["g3_showid"].size() ||
    eventColumnData1["event"].size() != gShowColumnData1["g4_showid"].size()
    ) {
        throw std::runtime_error("Vector sizes do not match");
    }

        if ( 
    eventColumnData2["event"].size() != runBeamcolumnData2["run"].size() ||
    eventColumnData2["event"].size() != runBeamcolumnData2["beam_beamid"].size() ||
    eventColumnData2["event"].size() != negativeTrkColumnData2["pim_trkid"].size() ||
    eventColumnData2["event"].size() != positiveTrkColumnData2["pip_trkid"].size() ||
    eventColumnData2["event"].size() != positiveTrkColumnData2["p_trkid"].size() ||
    eventColumnData2["event"].size() != gShowColumnData2["g1_showid"].size() ||
    eventColumnData2["event"].size() != gShowColumnData2["g2_showid"].size() ||
    eventColumnData2["event"].size() != gShowColumnData2["g3_showid"].size() ||
    eventColumnData2["event"].size() != gShowColumnData2["g4_showid"].size()
    ) {
        throw std::runtime_error("Vector sizes do not match");
    }



    // std::cout << "Sizes of vectors:" << std::endl;
    // std::cout << "eventColumnData1[event]: " << eventColumnData1["event"].size() << std::endl;
    // std::cout << "runBeamcolumnData1[run]: " << runBeamcolumnData1["run"].size() << std::endl;
    // std::cout << "runBeamcolumnData1[beam_beamid]: " << runBeamcolumnData1["beam_beamid"].size() << std::endl;
    // std::cout << "positiveTrkColumnData1[pip_trkid]: " << positiveTrkColumnData1["pip_trkid"].size() << std::endl;
    // std::cout << "negativeTrkColumnData1[pim_trkid]: " << negativeTrkColumnData1["pim_trkid"].size() << std::endl;
    // std::cout << "positiveTrkColumnData1[p_trkid]: " << positiveTrkColumnData1["p_trkid"].size() << std::endl;
    // std::cout << "gShowColumnData1[g1_showid]: " << gShowColumnData1["g1_showid"].size() << std::endl;
    // std::cout << "gShowColumnData1[g2_showid]: " << gShowColumnData1["g2_showid"].size() << std::endl;
    // std::cout << "gShowColumnData1[g3_showid]: " << gShowColumnData1["g3_showid"].size() << std::endl;
    // std::cout << "gShowColumnData1[g4_showid]: " << gShowColumnData1["g4_showid"].size() << std::endl;


    // Populate col1Vec
    for (size_t i = 0; i < eventColumnData1["event"].size(); ++i) {
        col1Vec.emplace_back(
            eventColumnData1["event"][i],
            runBeamcolumnData1["run"][i],
            runBeamcolumnData1["beam_beamid"][i],
            positiveTrkColumnData1["pip_trkid"][i],
            negativeTrkColumnData1["pim_trkid"][i],
            positiveTrkColumnData1["p_trkid"][i],
            gShowColumnData1["g1_showid"][i],
            gShowColumnData1["g2_showid"][i],
            gShowColumnData1["g3_showid"][i],
            gShowColumnData1["g4_showid"][i]
        );
    }

    // Populate col2Vec
    for (size_t i = 0; i < eventColumnData2["event"].size(); ++i) {
        col2Vec.emplace_back(
            eventColumnData2["event"][i],
            runBeamcolumnData2["run"][i],
            runBeamcolumnData2["beam_beamid"][i],
            positiveTrkColumnData2["pip_trkid"][i],
            negativeTrkColumnData2["pim_trkid"][i],
            positiveTrkColumnData2["p_trkid"][i],
            gShowColumnData2["g1_showid"][i],
            gShowColumnData2["g2_showid"][i],
            gShowColumnData2["g3_showid"][i],
            gShowColumnData2["g4_showid"][i]
        );
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
            runBeamcolumnData2["run"][i],
            runBeamcolumnData2["beam_beamid"][i],
            positiveTrkColumnData2["pip_trkid"][i],
            negativeTrkColumnData2["pim_trkid"][i],
            positiveTrkColumnData2["p_trkid"][i]
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
    for (size_t i = 0; i < col2Vec.size(); ++i) {
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

