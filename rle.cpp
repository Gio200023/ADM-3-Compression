#include "iostream"
#include "string"
#include "fstream"
#include "unordered_map"

void rle_encode(std::ifstream &file, const std::string &input_filename) {
    std::string output_filename = input_filename + ".rle";
    std::ofstream output_file(output_filename);

    std::string previousValue;

    int count = 0;

    std::string value;
    while (std::getline(file, value)) {
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

void rle_decode(std::ifstream &file) {
    std::string value;
    while (std::getline(file, value)) {
        std::string out = value.substr(0, value.find(" "));
        int64_t count = 1;
        int64_t t_count = std::strtoll(out.c_str(), NULL, 10);
        if (value != out && t_count != 0) {
            count = std::strtoll(out.c_str(), NULL, 10);
            value.erase(0, value.find(" ") + 1);
        }

        for (int i = 0; i < count; i++) {
            std::cout << value << std::endl;
        }
    }
}

void rle_compression(const std::string &encode_or_decode, const std::string &data_type, std::ifstream &file,
                     const std::string &input_filename) {

    if (encode_or_decode == "en") {
        rle_encode(file, input_filename);
    } else {
        rle_decode(file);
    }
}
