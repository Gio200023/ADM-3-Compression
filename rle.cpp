#include "iostream"
#include "string"
#include "fstream"
#include "unordered_map"
#include "vector"

#ifndef READ_DATA
#define READ_DATA

std::vector <std::string> readData(std::ifstream &file) {
    std::vector <std::string> data;
    std::string line;
    while (std::getline(file, line)) {
        data.push_back(line);
    }
    return data;
}

#endif

void rle_encode(std::ifstream &file, const std::string &input_filename) {
    std::vector <std::string> data = readData(file);
    std::string output_filename = input_filename + ".rle";
    std::ofstream output_file(output_filename);

    std::string previousValue;

    int count = 0;
    for (const auto &value: data) {
        if (value == previousValue || previousValue == "") {
            count++;
        } else {
            if (count == 1) {
                output_file << previousValue << std::endl;
            } else {
                output_file << count << " " << previousValue << std::endl;
            }
            count = 1;
        }

        previousValue = value;
    }

    if (count == 1) {
        output_file << previousValue << std::endl;
    } else {
        output_file << count << " " << previousValue << std::endl;
    }
    output_file.close();
}

void rle_decode(std::ifstream &file, const std::string &input_filename) {
    std::vector <std::string> data = readData(file);
    std::string output_filename = input_filename + ".csv";
    std::ofstream output_file(output_filename);

    for (auto value: data) {
        std::string out = value.substr(0, value.find(" "));
        int64_t count = 1;
        if (value != out) {
            count = std::strtoll(out.c_str(), NULL, 10);
            value.erase(0, value.find(" ") + 1);
        }

        for (int i = 0; i < count; i++) {
            output_file << value << std::endl;
        }
    }
    output_file.close();
}

void rle_compression(const std::string &encode_or_decode, const std::string &data_type, std::ifstream &file,
                     const std::string &input_filename) {

    if (encode_or_decode == "en") {
        rle_encode(file, input_filename);
    } else {
        rle_decode(file, input_filename);
    }
}
