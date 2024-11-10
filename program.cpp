#include "iostream"
#include "string"
#include "fstream"
#include "filesystem"
#include "functional"
#include <map>
#include "dic.cpp"
#include "bin.cpp"
#include "rle.cpp"
#include "dif.cpp"
#include "for.cpp"

#define SUCCESS 0

void processData(std::string encode_or_decode, const std::string compression_technique, const std::string data_type,
                 std::ifstream &filename, const std::string &input_filename) {

    // std::cout << "Processing file with compression technique '"
    //           << compression_technique << "' and data type '" << data_type << ", :"
    //           << ((encode_or_decode == "de") ? " decoding" : " encoding") << "'." << std::endl;

    // Map for each int-type for the compression encoding
    std::map<std::string, std::function<void()>> for_compression_map = {
            {"int8",  [&]() { for_compression<int8_t>(encode_or_decode, data_type, filename, input_filename); }},
            {"int16", [&]() { for_compression<int16_t>(encode_or_decode, data_type, filename, input_filename); }},
            {"int32", [&]() { for_compression<int32_t>(encode_or_decode, data_type, filename, input_filename); }},
            {"int64", [&]() { for_compression<int64_t>(encode_or_decode, data_type, filename, input_filename); }}
    };

    // Map for int-type for the `bin` compression encoding
    std::map<std::string, std::function<void()>> bin_compression_map = {
        {"int8",  [&]() { bin_compression<int8_t>(encode_or_decode, filename, input_filename); }},
        {"int16", [&]() { bin_compression<int16_t>(encode_or_decode, filename, input_filename); }},
        {"int32", [&]() { bin_compression<int32_t>(encode_or_decode, filename, input_filename); }},
        {"int64", [&]() { bin_compression<int64_t>(encode_or_decode, filename, input_filename); }}
    };

    if (compression_technique == "dic") {
        dic_compression(encode_or_decode, data_type, filename, input_filename);
    } else if (compression_technique == "bin") {
        // Call the appropriate bin_compression function
        auto it = bin_compression_map.find(data_type);
        if (it != bin_compression_map.end()) {
            it->second();
        } else {
            std::cerr << "Data type " << data_type << " is not supported for binary compression." << std::endl;
        };
    } else if (compression_technique == "rle") {
        rle_compression(encode_or_decode, data_type, filename, input_filename);
    } else if (compression_technique == "dif") {
        dif_compression(encode_or_decode, data_type, filename, input_filename);
    } else if (compression_technique == "for") {
        // Find the right function for the encoding
        auto it = for_compression_map.find(data_type);
        if (it != for_compression_map.end()) {
            it->second();
        } else {
            // Handle unknown data type (so, string in this case).
            std::cerr << "Data type " << data_type << " is not supported." << std::endl;
        }
    }
}

int main(int argc, char *argv[]) {

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
        std::cerr << "Error: second argument is invalid. Please specify 'bin', 'rle', 'dic', 'for', or 'dif'."
                  << std::endl;
        return 1;
    }

    std::string data_type;
    if (std::string(argv[3]) == "int8" || std::string(argv[3]) == "int16"
        || std::string(argv[3]) == "int32" || std::string(argv[3]) == "int64"
        || std::string(argv[3]) == "string") {
        data_type = std::string(argv[3]);
    } else {
        std::cerr << "Error: third argument is invalid. Please specify 'int8', 'int16', 'int32', 'int64', or 'string'."
                  << std::endl;
        return 1;
    }

    std::ifstream file;
    file.open(argv[4]);
    if (!file) {
        std::cerr << "Error: File not found." << std::endl;
        return 1;
    }
    std::string filename = std::filesystem::path(argv[4]).filename().string();

    processData(decode_or_encode, compression_technique, data_type, file, filename);

    return SUCCESS;
}
