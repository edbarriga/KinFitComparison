// #include <ROOT/RDataFrame.hxx>
// #include <ROOT/RVec.hxx>
// #include <TFile.h>
// #include <TTree.h>
// #include <iostream>
// #include <unordered_map>

// // Custom hash function for std::pair
// namespace std {
//     template<typename T1, typename T2>
//     struct hash<std::pair<T1, T2>> {
//         size_t operator()(const std::pair<T1, T2>& p) const {
//             auto h1 = std::hash<T1>{}(p.first);
//             auto h2 = std::hash<T2>{}(p.second);
//             return h1 ^ (h2 << 1); // Combine hash values
//         }
//     };
// }

// void matchTrees(const std::string& file1, const std::string& tree1,
//                 const std::string& file2, const std::string& tree2,
//                 const std::string& columnName) {
//     // Open the ROOT files and access the trees
//     ROOT::RDataFrame df1(tree1, file1);
//     ROOT::RDataFrame df2(tree2, file2);

//     // Read the columns into RVec (ROOT's vector-like container)
//     auto column1 = df1.Take<unsigned long long>(columnName);  // Replace 'unsigned long long' with the appropriate type of your column
//     auto column2 = df2.Take<unsigned long long>(columnName);  // Replace 'unsigned long long' with the appropriate type of your column

//     auto beamID1 = df1.Take<int>('beam_beamid');
//     auto beamID2 = df2.Take<int>('beam_beamid');

//     // // Convert RVec to STL containers
//     // std::vector<unsigned long long> col1Vec;
//     // std::vector<unsigned long long> col2Vec;

//     // // Populate col1Vec
//     // for (const auto& val : column1) {
//     //     col1Vec.push_back(val);
//     // }

//     // // Populate col2Vec
//     // for (const auto& val : column2) {
//     //     col2Vec.push_back(val);
//     // }

//      // Convert RVec to STL containers
//     std::vector<std::pair<unsigned long long, int>> col1Vec;
//     std::vector<std::pair<unsigned long long, int>> col2Vec;

//     // Populate col1Vec
//     for (size_t i = 0; i < column1.size(); ++i) {
//         col1Vec.emplace_back(column1[i], beamID1[i]);
//     }

//     // Populate col2Vec
//     for (size_t i = 0; i < column2.size(); ++i) {
//         col2Vec.emplace_back(column2[i], beamID2[i]);
//     }

//     // // Create a hash map to store values and their indices from the first tree
//     // std::unordered_map<unsigned long long, std::vector<size_t>> indexMap;
//     // for (size_t i = 0; i < col1Vec.size(); ++i) {
//     //     indexMap[col1Vec[i]].push_back(i);
//     // }
//       // Create a hash map to store values and their indices from the first tree
//     std::unordered_map<std::pair<unsigned long long, int>, std::vector<size_t>> indexMap;
//     for (size_t i = 0; i < col1Vec.size(); ++i) {
//         indexMap[col1Vec[i]].push_back(i);
//     }

//     // Match entries from the second tree with those from the first tree
//     for (size_t i = 0; i < col2Vec.size(); ++i) {
//         // auto it = indexMap.find(col2Vec[i]);
//         const auto& key = col2Vec[i];
//         auto it = indexMap.find(key);
//         if (it != indexMap.end()) {
//             //std::cout << "Value " << col2Vec[i] << " found in both trees." << std::endl;
//             std::cout << "Value " << col2Vec[i].first << " and beamID " << col2Vec[i].second << " found in both trees." << std::endl;
//             std::cout << "Indices in first tree: ";
//             for (size_t index : it->second) {
//                 std::cout << index << " ";
//             }
//             std::cout << std::endl;
//             std::cout << "Index in second tree: " << i << std::endl;
//         }
//     }
// }

// int main() {
//   std::string file1 = "lorax/tree_pi0pippimeta__B4_030406_flat.root";
//     std::string tree1 = "pi0pippimeta__B4";
//     std::string file2 = "lorax/tree_pi0pippimeta__B4_M17_030406_flat.root";
//     std::string tree2 = "pi0pippimeta__B4_M17";
//     std::string columnName = "event";

//     matchTrees(file1, tree1, file2, tree2, columnName);

//     return 0;
// }

// #include <ROOT/RDataFrame.hxx>
// #include <ROOT/RVec.hxx>
// #include <TFile.h>
// #include <TTree.h>
// #include <iostream>
// #include <unordered_map>
// #include <vector>
// #include <utility>
// #include <functional>

