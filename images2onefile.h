#pragma once

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <stb_image.h>
#include <stb_image_write.h>
#include <zlib.h>

struct ImageData {
    int width, height, channels;
    std::vector<unsigned char> data;
};

static bool load_image(__in const std::string& path, __out ImageData& img) {
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (data != nullptr) {
        img.width = width;
        img.height = height;
        img.channels = channels;
        img.data = std::vector<unsigned char>(data, data + width * height * channels);
        stbi_image_free(data);
        return true;
    }
    return false;
}

static bool compress_images(__in const std::vector<ImageData>& images, __in const std::string& outputPath) {
    std::ofstream file(outputPath, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "file not exist\n";
        return false;
    }
    for (const auto& img : images) {
        file.write(reinterpret_cast<const char*>(&img.width), sizeof(img.width));
        file.write(reinterpret_cast<const char*>(&img.height), sizeof(img.height));
        file.write(reinterpret_cast<const char*>(&img.channels), sizeof(img.channels));
        
        uLongf compressed_size = compressBound(img.data.size());
        std::vector<unsigned char> compressed_data(compressed_size);
        if (compress2(compressed_data.data(), &compressed_size, img.data.data(), img.data.size(), Z_BEST_COMPRESSION) != Z_OK) {
            return false;
        }
        
        file.write(reinterpret_cast<const char*>(&compressed_size), sizeof(compressed_size));
        file.write(reinterpret_cast<const char*>(compressed_data.data()), compressed_size);
    }
    file.close();
    return true;
}

static bool decompress_images(__in const std::string& inputPath, __out std::vector<ImageData>& images) {
    std::ifstream file(inputPath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    while (!file.eof()) {
        ImageData img;
        file.read(reinterpret_cast<char*>(&img.width), sizeof(img.width));
        file.read(reinterpret_cast<char*>(&img.height), sizeof(img.height));
        file.read(reinterpret_cast<char*>(&img.channels), sizeof(img.channels));

        uLongf compressed_size;
        file.read(reinterpret_cast<char*>(&compressed_size), sizeof(compressed_size));
        
        std::vector<unsigned char> compressed_data(compressed_size);
        file.read(reinterpret_cast<char*>(compressed_data.data()), compressed_size);
        
        uLongf decompressed_size = img.width * img.height * img.channels;
        img.data.resize(decompressed_size);
        uncompress(img.data.data(), &decompressed_size, compressed_data.data(), compressed_size);
        images.push_back(img);
    }
    images.erase(images.end() - 1);
    return true;
}
