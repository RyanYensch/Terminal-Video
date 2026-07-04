#include <iostream>
#include <cstdlib>
#include <string>

bool downloaderInstalled() {
    int res = std::system("yt-dlp --version > /dev/null 2>&1");
    return res == 0;
}

int main(int argc, char* argv[]) {
    if (!downloaderInstalled()) {
        std::cerr << "Error: 'yt-dlp' is not installed on your system PATH.\n";
        std::cerr << "Please install it to use this player.\n";
        std::cerr << " Linux: sudo apt install yt-dlp OR pip3 instal yt-dlp.\n";
        std::cerr << " macOS: brew install yt-dlp.\n";
        std::cerr << "Please install it to use this player.\n";
        exit(1);
    }

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " https://www.youtube.com/watch?v=VIDEO_ID\n";
        exit(1);
    }

    std::string command = "yt-dlp -f \"bestvideo[ext=mp4]+bestaudio[ext=m4a]/best[ext=mp4]\" -o video.mp4 " + std::string(argv[1]);

    int res = std::system(command.c_str());

    // Successful
    if (res == 0) {
        std::cout << "Video download as video.mp4\n";
    } else {
        std::cerr << "Error retrieving video\n";
        exit(1);
    }

    return 0;
}
