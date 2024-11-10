#include "iostream"
#include "string"
#include "fstream"
#include "unordered_map"
#include "vector"
#include "cstdint"

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
        std::vector<int32_t> encodedData; 
        int index = 0;

        for (const auto &value: data) {
            if (dictionary.find(value) == dictionary.end()) {
                dictionary[value] = index++;
            }
            encodedData.push_back(static_cast<int32_t>(dictionary[value])); 
        }

        std::ofstream output_file(output_filename, std::ios::binary);
        std::ofstream dictionary_file(dictionary_filename);
        if (output_file.is_open() && dictionary_file.is_open()) {
            for (const auto &code: encodedData) {
                output_file.write(reinterpret_cast<const char*>(&code), sizeof(code));
            }
            output_file.close();

            for (const auto &entry: dictionary) {
                dictionary_file << entry.second << " -> " << entry.first << "\n";
            }
            dictionary_file.close();

            std::cout << "Data has been encoded and saved to " << output_filename << std::endl;
            std::cout << "Dictionary has been saved to " << dictionary_filename << std::endl;
        } else {
            std::cerr << "Error: Could not write to file " << output_filename << " or " << dictionary_filename
                      << std::endl;
        }

    } else if (encode_or_decode == "de") {
        std::string output_filename = input_filename;
        std::string dictionary_filename = input_filename + "file";

        std::unordered_map<int, std::string> reverseDictionary;
        std::vector<int32_t> encodedData;

        std::ifstream encoded_file(input_filename, std::ios::binary);
        if (!encoded_file.is_open()) {
            std::cerr << "Error: Encoded file not found." << std::endl;
            return;
        }

        int32_t code16;
        while (encoded_file.read(reinterpret_cast<char*>(&code16), sizeof(code16))) {
            encodedData.push_back(code16);
        }
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
