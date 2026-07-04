# Terminal Video

Watch youtube videos within your terminal.

To use you must:
- [Install Dependencies and Compile](#how-to-install)
- [Run The Player](#how-to-run)

## How To Install

### Quick Install Script

This does exactly what the [Manual Installation](#manual-installation) is doing, but in a single command.

Run the command:
```BASH
./install.sh
```
[File Located Here](install.sh)

### Manual Installation
#### Dependencies
You must install the following packages
- yt-dlp
- OpenCV
- NodeJS

You might find the install commands in the [installation script](install.sh).

#### Compilation

From the folder directory run the commands:
```bash
make clean
make
```

## How To Run

After all the dependencies have been installed, run the following command to watch any YouTube video:

```bash
./player <youtube_url>
```
