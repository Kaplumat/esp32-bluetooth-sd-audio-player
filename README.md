# esp32-bluetooth-sd-audio-player
ESP32 Bluetooth A2DP audio player using SD card and double buffering
Play music from an SD card directly to your Bluetooth headphones using an ESP32, without needing a phone!

## Features

- 🎵 Play MP3 and WAV files from SD card
- 🎧 Stream audio to Bluetooth headphones via A2DP
- 📱 No phone required - standalone operation
- 🔌 Easy to use with PlatformIO

## Hardware Requirements

- **ESP32 Development Board** (ESP32-WROOM-32 or similar)
- **MicroSD Card Module** (SPI interface)
- **MicroSD Card** (formatted as FAT32)
- **Bluetooth Headphones** (A2DP compatible)
- **Jumper wires** for connections

## Wiring Diagram

### SD Card Module to ESP32

| SD Card Module | ESP32 Pin | Notes |
|---------------|-----------|-------|
| CS (Chip Select) | GPIO 5 | Configurable in code |
| MOSI | GPIO 23 | SPI Master Out |
| MISO | GPIO 19 | SPI Master In |
| SCK (Clock) | GPIO 18 | SPI Clock |
| VCC | 3.3V or 5V | Check your module |
| GND | GND | Ground |

**Note:** Some ESP32 boards may use different pins. If the default pins don't work, try:
- CS: GPIO 4
- MOSI: GPIO 15
- MISO: GPIO 2
- SCK: GPIO 14

You can modify the pin definitions in `src/main.cpp` if needed.

## Software Setup

### Prerequisites

1. **Install PlatformIO** (VS Code extension or standalone)
2. **Install Python** (required by PlatformIO)

### Installation Steps

1. **Clone or download this project**

2. **Open in PlatformIO**
   - Open VS Code
   - Open the project folder
   - PlatformIO should automatically detect the project

3. **Connect your ESP32**
   - Connect ESP32 to your computer via USB
   - Note the COM port (Windows) or /dev/tty* (Linux/Mac)

4. **Upload the code**
   - Click the "Upload" button in PlatformIO
   - Or run: `pio run --target upload`

5. **Monitor Serial Output**
   - Click "Monitor" in PlatformIO
   - Or run: `pio device monitor`
   - Set baud rate to 115200

## Preparing Audio Files

### Supported Formats
- **WAV** (PCM, 16-bit, 44.1kHz recommended)
- **MP3** (most common formats)

### File Preparation

1. **Format SD Card**
   - Format SD card as **FAT32**
   - Use SD card formatter tool if needed

2. **Copy Audio Files**
   - Copy your `.mp3` or `.wav` files to the root of the SD card
   - Keep filenames simple (avoid special characters)
   - Example: `music.mp3`, `song1.wav`, etc.

3. **Insert SD Card**
   - Insert SD card into the SD card module
   - Make sure it's properly seated

### Converting Audio Files

If you need to convert audio files to WAV format:

**Using FFmpeg (recommended):**
```bash
ffmpeg -i input.mp3 -ar 44100 -ac 2 -f wav output.wav
```

**Using Audacity:**
- Open file → Export → Export as WAV
- Choose: 44.1kHz, 16-bit, Stereo

## Usage

1. **Power on ESP32**
   - Connect ESP32 to power (USB or external power supply)

2. **Wait for Initialization**
   - ESP32 will initialize SD card
   - It will list all audio files found
   - Bluetooth will start with name "ESP32-AudioPlayer"

3. **Pair Bluetooth Headphones**
   - Put your headphones in pairing mode
   - Look for "ESP32-AudioPlayer" in your device list
   - Connect to it

4. **Playback**
   - The device will automatically play the first audio file found after 3 seconds
   - Audio will stream to your connected Bluetooth headphones

## Serial Monitor Commands

The Serial Monitor (115200 baud) will show:
- SD card initialization status
- List of audio files found
- Bluetooth connection status
- Playback status

## Troubleshooting

### SD Card Not Detected

**Symptoms:** "SD card initialization failed!"

**Solutions:**
1. Check wiring connections
2. Verify SD card is formatted as FAT32
3. Try a different SD card (some cards are incompatible)
4. Check if SD card module requires 5V (some modules need 5V, not 3.3V)
5. Try different SPI pins (see alternative pinout in code)

### Bluetooth Not Connecting

**Symptoms:** Headphones can't find or connect to ESP32

**Solutions:**
1. Make sure headphones support A2DP (most do)
2. Try resetting ESP32
3. Put headphones in pairing mode before powering ESP32
4. Check Serial Monitor for connection status messages

### No Audio / Distorted Audio

**Symptoms:** Connected but no sound or poor quality

**Solutions:**
1. Check audio file format (use 44.1kHz, 16-bit, stereo WAV for best compatibility)
2. Try a different audio file
3. Check if file is corrupted
4. Ensure Bluetooth connection is stable (keep devices close)

### Playback Stops or Stutters

**Symptoms:** Audio cuts out or stutters

**Solutions:**
1. Use a faster SD card (Class 10 recommended)
2. Reduce audio file bitrate
3. Keep ESP32 and headphones close together
4. Check for interference from other devices

### Out of Memory Errors

**Symptoms:** Crashes or "out of memory" messages

**Solutions:**
1. Use smaller audio files
2. Convert MP3 files to lower bitrate
3. Use WAV files with lower sample rate (e.g., 22kHz instead of 44.1kHz)

## Customization

### Change Auto-Play Behavior

In `src/main.cpp`, modify the `loop()` function to change when files start playing:

```cpp
// Remove auto-play entirely
// Or change the delay: millis() - start_time > 5000  // 5 seconds
```

### Play Specific Files

You can modify the code to play specific files or implement a playlist system.

### Change Bluetooth Name

In `setup()`, change:
```cpp
a2dp_source.start("ESP32-AudioPlayer", get_data_channels);
```
to:
```cpp
a2dp_source.start("YourCustomName", get_data_channels);
```

## Technical Details

- **Audio Format:** PCM, 16-bit, Stereo, 44.1kHz (standard for A2DP)
- **Bluetooth Profile:** A2DP (Advanced Audio Distribution Profile)
- **SD Card Interface:** SPI
- **File System:** FAT32

## Limitations

- Audio files must be in supported formats (MP3, WAV)
- SD card must be FAT32 formatted
- Maximum file size depends on available memory
- Some very high bitrate files may cause stuttering
- Only one Bluetooth connection at a time

## License

This project is provided as-is for educational and personal use.

## Credits

Uses the following libraries:
- [ESP32-A2DP](https://github.com/pschatzmann/ESP32-A2DP) by pschatzmann
- [ESP8266Audio](https://github.com/earlephilhower/ESP8266Audio) by earlephilhower

## Support

For issues and questions:
1. Check the Troubleshooting section above
2. Review Serial Monitor output for error messages
3. Verify all connections and SD card format

Enjoy your standalone Bluetooth audio player! 🎵

