#include <iostream>
#include <string>
#include <fstream>
#include <type_traits>

template <typename T>
void bin_compression(const std::string &encode_or_decode, std::ifstream &file, 
                     const std::string &input_filename) {
    static_assert(std::is_integral<T>::value, "Template type must be an integral type.");

    if (encode_or_decode == "en") {
        // Encoding: Read integer data from the file and write as binary.
        std::string output_filename = input_filename + ".bin";
        std::ofstream output_file(output_filename, std::ios::binary);

        if (!output_file.is_open()) {
            std::cerr << "Error: Could not write to file " << output_filename << std::endl;
            return;
        }

        int64_t value;  // Using 64-bit integer to handle all cases from file input
        while (file >> value) {
            T output_value = static_cast<T>(value);
            output_file.write(reinterpret_cast<const char *>(&output_value), sizeof(T));
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

        T input_value;
        while (binary_file.read(reinterpret_cast<char *>(&input_value), sizeof(T))) {
            output_file << static_cast<int64_t>(input_value) << "\n";  // Casting to handle all integer types
        }

        output_file.close();
        binary_file.close();
        std::cout << "Data has been decoded and saved to " << output_filename << std::endl;

    } else {
        std::cerr << "Invalid encode/decode option." << std::endl;
    }
}
