#include <Windows.h>
#include "images2onefile.h"

std::string get_current_path() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}

int main(int argc, const char* argv[]) {
    /* target image paths here */
    std::vector<std::string> img_paths = { 
        get_current_path() + "\\image1.png",
        get_current_path() + "\\image2.jpg"
    };
    /* target data file path here */
    std::string target_path = get_current_path() + "\\output.data";

    // compress images to one file
    std::vector<ImageData> input_images;
    for (const auto& path : img_paths) {
        ImageData img;
        if (load_image(path, img)) input_images.push_back(img);
        else std::cout << "failed to load image : " << path << "\n";
    }
    if (!compress_images(input_images, target_path)) {
        std::cout << "failed to compress images" << "\n";
    }

    // decompress images from one file
    std::vector<ImageData> output_images;
    if (decompress_images(target_path, output_images) == false)
        std::cerr << "failed to decompress images" << "\n";

    // re-write the image files (only supports .png and .jpg)
    for (size_t i = 0; i < output_images.size(); ++i) {
        const auto& img = output_images[i];
        std::string new_output_path = get_current_path() + "\\output" + std::to_string(i) + ".png";
        if (img.channels == 3) {
            new_output_path = get_current_path() + "\\output" + std::to_string(i) + ".jpg";
            stbi_write_jpg(new_output_path.c_str(), img.width, img.height, img.channels, img.data.data(), 100);
            continue;
        }
        stbi_write_png(new_output_path.c_str(), img.width, img.height, img.channels, img.data.data(), img.width * img.channels);
    }
}
