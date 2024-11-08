//
// Created by Rajeck Massa on 07/11/2024.
//
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <algorithm>

template <typename T>
std::pair<int64_t, std::vector<T> > readForData(std::ifstream &file) {
    int64_t minimumValue = std::numeric_limits<int64_t>::max();
    std::vector<T> data;
    std::string line;
    while (std::getline(file, line)) {
        int64_t num = std::stoll(line);
        if(num < minimumValue) {
            minimumValue = num;
        }
        data.push_back(static_cast<T>(num));
    }
    return std::make_pair(minimumValue, data);
}

size_t obtainOffset(const int &max_offset) {
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

template <typename offsetType, typename resultType>
void writeToDisk(const std::vector<resultType> &results, std::ofstream &file) {
    for (const auto &value: results) {
        auto offset = static_cast<offsetType>(value);
        file.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
    }
}

template <typename T>
void for_encode(int64_t minimumValue, const std::vector<T> &data, const std::string &filename) {
    std::vector<T> results;
    for (const auto &value: data) {
        results.push_back(static_cast<T>(value - minimumValue));
    }

    int max_offset = *std::max_element(results.begin(), results.end());
    size_t offset_size = obtainOffset(max_offset);

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "File " << filename << " could not be opened/created.";
        return;
    }

    file.write(reinterpret_cast<const char*>(&minimumValue), sizeof(minimumValue));
    file.write(reinterpret_cast<const char*>(&offset_size), sizeof(offset_size));

    if (offset_size == sizeof(int8_t)) {
        writeToDisk<int8_t, T>(results, file);
    } else if (offset_size == sizeof(int16_t)) {
        writeToDisk<int16_t, T>(results, file);
    } else if (offset_size == sizeof(int32_t)) {
        writeToDisk<int32_t, T>(results, file);
    } else {
        writeToDisk<int64_t, T>(results, file);
    }

    file.close();
}

template <typename T, typename resultType>
void readData(std::vector<resultType> &decodedValues, std::ifstream &file, const int64_t &minimumValue) {
    while (file) {
        T value = 0;
        file.read(reinterpret_cast<char*>(&value), sizeof(T));
        decodedValues.push_back(static_cast<resultType>(minimumValue + value));
    }
    decodedValues.pop_back();
}

template <typename T>
void for_decode(std::ifstream &file, const std::string &filename) {
    int64_t minimumValue = 0;
    size_t offset_size = 0;
    file.read(reinterpret_cast<char*>(&minimumValue), sizeof(minimumValue));
    file.read(reinterpret_cast<char*>(&offset_size), sizeof(offset_size));

    std::vector<T> decodedValues;
    if (offset_size == sizeof(int8_t)) {
        readData<int8_t, T>(decodedValues, file, minimumValue);
    } else if (offset_size == sizeof(int16_t)) {
        readData<int16_t, T>(decodedValues, file, minimumValue);
    } else if (offset_size == sizeof(int32_t)) {
        readData<int32_t, T>(decodedValues, file, minimumValue);
    } else {
        readData<int64_t, T>(decodedValues, file, minimumValue);
    }

    std::ofstream output(filename);
    for (const auto &value: decodedValues) {
        output << std::to_string(value) << std::endl;
    }
    output.close();

}

template <typename T>
void for_compression(const std::string &encode_or_decode, const std::string &data_type, std::ifstream &file,
                     const std::string &input_filename) {
    if (encode_or_decode == "en") {
        auto pair = readForData<T>(file);
        for_encode<T>(pair.first, pair.second, input_filename + ".for");
    } else {
        for_decode<T>(file, input_filename + ".decoded");
    }
}




