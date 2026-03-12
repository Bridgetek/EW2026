# Lobby Entry demo
---

## 📋 Overview

**Lobby Entry** is a comprehensive demonstration application for the Bridgetek VM820C board, showcasing a complete multimedia communication platform. The application features a multi-page GUI framework with QR code scanning, and dialing and calling interfaces. It demonstrates advanced multimedia capabilities including real-time video streaming, camera integration, gesture-based touch interaction, and graphics rendering on embedded systems.

---

## ✨ Features & Application Pages

### 1. Main Menu Page
Central hub for accessing all application features:
- **QR Code Scanner**: Direct link to scan and dial QR codes
- **Directory list**: Browse the Directory list
- **Dialer**: Manual number entry for outgoing calls
- **Help Section**: In-app guide and feature documentation
- Modern grid-based layout with large touch targets

### 2. Dialer Page
Full-featured numeric input interface:
- Large digit buttons (0-9) for easy touch input
- Real-time digit display with visual feedback
- Direct call initiation

### 3. Directory Page
- Pre-loaded Directory list
- Touch-based contact selection
- Office details display

### 4. QR Code Scanner Page
A barcode detection system:
- Real-time camera feed display
- JPEG-based image capture 
- QR code detection using QUIRC library
- Camera preview with capture indicators

### 5. Help/Information Page
- System information display

### 6. Video Call Page
- Local camera (Real-time JPEG video streaming)
- Remote LVDS video input (800x600)
- Picture-in-picture (PiP) mode with flexible positioning
- Draggable PiP window for custom layout
- Video swap functionality (primary/secondary exchange)
- Zoom In/Out with 10% increment steps
- Fullscreen mode for single video focus

---

## 🔧 Technical Specifications

### Hardware Requirements
| Component | Specification |
|-----------|---------------|
| MCU | STM32H743 |
| Camera| OV5640 |
| Display | 1920x1200 |
| Graphics | VM820C |
| Storage | MicroSD card (FAT32, 128MB-32GB) |

### Software Stack
| Component | Details |
|-----------|---------|
| OS | Bare-metal (no RTOS) |
| HAL | STM32H7xx HAL |
| Build Tool | STM32CubeIDE v2.0.0 |

---

## 🚀 Getting Started

### Quick Start
1. **Build**: Use STM32CubeIDE to compile the project
2. **Flash**: Upload firmware to VM820C board via ST-LINK
3. **Prepare SD Card**: Copy `assets_sdcard_eve/` folder to SD card root
4. **Run**: Insert SD card, power on board, and interact with touch interface

### Project Structure
```
LobbyEntry/
├── Core/                      # STM32 HAL initialization
├── LobbyEntry/                # Application source code
│   ├── app_*.c                # Application implementations
│   └── utils_*.c/h            # Utility modules
├── eve_bt820_stm32/           # EVE graphics library
├── assets_sdcard_eve/         # SD card assets
├── README.txt                 # User guide
```

---

### Build Instructions

STEP 1: Open Project in STM32CubeIDE
-------------------------------------
1. Launch STM32CubeIDE
2. Select File -> Open Projects from File System
3. Navigate to this project directory (LobbyEntry)
4. Click "Finish" to import the project

STEP 2: Build the Project
--------------------------
1. In the Project Explorer, right-click on "LobbyEntry" project
2. Select "Build Project" from the context menu
3. Wait for the build process to complete
   - Build output will be shown in the Console view
   - The compiled binary will be located in:
     Debug/LobbyEntry.elf (for Debug configuration)
     Release/LobbyEntry.elf (for Release configuration)

Build Configurations:
---------------------
- Debug: Includes debug symbols, optimized for debugging (-Og)
- Release: Optimized for size/speed, recommended for production

To switch build configuration:
1. Right-click on project -> Build Configurations -> Set Active
2. Select either "Debug" or "Release"

STEP 3: Flash the Firmware to VM820C Board
------------------------------------------
Using STM32CubeIDE:
1. Connect VM820C board to PC via USB (ST-LINK connection)
2. Power on the VM820C board
3. In STM32CubeIDE, click Run -> Debug As -> STM32 C/C++ Application
   OR
   Click the Debug button (bug icon) in the toolbar
4. The firmware will be automatically flashed and the debugger will start


---

## 📄 License & Copyright

**License**: MIT License  