// // Custom hash function for std::pair
// namespace std {
//     template<typename T1, typename T2>
//     struct hash<std::pair<T1, T2>> {
//         size_t operator()(const std::pair<T1, T2>& p) const {
//             auto h1 = std::hash<T1>{}(p.first);
//             auto h2 = std::hash<T2>{}(p.second);
//             return h1 ^ (h2 << 1); // Combine hash values
//         }
//     };
// }

// void matchTrees(const std::string& file1, const std::string& tree1,
//                 const std::string& file2, const std::string& tree2,
//                 const std::string& columnName) {
//     // Open the ROOT files and access the trees
//     ROOT::RDataFrame df1(tree1, file1);
//     ROOT::RDataFrame df2(tree2, file2);

//     // Read the columns into RVec (ROOT's vector-like container)
//     auto column1 = df1.Take<unsigned long long>(columnName);  // Adjust the type if necessary
//     auto column2 = df2.Take<unsigned long long>(columnName);  // Adjust the type if necessary

//     auto beamID1 = df1.Take<unsigned int>("beam_beamid");
//     auto beamID2 = df2.Take<unsigned int>("beam_beamid");

//     // Convert RVec to STL containers
//     std::vector<std::pair<unsigned long long, unsigned int>> col1Vec;
//     std::vector<std::pair<unsigned long long, unsigned int>> col2Vec;

//     // Access the underlying data
//     auto col1Data = *column1;  // Dereference to get the vector
//     auto col2Data = *column2;  // Dereference to get the vector
//     auto beamID1Data = *beamID1;
//     auto beamID2Data = *beamID2;

//     // Populate col1Vec
//     for (size_t i = 0; i < col1Data.size(); ++i) {
//         col1Vec.emplace_back(col1Data[i], beamID1Data[i]);
//     }

//     // Populate col2Vec
//     for (size_t i = 0; i < col2Data.size(); ++i) {
//         col2Vec.emplace_back(col2Data[i], beamID2Data[i]);
//     }

//     // Create a hash map to store values and their indices from the first tree
//     std::unordered_map<std::pair<unsigned long long, unsigned int>, std::vector<size_t>> indexMap;
//     for (size_t i = 0; i < col1Vec.size(); ++i) {
//         indexMap[col1Vec[i]].push_back(i);
//     }

//     // Match entries from the second tree with those from the first tree
//     for (size_t i = 0; i < col2Vec.size(); ++i) {
//         const auto& key = col2Vec[i];
//         auto it = indexMap.find(key);
//         if (it != indexMap.end()) {
//             std::cout << "Value " << key.first << " and beamID " << key.second << " found in both trees." << std::endl;
//             std::cout << "Indices in first tree: ";
//             for (size_t index : it->second) {
//                 std::cout << index << " ";
//             }
//             std::cout << std::endl;
//             std::cout << "Index in second tree: " << i << std::endl;
//         }
//     }
// }

// int main() {
//     std::string file1 = "lorax/tree_pi0pippimeta__B4_030406_flat.root";
//     std::string tree1 = "pi0pippimeta__B4";
//     std::string file2 = "lorax/tree_pi0pippimeta__B4_M17_030406_flat.root";
//     std::string tree2 = "pi0pippimeta__B4_M17";
//     std::string columnName = "event";

//     matchTrees(file1, tree1, file2, tree2, columnName);

//     return 0;
// }

// #include <ROOT/RDataFrame.hxx>
// #include <ROOT/RVec.hxx>
// #include <TFile.h>
// #include <TTree.h>
// #include <iostream>
// #include <unordered_map>
// #include <vector>
// #include <tuple>
// #include <TLorentzVector.h>


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

// void matchTrees(const std::string& file1, const std::string& tree1,
//                 const std::string& file2, const std::string& tree2,
//                 const std::vector<std::string>& columnNames) {
//     // Open the ROOT files and access the trees
//     ROOT::RDataFrame df1(tree1, file1);
//     ROOT::RDataFrame df2(tree2, file2);

//     // Read the columns into vectors
//     std::vector<std::vector<unsigned long long>> column1Data;
//     std::vector<std::vector<unsigned long long>> column2Data;

//     std::vector<std::vector<unsigned int>> column1UintData;
//     std::vector<std::vector<unsigned int>> column2UintData;

//     // Retrieve data for each column based on its type
//     for (const auto& colName : columnNames) {
//         if (colName == "event") {
//             auto result1 = df1.Take<unsigned long long>(colName);
//             auto result2 = df2.Take<unsigned long long>(colName);
//             column1Data.push_back(*result1);
//             column2Data.push_back(*result2);
//         } else {
//             auto result1 = df1.Take<unsigned int>(colName);
//             auto result2 = df2.Take<unsigned int>(colName);
//             column1UintData.push_back(*result1);
//             column2UintData.push_back(*result2);
//         }
//     }

