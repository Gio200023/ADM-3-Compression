#include "iostream"
#include "string"
#include "fstream"
#include "filesystem"
#include "unordered_map"
#include "vector"
#include <cstdint>
#define SUCCESS 0

std::vector<std::string> readData(std::ifstream &file) {
    std::vector<std::string> data;
    std::string line;
    while (std::getline(file, line)) {
        data.push_back(line);
    }
    return data;
}

void dic_compression(const std::string &encode_or_decode, const std::string &data_type, std::ifstream &file, const std::string &input_filename) {
    std::vector<std::string> data = readData(file);

    if (encode_or_decode == "en") {
        std::string output_filename = input_filename + ".dic";
        std::string dictionary_filename = output_filename + "file";
        
        std::unordered_map<std::string, int> dictionary;
        std::vector<int> encodedData;
        int index = 0;

        for (const auto &value : data) {
            if (dictionary.find(value) == dictionary.end()) {
                dictionary[value] = index++;
            }
            encodedData.push_back(dictionary[value]);
        }

        std::ofstream output_file(output_filename);
        std::ofstream dictionary_file(dictionary_filename);
        if (output_file.is_open() && dictionary_file.is_open()) {
            for (const auto &code : encodedData) {
                output_file << code << " ";
            }
            output_file.close();

            for (const auto &entry : dictionary) {
                dictionary_file << entry.second << " -> " << entry.first << "\n";
            }
            dictionary_file.close();

            std::cout << "Data has been encoded and saved to " << output_filename << std::endl;
            std::cout << "Dictionary has been saved to " << dictionary_filename << std::endl;
        } else {
            std::cerr << "Error: Could not write to file " << output_filename << " or " << dictionary_filename << std::endl;
        }

    } else if (encode_or_decode == "de") {
        std::string output_filename = input_filename + ".dic";
        std::string dictionary_filename = input_filename + "file";

        std::unordered_map<int, std::string> reverseDictionary;
        std::vector<int> encodedData;

        std::ifstream encoded_file(input_filename);
        int code;
        while (encoded_file >> code) {
            encodedData.push_back(code);
        }
        std::cerr << dictionary_filename << std::endl;
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
        for (const auto &code : encodedData) {
            decodedData.push_back(reverseDictionary[code]);
        }

        for (const auto &value : decodedData) {
            std::cout << value << "\n";
        }
    } else {
        std::cerr << "Invalid encode/decode option." << std::endl;
    }
}

