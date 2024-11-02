#include "iostream"
#include "string"
#include "fstream"
#include "filesystem"
#include "unordered_map"
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
    std::string output_filename = input_filename + ".dic";

    if (encode_or_decode == "en") {
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
        if (output_file.is_open()) {
            for (const auto &code : encodedData) {
                output_file << code << " ";
            }

            // std::cout << "\n\nDictionary:\n";
            
            // for (const auto &entry : dictionary) {
            //     std::cout << entry.first << " -> " << entry.second << "\n";
            // }
            
            output_file.close();
            std::cout << "Data has been encoded and saved to " << output_filename << std::endl;
        } else {
            std::cerr << "Error: Could not write to file " << output_filename << std::endl;
        }

        std::cout << "\nDictionary:\n";
        for (const auto &entry : dictionary) {
            std::cout << entry.first << " -> " << entry.second << std::endl;
        }

    } else if (encode_or_decode == "de") {

        std::unordered_map<int, std::string> reverseDictionary;
        std::vector<int> encodedData;

        std::vector<std::string> decodedData;
        for (const auto &code : encodedData) {
            decodedData.push_back(reverseDictionary[code]);
        }

        std::cout << "Decoded Data: ";
        for (const auto &value : decodedData) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    } else {
        std::cerr << "Invalid encode/decode option." << std::endl;
    }
}

void processData(std::string encode_or_decode,const std::string compression_technique, const std::string data_type, std::ifstream &filename, const std::string &input_filename){

    std::cout << "Processing file with compression technique '"
              << compression_technique << "' and data type '" << data_type << ", :" << ((encode_or_decode == "de") ? " decoding" : " encoding") << "'." << std::endl;

    if (compression_technique == "dic"){
        dic_compression(encode_or_decode, data_type, filename, input_filename);
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
    }
    std::string filename = std::filesystem::path(argv[4]).filename().string();

    processData(decode_or_encode,compression_technique, data_type, file, filename);
 
    return SUCCESS;
}