//     // Convert vec to tuple
//     std::vector<std::tuple<unsigned long long, unsigned int, unsigned int,
//                      unsigned int, unsigned int, unsigned int, unsigned int, 
//                      unsigned int, unsigned int, unsigned int>> col1Vec;
//     std::vector<std::tuple<unsigned long long, unsigned int, unsigned int, 
//                      unsigned int, unsigned int, unsigned int, unsigned int, 
//                      unsigned int, unsigned int, unsigned int>> col2Vec;

//     // Populate col1Vec
//     for (size_t i = 0; i < column1Data[0].size(); ++i) {
//         col1Vec.emplace_back(
//             column1Data[0][i],
//             column1UintData[0][i],
//             column1UintData[1][i],
//             column1UintData[2][i],
//             column1UintData[3][i],
//             column1UintData[4][i],
//             column1UintData[5][i],
//             column1UintData[6][i],
//             column1UintData[7][i],
//             column1UintData[8][i]
//         );
//     }

//     // Populate col2Vec
//     for (size_t i = 0; i < column2Data[0].size(); ++i) {
//         col2Vec.emplace_back(
//             column2Data[0][i],
//             column2UintData[0][i],
//             column2UintData[1][i],
//             column2UintData[2][i],
//             column2UintData[3][i],
//             column2UintData[4][i],
//             column2UintData[5][i],
//             column2UintData[6][i],
//             column2UintData[7][i],
//             column2UintData[8][i]
//         );
//     }

//     // Create a hash map to store values and their indices from the first tree
//     std::unordered_map<
//         std::tuple<unsigned long long, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>,
//         std::vector<size_t>
//     > indexMap;

//     for (size_t i = 0; i < col1Vec.size(); ++i) {
//         indexMap[col1Vec[i]].push_back(i);
//     }

//     std::string testColumnName= "pip_p4_meas";
//     // Retrieve TLorentzVector data
//     auto additionalData1 = df1.Take<TLorentzVector>(testColumnName);
//     auto additionalData2 = df2.Take<TLorentzVector>(testColumnName);

//     for (size_t i = 0; i < col2Vec.size(); ++i) {
//         const auto& key = col2Vec[i];
//         auto it = indexMap.find(key);
//         if (it != indexMap.end()) {
//             std::cout << "Match found!" << std::endl;
//             std::cout << "Indices in first tree: ";
//             for (size_t index : it->second) {
//                 std::cout << index << " ";
//             }
//             std::cout << std::endl;
//             std::cout << "Index in second tree: " << i << std::endl;

//             // Print the TLorentzVector values for the matched indices
//             for (size_t index : it->second) {
//                const TLorentzVector& vec1 = (*additionalData1)[index];
//                 std::cout << "Value of " << testColumnName << " at index " << index << " in the first tree: "
//                           << "Px: " << vec1.Px() << ", "
//                           << "Py: " << vec1.Py() << ", "
//                           << "Pz: " << vec1.Pz() << ", "
//                           << "E: " << vec1.E() << std::endl;
//             }
//             const TLorentzVector& vec2 = (*additionalData2)[i];
//             std::cout << "Value of " << testColumnName << " at index " << i << " in the second tree: "
//                       << "Px: " << vec2.Px() << ", "
//                       << "Py: " << vec2.Py() << ", "
//                       << "Pz: " << vec2.Pz() << ", "
//                       << "E: " << vec2.E() << std::endl;
//         }
//     }
// }

// int main() {
//     std::string file1 = "lorax/tree_pi0pippimeta__B4_030406_flat.root";
//     std::string tree1 = "pi0pippimeta__B4";
//     std::string file2 = "lorax/tree_pi0pippimeta__B4_M17_030406_flat.root";
//     std::string tree2 = "pi0pippimeta__B4_M17";
//     std::vector<std::string> columnNames = {"event", "run", "beam_beamid", 
//                             "pip_trkid", "pim_trkid", "p_trkid", "g1_showid", 
//                             "g2_showid", "g3_showid", "g4_showid"};

//     matchTrees(file1, tree1, file2, tree2, columnNames);

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
#include <tuple>


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

