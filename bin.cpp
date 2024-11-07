#include "iostream"
#include "string"
#include "fstream"
#include "vector"

void bin_compression(const std::string &encode_or_decode, const std::string &data_type, std::ifstream &file,
                     const std::string &input_filename) {
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
                output_file.write(reinterpret_cast<const char *>(&int8_value), sizeof(int8_t));
            } else if (data_type == "int16") {
                int16_t int16_value = static_cast<int16_t>(value);
                output_file.write(reinterpret_cast<const char *>(&int16_value), sizeof(int16_t));
            } else if (data_type == "int32") {
                int32_t int32_value = static_cast<int32_t>(value);
                output_file.write(reinterpret_cast<const char *>(&int32_value), sizeof(int32_t));
            } else if (data_type == "int64") {
                output_file.write(reinterpret_cast<const char *>(&value), sizeof(int64_t));
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
            while (binary_file.read(reinterpret_cast<char *>(&int8_value), sizeof(int8_t))) {
                output_file << static_cast<int>(int8_value) << "\n";
            }
        } else if (data_type == "int16") {
            int16_t int16_value;
            while (binary_file.read(reinterpret_cast<char *>(&int16_value), sizeof(int16_t))) {
                output_file << int16_value << "\n";
            }
        } else if (data_type == "int32") {
            int32_t int32_value;
            while (binary_file.read(reinterpret_cast<char *>(&int32_value), sizeof(int32_t))) {
                output_file << int32_value << "\n";
            }
        } else if (data_type == "int64") {
            int64_t int64_value;
            while (binary_file.read(reinterpret_cast<char *>(&int64_value), sizeof(int64_t))) {
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
