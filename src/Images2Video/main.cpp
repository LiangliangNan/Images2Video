/********************************************************************
 * Copyright (C) 2015 Liangliang Nan <liangliang.nan@gmail.com>
 * https://3d.bk.tudelft.nl/liangliang/
 *
 * This file is part of Easy3D. If it is useful in your research/work,
 * I would be grateful if you show your appreciation by citing it:
 * ------------------------------------------------------------------
 *      Liangliang Nan.
 *      Easy3D: a lightweight, easy-to-use, and efficient C++ library
 *      for processing and rendering 3D data.
 *      Journal of Open Source Software, 6(64), 3255, 2021.
 * ------------------------------------------------------------------
 *
 * Easy3D is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3
 * as published by the Free Software Foundation.
 *
 * Easy3D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 ********************************************************************/

#include <iostream>
#include <algorithm>

#include <easy3d/util/file_system.h>
#include <easy3d/util/progress.h>
#include <easy3d/util/initializer.h>
#include <easy3d/fileio/image_io.h>
#include <easy3d/video/video_encoder.h>

using namespace easy3d;

int main(int argc, char **argv)
{
    // Initialize Easy3D.
    initialize();

#if 0
    if (argc < 3) {
        std::cerr << "usage: \n\t"
                  << argv[0] << "  path_to_images  output_file\n"
                  << "e.g., ./Images2Video data/images data/video.mp4\n\n"
                  << "This program generates a video from a sequence of images.\n"
                  << "The output format is automatically guessed according to the file extension.\n";
        return EXIT_SUCCESS;
    }
    const std::string image_dir = argv[1];
    const std::string output_file = argv[2];
#else
    const std::string image_dir = std::string(DATA_DIR);
	const std::string output_file = image_dir + "/../result.mp4";
#endif
    if (!file_system::is_directory(image_dir)) {
        std::cerr << "Error: image directory doesn't exist: " << image_dir << std::endl;
        return EXIT_FAILURE;
    }
    std::vector<std::string> image_files;
    file_system::get_files(image_dir, image_files, false);
    if (image_files.empty()) {
        std::cerr << "Error: the specified image directory doesn't contain images" << std::endl;
        return EXIT_FAILURE;
    }
    std::sort(image_files.begin(), image_files.end());

    VideoEncoder encoder;
    encoder.start(output_file, 30, 100 * 1024 * 1024);
    for (std::size_t i = 0; i < image_files.size(); ++i) {
        print_progress(static_cast<float>(i + 1) / static_cast<float>(image_files.size()));
        const std::string file_name = image_dir + "/" + image_files[i];
        if (file_system::extension(file_name) != "png") {
            std::cerr << "file is not an image (only png format is supported): " << file_name << std::endl;
            continue;
        }

        std::vector<unsigned char> data;
        int w, h, c;
        if (ImageIO::load(file_name, data, w, h, c, 0, false))
            encoder.encode(data.data(), w, h, c == 3 ? VideoEncoder::PIX_FMT_RGB_888 : VideoEncoder::PIX_FMT_RGBA_8888);
    }
    encoder.end();
    return EXIT_SUCCESS;
}