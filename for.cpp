//
// Created by Rajeck Massa on 07/11/2024.
//
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <algorithm>

// TODO: Change int to template
std::pair<int, std::vector<int> > readForData(std::ifstream &file) {
    int minimumValue = std::numeric_limits<int>::max();
    std::vector<int> data;
    std::string line;
    while (std::getline(file, line)) {
        int num = std::stoi(line);
        if(num < minimumValue) {
            minimumValue = num;
        }
        data.push_back(num);
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

template <typename T>
void writeToDisk(const std::vector<int> &results, std::ofstream &file) {
    int count = 0;
    for (const auto &value: results) {
        T offset = static_cast<T>(value);
        file.write(reinterpret_cast<const char*>(&offset), sizeof(T));
        count++;
    }
    std::cout << "Count is " << count << std::endl;
}

// TODO: Change int to template
//
void for_encode(int minimumValue, std::vector<int> data, std::string filename) {
    std::vector<int> results;
    for (const auto &value: data) {
        results.push_back(value - minimumValue);
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
        writeToDisk<int8_t>(results, file);
    } else if (offset_size == sizeof(int16_t)) {
        writeToDisk<int16_t>(results, file);
    } else if (offset_size == sizeof(int32_t)) {
        writeToDisk<int32_t>(results, file);
    } else {
        writeToDisk<int64_t>(results, file);
    }

    file.close();
}

template <typename T>
void readData(std::vector<int> &decodedValues, std::ifstream &file, const int &minimumValue) {
    while (file) {
        T value = 0;
        file.read(reinterpret_cast<char*>(&value), sizeof(T));
        decodedValues.push_back(minimumValue + value);
    }
    decodedValues.pop_back();
}

void for_decode(std::ifstream &file, std::string filename) {
    int minimumValue = 0;
    size_t offset_size = 0;
    file.read(reinterpret_cast<char*>(&minimumValue), sizeof(minimumValue));
    file.read(reinterpret_cast<char*>(&offset_size), sizeof(offset_size));

    std::vector<int> decodedValues;
    if (offset_size == sizeof(int8_t)) {
        readData<int8_t>(decodedValues, file, minimumValue);
    } else if (offset_size == sizeof(int16_t)) {
        readData<int16_t>(decodedValues, file, minimumValue);
    } else if (offset_size == sizeof(int32_t)) {
        readData<int32_t>(decodedValues, file, minimumValue);
    } else {
        readData<int64_t>(decodedValues, file, minimumValue);
    }

    std::ofstream output(filename);
    for (const auto &value: decodedValues) {
        output << value << std::endl;
    }
    output.close();

}

void for_compression(const std::string &encode_or_decode, const std::string &data_type, std::ifstream &file,
                     const std::string &input_filename) {
    if (encode_or_decode == "en") {
        auto pair = readForData(file);
        for_encode(pair.first, pair.second, input_filename + ".for");
    } else {
        for_decode(file, input_filename + ".decoded");
    }
}




