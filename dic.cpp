#include "iostream"
#include "string"
#include "fstream"
#include "unordered_map"
#include "vector"
#include "cstdint"
#include "bitset"
#include "cmath"

#ifndef READ_DATA
#define READ_DATA

std::vector<std::string> readData(std::ifstream &file) {
    std::vector<std::string> data;
    std::string line;
    while (std::getline(file, line)) {
        data.push_back(line);
    }
    return data;
}

#endif

void dic_compression(const std::string &encode_or_decode, const std::string &data_type, std::ifstream &file,
                     const std::string &input_filename) {
    std::vector<std::string> data = readData(file);

    if (encode_or_decode == "en") {
        std::string output_filename = input_filename + ".dic";
        std::string dictionary_filename = output_filename + "file";

        std::unordered_map<std::string, int> dictionary;
        std::vector<uint8_t> packedData;  
        int index = 0;

        std::vector<int> encodedData;
        for (const auto &value: data) {
            if (dictionary.find(value) == dictionary.end()) {
                dictionary[value] = index++;
            }
            encodedData.push_back(dictionary[value]);
        }

        int bit_width = std::ceil(std::log2(index));
        if (bit_width < 1) bit_width = 1;  

        uint8_t buffer = 0;   
        int bits_in_buffer = 0;

        for (const auto &code: encodedData) {
            for (int i = 0; i < bit_width; ++i) {
                buffer |= ((code >> i) & 1) << bits_in_buffer;
                bits_in_buffer++;

                if (bits_in_buffer == 8) {
                    packedData.push_back(buffer);
                    buffer = 0;
                    bits_in_buffer = 0;
                }
            }
        }

        if (bits_in_buffer > 0) {
            packedData.push_back(buffer);
        }

        std::ofstream output_file(output_filename, std::ios::binary);
        std::ofstream dictionary_file(dictionary_filename);

        if (output_file.is_open() && dictionary_file.is_open()) {
            output_file.write(reinterpret_cast<const char*>(packedData.data()), packedData.size());
            output_file.close();

            for (const auto &entry: dictionary) {
                dictionary_file << entry.second << " -> " << entry.first << "\n";
            }
            dictionary_file.close();

            std::cout << "Data has been encoded and saved to " << output_filename << std::endl;
            std::cout << "Dictionary has been saved to " << dictionary_filename << std::endl;
        } else {
            std::cerr << "Error: Could not write to file " << output_filename << " or " << dictionary_filename << std::endl;
        }

    } else if (encode_or_decode == "de") {
        std::string output_filename = input_filename;
        std::string dictionary_filename = input_filename + "file";

        std::unordered_map<int, std::string> reverseDictionary;

        std::ifstream encoded_file(input_filename, std::ios::binary);
        if (!encoded_file.is_open()) {
            std::cerr << "Error: Encoded file not found." << std::endl;
            return;
        }

        std::vector<uint8_t> packedData((std::istreambuf_iterator<char>(encoded_file)),
                                        std::istreambuf_iterator<char>());
        encoded_file.close();

        std::ifstream dict_file(dictionary_filename);
        if (!dict_file.is_open()) {
            std::cerr << "Error: Dictionary file not found." << std::endl;
            return;
        }

        std::string line;
        while (std::getline(dict_file, line)) {
            size_t pos = line.find(" -> ");
            if (pos != std::string::npos) {
                int key = std::stoi(line.substr(0, pos));
                std::string value = line.substr(pos + 4);
                reverseDictionary[key] = value;
            }
        }
        dict_file.close();

        int bit_width = std::ceil(std::log2(reverseDictionary.size()));
        if (bit_width < 1) bit_width = 1;

        std::vector<int> encodedData;
        int current_value = 0;
        int bits_in_value = 0;

        for (const auto &byte : packedData) {
            for (int i = 0; i < 8; ++i) {
                int bit = (byte >> i) & 1;
                current_value |= (bit << bits_in_value);
                bits_in_value++;

                if (bits_in_value == bit_width) {
                    encodedData.push_back(current_value);
                    current_value = 0;
                    bits_in_value = 0;
                }
            }
        }

        std::vector<std::string> decodedData;
        for (const auto &code: encodedData) {
            decodedData.push_back(reverseDictionary[code]);
        }

        for (const auto &value: decodedData) {
            std::cout << value << "\n";
        }
    } else {
        std::cerr << "Invalid encode/decode option." << std::endl;
    }
}
