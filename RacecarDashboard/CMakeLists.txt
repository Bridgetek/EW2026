# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#check for pico SDK
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
include(pico_sdk_import.cmake)

# project(RacecarDashboard C CXX ASM)
# Set name of project (as current working directory)
get_filename_component(ProjectName ${CMAKE_CURRENT_LIST_DIR} NAME)
string(REPLACE " " "_" ProjectName ${ProjectName})
project(${ProjectName} C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Set the location of the EVE-MCU-Dev tree
set(API_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/eve_library/")

message(API_DIRECTORY="${API_DIRECTORY}")
message(ProjectName="${PROJECT_SOURCE_DIR}")
message(CMAKE_C_COMPILER_ID="${CMAKE_C_COMPILER_ID}")
message(CMAKE_SYSTEM_NAME="${CMAKE_SYSTEM_NAME}")

set(EXAMPLE_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/example")
set(EXAMPLE_SNIPPETS "${CMAKE_CURRENT_SOURCE_DIR}/eve_library/examples/snippets")

# Add executable. Default name is the project name, version 0.1

add_executable(${ProjectName} main.c )

pico_set_program_name(${ProjectName} "RacecarDashboard")
pico_set_program_version(${ProjectName} "1.0")

# directories we wish to include in the project
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include
        ${EXAMPLE_SOURCE}
		${EXAMPLE_SNIPPETS}/maths
)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(${ProjectName} 0)
pico_enable_stdio_usb(${ProjectName} 1)

# Tell EVE library to use RP2040 as base class
add_compile_definitions(PLATFORM_RP2040)

# use look up table instead of maths funcitons for trig in example
add_compile_definitions(USE_TRIG_TABLE) 

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
        ${EXAMPLE_SNIPPETS}/maths/trig_furman.c
)
 
# Header files used for example code library
target_include_directories(eve_example PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${API_DIRECTORY}/include # Headers for EVE library
        ${EXAMPLE_SOURCE}/ # Headers for example code
        ${EXAMPLE_SNIPPETS}/maths # Headers for snippets
)

# Example code dependencies
target_link_libraries(eve_example
        eve_library # Example code depends on EVE library
)
 
# Add the standard library, and other dependencies to the build
target_link_libraries(${ProjectName}
        eve_example # Project depends on example code library
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(${ProjectName} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(eve_library 
        hardware_spi
        pico_time
        )

pico_add_extra_outputs(${ProjectName})

