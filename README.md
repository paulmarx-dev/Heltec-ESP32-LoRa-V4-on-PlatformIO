# PlatformIO Support for Heltec ESP32 LoRaWAN V4

A complete guide to add Heltec ESP32 LoRaWAN V4 board support to PlatformIO for Visual Studio Code on macOS.

---

## Overview

### About PlatformIO

[PlatformIO](https://platformio.org/) is a powerful extension for Visual Studio Code that makes embedded systems development straightforward and enjoyable. It provides features like IntelliSense, code completion, integrated debugging, and seamless GitHub Copilot support—all within your favorite editor.

### About Heltec ESP32 LoRaWAN V4

The [Heltec ESP32 LoRaWAN V4](https://wiki.heltec.org/docs/devices/open-source-hardware/esp32-series/lora-32/wifi-lora-32-v4/) is one of the most advanced ESP32-based boards available. It's ideal for IoT projects, long-range LoRaWAN communication, GNSS tracking, and much more.

### The Problem

As of February 2026, PlatformIO lacks official support for the Heltec ESP32 LoRaWAN V4. This means that Heltec's provided examples won't compile in PlatformIO, resulting in errors for undeclared macros and linkage issues. This makes PlatformIO essentially unusable for V4 development—until now.

### The Solution

This guide walks you through a straightforward process to enable full PlatformIO support for the Heltec ESP32 V4. Once completed, you'll be able to compile, upload, and debug Heltec V4 projects with all the benefits of PlatformIO's development environment.

---

## Prerequisites

Before you begin, ensure you have:
- **Visual Studio Code** installed
- **PlatformIO extension** for VS Code
- **Arduino IDE** installed
- **Heltec V4 board libraries** installed (see instructions below)

---

## Step-by-Step Setup

### Step 1: Install Visual Studio Code and PlatformIO Extension

If you haven't already:

1. Download and install **Visual Studio Code** from [code.visualstudio.com](https://code.visualstudio.com/)
2. Open VS Code and navigate to the Extensions marketplace (Ctrl+Shift+X / Cmd+Shift+X)
3. Search for **"PlatformIO IDE"** and click **Install**

Alternatively, follow the [official PlatformIO installation guide](https://platformio.org/install/ide?install=vscode).

### Step 2: Install USB Driver

The Heltec ESP32 V4 board uses a CP2102N USB-to-UART bridge chip. Your computer needs the proper drivers to recognize and communicate with the board over USB.

1. Visit the [Silicon Labs USB-to-UART Bridge Driver downloads page](https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads)
2. Download the driver for your operating system (macOS, Windows, or Linux)
3. Install the driver following the provided instructions
4. Restart your computer after installation

**Note:** Without this driver, your board won't appear in the device list when you try to upload, and you'll see "Port not found" errors.

### Step 3: Install Arduino IDE

1. Download the latest **Arduino IDE** from [arduino.cc/en/software](https://www.arduino.cc/en/software/)
2. Install it following their standard installation process
3. Note the installation location (you'll need it later)

### Step 4: Install Heltec V4 Board Libraries

Follow the official Heltec installation guide for your operating system:

1. Visit the [Heltec WiFi Kit Installation Guide](https://github.com/Heltec-Aaron-Lee/WiFi_Kit_series/tree/master/InstallGuide)
2. Download and follow the instructions for your OS (macOS, Windows, or Linux)
3. This installs the core board support package for the Heltec ESP32 V4

### Step 5: Install Heltec ESP32 Libraries (Optional but Recommended)

For advanced examples and peripheral support (LoRa, OLED display, GPS/GNSS, etc.):

1. Visit the [Heltec_ESP32 GitHub repository](https://github.com/HelTecAutomation/Heltec_ESP32)
2. Follow the README instructions to install the Heltec ESP32 library
3. **Note:** Many Heltec examples require pin configuration adjustments for the V4 board. See the troubleshooting section below.

---

## Configuring PlatformIO for Heltec V4

The following steps enable PlatformIO to recognize and support the Heltec V4 board.

### Step 6: Copy Board Configuration File

1. Locate your PlatformIO installation directory. On macOS, it's typically:
   ```
   ~/.platformio/platforms/espressif32/boards/
   ```

2. Download the board configuration file: [`heltec_wifi_lora_32_V4.json`](https://github.com/paulmarx-dev/Heltec-ESP32-LoRa-V4-on-PlatformIO/blob/main/heltec_wifi_lora_32_V4.json)

3. Copy this file into the `boards/` directory

### Step 7: Add Pin Configuration

1. Navigate to the variants directory:
   ```
   ~/.platformio/packages/framework-arduinoespressif32/variants/
   ```

2. Create a new folder named `heltec_wifi_lora_32_V4`:
   ```bash
   mkdir heltec_wifi_lora_32_V4
   ```

3. Download the pin configuration file: [`pins_arduino.h`](https://github.com/paulmarx-dev/Heltec-ESP32-LoRa-V4-on-PlatformIO/blob/main/pins_arduino.h)

4. Place `pins_arduino.h` inside the newly created `heltec_wifi_lora_32_V4/` folder

### Step 8: Restart Visual Studio Code

Close and reopen VS Code completely. PlatformIO will now recognize the Heltec ESP32 V4 board.

---

## Creating Your First Project

1. In VS Code, open the **PlatformIO Home** (click the PlatformIO icon in the sidebar)
2. Click **"Create New Project"**
3. Configure your project:
   - **Project Name:** Enter your project name
   - **Board:** Search for and select **"Heltec WiFi LoRa 32 V4"**
   - **Framework:** Select **"Arduino"**
   - **Location:** Choose your preferred directory
4. Click **Create Project**

Your project is now ready to code and compile!

---

## Configuring Arduino Library Paths

If you're using Heltec examples or libraries that depend on Arduino libraries:

1. Open your project's `platformio.ini` file
2. Add the Arduino libraries path:
   ```ini
   [env:heltec_wifi_lora_32_V4]
   platform = espressif32
   board = heltec_wifi_lora_32_V4
   framework = arduino
   lib_extra_dirs = /Users/YOUR_USERNAME/Documents/Arduino/libraries
   ```

3. Replace `YOUR_USERNAME` with your actual macOS username

**Note:** You'll need to add this line to every new PlatformIO project. The configuration files from Steps 6-7 only need to be set up once.

---

## Building and Uploading

In addition to the IDE buttons and commands, you can use the following bash commands to compile and upload your projects with PaltformIO.

### Build Your Project
```bash
platformio run
```

### Upload to Your Board
```bash
platformio run --target upload
```

### Open Serial Monitor
```bash
platformio device monitor
```



---

## Troubleshooting

### Upload Fails with "Port Busy" Error

**Symptom:** You see an error like `could not open port /dev/cu.usbmodem1101: Resource busy`

**Solution:**
1. Close any open Serial Monitors (even in other IDEs)
2. Disconnect and reconnect the USB cable
3. Retry the upload

### Still Having Issues?

If the above doesn't work, try **manual upload mode**:

1. **Press and hold** the USR/PROG button on the board
2. **Short press** the RST button (while holding USR/PROG)
3. **Release** the USR/PROG button
4. Run the upload command again

### Heltec Examples Don't Compile

Make sure you've completed **Steps 4-5** and added the `lib_extra_dirs` path to your `platformio.ini` (see the configuration section above).

### Examples Work But Pin Definitions Are Wrong

The Heltec V4 has different pin assignments than previous ESP32 models. If you're adapting Heltec examples:

1. Refer to the official [Heltec V4 pin documentation](https://wiki.heltec.org/docs/devices/open-source-hardware/esp32-series/lora-32/wifi-lora-32-v4/)
2. Update the pin definitions in your code accordingly
3. Common peripherals that may need adjustment:
   - OLED Display pins
   - LoRa SPI pins
   - GPS/GNSS serial pins
   - Button pins

---

## Example Projects

This repository includes working examples adapted for the Heltec V4 board. Check the `Examples/` folder for reference implementations that correctly configure pins for GPS, LoRa, and OLED usage.

---

## What's Next?

With PlatformIO now fully supporting your Heltec V4 board, you can:

✅ Use **IntelliSense** for intelligent code completion  
✅ Leverage **GitHub Copilot** for AI-assisted development  
✅ Enjoy integrated **debugging** and **serial monitoring**  
✅ Manage **libraries** and **dependencies** effortlessly  
✅ Compile and upload with a single command  

---

## Additional Resources

- **Heltec V4 Pin Documentation:** https://wiki.heltec.org/docs/devices/open-source-hardware/esp32-series/lora-32/wifi-lora-32-v4/
- **PlatformIO Documentation:** https://docs.platformio.org/
- **Heltec_ESP32 Library:** https://github.com/HelTecAutomation/Heltec_ESP32
- **Arduino IDE:** https://www.arduino.cc/en/software/

---

## Support

If you encounter issues or have suggestions for improvement, feel free to:
- Review the troubleshooting section above
- Check the linked documentation
- Open an issue on this repository

---

## License

This guide and configuration files are provided as-is for community use. Please respect the original licenses of PlatformIO, Arduino, and Heltec software.

---

**Happy coding with your Heltec V4 board!** 🚀
