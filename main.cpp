#include <iostream>
#include <cstdlib>
#include <string>
#include <opencv2/opencv.hpp>

#define PIXEL "■"

bool downloaderInstalled() {
    int res = std::system("yt-dlp --version > /dev/null 2>&1");
    return res == 0;
}

void convertFrame(cv::Mat frame) {
    // Convert to RGB
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

    cv::Mat resizedFrame;
    cv::Size termialSize(100, 40);

    // Resize to the terminal
    cv::resize(frame, resizedFrame, termialSize, 0, 0, cv::INTER_AREA);

    for (int y{0}; y < resizedFrame.rows; ++y) {
        for (int x{0}; x < resizedFrame.cols; ++x) {
            // get the pixel at (y, x)
            cv::Vec3b pixel = resizedFrame.at<cv::Vec3b>(y, x);

            // Get the RGB values
            int r = pixel[0], g = pixel[1], b = pixel[2];

            // Print with ANSI characters
            std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m  ";
        }

        std::cout << "\033[0m\n";
    }
}

int main(int argc, char* argv[]) {
    if (!downloaderInstalled()) {
        std::cerr << "Error: 'yt-dlp' is not installed on your system PATH.\n";
        std::cerr << "Please install it to use this player.\n";
        std::cerr << " Linux: sudo apt install yt-dlp OR pip3 instal yt-dlp.\n";
        std::cerr << " macOS: brew install yt-dlp.\n";
        std::cerr << "Please install it to use this player.\n";
        exit(EXIT_FAILURE);
    }

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " https://www.youtube.com/watch?v=VIDEO_ID\n";
        exit(EXIT_FAILURE);
    }

    std::string command = "yt-dlp -f \"bestvideo[ext=mp4]+bestaudio[ext=m4a]/best[ext=mp4]\" -o video.mp4 " + std::string(argv[1]);

    int res = std::system(command.c_str());

    // Successful
    if (res == 0) {
        std::cout << "Video download as video.mp4\n";
    } else {
        std::cerr << "Error retrieving video\n";
        exit(EXIT_FAILURE);
    }

    cv::VideoCapture cap("video.mp4");

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file.\n";
        exit(EXIT_FAILURE);
    }

    cv::Mat frame;

    while (true) {
        // Get the frame of the video
        cap >> frame;

        // Check if at the end of the video
        if (frame.empty()) {
            std::cout << "End of video.\n";
            break;
        }

        convertFrame(frame);
        break;
    }

    cap.release();
    return 0;
}