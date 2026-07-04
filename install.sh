#!/bin/bash

echo "Starting Terminal-Video installation..."

# macOS
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "macOS detected. Installing via Homebrew..."
    brew install yt-dlp opencv nodejs
    echo "Dependencies installed!"

# Linux
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Detect specific distro
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        if [[ "$ID" == "fedora" ]]; then
            echo "Fedora detected. Installing via dnf..."
            sudo dnf install -y yt-dlp opencv opencv-devel nodejs
            echo "Dependencies installed!"
        elif [[ "$ID" == "ubuntu" || "$ID" == "debian" ]]; then
            echo "Ubuntu/Debian detected. Installing via apt..."
            sudo apt update
            sudo apt install -y yt-dlp libopencv-dev nodejs
            echo "Dependencies installed!"
        else
            echo "Unsupported Linux Distro. Please install yt-dlp, OpenCV, and NodeJS manually."
        fi
    fi
else
    echo "Unsupported OS. Please install dependencies manually."
fi

# Compile the program
echo "Compiling the player"
make clean
make

echo "Installation complete! Run ./player <youtube_url> to start."
