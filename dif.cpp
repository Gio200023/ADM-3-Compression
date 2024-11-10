#include "iostream"
#include "string"
#include "fstream"
#include "unordered_map"

void dif_encode(std::ifstream &file, const std::string &input_filename) {
    std::string output_filename = input_filename + ".dif";
    std::ofstream output_file(output_filename);

    std::string text;
    int64_t value;
    int64_t previousValue;
    bool isFirst = true;
    while (std::getline(file, text)) {
        value = std::strtoll(text.c_str(), NULL, 10);

        if(isFirst) {
            output_file << value << std::endl;
        } else {
            output_file << value - previousValue << std::endl;
        }
        previousValue = value;
        isFirst = false;
    }
    output_file.close();
}

void dif_decode(std::ifstream &file) {
    std::string text;
    int64_t value;
    int64_t previousValue = 0;
    bool isFirst = true;
    while (std::getline(file, text)) {
        value = std::strtoll(text.c_str(), NULL, 10);

        if(isFirst) {
            std::cout << value << std::endl;
        } else {
            std::cout << value + previousValue << std::endl;
        }
        previousValue += value;
        isFirst = false;
    }
}

void dif_compression(const std::string &encode_or_decode, const std::string &data_type, std::ifstream &file,
                     const std::string &input_filename) {

    if (encode_or_decode == "en") {
        dif_encode(file, input_filename);
    } else {
        dif_decode(file);
    }
}
