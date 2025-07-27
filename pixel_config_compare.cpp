#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <iomanip>
#include <sstream>

using json = nlohmann::json;

// Base64 decoding function
std::vector<uint8_t> base64_decode(const std::string& encoded_string) {
    const std::string base64_chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
    
    std::vector<uint8_t> decoded;
    int val = 0, valb = -8;
    
    for (char c : encoded_string) {
        if (c == '=') break;
        
        auto it = std::find(base64_chars.begin(), base64_chars.end(), c);
        if (it == base64_chars.end()) continue;
        
        val = (val << 6) | (it - base64_chars.begin());
        valb += 6;
        
        if (valb >= 0) {
            decoded.push_back((val >> valb) & 0xFF);
            valb -= 8;
        }
    }
    
    return decoded;
}

// Function to find bit differences between two bytes
std::vector<int> find_bit_differences(uint8_t byte1, uint8_t byte2) {
    std::vector<int> differences;
    uint8_t xor_result = byte1 ^ byte2;
    
    for (int bit = 0; bit < 8; ++bit) {
        if (xor_result & (1 << bit)) {
            differences.push_back(bit);
        }
    }
    
    return differences;
}

// Function to extract all PixelConfig values from JSON
std::vector<std::string> extract_pixel_configs(const json& j) {
    std::vector<std::string> configs;
    
    // Recursive function to search for PixelConfig
    std::function<void(const json&)> search_config = [&](const json& obj) {
        if (obj.is_object()) {
            for (auto it = obj.begin(); it != obj.end(); ++it) {
                if (it.key() == "PixelConfig" && it.value().is_string()) {
                    configs.push_back(it.value().get<std::string>());
                } else if (it.value().is_object() || it.value().is_array()) {
                    search_config(it.value());
                }
            }
        } else if (obj.is_array()) {
            for (const auto& item : obj) {
                if (item.is_object() || item.is_array()) {
                    search_config(item);
                }
            }
        }
    };
    
    search_config(j);
    return configs;
}

int main() {
    try {
        // Read the first JSON file
        std::ifstream file1("masks/maskN.srvl");
        if (!file1.is_open()) {
            std::cerr << "Error: Could not open masks/maskN.srvl" << std::endl;
            return 1;
        }
        
        json json1;
        file1 >> json1;
        file1.close();
        
        // Read the second JSON file
        std::ifstream file2("masks/mask256_511_25_1.srvl");
        if (!file2.is_open()) {
            std::cerr << "Error: Could not open masks/mask256_511_25_1.srvl" << std::endl;
            return 1;
        }
        
        json json2;
        file2 >> json2;
        file2.close();
        
        // Extract PixelConfig values
        std::vector<std::string> configs1 = extract_pixel_configs(json1);
        std::vector<std::string> configs2 = extract_pixel_configs(json2);
        
        std::cout << "Found " << configs1.size() << " PixelConfig entries in maskN.srvl" << std::endl;
        std::cout << "Found " << configs2.size() << " PixelConfig entries in mask256_511_25_1.srvl" << std::endl;
        
        if (configs1.size() != configs2.size()) {
            std::cout << "Warning: Different number of PixelConfig entries found!" << std::endl;
        }
        
        // Compare each PixelConfig entry
        size_t max_configs = std::min(configs1.size(), configs2.size());
        
        for (size_t i = 0; i < max_configs; ++i) {
            std::cout << "\n=== Comparing PixelConfig entry " << (i + 1) << " ===" << std::endl;
            
            // Decode base64 strings
            std::vector<uint8_t> bytes1 = base64_decode(configs1[i]);
            std::vector<uint8_t> bytes2 = base64_decode(configs2[i]);
            
            std::cout << "Decoded " << bytes1.size() << " bytes from maskN.srvl" << std::endl;
            std::cout << "Decoded " << bytes2.size() << " bytes from mask256_511_25_1.srvl" << std::endl;
            
            if (bytes1.size() != bytes2.size()) {
                std::cout << "Warning: Different byte lengths!" << std::endl;
            }
            
            // Compare bytes
            size_t max_bytes = std::min(bytes1.size(), bytes2.size());
            bool found_differences = false;
            
            for (size_t byte_idx = 0; byte_idx < max_bytes; ++byte_idx) {
                if (bytes1[byte_idx] != bytes2[byte_idx]) {
                    if (!found_differences) {
                        std::cout << "\nDifferences found:" << std::endl;
                        found_differences = true;
                    }
                    
                    std::vector<int> bit_diffs = find_bit_differences(bytes1[byte_idx], bytes2[byte_idx]);
                    
                    std::cout << "  Byte " << std::setw(4) << byte_idx 
                              << ": 0x" << std::hex << std::setfill('0') << std::setw(2) << (int)bytes1[byte_idx]
                              << " vs 0x" << std::setw(2) << (int)bytes2[byte_idx] << std::dec << std::setfill(' ')
                              << " (bits ";
                    
                    for (size_t j = 0; j < bit_diffs.size(); ++j) {
                        if (j > 0) std::cout << ", ";
                        std::cout << bit_diffs[j];
                    }
                    std::cout << " differ)" << std::endl;
                }
            }
            
            if (!found_differences) {
                std::cout << "No differences found in this PixelConfig entry." << std::endl;
            }
        }
        
        if (max_configs == 0) {
            std::cout << "No PixelConfig entries found in one or both files." << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 