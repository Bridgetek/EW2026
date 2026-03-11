================================================================================
                         LobbyEntry EW2026 Demo
             Video Call Application for BT820 Evaluation Board
================================================================================

OVERVIEW
--------
This is a video call demonstration application designed for the Bridgetek 
BT820 evaluation board with STM32H743 microcontroller. The application 
features a modern UI with video streaming, camera control, zoom functionality,
and touch-based interactions.

HARDWARE REQUIREMENTS
---------------------
- BT820 Evaluation Board (with STM32H743VIT6 MCU)
- MicroSD card (formatted as FAT32)
- Camera module connected to DCMI interface
- LVDS display connection 
- Power supply (USB or external 5V)
- SD card reader (for preparing the SD card)

SOFTWARE REQUIREMENTS
---------------------
- STM32CubeIDE 
- STM32CubeProgrammer (optional, for standalone flashing)

================================================================================
BUILD INSTRUCTIONS
================================================================================

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
   OR
   Use the toolbar button (hammer icon)
   OR
   Press Ctrl+B (Windows/Linux) or Cmd+B (Mac)

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

STEP 3: Flash the Firmware to BT820 Board
------------------------------------------
Using STM32CubeIDE:
1. Connect BT820 board to PC via USB (ST-LINK connection)
2. Power on the BT820 board
3. In STM32CubeIDE, click Run -> Debug As -> STM32 C/C++ Application
   OR
   Click the Debug button (bug icon) in the toolbar
4. The firmware will be automatically flashed and the debugger will start

================================================================================
RUN INSTRUCTIONS
================================================================================

STEP 1: Prepare the SD Card
----------------------------
1. Format a microSD card as FAT32
   - Card capacity: 128MB to 32GB recommended
   - Use Windows Disk Management, macOS Disk Utility, or Linux fdisk/gparted

2. Copy the assets folder to the SD card:
   - Source: assets_sdcard_eve
   - Destination: Root of the SD card
   
   Your SD card structure should look like:
   
   [SD Card Root]/
   └── Lobby-entry-ew2026/
       ├── avi/
       ├── font/
       └── img/
           ├── 00_background_*.raw
           ├── 01_digit_0_*.raw
           ├── 02_digit_1_*.raw
           └── ... (all image assets)

3. Safely eject the SD card from your computer

STEP 2: Install SD Card into BT820 Board
-----------------------------------------
1. Power off the BT820 board (disconnect power)
2. Locate the microSD card slot on the BT820 board
3. Insert the prepared microSD card into the slot
   - Ensure the card is fully inserted with a click
   - The label side should face up (check board markings)
4. Verify the card is securely seated

STEP 3: Power On and Run
-------------------------
1. Connect the LVDS display cable (if not already connected)
2. Ensure the camera module is properly connected to the DCMI port
3. Connect power supply to the BT820 board:
   - Via USB cable (USB-C or micro-USB depending on board revision)
   OR
   - Via external 5V power supply to the DC jack

4. Power on the board:
   - The board should boot within 2-3 seconds
   - The display should show the main menu interface
   - Touch the screen to navigate through the application

STEP 4: Using the Application
------------------------------
Main Menu:
- QR Code Scanner: Scan QR codes for quick dialing
- Directory: Browse contact list
- Dialer: Manual phone number input
- Help: Application guide

Video Call Page:
- Touch contact or dial number to initiate a call
- During call:
  - Zoom In/Out: Adjust video zoom level
  - Swap: Switch between local and remote video
  - Camera On/Off: Toggle local camera
  - Reset: Reset zoom to default
  - Fullscreen: Toggle fullscreen mode
  - Hangup: End the call

================================================================================
PROJECT STRUCTURE
================================================================================

LobbyEntry/
├── Core/                      # STM32 HAL and main initialization
│   ├── Inc/                   # Header files (DCMI, GPIO, I2C, etc.)
│   └── Src/                   # Implementation files
├── LobbyEntry/                # Application source code
│   ├── app_page_*.c           # UI page implementations
│   ├── app_assets.c/h         # Asset definitions and loading
│   ├── app_camera_control.c   # Camera interface
│   ├── app_hal_events.c       # HAL callbacks
│   ├── utils_*.c/h            # Utility modules (image, gesture, etc.)
│   └── assets/                # Embedded assets (if any)
├── eve_bt820_stm32/           # EVE graphics library for BT820
├── Drivers/                   # STM32 HAL drivers
├── Utilities/                 # JPEG utilities
├── quirc_lib/                 # QR code decoding library
├── assets_sdcard_eve/         # SD card assets (copy to SD card)
├── Debug/                     # Debug build output
├── Release/                   # Release build output
└── README.txt                 # This file
