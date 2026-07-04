#!/bin/bash

# ANSI COLOUR CODES
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'
RESET='\033[0m'


echo -e "${CYAN}Starting Terminal-Video installation...${RESET}"

# macOS
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo -e "${YELLOW}macOS detected. Installing via Homebrew...${RESET}"
    brew install yt-dlp opencv nodejs
    echo -e "${GREEN}Dependencies installed!${RESET}"

# Linux
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Detect specific distro
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        if [[ "$ID" == "fedora" ]]; then
            echo -e "${YELLOW}Fedora detected. Installing via dnf...${RESET}"
            sudo dnf install -y yt-dlp opencv opencv-devel nodejs
            echo -e "${GREEN}Dependencies installed!${RESET}"
        elif [[ "$ID" == "ubuntu" || "$ID" == "debian" ]]; then
            echo -e "${YELLOW}Ubuntu/Debian detected. Installing via apt...${RESET}"
            sudo apt update
            sudo apt install -y yt-dlp libopencv-dev nodejs
            echo -e "${GREEN}Dependencies installed!${RESET}"
        else
            echo -e "${RED}Unsupported Linux Distro. Please install yt-dlp, OpenCV, and NodeJS manually.${RESET}"
        fi
    fi
else
    echo -e "${RED}Unsupported OS. Please install dependencies manually.${RESET}"
fi

# Compile the program
echo -e "${CYAN}Compiling the player${RESET}"
make clean
make

echo -e "${GREEN}Installation complete! Run ${CYAN}./player <youtube_url>${GREEN} to start.${RESET}"