void matchTrees(const std::string& file1, const std::string& tree1,
                const std::string& file2, const std::string& tree2,
                const std::vector<std::string>& columnNames,
                const std::vector<std::string>& testColumnNames) {
    // Open the ROOT files and access the trees
    ROOT::RDataFrame df1(tree1, file1);
    ROOT::RDataFrame df2(tree2, file2);

    // Maps to store column data
    std::unordered_map<std::string, std::vector<unsigned long long>> column1Data;
    std::unordered_map<std::string, std::vector<unsigned long long>> column2Data;

    std::unordered_map<std::string, std::vector<unsigned int>> column1UintData;
    std::unordered_map<std::string, std::vector<unsigned int>> column2UintData;

    // Retrieve data for each column based on its type
    for (const auto& colName : columnNames) {
        if (colName == "event") {
            auto result1 = df1.Take<unsigned long long>(colName);
            auto result2 = df2.Take<unsigned long long>(colName);
            column1Data[colName] = *result1;
            column2Data[colName] = *result2;
        } else {
            auto result1 = df1.Take<unsigned int>(colName);
            auto result2 = df2.Take<unsigned int>(colName);
            column1UintData[colName] = *result1;
            column2UintData[colName] = *result2;
        }
    }

    // Convert vec to tuple
    std::vector<std::tuple<unsigned long long, unsigned int, unsigned int,
                     unsigned int, unsigned int, unsigned int, unsigned int, 
                     unsigned int, unsigned int, unsigned int>> col1Vec;
    std::vector<std::tuple<unsigned long long, unsigned int, unsigned int, 
                     unsigned int, unsigned int, unsigned int, unsigned int, 
                     unsigned int, unsigned int, unsigned int>> col2Vec;

    // Populate col1Vec
    for (size_t i = 0; i < column1Data["event"].size(); ++i) {
        col1Vec.emplace_back(
            column1Data["event"][i],
            column1UintData["run"][i],
            column1UintData["beam_beamid"][i],
            column1UintData["pip_trkid"][i],
            column1UintData["pim_trkid"][i],
            column1UintData["p_trkid"][i],
            column1UintData["g1_showid"][i],
            column1UintData["g2_showid"][i],
            column1UintData["g3_showid"][i],
            column1UintData["g4_showid"][i]
        );
    }

    // Populate col2Vec
    for (size_t i = 0; i < column2Data["event"].size(); ++i) {
        col2Vec.emplace_back(
            column2Data["event"][i],
            column2UintData["run"][i],
            column2UintData["beam_beamid"][i],
            column2UintData["pip_trkid"][i],
            column2UintData["pim_trkid"][i],
            column2UintData["p_trkid"][i],
            column2UintData["g1_showid"][i],
            column2UintData["g2_showid"][i],
            column2UintData["g3_showid"][i],
            column2UintData["g4_showid"][i]
        );
    }

    // Create a hash map to store values and their indices from the first tree
    std::unordered_map<
        std::tuple<unsigned long long, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int>,
        std::vector<size_t>
    > indexMap;

    for (size_t i = 0; i < col1Vec.size(); ++i) {
        indexMap[col1Vec[i]].push_back(i);
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
    TH2F hist("h_chisq/ndf", "#chi^{2}/ndf Comparison;#chi^{2}/ndf Tree1;#chi^{2}/ndf Tree2", 100, 0, 1000, 100, 0, 1000);


    // Process matches
    for (size_t i = 0; i < col2Vec.size(); ++i) {
        const auto& key = col2Vec[i];
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
                for (const auto& colName : columnNames) {
                    if (colName == "event") {
                        std::cout << column1Data[colName][index] << " ";
                    } else {
                        std::cout << column1UintData[colName][index] << " ";
                    }
                }
                std::cout << std::endl;
            }
            std::cout << "In second tree:" << std::endl;
            std::cout << "Index " << i << ": ";
            for (const auto& colName : columnNames) {
                if (colName == "event") {
                    std::cout << column2Data[colName][i] << " ";
                } else {
                    std::cout << column2UintData[colName][i] << " ";
                }
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
}

int main() {
    std::string file1 = "lorax/tree_pi0pippimeta__B4_030406_flat.root";
    std::string tree1 = "pi0pippimeta__B4";
    std::string file2 = "lorax/tree_pi0pippimeta__B4_M17_030406_flat.root";
    std::string tree2 = "pi0pippimeta__B4_M17";
    std::vector<std::string> columnNames = {"event", "run", "beam_beamid", 
                            "pip_trkid", "pim_trkid", "p_trkid", "g1_showid", 
                            "g2_showid", "g3_showid", "g4_showid"};
    std::vector<std::string> testColumnNames = {"pip_p4_meas", "pim_p4_meas",
                            "beam_p4_meas", "p_p4_meas", "g1_p4_meas", 
                            "g2_p4_meas", "g3_p4_meas", "g4_p4_meas"};

    matchTrees(file1, tree1, file2, tree2, columnNames, testColumnNames);

    return 0;
}
