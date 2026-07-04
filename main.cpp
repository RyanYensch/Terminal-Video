#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <sys/ioctl.h>
#include <opencv2/opencv.hpp>

bool downloaderInstalled() {
    int res = std::system("yt-dlp --version > /dev/null 2>&1");
    return res == 0;
}

cv::Size getTerminalSize() {
    struct winsize w;

    // If it failed return a default size
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        return cv::Size(100, 40);
    }

    int pixelWidth = w.ws_col / 2; //each pixel is 2 wide
    int pixelHeight = w.ws_row;

    return cv::Size(pixelWidth, pixelHeight);
}

void convertFrame(cv::Mat frame) {

    cv::Mat resizedFrame;
    cv::Size terminalSize = getTerminalSize();

    // Resize and convert to RGB
    cv::resize(frame, resizedFrame, terminalSize, 0, 0, cv::INTER_AREA);
    cv::cvtColor(resizedFrame, resizedFrame, cv::COLOR_BGR2RGB);

    // frame buffer for cout efficiency and move cursor
    std::string frameBuffer = "\033[H";
    frameBuffer.reserve(resizedFrame.rows * resizedFrame.cols * 21 + resizedFrame.rows * 5 + 3);

    for (int y{0}; y < resizedFrame.rows; ++y) {
        for (int x{0}; x < resizedFrame.cols; ++x) {
            // get the pixel at (y, x)
            cv::Vec3b pixel = resizedFrame.at<cv::Vec3b>(y, x);

            // Get the RGB values
            int r = pixel[0], g = pixel[1], b = pixel[2];

            // Add rgb character to the frame buffer
            frameBuffer += "\033[48;2;" +
                            std::to_string(r) + ";" +
                            std::to_string(g) + ";" +
                            std::to_string(b) + "m  ";
        }

        // Add newline and reset colour
        frameBuffer += "\033[0m\n";
    }

    // Print the frame to terminal
    std::cout << frameBuffer;
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

    // Clear the screen before video
    std::cout << "\033[?1049h\033[?25l";

    while (true) {
        // Get the frame of the video
        cap >> frame;

        // Check if at the end of the video
        if (frame.empty()) {
            std::cout << "End of video.\n";
            break;
        }

        convertFrame(frame);
    }

    // restore terminal
    std::cout << "\033[?1049l\033[?25h";

    // Free the capture
    cap.release();
    return 0;
}