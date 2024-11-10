//
// Created by Rajeck Massa on 07/11/2024.
//
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <algorithm>

/**
 * Reads the data from the file and stores it in a pair: <minimumValue, data>
 * The minimumValue is the base for the FOR-encoding
 *
 * @tparam integer_type Type to which each integer from the file should be cast. Can be one of the provided
 * data types in the assignment PDF.
 * @param file The input file stream from which data is read.
 * @return A pair containing the minimum value and a vector of data values read from the file.
 */
template <typename integer_type>
std::pair<int64_t, std::vector<integer_type> > readForData(std::ifstream &file) {
    // Set the minimum value to the largest number possible
    int64_t minimumValue = std::numeric_limits<int64_t>::max();
    std::vector<integer_type> data;
    std::string line;
    // Loop through file and store the smallest number
    // as base
    while (std::getline(file, line)) {
        int64_t num = std::stoll(line);
        if(num < minimumValue) {
            minimumValue = num;
        }
        // Store all the numbers in the vector
        data.push_back(static_cast<integer_type>(num));
    }
    return std::make_pair(minimumValue, data);
}


/**
 * Calculates the right offset to write to the file on the disk. This is based on the
 * biggest 'gap'.
 * @param max_offset Biggest number (offset) found in the file
 * @return The correct offset size
 */
size_t obtainOffset(const int &max_offset) {
    // Check which offset we need
    size_t offsetSize = 0;
    if (max_offset <= std::numeric_limits<int8_t>::max()) {
        offsetSize = sizeof(int8_t);
    } else if (max_offset <= std::numeric_limits<int16_t>::max()) {
        offsetSize = sizeof(int16_t);
    } else if (max_offset <= std::numeric_limits<int32_t>::max() ){
        offsetSize = sizeof(int32_t);
    } else {
        offsetSize = sizeof(int64_t);
    }
    return offsetSize;
}

/**
 * Write the encodings, in binary mode, to a disk on the file.
 * @tparam offsetType Type of the biggest offset: based on the biggest gap in the decoded file.
 * @tparam resultType Type to where the results are casted to, so can be int8_t, int16_t till int64_t.
 * @param results Vector with all the gaps (results).
 * @param file The output file stream to where the data is written to.
 */
template <typename offsetType, typename resultType>
void writeToDisk(const std::vector<resultType> &results, std::ofstream &file) {
    // Loop through the vector and write to the file
    for (const auto &value: results) {
        auto offset = static_cast<offsetType>(value);
        file.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
    }
}

/**
 * Function that encodes the data.
 * @tparam integer_type Integer type chosen by the user.
 * @param minimumValue Base value for the FOR encoding
 * @param data Vector with all the gaps
 * @param filename Name of the output file to write to
 */
template <typename integer_type>
void for_encode(int64_t minimumValue, const std::vector<integer_type> &data, const std::string &filename) {
    // Store the 'gaps'
    std::vector<integer_type> results;
    for (const auto &value: data) {
        results.push_back(static_cast<integer_type>(value - minimumValue));
    }
    // Obtain the max element, so the biggest gap and
    // calculate the right offset size
    int max_offset = *std::max_element(results.begin(), results.end());
    size_t offset_size = obtainOffset(max_offset);

    // Create an output file and do some error handling
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "File " << filename << " could not be opened/created.";
        return;
    }
    // Write the base value and the offset size to the file
    file.write(reinterpret_cast<const char*>(&minimumValue), sizeof(minimumValue));
    file.write(reinterpret_cast<const char*>(&offset_size), sizeof(offset_size));

    // Determine which size we have based on the offset size
    // and call writeToDisk based on the size
    if (offset_size == sizeof(int8_t)) {
        writeToDisk<int8_t, integer_type>(results, file);
    } else if (offset_size == sizeof(int16_t)) {
        writeToDisk<int16_t, integer_type>(results, file);
    } else if (offset_size == sizeof(int32_t)) {
        writeToDisk<int32_t, integer_type>(results, file);
    } else {
        writeToDisk<int64_t, integer_type>(results, file);
    }

    file.close();
}

/**
 * Function which reads the data form an encoded file.
 * @tparam offset_type Offset type, based on the biggest gap in the encoding
 * @tparam integer_type Integer type, chosen by the user.
 * @param decodedValues Call by reference given vector to store the decoded values
 * @param file The input file stream from which data is read.
 * @param minimumValue The base value.
 */
template <typename offset_type, typename integer_type>
void readData(std::vector<integer_type> &decodedValues, std::ifstream &file, const int64_t &minimumValue) {
    // Loop through the file and obtain the values
    while (file) {
        offset_type value = 0;
        file.read(reinterpret_cast<char*>(&value), sizeof(offset_type));
        decodedValues.push_back(static_cast<integer_type>(minimumValue + value));
    }
    // Pop the last value (which is not an number, but a line break / EOF token)
    decodedValues.pop_back();
}

/**
 * Function to decode an encoded file
 * @tparam integer_type Integer type chosen by the user
 * @param file The input file stream from which data is read.
 * @param filename The output file name to store the decoded numbers.
 */
template <typename integer_type>
void for_decode(std::ifstream &file, const std::string &filename) {
    // Read the base value and the offset size
    int64_t minimumValue = 0;
    size_t offset_size = 0;
    file.read(reinterpret_cast<char*>(&minimumValue), sizeof(minimumValue));
    file.read(reinterpret_cast<char*>(&offset_size), sizeof(offset_size));
    // Read all the data into the vector
    std::vector<integer_type> decodedValues;
    if (offset_size == sizeof(int8_t)) {
        readData<int8_t, integer_type>(decodedValues, file, minimumValue);
    } else if (offset_size == sizeof(int16_t)) {
        readData<int16_t, integer_type>(decodedValues, file, minimumValue);
    } else if (offset_size == sizeof(int32_t)) {
        readData<int32_t, integer_type>(decodedValues, file, minimumValue);
    } else {
        readData<int64_t, integer_type>(decodedValues, file, minimumValue);
    }

    // Convert the value to the string and write to disk
    for (const auto &value: decodedValues) {
        std::cout << std::to_string(value) << std::endl;
    }

}

/**
 * Function to (de)compress the file
 * @tparam integer_type Integer type chosen by the user
 * @param encode_or_decode String to chose the encoding or decoding method
 * @param data_type The integer type chosen by the user
 * @param file The file to read from, if we want to decode
 * @param input_filename The filename to store the encoded or decoded results
 */
template <typename integer_type>
void for_compression(const std::string &encode_or_decode, const std::string &data_type, std::ifstream &file,
                     const std::string &input_filename) {
    if (encode_or_decode == "en") {
        auto pair = readForData<integer_type>(file);
        for_encode<integer_type>(pair.first, pair.second, input_filename + ".for");
    } else {
        for_decode<integer_type>(file, input_filename + ".decoded");
    }
}




