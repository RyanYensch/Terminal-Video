#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <csignal>
#include <cstdio>
#include <sys/ioctl.h>
#include <opencv2/opencv.hpp>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

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
    cv::Size maxTerminalSize = getTerminalSize();

    double scaleX = static_cast<double>(maxTerminalSize.width) / frame.cols;
    double scaleY = static_cast<double>(maxTerminalSize.height) / frame.rows;

    double scale = std::min(scaleX, scaleY);

    int newWidth = std::max(1, static_cast<int>(frame.cols * scale));
    int newHeight = std::max(1, static_cast<int>(frame.rows * scale));

    cv::Size newSize(newWidth, newHeight);

    // Resize and convert to RGB
    cv::resize(frame, resizedFrame, newSize, 0, 0, cv::INTER_AREA);
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

// Handle user force closing
void handleInterrupt(int signum) {
    std::cout << "\033[?1049l\033[?25h\033[0m\n";
    std::cout << "Video interuppted.\n";

    std::remove("video.mp4");
    std::remove("audio.wav");

    exit(signum);
}

int main(int argc, char* argv[]) {
    std::signal(SIGINT, handleInterrupt);

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

    std::string videoCommand = "yt-dlp --force-overwrites -f \"bestvideo[ext=mp4]/best[ext=mp4]\" -o video.mp4 " + std::string(argv[1]);
    std::string audioCommand = "yt-dlp --force-overwrites -f \"bestaudio\" --extract-audio --audio-format wav -o audio.wav " + std::string(argv[1]);

    std::cout << "Downloading Video Track...\n";
    int resVideo = std::system(videoCommand.c_str());


    std::cout << "Downloading Audio Track.\n";
    int resAudio = std::system(audioCommand.c_str());

    // Successful
    if (resVideo == 0 && resAudio == 0) {
        std::cout << "Video download as video.mp4 and audio.wav\n";
    } else {
        std::cerr << "Error retrieving video or audio\n";
        exit(EXIT_FAILURE);
    }

    cv::VideoCapture cap("video.mp4");

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file.\n";
        exit(EXIT_FAILURE);
    }

    ma_engine engine;
    if(ma_engine_init(NULL, &engine) != MA_SUCCESS) {
        std::cerr << "Error: Failed to initialise audio engine.\n";
        exit(EXIT_FAILURE);
    }

    // Clear the screen before video
    std::cout << "\033[?1049h\033[?25l";

    cv::Mat frame;
    int currentFrame = 0;

    ma_engine_play_sound(&engine, "audio.wav", NULL);

    // Get the fps
    double fps = cap.get(cv::CAP_PROP_FPS);
    if (fps <= 0) fps = 30.0;

    while (true) {
        // Get the frame of the video
        cap >> frame;

        // Check if at the end of the video
        if (frame.empty()) {
            std::cout << "End of video.\n";
            break;
        }

        double expectedVideoTimeMs = currentFrame * (1000.0 / fps);
        double currentAudioTimeMs = static_cast<double>(ma_engine_get_time_in_milliseconds(&engine));

        // Video more than 50ms behind it is lag
        if (expectedVideoTimeMs < currentAudioTimeMs - 50.0) {
            // skip the frame to catch up
            currentFrame++;
            continue;
        }

        // Video is ahead of audio, delay to match
        if (expectedVideoTimeMs > currentAudioTimeMs) {
            int sleepTime = static_cast<int>(expectedVideoTimeMs - currentAudioTimeMs);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        }

        // render the frame
        convertFrame(frame);

        currentFrame++;
    }

    // restore terminal
    std::cout << "\033[?1049l\033[?25h";

    // shut down the audio engine
    ma_engine_uninit(&engine);

    // Free the capture
    cap.release();

    // Remove video file
    std::remove("video.mp4");
    std::remove("audio.wav");

    return 0;
}