void bin_compression(const std::string &encode_or_decode, const std::string &data_type, std::ifstream &file, const std::string &input_filename) {
    if (encode_or_decode == "en") {
        // Encoding: Read integer data from the file, write it as binary.
        std::string output_filename = input_filename + ".bin";
        std::ofstream output_file(output_filename, std::ios::binary);
        
        if (!output_file.is_open()) {
            std::cerr << "Error: Could not write to file " << output_filename << std::endl;
            return;
        }

        int64_t value;  // Using 64-bit integer to handle all cases
        while (file >> value) {
            if (data_type == "int8") {
                int8_t int8_value = static_cast<int8_t>(value);
                output_file.write(reinterpret_cast<const char*>(&int8_value), sizeof(int8_t));
            } else if (data_type == "int16") {
                int16_t int16_value = static_cast<int16_t>(value);
                output_file.write(reinterpret_cast<const char*>(&int16_value), sizeof(int16_t));
            } else if (data_type == "int32") {
                int32_t int32_value = static_cast<int32_t>(value);
                output_file.write(reinterpret_cast<const char*>(&int32_value), sizeof(int32_t));
            } else if (data_type == "int64") {
                output_file.write(reinterpret_cast<const char*>(&value), sizeof(int64_t));
            } else {
                std::cerr << "Error: Unsupported data type for binary encoding." << std::endl;
                output_file.close();
                return;
            }
        }
        output_file.close();
        std::cout << "Data has been encoded and saved to " << output_filename << std::endl;

    } else if (encode_or_decode == "de") {
        // Decoding: Read binary data and output as text.
        std::string output_filename = input_filename + ".csv";
        std::ofstream output_file(output_filename);

        if (!output_file.is_open()) {
            std::cerr << "Error: Could not write to file " << output_filename << std::endl;
            return;
        }

        std::ifstream binary_file(input_filename, std::ios::binary);
        if (!binary_file.is_open()) {
            std::cerr << "Error: Could not open binary file " << input_filename << std::endl;
            return;
        }

        if (data_type == "int8") {
            int8_t int8_value;
            while (binary_file.read(reinterpret_cast<char*>(&int8_value), sizeof(int8_t))) {
                output_file << static_cast<int>(int8_value) << "\n";
            }
        } else if (data_type == "int16") {
            int16_t int16_value;
            while (binary_file.read(reinterpret_cast<char*>(&int16_value), sizeof(int16_t))) {
                output_file << int16_value << "\n";
            }
        } else if (data_type == "int32") {
            int32_t int32_value;
            while (binary_file.read(reinterpret_cast<char*>(&int32_value), sizeof(int32_t))) {
                output_file << int32_value << "\n";
            }
        } else if (data_type == "int64") {
            int64_t int64_value;
            while (binary_file.read(reinterpret_cast<char*>(&int64_value), sizeof(int64_t))) {
                output_file << int64_value << "\n";
            }
        } else {
            std::cerr << "Error: Unsupported data type for binary decoding." << std::endl;
        }
        output_file.close();
        binary_file.close();
        std::cout << "Data has been decoded and saved to " << output_filename << std::endl;
    } else {
        std::cerr << "Invalid encode/decode option." << std::endl;
    }
}


void processData(std::string encode_or_decode,const std::string compression_technique, const std::string data_type, std::ifstream &filename, const std::string &input_filename){

    std::cout << "Processing file with compression technique '"
              << compression_technique << "' and data type '" << data_type << ", :" << ((encode_or_decode == "de") ? " decoding" : " encoding") << "'." << std::endl;

    if (compression_technique == "dic"){
        dic_compression(encode_or_decode, data_type, filename, input_filename);
    } else if (compression_technique == "bin") {
        bin_compression(encode_or_decode, data_type, filename, input_filename);
    }
    // add your function based on datatype and compression tech.
}

int main(int argc, char* argv[]){

    if (argc < 4) {
        std::cerr << "Error: Missing argument. Please specify 4 arguments" << std::endl;
        return 1;
    }

    std::string decode_or_encode;
    if (std::string(argv[1]) == "de") {
        decode_or_encode = "de";
    } else if (std::string(argv[1]) == "en") {
        decode_or_encode = "en";
    } else {
        std::cerr << "Error: first argument is invalid. Please specify 'de' or 'en'." << std::endl;
        return 1;
    }

    std::string compression_technique;
    if (std::string(argv[2]) == "bin" || std::string(argv[2]) == "rle" 
        || std::string(argv[2]) == "dic" || std::string(argv[2]) == "for"
        || std::string(argv[2]) == "dif") {
        compression_technique = std::string(argv[2]);
    } else {
        std::cerr << "Error: second argument is invalid. Please specify 'bin', 'rle', 'dic', 'for', or 'dif'." << std::endl;
        return 1;
    }

    std::string data_type;
    if (std::string(argv[3]) == "int8" || std::string(argv[3]) == "int16" 
        || std::string(argv[3]) == "int32" || std::string(argv[3]) == "int64"
        || std::string(argv[3]) == "string") {
        data_type = std::string(argv[3]);
    } else {
        std::cerr << "Error: third argument is invalid. Please specify 'int8', 'int16', 'int32', 'int64', or 'string'." << std::endl;
        return 1;
    }

    std::ifstream file;
    file.open(argv[4]);
    if (!file){
        std::cerr << "Error: File not found." << std::endl;
        return 1;
    }
    std::string filename = std::filesystem::path(argv[4]).filename().string();

    processData(decode_or_encode,compression_technique, data_type, file, filename);
 
    return SUCCESS;
}