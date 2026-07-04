#include <iostream>
#include <cstdlib>
#include <string>
#include <opencv2/opencv.hpp>

bool downloaderInstalled() {
    int res = std::system("yt-dlp --version > /dev/null 2>&1");
    return res == 0;
}

void convertFrame(cv::Mat frame) {
    //TODO: convert frame into unicode
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

        std::cout << frame;
        break;
    }

    cap.release();
    return 0;
}