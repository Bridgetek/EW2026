# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT EXISTS ${VSCODE_CWD})
        if (NOT DEFINED ENV{PICO_SDK_PATH})
                message(FATAL_ERROR "PICO_SDK_PATH environment variable is not set")
        endif()
endif()

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
if (EXISTS ${VSCODE_CWD})
        include(pico_sdk_import.cmake)
else ()
        include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
endif ()

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

# Set the location of the EVE-MCU-Dev tree
set(API_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/eve_library/")

# Set name of project (as current working directory)
get_filename_component(ProjectName ${CMAKE_CURRENT_LIST_DIR} NAME)
string(REPLACE " " "_" ProjectName ${ProjectName})
project(${ProjectName} C CXX ASM)

message(API_DIRECTORY="${API_DIRECTORY}")
message(PROJECT_NAME="${PROJECT_SOURCE_DIR}")
message(CMAKE_C_COMPILER_ID="${CMAKE_C_COMPILER_ID}")
message(CMAKE_SYSTEM_NAME="${CMAKE_SYSTEM_NAME}")

# Setup command line options for build - feeds into EVE_config.h
option(MODULE_TYPE "EVE Module Name" )
IF(MODULE_TYPE)
    ADD_DEFINITIONS(-DMODULE_TYPE=${MODULE_TYPE})
ENDIF(MODULE_TYPE)
option(FT8XX_TYPE "EVE Device Type Name" )
IF(FT8XX_TYPE)
    ADD_DEFINITIONS(-DFT8XX_TYPE=${FT8XX_TYPE})
ENDIF(FT8XX_TYPE)
option(PANEL_TYPE "EVE Panel Type" )
IF(PANEL_TYPE)
    ADD_DEFINITIONS(-DPANEL_TYPE=${PANEL_TYPE})
ENDIF(PANEL_TYPE)
option(DISPLAY_RES "Panel Display Resolution" )
IF(DISPLAY_RES)
    ADD_DEFINITIONS(-DDISPLAY_RES=${DISPLAY_RES})
ENDIF(DISPLAY_RES)

# Add environment for example code
set(EXAMPLE_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/example")
set(EXAMPLE_SNIPPETS "${CMAKE_CURRENT_SOURCE_DIR}/eve_library/examples/snippets")

# Point to where to find the platform source file
add_executable(${PROJECT_NAME}
        main.c
	sensor/SparkFun_Bio_Sensor_Hub_Library.c
)

pico_set_program_name(${PROJECT_NAME} "PulseOximeter")
pico_set_program_version(${PROJECT_NAME} "0.1")
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/sensor
        ${EXAMPLE_SOURCE}
        ${EXAMPLE_SNIPPETS}/maths
)

# Tell EVE library to use RP2040 as base class
add_compile_definitions(PLATFORM_RP2040)
add_compile_definitions(USE_TRIG_TABLE) #use look up table instead of maths funcitons for trig
# Enable debug logging output to stdio
add_compile_definitions(DEBUG_LEVEL=0)
# Milliseconds to wait for stdio connection
# (will also have stdio_usb_connected() call in main to wait for connection)
add_compile_definitions(PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS=0)

# I2C Pins for CN2 connector on IDM2040-43A SCL=GPIO_21, SDO=GPIO_20
add_compile_definitions(PICO_DEFAULT_I2C_SCL_PIN=21)
add_compile_definitions(PICO_DEFAULT_I2C_SDA_PIN=20)
# I2C Bus used by SparkFun Bio Sensor Hub 
add_compile_definitions(SPARKFUN_BIO_SENSOR_HUB_I2C_BUS=i2c0)
# I2C Address for SparkFun Bio Sensor Hub 7-bit
add_compile_definitions(SPARKFUN_BIO_SENSOR_HUB_ADDRESS=0x55)
# Reset pin for SparkFun Bio Sensor Hub CN14 pin 1 GPIO_0
add_compile_definitions(SPARKFUN_BIO_SENSOR_HUB_RESET_PIN=0)
# MFIO pin for SparkFun Bio Sensor Hub CN14 pin 3 GPIO_1
add_compile_definitions(SPARKFUN_BIO_SENSOR_HUB_MFIO_PIN=1)
# I2C Baud rate
add_compile_definitions(SPARKFUN_BIO_SENSOR_HUB_BAUD_RATE=100000)

# Modify the below lines to enable/disable output over USB/UART
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 0)

# Source code for EVE library (targetted at RP2040)
add_library(eve_library # EVE library name
        # Source code for EVE library
        ${API_DIRECTORY}/source/EVE_API.c
        ${API_DIRECTORY}/source/EVE_HAL.c
        ${API_DIRECTORY}/ports/eve_arch_rpi/EVE_MCU_RP2040.c
)
# Header files used for EVE library
target_include_directories(eve_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include
)

# Source for example code
add_library(eve_example # Example code library name
        # Source files for example code library
        ${EXAMPLE_SOURCE}/eve_example.c
        ${EXAMPLE_SOURCE}/eve_assets.c
        ${EXAMPLE_SNIPPETS}/touch.c
        ${EXAMPLE_SNIPPETS}/maths/trig_furman.c
)

# Header files used for example code library
target_include_directories(eve_example PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include # Headers for EVE library
        ${CMAKE_CURRENT_SOURCE_DIR}/sensor
        ${EXAMPLE_SOURCE}/ # Headers for example code
        ${EXAMPLE_SNIPPETS}/ # Headers for snippets
)
# Example code dependencies
target_link_libraries(eve_example
        eve_library # Example code depends on EVE library
        pico_stdlib
)

# Add the standard library to the build
target_link_libraries(${PROJECT_NAME}
        eve_example # Project depends on example code library
        hardware_flash # For non-volatile storage
        hardware_sync # For non-volatile storage
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(eve_library 
        pico_stdlib
        hardware_spi
        hardware_i2c
        pico_time
        pico_stdio_usb
        )

pico_add_extra_outputs(${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND arm-none-eabi-size --format=berkeley ${PROJECT_NAME}.elf
    VERBATIM
)


