/**
 @file eve_example.c
 */
 /*
  * ============================================================================
  * (C) Copyright,  Bridgetek Pte. Ltd.
  * ============================================================================
  *
  * This source code ("the Software") is provided by Bridgetek Pte Ltd
  * ("Bridgetek") subject to the licence terms set out
  * https://brtchip.com/wp-content/uploads/2021/11/BRT_Software_License_Agreement.pdf ("the Licence Terms").
  * You must read the Licence Terms before downloading or using the Software.
  * By installing or using the Software you agree to the Licence Terms. If you
  * do not agree to the Licence Terms then do not download or use the Software.
  *
  * Without prejudice to the Licence Terms, here is a summary of some of the key
  * terms of the Licence Terms (and in the event of any conflict between this
  * summary and the Licence Terms then the text of the Licence Terms will
  * prevail).
  *
  * The Software is provided "as is".
  * There are no warranties (or similar) in relation to the quality of the
  * Software. You use it at your own risk.
  * The Software should not be used in, or for, any medical device, system or
  * appliance. There are exclusions of Bridgetek liability for certain types of loss
  * such as: special loss or damage; incidental loss or damage; indirect or
  * consequential loss or damage; loss of income; loss of business; loss of
  * profits; loss of revenue; loss of contracts; business interruption; loss of
  * the use of money or anticipated savings; loss of information; loss of
  * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
  * corruption of data.
  * There is a monetary cap on Bridgetek's liability.
  * The Software may have subsequently been amended by another user and then
  * distributed by that other user ("Adapted Software").  If so that user may
  * have additional licence terms that apply to those amendments. However, Bridgetek
  * has no liability in relation to those amendments.
  * ============================================================================
  */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <EVE.h>
#include <MCU.h> // For DEBUG_PRINTF only

#include "eve_example.h"
//#include "math.h"
#include "trig_furman.h"

// ######################################################################################################################################################################################################
// #######################################################      Ensure correct EVE settings and enable any defines we wish to use     ###################################################################
// ######################################################################################################################################################################################################

#if IS_EVE_API(1,2,3,5)
#error This project needs EVE API 4 (BT81x). Please configure ../eve_library/include/EVE_config.h accordingly 
#endif

#if EVE_DISP_WIDTH < 800 || EVE_DISP_HEIGHT < 480
#error This project requires a screen of at least 800x480 pixels. Please configure ../eve_library/include/EVE_config.h accordingly
#endif

// set define for 3d background effect on shift light
// comment line to disable
#define SHIFT_3D ;

// set define for motion blur in traction widget
// comment line to disable
//#define MOTION_BLUR_TRACTION ;

// set define for rotating steering wheel in steering angle widget
// comment line to disable
//#define STEERING_ROTATE ;

// set define for afr graph style (with blanking or without blanking on the graph line)
// comment line to disable
//#define AFR_BLANKING ;


// ######################################################################################################################################################################################################
// #######################################################            Set properties of all of the assets to be loaded                ###################################################################
// ######################################################################################################################################################################################################

// Use Flash storage to keep preconverted assets
// These are compressed into ASTC bitmaps

typedef struct
{
    uint8_t Handle;
    uint8_t Cell;
    uint32_t Size;
    uint32_t Flash_Start;
    uint32_t Flash_Size;
    uint32_t RAM_G_Start;
    uint32_t RAM_G_EndAddr;
    uint32_t Format;
    uint16_t Width;
    uint16_t Height;
    uint16_t CellHeight;
} EVE_ASSET_PROPS;
 
EVE_ASSET_PROPS Carbon_Fiber_800x480_asset; 
EVE_ASSET_PROPS LED_32x32_asset;  
EVE_ASSET_PROPS Trackmap_96x60_asset;
EVE_ASSET_PROPS Arrows_96x192_asset;
EVE_ASSET_PROPS Widget_Gear_152x152_asset;
EVE_ASSET_PROPS Widget_RPM_152x56_asset;
EVE_ASSET_PROPS Widget_Speed_104x56_asset;
EVE_ASSET_PROPS Bottom_Bar_800x8_asset;
EVE_ASSET_PROPS Car_Overhead_44x80_asset;
EVE_ASSET_PROPS Battery_Cells_40x720_asset;
EVE_ASSET_PROPS Oil_Water_Icons_32x64_asset;
EVE_ASSET_PROPS Steering_Wheel_32x32_asset;
EVE_ASSET_PROPS Side_Gauge_L_136x104_asset;
EVE_ASSET_PROPS Side_Gauge_R_136x104_asset;
EVE_ASSET_PROPS Side_Gauge_L_Fill_136x104_asset;
EVE_ASSET_PROPS Side_Gauge_R_Fill_136x104_asset;
EVE_ASSET_PROPS PIT_SC_Widget_80x36_asset;
EVE_ASSET_PROPS eurostile_150_L8;

// Flash Map
// Name                                   : Offset : Size 
// default-fl.blob                        : 0      : 4096  
// Carbon_Fiber_800x480_ASTC_4X4.raw      : 4096   : 384000
// LED_32x32_ASTC_4X4.raw                 : 388096 : 1024  
// Trackmap_96x60_ASTC_4X4.raw            : 389120 : 5760  
// Arrows_96x192_ASTC_4X4.raw             : 394880 : 18432 
// Widget_Gear_152x152_ASTC_4X4.raw       : 413312 : 23104 
// Widget_RPM_152x56_ASTC_4X4.raw         : 436416 : 8512  
// Widget_Speed_104x56_ASTC_4X4.raw       : 444928 : 5824  
// Bottom_Bar_800x8_ASTC_4X4.raw          : 450752 : 6400  
// Car_Overhead_44x80_ASTC_4X4.raw        : 457152 : 3520  
// Battery_Cells_40x1440_ASTC_4X4.raw     : 460672 : 57600 
// Oil_Water_Icons_32x64_ASTC_4X4.raw     : 518272 : 2048  
// Steering_Wheel_32x32_ASTC_4X4.raw      : 520320 : 1024  
// Side_Gauge_L_136x104_ASTC_4X4.raw      : 521344 : 14144 
// Side_Gauge_R_136x104_ASTC_4X4.raw      : 535488 : 14144 
// Side_Gauge_L_Fill_136x104_ASTC_4X4.raw : 549632 : 14144 
// Side_Gauge_R_Fill_136x104_ASTC_4X4.raw : 563776 : 14144 
// PIT_SC_Widget_80x36_80x36_ASTC_4X4.raw : 577920 : 2880  
// eurostile_150_L8.raw                   : 580800 : 93988

void set_asset_props(void)
{
    //images
    Carbon_Fiber_800x480_asset.Flash_Start = 4096;
    Carbon_Fiber_800x480_asset.Flash_Size = 384000;
    Carbon_Fiber_800x480_asset.Format = EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR;
    Carbon_Fiber_800x480_asset.Width = 800;
    Carbon_Fiber_800x480_asset.Height = 480;
    Carbon_Fiber_800x480_asset.CellHeight = 480;

    LED_32x32_asset.Flash_Start = 388096 ;
    LED_32x32_asset.Flash_Size = 1024;
    LED_32x32_asset.Format = EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR;
    LED_32x32_asset.Width = 32;
    LED_32x32_asset.Height = 32;
    LED_32x32_asset.CellHeight = 32;

    Trackmap_96x60_asset.Flash_Start = 389120;
    Trackmap_96x60_asset.Flash_Size = 5760;
    Trackmap_96x60_asset.Format = EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR;
    Trackmap_96x60_asset.Width = 96;
    Trackmap_96x60_asset.Height = 60;
    Trackmap_96x60_asset.CellHeight = 60;

    Arrows_96x192_asset.Flash_Start = 394880;
    Arrows_96x192_asset.Flash_Size = 18432;
    Arrows_96x192_asset.Format = EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR;
    Arrows_96x192_asset.Width = 96;
    Arrows_96x192_asset.Height = 192; // set this to the height of all cells
    Arrows_96x192_asset.CellHeight = 96; // set this to the height of one cell

    Widget_Gear_152x152_asset.Flash_Start = 413312;
    Widget_Gear_152x152_asset.Flash_Size = 23104;
    Widget_Gear_152x152_asset.Format = EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR;
    Widget_Gear_152x152_asset.Width = 152;
    Widget_Gear_152x152_asset.Height = 152;
    Widget_Gear_152x152_asset.CellHeight = 152;

    Widget_RPM_152x56_asset.Flash_Start = 436416;
    Widget_RPM_152x56_asset.Flash_Size = 8512; 
    Widget_RPM_152x56_asset.Format = EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR;
    Widget_RPM_152x56_asset.Width = 152;
    Widget_RPM_152x56_asset.Height = 56;
    Widget_RPM_152x56_asset.CellHeight = 56;

    Widget_Speed_104x56_asset.Flash_Start = 444928;
    Widget_Speed_104x56_asset.Flash_Size = 5824; 
    Widget_Speed_104x56_asset.Format = EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR;
    Widget_Speed_104x56_asset.Width = 104;
    Widget_Speed_104x56_asset.Height = 56;
    Widget_Speed_104x56_asset.CellHeight = 56;

    Bottom_Bar_800x8_asset.Flash_Start = 450752;
    Bottom_Bar_800x8_asset.Flash_Size = 6400;
    Bottom_Bar_800x8_asset.Format = EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR;
    Bottom_Bar_800x8_asset.Width = 800;
    Bottom_Bar_800x8_asset.Height = 8;
    Bottom_Bar_800x8_asset.CellHeight = 8;

    Car_Overhead_44x80_asset.Flash_Start = 457152;
    Car_Overhead_44x80_asset.Flash_Size = 3520;
    Car_Overhead_44x80_asset.Format = EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR;
    Car_Overhead_44x80_asset.Width = 44;
    Car_Overhead_44x80_asset.Height = 80; // set this to the height of all cells
    Car_Overhead_44x80_asset.CellHeight = 80; // set this to the height of one cell

    Battery_Cells_40x720_asset.Flash_Start = 460672;
    Battery_Cells_40x720_asset.Flash_Size = 57600;
    Battery_Cells_40x720_asset.Format = EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR;
    Battery_Cells_40x720_asset.Width = 40;
    Battery_Cells_40x720_asset.Height = 1440; // set this to the height of all cells
    Battery_Cells_40x720_asset.CellHeight = 72; // set this to the height of one cell

    Oil_Water_Icons_32x64_asset.Flash_Start = 518272;
    Oil_Water_Icons_32x64_asset.Flash_Size = 2048; 
    Oil_Water_Icons_32x64_asset.Format = EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR;
    Oil_Water_Icons_32x64_asset.Width = 32;
    Oil_Water_Icons_32x64_asset.Height = 64; // set this to the height of all cells
    Oil_Water_Icons_32x64_asset.CellHeight = 32; // set this to the height of one cell

    Steering_Wheel_32x32_asset.Flash_Start = 520320;
    Steering_Wheel_32x32_asset.Flash_Size = 1200; 
    Steering_Wheel_32x32_asset.Format = EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR;
    Steering_Wheel_32x32_asset.Width = 32;
    Steering_Wheel_32x32_asset.Height = 32;
    Steering_Wheel_32x32_asset.CellHeight = 32;

    Side_Gauge_L_136x104_asset.Flash_Start = 521344;
    Side_Gauge_L_136x104_asset.Flash_Size = 14144;
    Side_Gauge_L_136x104_asset.Format = EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR;
    Side_Gauge_L_136x104_asset.Width = 136;
    Side_Gauge_L_136x104_asset.Height = 104;
    Side_Gauge_L_136x104_asset.CellHeight = 104;

    Side_Gauge_R_136x104_asset.Flash_Start = 535488;
    Side_Gauge_R_136x104_asset.Flash_Size = 14144; 
    Side_Gauge_R_136x104_asset.Format = EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR;
    Side_Gauge_R_136x104_asset.Width = 136;
    Side_Gauge_R_136x104_asset.Height = 104;
    Side_Gauge_R_136x104_asset.CellHeight = 104;

    Side_Gauge_L_Fill_136x104_asset.Flash_Start = 549632;
    Side_Gauge_L_Fill_136x104_asset.Flash_Size = 14144;
    Side_Gauge_L_Fill_136x104_asset.Format = EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR;
    Side_Gauge_L_Fill_136x104_asset.Width = 136;
    Side_Gauge_L_Fill_136x104_asset.Height = 104;
    Side_Gauge_L_Fill_136x104_asset.CellHeight = 104;

    Side_Gauge_R_Fill_136x104_asset.Flash_Start = 563776;
    Side_Gauge_R_Fill_136x104_asset.Flash_Size = 14144; 
    Side_Gauge_R_Fill_136x104_asset.Format = EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR;
    Side_Gauge_R_Fill_136x104_asset.Width = 136;
    Side_Gauge_R_Fill_136x104_asset.Height = 104;
    Side_Gauge_R_Fill_136x104_asset.CellHeight = 104;

    PIT_SC_Widget_80x36_asset.Flash_Start = 577920;
    PIT_SC_Widget_80x36_asset.Flash_Size = 2880; 
    PIT_SC_Widget_80x36_asset.Format = EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR;
    PIT_SC_Widget_80x36_asset.Width = 80;
    PIT_SC_Widget_80x36_asset.Height = 36;
    PIT_SC_Widget_80x36_asset.CellHeight = 36;

    //custom font
    eurostile_150_L8.Flash_Start = 580800;
    eurostile_150_L8.Flash_Size = 94016;//93988 (comment is actual value, but we need to use a number /64);
    eurostile_150_L8.Format = EVE_FORMAT_L8;
    eurostile_150_L8.Width = 0;
    eurostile_150_L8.Height = 0; 
}

// ######################################################################################################################################################################################################
// #######################################################                  Code to set flash to full speed mode                      ###################################################################
// ######################################################################################################################################################################################################

void flash_full_speed(void)
{
    uint8_t Flash_Status = 0;

#if IS_EVE_API(3,4)
    // Detach Flash
    EVE_LIB_BeginCoProList();
    EVE_CMD_FLASHDETATCH();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    Flash_Status = EVE_LIB_MemRead8(EVE_REG_FLASH_STATUS);

    if (EVE_FLASH_STATUS_DETACHED != Flash_Status)
    {
        EVE_LIB_BeginCoProList();
        EVE_CMD_DLSTART();
        EVE_CLEAR_COLOR_RGB(255, 0, 0);
        EVE_CLEAR(1,1,1);
        EVE_COLOR_RGB(255,255,255);
        EVE_CMD_TEXT(100, 50, 28, 0, "Error detaching flash");
        EVE_CMD_TEXT(100,100, 28, EVE_OPT_FORMAT, "Mode is %d ", Flash_Status);
        EVE_DISPLAY();
        EVE_CMD_SWAP();
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();
        while(1)
        {
        }
    }

    // Attach
    EVE_LIB_BeginCoProList();
    EVE_CMD_FLASHATTACH();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    Flash_Status = EVE_LIB_MemRead8(EVE_REG_FLASH_STATUS);

    if (EVE_FLASH_STATUS_BASIC != Flash_Status)
    {
        EVE_LIB_BeginCoProList();
        EVE_CMD_DLSTART();
        EVE_CLEAR_COLOR_RGB(0, 255, 0);
        EVE_CLEAR(1,1,1);
        EVE_COLOR_RGB(255,255,255);
        EVE_CMD_TEXT(100, 50, 28, 0, "Error attaching flash");
        EVE_CMD_TEXT(100,100, 28, EVE_OPT_FORMAT, "Mode is %d ", Flash_Status);
        EVE_DISPLAY();
        EVE_CMD_SWAP();
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();
        while(1)
        {
        }
    }

    // Fast mode
    EVE_LIB_BeginCoProList();
    EVE_CMD_FLASHFAST(0);
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

    Flash_Status = EVE_LIB_MemRead8(EVE_REG_FLASH_STATUS);

    if (EVE_FLASH_STATUS_FULL != Flash_Status)
    {

        EVE_LIB_BeginCoProList();
        EVE_CMD_DLSTART();
        EVE_CLEAR(1,1,1);
        EVE_CLEAR_COLOR_RGB(0, 0, 255);
        EVE_COLOR_RGB(255,255,255);
        EVE_CMD_TEXT(100, 50, 28, 0, "Error going to full mode");
        EVE_CMD_TEXT(100,100, 28, EVE_OPT_FORMAT, "Mode is %d ", Flash_Status);
        EVE_DISPLAY();
        EVE_CMD_SWAP();
        EVE_LIB_EndCoProList();
        EVE_LIB_AwaitCoProEmpty();
        while(1)
        {
        }
    }
#endif
}

// ######################################################################################################################################################################################################
// #######################################################                      Code for the Loading Assets                         #####################################################################
// ######################################################################################################################################################################################################

void assetLoad(EVE_ASSET_PROPS *asset)
{
    uint32_t final;

    // Read directy from flash
    EVE_LIB_BeginCoProList();
    DEBUG_PRINTF("RAM_G: %d Flash: %d for %d\n", asset->RAM_G_Start, asset->Flash_Start, asset->Flash_Size);
    EVE_CMD_FLASHREAD(asset->RAM_G_Start, asset->Flash_Start, asset->Flash_Size); // Destination, Source, Size
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();
    
    final = asset->RAM_G_Start + asset->Flash_Size;

    asset->RAM_G_EndAddr = final;
}

void eve_display_load_assets(void)
{
    uint8_t AssetHandle = 0;

    Carbon_Fiber_800x480_asset.RAM_G_Start = 0;
    Carbon_Fiber_800x480_asset.Handle = AssetHandle;
    assetLoad(&Carbon_Fiber_800x480_asset); // Destination, Source, Size
    
    AssetHandle ++; // 1

    LED_32x32_asset.RAM_G_Start = Carbon_Fiber_800x480_asset.RAM_G_EndAddr;
    LED_32x32_asset.Handle = AssetHandle;
    assetLoad(&LED_32x32_asset); // Destination, Source, Size
    
    AssetHandle ++; // 2

    Trackmap_96x60_asset.RAM_G_Start = LED_32x32_asset.RAM_G_EndAddr;
    Trackmap_96x60_asset.Handle = AssetHandle;
    assetLoad(&Trackmap_96x60_asset); // Destination, Source, Size
    
    AssetHandle ++; // 3

    Arrows_96x192_asset.RAM_G_Start = Trackmap_96x60_asset.RAM_G_EndAddr;
    Arrows_96x192_asset.Handle = AssetHandle;
    assetLoad(&Arrows_96x192_asset); // Destination, Source, Size
    
    AssetHandle ++; // 4

    Widget_Gear_152x152_asset.RAM_G_Start = Arrows_96x192_asset.RAM_G_EndAddr;
    Widget_Gear_152x152_asset.Handle = AssetHandle;
    assetLoad(&Widget_Gear_152x152_asset); // Destination, Source, Size
    
    AssetHandle ++; // 5

    Widget_RPM_152x56_asset.RAM_G_Start = Widget_Gear_152x152_asset.RAM_G_EndAddr;
    Widget_RPM_152x56_asset.Handle = AssetHandle;
    assetLoad(&Widget_RPM_152x56_asset); // Destination, Source, Size
    
    AssetHandle ++; // 6

    Widget_Speed_104x56_asset.RAM_G_Start = Widget_RPM_152x56_asset.RAM_G_EndAddr;
    Widget_Speed_104x56_asset.Handle = AssetHandle;
    assetLoad(&Widget_Speed_104x56_asset); // Destination, Source, Size
    
    AssetHandle ++; // 7

    Bottom_Bar_800x8_asset.RAM_G_Start = Widget_Speed_104x56_asset.RAM_G_EndAddr;
    Bottom_Bar_800x8_asset.Handle = AssetHandle;
    assetLoad(&Bottom_Bar_800x8_asset); // Destination, Source, Size

    AssetHandle ++; // 8

    Car_Overhead_44x80_asset.RAM_G_Start = Bottom_Bar_800x8_asset.RAM_G_EndAddr;
    Car_Overhead_44x80_asset.Handle = AssetHandle;
    assetLoad(&Car_Overhead_44x80_asset); // Destination, Source, Size
    
    AssetHandle ++; // 9

    Battery_Cells_40x720_asset.RAM_G_Start = Car_Overhead_44x80_asset.RAM_G_EndAddr;
    Battery_Cells_40x720_asset.Handle = AssetHandle;
    assetLoad(&Battery_Cells_40x720_asset); // Destination, Source, Size

    AssetHandle ++; // 10

    Oil_Water_Icons_32x64_asset.RAM_G_Start = Battery_Cells_40x720_asset.RAM_G_EndAddr;
    Oil_Water_Icons_32x64_asset.Handle = AssetHandle;
    assetLoad(&Oil_Water_Icons_32x64_asset); // Destination, Source, Size

    AssetHandle ++; // 11

    Steering_Wheel_32x32_asset.RAM_G_Start = Oil_Water_Icons_32x64_asset.RAM_G_EndAddr;
    Steering_Wheel_32x32_asset.Handle = AssetHandle;
    assetLoad(&Steering_Wheel_32x32_asset); // Destination, Source, Size
    
    AssetHandle ++; // 12

    Side_Gauge_L_136x104_asset.RAM_G_Start = Steering_Wheel_32x32_asset.RAM_G_EndAddr;
    Side_Gauge_L_136x104_asset.Handle = AssetHandle;
    assetLoad(&Side_Gauge_L_136x104_asset); // Destination, Source, Size
    
    AssetHandle ++; // 13

    Side_Gauge_R_136x104_asset.RAM_G_Start = Side_Gauge_L_136x104_asset.RAM_G_EndAddr;
    Side_Gauge_R_136x104_asset.Handle = AssetHandle;
    assetLoad(&Side_Gauge_R_136x104_asset); // Destination, Source, Size
    
    AssetHandle ++; // 14

    Side_Gauge_L_Fill_136x104_asset.RAM_G_Start = Side_Gauge_R_136x104_asset.RAM_G_EndAddr;
    Side_Gauge_L_Fill_136x104_asset.Handle = AssetHandle;
    assetLoad(&Side_Gauge_L_Fill_136x104_asset); // Destination, Source, Size
    
#ifdef SHIFT_3D // normally defined, if it is we need to reseve bitmap handle 15 as it is used in CMD_BUTTON
    AssetHandle = AssetHandle + 2; // 16
#else
    AssetHandle ++; //15
#endif

    Side_Gauge_R_Fill_136x104_asset.RAM_G_Start = Side_Gauge_L_Fill_136x104_asset.RAM_G_EndAddr;
    Side_Gauge_R_Fill_136x104_asset.Handle = AssetHandle;
    assetLoad(&Side_Gauge_R_Fill_136x104_asset); // Destination, Source, Size
    
    AssetHandle ++; // 16 (or 17)

    PIT_SC_Widget_80x36_asset.RAM_G_Start = Side_Gauge_R_Fill_136x104_asset.RAM_G_EndAddr;
    PIT_SC_Widget_80x36_asset.Handle = AssetHandle;
    assetLoad(&PIT_SC_Widget_80x36_asset); // Destination, Source, Size
    
    AssetHandle ++; // 17 (or 18)

    eurostile_150_L8.RAM_G_Start = PIT_SC_Widget_80x36_asset.RAM_G_EndAddr;
    eurostile_150_L8.Handle = AssetHandle;
    assetLoad(&eurostile_150_L8); // Destination, Source, Size
    
    // adjust ram_G offset used in the conversion to where we have actually loaded this into RAM_G
    EVE_LIB_MemWrite32((eurostile_150_L8.RAM_G_Start + 144), (eurostile_150_L8.RAM_G_Start + 148)); 

    // NOTE: similar to CMD_BUTTON we are using inbuilt font 20 in this example, so should not exceed this number here to re-assign that handle.
    // Another option if we do exceed 20 and wish to use the inbuilt fonts from that number is to usilise the CMD_ROMFONT command 
}

// ######################################################################################################################################################################################################
// #######################################################                      Code for the Widgets Example                          ###################################################################
// ######################################################################################################################################################################################################


void tractionCircle(uint16_t input_x, uint16_t input_y, uint16_t array_end, uint16_t count, int16_t *acc_x, int16_t *acc_y) {
 
    //total width 108, height 108
    //----------------------------------------
 
    // save graphics context
    EVE_SAVE_CONTEXT();
 
    // set alpha and color for background shading
    EVE_COLOR_A(120);
    EVE_COLOR_RGB(0,0,0);
    EVE_LINE_WIDTH(64); //4
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_VERTEX_FORMAT(0);
    EVE_VERTEX2F((input_x + 5), (input_y + 5));
    EVE_VERTEX2F((input_x + 105), (input_y + 105));
    // reset color and alpha
    EVE_COLOR_A(255);
    EVE_COLOR_RGB(255, 255, 255);
 
    // centre point
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE(32); //2
    EVE_VERTEX2F((input_x + 55), (input_y +  55));
 
    // draw cross hair lines 
    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_LINE_WIDTH(5); // 0.3125
    // vertical line
    EVE_VERTEX2F((input_x + 55), (input_y + 10));
    EVE_VERTEX2F((input_x + 55), (input_y + 100));
    // horizontal line
    EVE_VERTEX2F((input_x + 10), (input_y + 55));
    EVE_VERTEX2F((input_x + 100), (input_y + 55));
 
    // alpha blednding section for circles on graph
    // diable all colours, leave alpha enabled
    EVE_COLOR_MASK(0,0,0,1);
 
    // clear color buffer before doing blending
    EVE_CLEAR(1,0,0);
 
    // draw circles
    EVE_BEGIN(EVE_BEGIN_POINTS);
    // add to alpha nuffer
    // 4g circle outer edge
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE(16 * 41); // outer circle
    EVE_VERTEX2F((input_x + 55), (input_y + 55));
    // erase from alpha buffer
    // 4g circle inner edge
    EVE_BLEND_FUNC(EVE_BLEND_ZERO, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE(16 * 40); // inner circle
    EVE_VERTEX2F((input_x + 55), (input_y + 55));
 
    // 3g circle outer edge
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE(16 * 31); // outer circle
    EVE_VERTEX2F((input_x + 55), (input_y + 55));
    // erase from alpha buffer
    // 3g circle inner edge
    EVE_BLEND_FUNC(EVE_BLEND_ZERO, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE(16 * 30); // inner circle
    EVE_VERTEX2F((input_x + 55), (input_y + 55));
 
    // 2g circle outer edge
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE(16 * 21); // outer circle
    EVE_VERTEX2F((input_x + 55), (input_y + 55));
    // erase from alpha buffer
    // 2g circle inner edge
    EVE_BLEND_FUNC(EVE_BLEND_ZERO, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE(16 * 20); // inner circle
    EVE_VERTEX2F((input_x + 55), (input_y + 55));
 
    // 1g circle outer edge
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE(16 * 11); // outer circle
    EVE_VERTEX2F((input_x + 55), (input_y + 55));
    // erase from alpha buffer
    // 1g circle inner edge
    EVE_BLEND_FUNC(EVE_BLEND_ZERO, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE(16 * 10); // inner circle
    EVE_VERTEX2F((input_x + 55), (input_y + 55));
 
    // re-enable colours
    EVE_COLOR_MASK(1,1,1,1);
    EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_ONE);
    EVE_BEGIN(EVE_BEGIN_RECTS); // Visit every pixel on the screen
    EVE_VERTEX2F(input_x,input_y);
    EVE_VERTEX2F((input_x + 110), (input_y + 110));
 
    // stop blending
    EVE_BLEND_FUNC(EVE_BLEND_SRC_ALPHA, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
 
    // draw current G reading on the graph 
    // set back to 1/16th pixel precision as the accel data has been nomralised to this
    EVE_VERTEX_FORMAT(4);
 
    // set colour to red
    EVE_COLOR_RGB(255, 0, 0);
 
    // line from centre to current point
    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_LINE_WIDTH(18); // 1.25
    EVE_VERTEX2F(((input_x + 55) * 16), ((input_y +  55) * 16));
    EVE_VERTEX2F(((input_x + 55) * 16) + acc_x[count], ((input_y + 55)* 16) - acc_y[count]);
 
    // draw points
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE(64); // 4
    // draw current g force point
    EVE_VERTEX2F(((input_x + 55) * 16) + acc_x[count], ((input_y + 55)* 16) - acc_y[count]);
 
    // draw historical points
#ifdef MOTION_BLUR_TRACTION // Normally not defined
    // nominally we would wait until we have some histroical data, but as we are using a fixed array to loop through for screen updates
    // we can take values from the back end of the array to act as our historical data at the beginning of the loop
    EVE_POINT_SIZE(48); // 3
    // set lower alpha for older data
    EVE_COLOR_A(100);
    // take current count through data array, display value before this, ensuring we stay within array data set using the moduulo of the array size
    EVE_VERTEX2F(((input_x + 55) * 16) + acc_x[((count-1) + array_end) % array_end], ((input_y + 55)* 16) - acc_y[((count-1) + array_end) % array_end]);
 
    // second last data point
    EVE_VERTEX2F(((input_x + 55) * 16) + acc_x[((count-2) + array_end) % array_end], ((input_y + 55)* 16) - acc_y[((count-2) + array_end) % array_end]);
 
    // third last data point
    EVE_VERTEX2F(((input_x + 55) * 16) + acc_x[((count-2) + array_end) % array_end], ((input_y + 55)* 16) - acc_y[((count-3) + array_end) % array_end]);
 
    // 4th last data point
    EVE_VERTEX2F(((input_x + 55) * 16) + acc_x[((count-4) + array_end) % array_end], ((input_y + 55)* 16) - acc_y[((count-4) + array_end) % array_end]);
#endif // MOTION_BLUR_TRACTION

    // restore graphics context
    EVE_RESTORE_CONTEXT();
}

void iconIndicator(uint16_t input_x, uint16_t input_y, EVE_ASSET_PROPS *image, uint8_t cell, uint32_t color, bool state){

    //total width 42, height 42
    //----------------------------------------

    // expects a 32x32 bitmap as the input image

    // save graphics context
    EVE_SAVE_CONTEXT();

    // draw outline
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_COLOR_RGB(0, 0, 0); // black
    EVE_COLOR_A(120);
    EVE_LINE_WIDTH(80); // 5
    EVE_VERTEX2F((input_x - 16) * 16, (input_y - 16) * 16);
    EVE_VERTEX2F((input_x + 16) * 16, (input_y + 16) * 16);

    // begin bitmaps 
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(image->Handle);
    // select the bitmap cell that has been passed in (if one one cell in the bitmap this value should be 0)
    EVE_CELL(cell);
    // colour based on input
    EVE_COLOR_RGB(((uint8_t)(color >> 16)), ((uint8_t)(color >> 8)), ((uint8_t)(color)));

    // set alpha for bitmap based on on/off state
    if (state == true)
        EVE_COLOR_A(255);
    else
        EVE_COLOR_A(50);

    // place bitmap on screen
    EVE_VERTEX2F(((input_x - (image->Width/2) + 1) * 16), ((input_y - (image->CellHeight/2)) * 16));

    // restore graphics context
    EVE_RESTORE_CONTEXT();

}

void shiftLight(uint16_t input_x, uint16_t input_y, int8_t value){

    // declare local variables
    uint8_t led_start_x = (input_x + 15);
    uint8_t led_y = (input_y + 10);
    uint8_t led_steps_x = 40;

    // ensure value is in range
    if (value > 6)
        value = 6;
    if (value < 0)
        value = 0;

    // save graphics context
    EVE_SAVE_CONTEXT();

    // ensure we are using the desried pixel precision
    EVE_VERTEX_FORMAT(0);

    // set colour and alpha for background box
    EVE_COLOR_RGB(0, 0, 0); //black
    EVE_COLOR_A(255);

#ifdef SHIFT_3D // normally defined
    // we can use a button widget without text here to render a nice 3d effect background
    EVE_CMD_FGCOLOR(0x000000); //black
    EVE_CMD_BUTTON(input_x, input_y, 500,50,21,0, "");
#else
    // draw outline box
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_LINE_WIDTH(80); //5
    EVE_VERTEX2F((input_x + 5), (input_y + 5));
    EVE_VERTEX2F((input_x + 495), (input_y + 45));
#endif // SHIFT_3D

    // add LED images
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(LED_32x32_asset.Handle);

    // start the first LED green
    EVE_COLOR_RGB(0,255,0); //green

    // place LED bitmaps on the screen
    for (int i = 0; i <= 11; i++){
        
        // colour the rest of the LEDS as necessary
        if (i == 2 || i == 8)
            EVE_COLOR_RGB(255,255,0); // yellow
        if (i == 4)
            EVE_COLOR_RGB(255,0,0); // red
        if (i == 10)
            EVE_COLOR_RGB(0,255,0); // green     

        // decide if the LEDs should be on or not
        if (value == i)
            EVE_COLOR_A(50);
        if (value == (12-i))// for reverse side
            EVE_COLOR_A(255);

        // place LEDs on screen
        EVE_VERTEX2F((led_start_x + (i*led_steps_x)), (led_y));
    }

    // restore graphics context
    EVE_RESTORE_CONTEXT();

}

void cornerIndicator(uint16_t input_x, uint16_t input_y, uint8_t corner_number, uint8_t corner_direction, EVE_ASSET_PROPS *image){

    //total width 108, height 108
    //----------------------------------------

    // save graphics context
    EVE_SAVE_CONTEXT();

    // set alpha and color for background shading
    EVE_COLOR_A(120);
    EVE_COLOR_RGB(0,0,0);

    // ensure we are using the desried pixel precision
    EVE_VERTEX_FORMAT(0);

    // draw background box
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_LINE_WIDTH(64); //4
    EVE_VERTEX2F((input_x + 5), (input_y + 5));
    EVE_VERTEX2F((input_x + 105), (input_y + 105));
    
    // reset color and alpha
    EVE_COLOR_A(255);
    EVE_COLOR_RGB(255, 255, 255);

    // place bitmap on screen
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(image->Handle);
    EVE_CELL(corner_direction);
    EVE_VERTEX2F((input_x + ((110 - image->Width)/2) + 1) , (input_y + ((110 - image->CellHeight)/2) + 1));
        
    // add a number on top to indicate which corner;
    // decide where to print corner number on arrow based on cell number
    // we are using a bitmap with only two cells (0 (left arrow) and 1 (right arrow))
    if(corner_direction == 1)
        EVE_CMD_NUMBER((input_x + 90), (input_y + 85), 29, EVE_OPT_CENTER, corner_number);
    else
        EVE_CMD_NUMBER((input_x + 20), (input_y + 85), 29, EVE_OPT_CENTER, corner_number);

    // restore graphics context
    EVE_RESTORE_CONTEXT();

}

void trackMap(uint16_t input_x, uint16_t input_y, uint16_t angle, EVE_ASSET_PROPS *image){

    //total width 108, height 108
    //----------------------------------------

    // save graphics context
    EVE_SAVE_CONTEXT();

    // set pixel precision format that we want to use
    EVE_VERTEX_FORMAT(0);

    // set alpha and color for background shading
    EVE_COLOR_A(120);
    EVE_COLOR_RGB(0,0,0);

    // draw background box
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_LINE_WIDTH(64); //4
    EVE_VERTEX2F((input_x + 5), (input_y + 5)); // + line width used above
    EVE_VERTEX2F((input_x + 105), (input_y + 105)); // w/h - line width used above 
    
    // reset color and alpha
    EVE_COLOR_A(255);
    EVE_COLOR_RGB(255, 255, 255);

    // draw bitmap here
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(image->Handle);
    // this allows us to rotate the bitmap without cutting any sections out during the rotation
    EVE_BITMAP_SIZE(EVE_FILTER_NEAREST, EVE_WRAP_BORDER, EVE_WRAP_BORDER, (image->Width * 2), (image->Width*2));

    EVE_CMD_LOADIDENTITY();
    // max circle value 65536
    // (+55 is the x/y vertex translate value, this will draw our 96*60 image in the middle of the 192*192 box we configured for it in the BITMAP_SIZE command above)
    // this allows us to rotate the bitmap without clipping any edges
    EVE_CMD_TRANSLATE(((image->Width/2) + 55) * 65536, ((image->CellHeight/2) + 55) * 65536);
    // take the angle and convert it to furmans
    // use this number to rotate the image
    EVE_CMD_ROTATE(DEG2FURMAN(angle));
    EVE_CMD_TRANSLATE(((image->Width/2) - image->Width)*65536, ((image->CellHeight/2) - image->CellHeight)*65536);
    // Configure transform matrix of bitmap
    EVE_CMD_SETMATRIX(); 

    // place image in centre of widget
    EVE_VERTEX2F((input_x - (image->Width/2)) , (input_y + - (image->CellHeight/2)));

    // restore graphics context
    EVE_RESTORE_CONTEXT();
}

void revCounter(uint16_t input_x, uint16_t input_y, uint16_t radius, uint8_t gauge_thickness, uint8_t start_angle, uint8_t max_RPM, uint8_t redline, uint8_t font_handle, int16_t value){

    // 0 degrees is the bottom of the circle
    // start_angle currently cannot be smaller than 90 degreees, max_RPM cannot be past 270 degrees (max_RPM is the number of degrees past start_angle)

    // decalare variables for use in gauge
    // sizing variables
    uint16_t gauge_radius = radius;
    uint8_t line_width = 2; // this is the width of the thickest line drawn on the rev bar, and is used in the scissor and positioning variables (must be at least 2)
    uint16_t thickness = (gauge_radius - gauge_thickness);
    uint16_t underline_offset = (line_width * 4);
    uint16_t number_offset = (thickness - underline_offset - line_width) - 19; // can adjust this 19 value here based on a font metric block read

    // angle vairbles 
    // ensure these are within limits
    if (start_angle < 90)
        start_angle = 90;

    uint16_t arc_min_limit = start_angle; 
    uint16_t arc_max_limit = (arc_min_limit + max_RPM);    
    uint16_t redline_start = (arc_min_limit + redline);

    // ensure we dont try and draw past 270 degrees
    if (arc_max_limit > 270)
        arc_max_limit = 270;

    // set a variable for the guage reading limit
    uint16_t reading_limit = ((arc_max_limit - arc_min_limit) + 1); 

    // set a variables to count how many rev lines we have drawn (used to mark K rev lines) and to add rev numbers
    uint16_t rev_line_count = 0;
    uint8_t rev_number = 0;

    // variables for caulating x/y positions for the lines in the gauge
    // we are going to use math.h sin/cos functions here instead of frumans as we need the line poisitoning to be more accurate than the 16bit value furmans provides
    int16_t line_end_x = 0;
    int16_t line_end_y = 0;
    int16_t line_start_x = 0;
    int16_t line_start_y = 0;

    // positioning variables for the center of the circle we wish to draw to generate the arc
    // had to add fixed numbers to adjust properly 
    uint16_t gauge_circle_x = ((input_x + (line_width * 2) + CIRC_X(gauge_radius, DEG2FURMAN(arc_min_limit)))); //take the min limit to offset x
    // if a 1k rev mark is at the top of the circle we would need to use (line_width * 2) here
    uint16_t gauge_circle_y = ((input_y + line_width - CIRC_Y(gauge_radius, DEG2FURMAN(180)))); // 180 degrees will offset y to top of the arc

    // variables for x/y positions we will use in the stencil for the underline
    // perform some maths that will be used to define where the stencil is drawn for the underline
    // not required if we do not scissor the underline for a flat edge
    int16_t scissor_min_limit_y = CIRC_Y_DEG(gauge_radius, arc_min_limit);
    int16_t scissor_min_limit_x = CIRC_X_DEG(gauge_radius, arc_min_limit);
    int16_t scissor_max_limit_x = CIRC_X_DEG(gauge_radius, arc_max_limit);

    // ensure value does not exceeed accetable reading limits
    if (value > reading_limit)
        value = reading_limit;
    if (value < 0)
        value = 0;

    // set current reading to the value input
    uint16_t reading = value;

    //--------------------------------------------------------------------------------------
    // save current graphics context
    EVE_SAVE_CONTEXT();

    // scissor the area we want to draw the underline in
    // predominetly used to cut the edge of the underline arc to give a nice flat edge.
    // however, this will only work for an arc that is centered on the top of the circle.
    // can do (scissor_max_limit_x > 0 ? scissor_max_limit_x : -scissor_max_limit_x) etc, but in this case we know the signs of the numbers we are using
    EVE_SCISSOR_SIZE(scissor_min_limit_x + (-1 * scissor_max_limit_x) + (line_width * 4) , (gauge_circle_y - input_y - (-1 *scissor_min_limit_y)) + (gauge_thickness/2)); 
    EVE_SCISSOR_XY(input_x, input_y);

    // ensure we have the correct vertex format (1/16th precicison)
    // EVE_VERTEX_FORMAT(4);

    //--------------------------------------------------------------------------------------
    // create our underline

    //start drawing the line strip for underline
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    // set line width and colour
    EVE_LINE_WIDTH(((line_width) * 16) - 4); 
    EVE_COLOR_RGB(255, 255, 255); // white

    // for every degree we will add a line strip
    for(int angle = arc_min_limit; angle <= arc_max_limit; angle ++){

        // if we get to the redline we want to colour the underline red
        if((angle - 1) == redline_start)
            EVE_COLOR_RGB(255,0,0); // red

        // calculate x/y end for current line
        // multiplying by 16 here gives use nicer integer numbers for positioning the lines on the circle (+ 8 to round the number)
        // we then can add this value directly into the vertex2f command without having to multiply it to correct for the pixel precision
        line_start_x = CIRC_X_DEG((thickness - underline_offset) * 16 + 8, angle);
        line_start_y = CIRC_Y_DEG((thickness - underline_offset) * 16 + 8, angle);

        // add the vertexs for the line strip
        EVE_VERTEX2F((gauge_circle_x * 16) - line_start_x, (gauge_circle_y * 16) + line_start_y); 
    }
    EVE_END();
   
    // reset scissor
    EVE_SCISSOR_SIZE(2048,2048);
    EVE_SCISSOR_XY(0, 0);

    //--------------------------------------------------------------------------------------
    // draw the revlines into the gauge

    // colour white
    EVE_COLOR_RGB(255,255,255);

    // we will use the alpha value to "fill" the bar, so here we want to set the alpha to max
    EVE_COLOR_A(255);

    // start drawing the lines
    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_LINE_WIDTH((line_width * 16) / 2); // line_width is the thickest line, we want these to be thinner

    for(int angle = arc_min_limit; angle <= arc_max_limit; angle++){

        // calculate x/y end for current line
        // multiplying by 16 here gives use nicer integer numbers for positioning the lines on the circle (+ 8 to round the number)
        // we then can add this value directly into the vertex2f command without having to multiply it to correct for the pixel precision
        line_end_x = CIRC_X_DEG(gauge_radius * 16 + 8, angle);
        line_end_y = CIRC_Y_DEG(gauge_radius * 16 + 8, angle);

        line_start_x = CIRC_X_DEG(thickness * 16 + 8, angle);
        line_start_y = CIRC_Y_DEG(thickness * 16 + 8, angle);

        // logic to add thicker lines for K's 
        if(rev_line_count % 10 == 0)
            EVE_LINE_WIDTH(line_width * 16);

        // draw redline colour when we reach the redline
        if(angle == redline_start)
            EVE_COLOR_RGB(255,0,0);

        // when count hits reading chnage colour alpha for the rest of the lines
        // i.e. fill in everything with full alpha before the reading value
        if(rev_line_count == (reading+1))
            EVE_COLOR_A(50);

        // draw the lines
        EVE_VERTEX2F((gauge_circle_x * 16) - line_start_x, (gauge_circle_y * 16) + line_start_y); 
        EVE_VERTEX2F((gauge_circle_x * 16) - line_end_x, (gauge_circle_y * 16) + line_end_y);

        // logic to reset thicker lines for K's
        if(rev_line_count % 10 == 0)
            EVE_LINE_WIDTH((line_width/2) * 16);

        rev_line_count = rev_line_count + 1;
    }

    //--------------------------------------------------------------------------------------
    // add numbers for every 1 K revs onto the gauge

    // ensure alpha is set to full
    EVE_COLOR_A(255);
    // color these white
    EVE_COLOR_RGB(255,255,255);

    // we only wish to add these every 10 lines
    for(int numb = arc_min_limit; numb <= arc_max_limit; numb = numb +10){

        // calculate x/y for numbers (no sense declaring new variable for this so just use the line variables)
        // multiplying by 16 here gives use nicer integer numbers for positioning the numbers on the circle
        // we then can divide this back down by 16 when inputting it into the CMD_Number command
        line_end_x = CIRC_X_DEG(number_offset * 16, numb);
        line_end_y = CIRC_Y_DEG(number_offset * 16, numb);

        // draw the numbers
        EVE_CMD_NUMBER((gauge_circle_x - (line_end_x/16)), (gauge_circle_y + (line_end_y/16)), font_handle, EVE_OPT_CENTER, rev_number);
            
        rev_number = rev_number + 1;

    }

    // finally add a label to denote the tacho units
    // place this half way between the 0 and the 1 on the rec counter
    line_end_x = CIRC_X_DEG((number_offset + 4) * 16, (arc_min_limit + 6));
    line_end_y = CIRC_Y_DEG((number_offset + 4) * 16, (arc_min_limit + 6));
    // add text label
    // use the CMD_FILLWIDTH command and OPT_FILL in the CMD_TEXT command to limit how wide the text string is before a newline
    EVE_CMD_FILLWIDTH(33);
    EVE_CMD_TEXT((gauge_circle_x - (line_end_x/16)), (gauge_circle_y + (line_end_y/16)), 20, EVE_OPT_FILL, " 1/min x 1000");

    // restore graphics context
    EVE_RESTORE_CONTEXT();

}

void fuelIndicator(uint16_t input_x, uint16_t input_y, EVE_ASSET_PROPS *outline, EVE_ASSET_PROPS *fill, uint32_t color, int8_t value){

    //total width 136, height 120
    //----------------------------------------

    // Ensure the value is within limits
    if (value > 100)
       value = 100;
    if (value < 0)
       value = 0;

    // save graphics context
    EVE_SAVE_CONTEXT();

    // mask off colours and alpha so we can create a stencil of the fill bitmap
    EVE_COLOR_MASK(0,0,0,0);

    // we only want to include pixels with an alpha value greater than one in our stencil
    EVE_ALPHA_FUNC(EVE_TEST_GREATER, 1);

    // ensure the stencil buffer has a value of 1 for our fill image
    EVE_STENCIL_FUNC(EVE_TEST_ALWAYS, 1, 255);

    // replace all preceeding pixels with the above stencil value
    EVE_STENCIL_OP(EVE_STENCIL_REPLACE, EVE_STENCIL_REPLACE);

    // draw fill bitmap (done first so it is underneath the guage outline)
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    // use the right side guage fill asset
    EVE_BITMAP_HANDLE(fill->Handle);
    // place bitmap on screen
    EVE_VERTEX2F(input_x * 16, input_y * 16);
    
    // re-enable colours and alpha, and stop incrementing the stencil buffer
    EVE_COLOR_MASK(1,1,1,1);
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP);

    // now we draw a shape whose size is based on the input value 
    // where this shape is drawn will render the fill bitmap
    // draw fill (this will colour all pixels where the stencil = 1)
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 1, 255);
    // we can use a rectangle to cover the full area of the bitmap
    EVE_BEGIN(EVE_BEGIN_RECTS);
    // colour based on input
    EVE_COLOR_RGB(((uint8_t)(color >> 16)), ((uint8_t)(color >> 8)), ((uint8_t)(color)));

    // these vertexes draw the fill (first one moves with vertically with the value input)
    // use input x/y and fill bitmap size to determine where to draw the rect
    // fill bitmap is 1 px shorter on the top and bottom than the outline so adjust for that as well

    // set desried vertex format
    EVE_VERTEX_FORMAT(0);
    // draw indices
    EVE_VERTEX2F(input_x, ((input_y + fill->Height - 2) - value)); //move with value input
    EVE_VERTEX2F((input_x + fill->Width), ((input_y + fill->Height) - 3)); 

    // restore graphics context to reset stencil
    EVE_RESTORE_CONTEXT();

    // draw guage outline on top of the fill
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    // use the right side guage asset
    EVE_BITMAP_HANDLE(outline->Handle);    
    // ensure nomral image colours
    EVE_COLOR_RGB(255,255,255);
    // place bitmap on screen
    EVE_VERTEX2F(input_x * 16, input_y * 16);

    // finally add text to denote guage reading
    EVE_CMD_TEXT((input_x + 110), (input_y + 104) ,20 ,0 ,"F u e l");

    // restore graphics context
    EVE_RESTORE_CONTEXT();

}

void pitAndScWidget(uint16_t input_x, uint16_t input_y, EVE_ASSET_PROPS *image, bool pit, bool sc){

    //total width 260, height 38
    //----------------------------------------

    // save graphics context
    EVE_SAVE_CONTEXT();

    // we are going to draw the same bitmap twice, first time in its nomral orientation
    // second time we are going to mirror the bitmap

    // draw bitmap here initially 
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(image->Handle);

    // here we can colour the white sections of the bitmap image
    if (pit) // if pit bool is true
        EVE_COLOR_RGB(255,119,0); // orange

    // place bitmap on screen at the input x/y
    EVE_VERTEX2F((input_x * 16), (input_y * 16));

    // place text on bitmaps for PIT
    EVE_CMD_TEXT((input_x + 70), (input_y + 21), 20, EVE_OPT_CENTERY | EVE_OPT_RIGHTX, "PITSTOP");

    if (pit) // reset colour
        EVE_COLOR_RGB(255,255,255);

    // now we wish to mirror the bitmap
    // set initial bitmap transfrom matrix
    EVE_CMD_LOADIDENTITY();
    // max circle value 65536
    // adjust the translate to the middle of the bitmap
    EVE_CMD_TRANSLATE((image->Width/2) * 65536, 0);
    // scale by -1 in the x axis to mirror the image
    EVE_CMD_SCALE((-1 * 65536), (1 * 65536));
    EVE_CMD_TRANSLATE((-1 * ((image->Width/2) * 65536)), 0);
    // set new bitmap transform matrix
    EVE_CMD_SETMATRIX(); 

    if (sc)// if sc (saftey car) bool is true
        EVE_COLOR_RGB(255,255,0); // yellow

    // place the mirrored bitmap on the screen
    EVE_VERTEX2F((input_x + 177) * 16, (input_y * 16));

    // restore graphics context to reset the bitmap transform matrix and colour
    EVE_RESTORE_CONTEXT();

    //place text on bitmap for saftey car
    EVE_CMD_TEXT((input_x + 187), (input_y + 21), 20, EVE_OPT_CENTERY, "SAFETY");

    // restore graphics context
    EVE_RESTORE_CONTEXT();

}

void drsIcon(uint16_t input_x, uint16_t input_y, uint8_t value){

    //total width 60, height 40
    //----------------------------------------

    // save graphics context
    EVE_SAVE_CONTEXT();

    // set alpha and color for background shading
    EVE_COLOR_A(120);
    EVE_COLOR_RGB(0,0,0); //black

    // set pixel precision format that we want to use
    //EVE_VERTEX_FORMAT(4);

    //draw background box
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_LINE_WIDTH(80); //5
    EVE_VERTEX2F((input_x + 5) * 16, (input_y + 5) * 16);
    EVE_VERTEX2F((input_x + 55) * 16, (input_y + 35) * 16);
    
    // reset color and alpha
    EVE_COLOR_A(255);
    EVE_COLOR_RGB(255, 255, 255);

    // alpha blending section for inner icon box
    // diables all colours, leave alpha enabled
    EVE_COLOR_MASK(0,0,0,1);

    // clear color buffer before doing blending
    EVE_CLEAR(1,0,0);

    // draw rects
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_LINE_WIDTH(25); //1.5625

    // add to alpha buffer
    // outline outer edge
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_VERTEX2F((input_x + 5) * 16, (input_y + 5) * 16);
    EVE_VERTEX2F((input_x + 55) * 16, (input_y + 35) * 16);

    // erase from alpha buffer
    // outline inner edge
    EVE_BLEND_FUNC(EVE_BLEND_ZERO, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_VERTEX2F((input_x + 6) * 16, (input_y + 6) * 16);
    EVE_VERTEX2F((input_x + 54) * 16, (input_y + 34) * 16);

    // re-enable colours
    EVE_COLOR_MASK(1,1,1,0);
    EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_ONE);
    EVE_BEGIN(EVE_BEGIN_RECTS); // Visit every pixel on the screen
    EVE_VERTEX2F(input_x * 16, input_y * 16);
    EVE_VERTEX2F((input_x + 55) * 16, (input_y + 35) * 16);

    // stop blending
    EVE_BLEND_FUNC(EVE_BLEND_SRC_ALPHA, EVE_BLEND_ONE_MINUS_SRC_ALPHA);

    // colour in the icon if DRS is active
    if (value == 1){
        EVE_COLOR_RGB(0,175,0); // green
        EVE_BEGIN(EVE_BEGIN_RECTS);
        EVE_VERTEX2F((input_x + 6) * 16, (input_y + 6) * 16);
        EVE_VERTEX2F((input_x + 54) * 16, (input_y + 34) * 16);
    }

    // set text colour
    EVE_COLOR_RGB(255, 255, 255);
    // add DRS text into widget
    EVE_CMD_TEXT((input_x + 29), (input_y + 20), 29, EVE_OPT_CENTER, "DRS");

    // restore graphics context
    EVE_RESTORE_CONTEXT();

}

void boostIndicator(uint16_t input_x, uint16_t input_y, EVE_ASSET_PROPS *outline, EVE_ASSET_PROPS *fill, int8_t value){

    //total width 136, height 120
    //----------------------------------------

    // Ensure the value is within limits
    if (value > 100)
       value = 100;
    if (value < 0)
       value = 0;

    // save graphics context
    EVE_SAVE_CONTEXT();

    // first draw the fill, but at a lower alpha value to produce a shadow fill effect (first pass)
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(fill->Handle);    
    // set desried alpha value
    EVE_COLOR_A(50);
    // place bitmap on screen
    EVE_VERTEX2F(input_x * 16, input_y * 16);
    // reset alpha
    EVE_COLOR_A(255);

    // mask off colours and alpha so we can create a stencil of the fill bitmap
    EVE_COLOR_MASK(0,0,0,0);

    // increment the stencil value for the pixles that the preceeding shape touches
    EVE_STENCIL_OP(EVE_STENCIL_INCR, EVE_STENCIL_INCR);

    // we can use a rectangle to cover the full area of the bitmap
    EVE_BEGIN(EVE_BEGIN_RECTS);

    // these vertexes will be used to apply the stencil to the area we wish to draw the second pass of the fill bitmap
    // use input x/y and fill bitmap size to determine where to draw the rect
    // fill bitmap is 1 px shorter on the top and bottom than the outline so adjust for that as well

    // set desried vertex format
    EVE_VERTEX_FORMAT(0);
    // draw indices for guage fill

    // if we are in the top half of the guage we want to fill upwards from the white line
    // if we are in the bottom half of the guage we want to fill downwards
    // setting the the y value in the second vertex call correctly will do this for us
    EVE_VERTEX2F(input_x, ((input_y + fill->Height - 2) - value)); //move with value input
    EVE_VERTEX2F((input_x + fill->Width), ((input_y + fill->Height) - 32)); 
    
    // re-enable colours and alpha, and stop incrementing the stencil buffer
    EVE_COLOR_MASK(1,1,1,1);
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP);

    // now we  only draw the second pass of the fill bitmap where the stencil value = 1 
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 1, 255);
    // draw fill bitmap (done first so it is underneath the guage outline)
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    // use the right side guage fill asset
    EVE_BITMAP_HANDLE(fill->Handle);
    // place bitmap on screen
    EVE_VERTEX2F(input_x, input_y);

    // restore graphics context to reset stencil
    EVE_RESTORE_CONTEXT();

    // draw guage outline on top of the fill
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    // use the right side guage asset
    EVE_BITMAP_HANDLE(outline->Handle);    
    // ensure nomral image colours
    EVE_COLOR_RGB(255,255,255);
    // place bitmap on screen
    EVE_VERTEX2F(input_x * 16, input_y * 16);

    // finally add text to denote guage reading
    EVE_CMD_TEXT((input_x - 1), (input_y + 104),20 ,0 ,"Boost");

    // restore graphics context
    EVE_RESTORE_CONTEXT();

}

void iatGraph(uint16_t input_x, uint16_t input_y, uint8_t *values){

    //total width 64, height 25
    //----------------------------------------

    // we are going to construct a bargraph bitmap on the fly in this widget
    // so we need to declare a vairble for the bitmap data location in RAM_G
    // we can use the address after the last asset we have loaded into RAM_G
    uint32_t barGraphAddr = eurostile_150_L8.RAM_G_EndAddr;

    //--------------------------------------------------------------------------------------------------------
    // Draw widget shape and graph lines
    //--------------------------------------------------------------------------------------------------------

    // save graphics context
    EVE_SAVE_CONTEXT();

    // set desried pixel precision format
    // EVE_VERTEX_FORMAT(4);

    // draw outline
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_COLOR_RGB(0, 0, 0); // black
    EVE_COLOR_A(120);
    EVE_VERTEX2F((input_x * 16), (input_y * 16));
    EVE_VERTEX2F((input_x + 63) * 16, (input_y + 25) * 16);

    // set colour and alpha for the graph lines
    EVE_COLOR_A(175);
    EVE_COLOR_RGB(255, 0,0); // red
    // draw some graph lines 
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    EVE_LINE_WIDTH(8); // 0.5
    EVE_VERTEX2F((input_x + 12) * 16, (input_y + 2) * 16); // top of L
    EVE_VERTEX2F((input_x + 12) * 16, (input_y + 22) * 16); // middle of L
    EVE_VERTEX2F((input_x + 61) * 16, (input_y + 22) * 16); // end of L

    //--------------------------------------------------------------------------------------------------------
    // Add bargraph bitmap into the widget
    //--------------------------------------------------------------------------------------------------------

    // set the desired colour
    EVE_COLOR_RGB(80, 80, 140);  // purple

    // we want to create a bargraph to denote out IAT temps, we can use the BARGRAPH bitmap format to achieve this
    // We could set this bitmap up with the other bitmaps in the configure_bitmaps() function, but we will do it on
    // the fly here. First we need to pick a bitmap handle that we can use. 0-18 are being used by the other custom
    // bitmaps we have configured, and various inbuilt fonts are also being used, so lets pick the handle of an 
    // inbuilt font we know were not using, or a handle we havent previously used such as 19.
    EVE_BITMAP_HANDLE(19);

    // then we can call setbitmap with the address we want for the bitmap data, and its size
    // here we set the width to the total width we want for our bargraph (48)
    EVE_CMD_SETBITMAP(barGraphAddr, EVE_FORMAT_BARGRAPH, 48, 1);

    // would need to call EVE_BITMAP_SIZE_H(1, 0); for wider bitmaps
    // now we can set the height we actaully want for the bitmap here (19)
    // nomrally we would use EVE_FILTER_NEAREST below, but if we use BILINEAR and postionion the bitmap
    // on a Y value with .5 then we get a nice alpha effect on the top edge
    EVE_BITMAP_SIZE(EVE_FILTER_BILINEAR, EVE_WRAP_BORDER, EVE_WRAP_BORDER, 0, 19);

    // as the bitmap height is 19, and we have used the trick above for the alpha edge the valid range
    // of graph values is from 0-19 (with the NEAREST setting above it would be 0-18)
    // NOTE: a value of 0 represents a full bar, with 19 representing an empty bar

    // now we draw the bargraph on screen
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_VERTEX2F((input_x + 13) * 16, ((input_y + 1) * 16) + 8);
    EVE_END();

    // now we can dynamically determine the values for each of the bars in our graph by populating the 
    // area in RAM_G where the bargrah has been configured via the CMD_MEMSET command, we can write
    // the value of and number of bytes we want. As the total bargraph is 48 pixels, and we want 6 bars
    // the we can write a value for every 8 bytes of data from the start of the bargraph data up to 48 bytes

    EVE_CMD_MEMSET(barGraphAddr, values[0], 8);
    EVE_CMD_MEMSET((barGraphAddr + 8), values[1], 8);
    EVE_CMD_MEMSET((barGraphAddr + 16), values[2], 8);
    EVE_CMD_MEMSET((barGraphAddr + 24), values[3], 8);
    EVE_CMD_MEMSET((barGraphAddr + 32), values[4], 8);
    EVE_CMD_MEMSET((barGraphAddr + 40), values[5], 8);

    //--------------------------------------------------------------------------------------------------------
    // Add numbers on top of bargraph (to indicate cylinder number)
    //--------------------------------------------------------------------------------------------------------

    /*
    // set colour (alpha still set from above)
    EVE_COLOR_RGB(150, 150, 150); // grey

    // print font 20 characters via the vertex2ii command
    // can alternately use VERTEX2F and CELL commands with a BITMAP_HANDLE call for the font we want to use 
    EVE_VERTEX2II((input_x + 15), (input_y + 5), 20, '1');
    EVE_VERTEX2II((input_x + 23), (input_y + 5), 20, '2');
    EVE_VERTEX2II((input_x + 31), (input_y + 5), 20, '3');
    EVE_VERTEX2II((input_x + 39), (input_y + 5), 20, '4');
    EVE_VERTEX2II((input_x + 47), (input_y + 5), 20, '5');
    EVE_VERTEX2II((input_x + 55), (input_y + 5), 20, '6');
    */

    // begin bitmaps to print font
    EVE_BEGIN(EVE_BEGIN_BITMAPS);

    // select the font handle we wish to use
    EVE_BITMAP_HANDLE(20);

    // now we can loop through the intput array (6 total values) to determine what coour we wish to print the number 
    // on to of each bar in the bargraph
    for (int i = 0; i < 6; i++){
        // if the value for an array entry is 0 (this represents a full bar in te bargraph)
        if(values[i] == 0){
            // colour it accrodingly
            EVE_COLOR_RGB(127, 0, 0);  // dark red
            // ASCII value for character '1' then add i as we go through the loop to increment number
            EVE_CELL(49 + i);
            // place on screen with a predetermined offset then att 8 pixels to x for every time around the loop
            EVE_VERTEX2F(((input_x + 15) + (i * 8)) * 16, (input_y + 5) * 16); 
        }
        else{
            EVE_COLOR_RGB(150, 150, 150); // grey
            EVE_CELL(49 + i);
            EVE_VERTEX2F(((input_x + 15) + (i * 8)) * 16, (input_y + 5) * 16);
        }
    }
    
    //--------------------------------------------------------------------------------------------------------
    // Rotate a in-built font to use along the edge of the widget
    //--------------------------------------------------------------------------------------------------------

    // restore graphics context to reset colour and alpha, etc
    EVE_RESTORE_CONTEXT();

    // we wish to use font 26 here
    // the rest of the bitmap properites for this font will have been configured during EVE initialization
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(20);

    // we want to rotate one of the inbuilt fonts to read vertically along the edge of the graph
    EVE_CMD_LOADIDENTITY();
    // here we wish to translate X and Y by the below amounts to keep the font charecters within the 
    // bitmap size that is configured for the font (we can alternatively increase the bitmap size parameters or re-translate)
    EVE_CMD_TRANSLATE((-2 * 65535), (12 * 65535));
    // rotate by 270 (or -90) degrees
    EVE_CMD_ROTATE((270 * 65535 ) / 360);
    // Configure transform matrix of bitmap
    EVE_CMD_SETMATRIX();   

    // manually place letters on the graph from inbuilt font 26
    // x/y values have been pre-computed in a EVE Screen Editor project
    EVE_VERTEX2II((input_x), (input_y + 10), 20, 'I');
    EVE_VERTEX2II((input_x), (input_y + 5), 20, 'A');
    EVE_VERTEX2II((input_x), (input_y - 4), 20, 'T');

    // restore graphics context (resets bitmap matrix)
    EVE_RESTORE_CONTEXT();
}

void speedWidget(uint16_t input_x, uint16_t input_y, uint8_t img_handle, uint8_t font_handle, const char* text, int32_t value){

    // save graphics context
    EVE_SAVE_CONTEXT();

    // draw widget bitmap
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(img_handle);
    EVE_VERTEX2F((input_x * 16), (input_y * 16)); // in 1/16th precision unless we call VERTEX_FORMAT
    
    // set colour
    EVE_COLOR_RGB(255,255,255);
    // print RPM number into widget, use OPT_RIGHTX so the position remains static as the number grows
    EVE_CMD_NUMBER((input_x +  85), (input_y + 6), font_handle, EVE_OPT_RIGHTX, value);
    // add text description to top of widget
    // can make this relate to input font size if we read the font data (right now we'll fix the font and offset from Y)
    EVE_CMD_TEXT(input_x, (input_y - 7), 26, EVE_OPT_CENTERY, text);

    // restore graphics context
    EVE_RESTORE_CONTEXT();

}

void rpmWidget(uint16_t input_x, uint16_t input_y, uint8_t img_handle, uint8_t font_handle, const char* text, uint32_t color , uint16_t value){

    // save graphics context
    EVE_SAVE_CONTEXT();

    // draw widget bitmap
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(img_handle);
    EVE_VERTEX2F((input_x * 16), (input_y * 16)); // in 1/16th precision unless we call VERTEX_FORMAT

    // set colour for text
    EVE_COLOR_RGB(((uint8_t)(color >> 16)), ((uint8_t)(color >> 8)), ((uint8_t)(color)));

    // print RPM number into widget, use OPT_RIGHTX so the position remains static as the number grows
    EVE_CMD_NUMBER((input_x +  133), (input_y + 7), font_handle, EVE_OPT_RIGHTX, value);

    // add text description to top of widget
    // can make this relate to input font size if we read the font data (right now well fix the font and offset from Y)
    EVE_COLOR_RGB(255,255,255);
    EVE_CMD_TEXT(input_x, (input_y - 7), 26, EVE_OPT_CENTERY, text);

    // restore graphics context
    EVE_RESTORE_CONTEXT();

}

void gearWidget(uint16_t input_x, uint16_t input_y, uint8_t img_handle, uint8_t font_handle, uint8_t value){

    // save graphics context
    EVE_SAVE_CONTEXT();

    // draw widget bitmap
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(img_handle);
    EVE_VERTEX2F((input_x * 16), (input_y * 16)); // in 1/16th precision unless we call VERTEX_FORMAT

    // set colour
    EVE_COLOR_RGB(255,255,255);
    // print RPM number into widget, use OPT_RIGHTX so the position remains static as the number grows
    // can make x/y offset relative to font widths, but we will hardcode the values here
    EVE_CMD_NUMBER((input_x + 77), (input_y + 75), font_handle, EVE_OPT_CENTER, value);

    // restore graphics context
    EVE_RESTORE_CONTEXT();
}

// Global varibles for use in simple_scrolling_graph function
// graph buffer size can be equal to chart width passed into function
// input index keeps track of what value we are updatin in the buffer
static uint16_t graph_buffer[58];
static uint16_t input_index_graph = 0;

void simple_scrolling_graph(uint16_t chart_topleft_x, uint16_t chart_topleft_y, uint16_t chart_width, uint16_t chart_height, uint16_t max_range, uint32_t graph_color ,uint8_t value, uint8_t visible)
{
    // Local varibles for use in function    
    uint16_t output_index;
    uint16_t chart_index_x;
    uint16_t chart_value = 0; 

    // ensure the input value is within the max range of the chart
    if (value > max_range)
        value = max_range;

    // First, we add the new measurement taken above to the results buffer
    graph_buffer[input_index_graph] = value;
    // Now, we increment the pointer in a circular fashion. This sets up the pointer for the next reading we take the next time round the loop...
    input_index_graph++;

    // Wrap around at the max x value of the graph
    if (input_index_graph >= chart_width)
        input_index_graph = 0;

    // ... and also points to the starting location for the chart drawing
    output_index = input_index_graph; 

    // If we wish for the graph line to be visible the draw in the edge strip with the data points
    if (visible == 0xFF)
    {
 
        // Set the line width for the graph
        EVE_LINE_WIDTH(10);  // 0.625
        // A line strip will be used to plot the chart
        EVE_BEGIN(EVE_BEGIN_LINE_STRIP);    
        // Set colour and alpha for graph line                       
        EVE_COLOR_A(255);
        //set colour based on input
        EVE_COLOR_RGB(((uint8_t)(graph_color >> 16)), ((uint8_t)(graph_color >> 8)), ((uint8_t)(graph_color)));

        // Chart will be drawn from offset 0 (left hand side) moving rightwards
        chart_index_x = 0;                                           

        // for each of the chart points...
        while (chart_index_x < chart_width)                                   
        {
            // Draw the current point of the line strip
            // The X coordinate is increased from 0 to chart_width each time round this loop
            // The Y coordinate is derived from going to the bottom of the chart area and subtracting the value corresponding to the current value to be displayed (in pixels)

            // set chart value to the buffer value for the current output index value
            chart_value = graph_buffer[output_index];         

#ifdef AFR_BLANKING 
            // here we want to make the leading edge of the graph plot see through so it is easy to see where we are updating from
            // so we alter the alpha values for these points
            if (chart_index_x == (input_index_graph)) 
            {
                EVE_COLOR_A(0);
            }
            if (chart_index_x == ((input_index_graph) + 10))
            {
                EVE_COLOR_A(255);
            }

            // add graph plots to line strip
            EVE_VERTEX2F((chart_topleft_x * 16 + (chart_index_x * 16)), ((chart_topleft_y + chart_height - (chart_value)) * 16));
#else
            // add graph plots to line strip
            EVE_VERTEX2F((chart_topleft_x * 16 + (chart_index_x * 16)), ((chart_topleft_y + chart_height - (chart_value)) * 16));
#endif // AFR_BLANKING

            // Increment the counter to draw the next point the following time round this loop
            // Because we begin the plot from the location after the last data added, it won't always begin from index 0.
            // Therefore, must handle roll-over.
            output_index++;

            if (output_index >= chart_width)
                output_index = 0;

            // Increment chart index counter
            chart_index_x++;
        }

        // End drawing line strip
        EVE_END();
    }

}

void afrGraph(uint16_t input_x, uint16_t input_y, int8_t value){

    //total width 74, height 30
    //----------------------------------------
    
    // the graph plot expects a value range of 0-24
    uint8_t graph_max = 24;

    // Esnure value is within limit that we require
    if (value > graph_max)
        value = graph_max;
    if (value < 0)
        value = 0;

    //--------------------------------------------------------------------------------------------------------
    // Draw widget shape and graph lines
    //--------------------------------------------------------------------------------------------------------

    // save graphics context
    EVE_SAVE_CONTEXT();

    // draw outline
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_COLOR_RGB(0, 0, 0); // black
    EVE_COLOR_A(120);
    EVE_VERTEX2F((input_x * 16), (input_y * 16));
    EVE_VERTEX2F((input_x + 74) * 16, (input_y + 30) * 16);

    // set colour and alpha for the graph lines
    EVE_COLOR_A(175);
    EVE_COLOR_RGB(255, 0,0); // red
    // draw some graph lines 
    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_LINE_WIDTH(8); // 0.5
    // left
    EVE_VERTEX2F((input_x + 14) * 16, (input_y + 2) * 16);
    EVE_VERTEX2F((input_x + 14) * 16, (input_y + 28) * 16);
    // middle line
    EVE_VERTEX2F((input_x + 15) * 16, (input_y + 15) * 16);
    EVE_VERTEX2F((input_x + 71) * 16, (input_y + 15) * 16);
    // right line
    EVE_VERTEX2F((input_x + 72) * 16, (input_y + 2) * 16);
    EVE_VERTEX2F((input_x + 72) * 16, (input_y + 28) * 16);

    // restore graphics context to reset colour and alpha
    EVE_RESTORE_CONTEXT();

    //--------------------------------------------------------------------------------------------------------
    // Rotate a in-built font to use along the edge of the widget
    //--------------------------------------------------------------------------------------------------------

    // we wish to use font 21 here
    // the rest of the bitmap properites for this font will have been configured during EVE initialization
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(21);

    // we want to rotate one of the inbuilt fonts to read vertically along the edge of the graph
    EVE_CMD_LOADIDENTITY();
    // here we wish to translate X and Y by the below amounts to keep the font charecters within the 
    // bitmap size that is configured for the font (we can alternatively increase the bitmap size parameters or re-translate)
    EVE_CMD_TRANSLATE((-1 * 65535), (13 * 65535));
    // rotate by 270 (or -90) degrees
    EVE_CMD_ROTATE((270 * 65535 ) / 360);
    // Configure transform matrix of bitmap
    EVE_CMD_SETMATRIX();   

    // manually place letters on the graph from inbuilt font 26
    // x/y values have been pre-computed in a EVE Screen Editor project
    EVE_VERTEX2II((input_x - 1), (input_y + 15), 21, 'A');
    EVE_VERTEX2II((input_x - 1), (input_y + 7), 21, 'F');
    EVE_VERTEX2II((input_x - 1), (input_y - 1), 21, 'R');

    // restore the bitmap transform matrix for font 21 via restore context
    EVE_RESTORE_CONTEXT();

    //--------------------------------------------------------------------------------------------------------
    // Add scrolling graph onto the widget
    //--------------------------------------------------------------------------------------------------------

    // add in the scrolling graph to the widget
    // this funciton utilises some global varibles to keep track of previous graph values and current index value
    // we feed the most recent value into this funciton to update the graph
    // we have also defined set max graph range to be 24, so we can colour the graph line differntly if we hit the mix or max
    // fuction takes chart height and max value inputs we can set these both to 24
    if (value == 0)
        simple_scrolling_graph((input_x + 15), (input_y + 3), 56, graph_max, graph_max, 0x00BEFF, value, 0xFF); // blue
    else if (value == graph_max)
        simple_scrolling_graph((input_x + 15), (input_y + 3), 56, graph_max, graph_max, 0xFF0000, value, 0xFF);  // red
    else
        simple_scrolling_graph((input_x + 15), (input_y + 3), 56, graph_max, graph_max, 0xFFFFFF, value, 0xFF);  // white

    // restore graphics context
    EVE_RESTORE_CONTEXT();
}

void verticalBarGauge(uint16_t input_x, uint16_t input_y, uint32_t color, const char* text, int8_t value){

    // ensure value is in range
    if (value > 100)
        value = 100;
    if (value < 0)
        value = 0;

    //total width 50, height 100
    //----------------------------------------

    // save graphics context
    EVE_SAVE_CONTEXT();

    // set scissor to size of widget and starting position
    EVE_SCISSOR_SIZE(50,100);
    EVE_SCISSOR_XY(input_x, input_y);

    //----------------------------------------
    //outline shapes (lines)

    // set vertex fomrat we want to use
    EVE_VERTEX_FORMAT(0);

    // set outline colour
    EVE_COLOR_RGB(255,255,255);
    EVE_BEGIN(EVE_BEGIN_LINES);
    // leftside veritcal line
    EVE_VERTEX2F((input_x + 3), input_y);
    EVE_VERTEX2F((input_x + 3), (input_y + 100));
    // right side vertical liine
    EVE_VERTEX2F((input_x +  46), input_y);
    EVE_VERTEX2F((input_x + 46), (input_y + 100));

    // 100% and 0% lines
    EVE_VERTEX2F(input_x, input_y);
    EVE_VERTEX2F((input_x + 2), input_y);
    EVE_VERTEX2F(input_x, (input_y + 99));
    EVE_VERTEX2F((input_x + 2), (input_y + 99));

    EVE_VERTEX2F((input_x + 47), (input_y + 99));
    EVE_VERTEX2F((input_x + 50), (input_y + 99));
    EVE_VERTEX2F((input_x + 47), input_y);
    EVE_VERTEX2F((input_x + 50), input_y);

    // 50%
    EVE_VERTEX2F((input_x + 47), (input_y + 50));
    EVE_VERTEX2F((input_x + 50), (input_y + 50));

    EVE_VERTEX2F(input_x, (input_y + 50));
    EVE_VERTEX2F((input_x + 2 ), (input_y + 50));

    // draw fill
    //----------------------------------------

    // draw shadow   
    EVE_COLOR_A(30);
    // use input colour
    EVE_COLOR_RGB(((uint8_t)(color >> 16)), ((uint8_t)(color >> 8)), ((uint8_t)(color)));
    EVE_BEGIN(EVE_BEGIN_RECTS);
    // within the percentage lines
    EVE_VERTEX2F((input_x + 7), (input_y + 100));
    EVE_VERTEX2F((input_x + 42), input_y);
    // reset alpha
    EVE_COLOR_A(255);

    // stencil section to fill and add number
    // clear the stencil buffer so no previous values in the effect this operation 
    EVE_CLEAR(0, 1, 0);

    // disable colours
    EVE_COLOR_MASK(0, 0, 0, 1);
    // increment the stencil
    EVE_STENCIL_OP(EVE_STENCIL_INCR, EVE_STENCIL_INCR);
    // these vertexes draw the fill (first one moves with vertically with the value input)
    EVE_VERTEX2F((input_x + 43), ((input_y + 100) - value) + 1); 
    EVE_VERTEX2F((input_x + 7), (input_y + 100));
    // we only want to add pixels with a alpha value great than 10 to the stencil (predominently for the number)
    EVE_ALPHA_FUNC(EVE_TEST_GEQUAL, 75);
    // draw the number into the middle of the gauge
    EVE_CMD_NUMBER((input_x + 24), (input_y + 50), 28, EVE_OPT_CENTER, value);
    // keep the stencil
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP);
    // re-enable colours
    EVE_COLOR_MASK(1, 1, 1, 0);

    // draw fill (this will colour all pixels where the stenicl = 1)
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 1, 255);
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_VERTEX2F((input_x + 7), (input_y + 0));
    EVE_VERTEX2F((input_x + 42), (input_y + 100));

    // reset scissor
    EVE_SCISSOR_SIZE(2048,2048);
    EVE_SCISSOR_XY(0,0);

    // print text
    EVE_STENCIL_FUNC(EVE_TEST_ALWAYS, 1, 255);
    EVE_COLOR_RGB(255,255,255);
    // can make this relate to input font size if we read the font data table (right now we'll fix the font and offset from Y)
    EVE_CMD_TEXT((input_x + 24), (input_y + 110), 26, EVE_OPT_CENTER, text);

    // restore graphics context
    EVE_RESTORE_CONTEXT();

}

void arc_line_gauge(uint16_t arc_centerx, uint16_t arc_centery, uint16_t arc_radius, uint16_t arc_thickness, uint32_t arc_active_color, uint8_t arc_value_user)
{
    
    // we only want the top half of the circle
    uint16_t arc_min_limit = 90;
    uint16_t arc_max_limit = 270;
    uint16_t arc_range = (arc_max_limit - arc_min_limit);
    // nomralise the reading steps to the 8 bit input value
    uint16_t arc_value = arc_min_limit + (((arc_value_user) * arc_range) / 255);

    // used to determine line widths in arc
    uint8_t line_width = 1;

    // varibles for line indicator position
    int16_t line_start_x = 0;
    int16_t line_start_y = 0;

    int16_t line_end_x = 0;
    int16_t line_end_y = 0;

    // Ensure the value is within limits
    if (arc_value > arc_max_limit)
       arc_value = arc_max_limit;
    if (arc_value < arc_min_limit)
       arc_value = arc_min_limit;

    //--------------------------------------------------------------------------------------------------------
    // Process the angle data which will be used to make a uniform motion of the indicator line across the arc
    //--------------------------------------------------------------------------------------------------------
 
    // Calculate the coordinates of the starting and end points, for the line indicator used to denote current reading
    line_start_x = CIRC_X(((arc_radius + 1) * 16), DEG2FURMAN(arc_value));
    line_start_y = CIRC_Y(((arc_radius + 1) * 16), DEG2FURMAN(arc_value));
    line_end_x = CIRC_X(((arc_radius - arc_thickness - 1) * 16), DEG2FURMAN(arc_value));
    line_end_y = CIRC_Y(((arc_radius - arc_thickness - 1) * 16), DEG2FURMAN(arc_value));

    //--------------------------------------------------------------------------------------------------------
    // Write to the alpha buffer and disable writing colours to the screen to make an invisible arc, 
    // add to stencil values for shapes, as we are going to use alpha blending and stenciling here 
    //--------------------------------------------------------------------------------------------------------

    // save current graphics context
    EVE_SAVE_CONTEXT();

    // scissor so we only get the top half of the circle
    EVE_SCISSOR_SIZE((arc_radius * 2) + 1, (arc_radius) + 1);
    EVE_SCISSOR_XY((arc_centerx - arc_radius), (arc_centery - arc_radius));

    // set desried pixel precision format
    // EVE_VERTEX_FORMAT(4);
 
    // disable all colours, leave alpha enabled
    EVE_COLOR_MASK(0, 0, 0, 1);
    // clear colour, set full alpha
    EVE_CLEAR(1, 0, 0);
    EVE_COLOR_A(255);

    // increment the stencil for all the shapes drawn here
    EVE_STENCIL_OP(EVE_STENCIL_INCR, EVE_STENCIL_INCR);

    // these inital shapes we dont want in the alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ZERO,EVE_BLEND_ZERO);
 
    // begin drawing circles for our arc
    //-------------------------------------------------------------------------
    EVE_BEGIN(EVE_BEGIN_POINTS);
    // outer arc edge
    EVE_POINT_SIZE(arc_radius * 16);
    EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));
    // inner arc edge
    EVE_POINT_SIZE((arc_radius - arc_thickness) * 16);
    EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));

    // now we want to use alpha belnding so we can create a nice undler line for the arc
    // add to alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE((arc_radius - arc_thickness + line_width) * 16);
    EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));
 
    // remove from alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ZERO, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE((arc_radius - arc_thickness - line_width) * 16);
    EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));

    // stop incrementing the stencil for all the shapes drawn here
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP);

    // re-enable colours and alpha
    EVE_COLOR_MASK(1, 1, 1, 1);

    // set blend function to beln in underline
    EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_ONE);
    EVE_COLOR_RGB(255,255,255); //can set colour here 
    EVE_POINT_SIZE(arc_radius * 16);
    EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));

    // set blend function to default
    EVE_BLEND_FUNC(EVE_BLEND_SRC_ALPHA, EVE_BLEND_ONE_MINUS_SRC_ALPHA);

    // use the stencil fucntion and the shapes we created in the stencil earlier to fill the main section of the arc
    EVE_STENCIL_FUNC(EVE_TEST_LEQUAL, 2, 255);
    EVE_COLOR_A(75); //can set the alpha for this here
    EVE_COLOR_RGB(((uint8_t)(arc_active_color >> 16)), ((uint8_t)(arc_active_color >> 8)), ((uint8_t)(arc_active_color)));
    EVE_POINT_SIZE(arc_radius * 16);
    EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));

    // restore previous graphics context
    // doing this here should reset the stencil, alpha values and scissor function
    EVE_RESTORE_CONTEXT();

    //--------------------------------------------------------------------------------------------------------
    // draw indicator lines on top of our rendered arc
    //--------------------------------------------------------------------------------------------------------

    // add marker lines for start, middle, and end of arc
    // set desried vertex format
    EVE_BEGIN(EVE_BEGIN_LINES);
    // set line width
    EVE_LINE_WIDTH(line_width * 16);
    EVE_COLOR_RGB(255,255,255); //can set line colour here, could feed a var in to set bot these and underline colour
    // left side
    EVE_VERTEX2F(((arc_centerx - arc_radius) * 16), (arc_centery * 16));
    EVE_VERTEX2F(((arc_centerx - arc_radius + arc_thickness) * 16) , (arc_centery * 16 ));
    // right side
    EVE_VERTEX2F(((arc_centerx + arc_radius) * 16), (arc_centery * 16));
    EVE_VERTEX2F(((arc_centerx + arc_radius - arc_thickness) * 16), (arc_centery * 16 ));
    // top
    EVE_VERTEX2F((arc_centerx * 16), ((arc_centery - arc_radius) * 16));
    EVE_VERTEX2F((arc_centerx * 16), ((arc_centery - arc_radius + arc_thickness) * 16));

    // add in current reading line
    //-------------------------------------------------------------------------

    // make this a half line width thicker
    EVE_LINE_WIDTH((line_width * 16) + 8); 
    EVE_COLOR_RGB(238, 0, 0); //can set line colour here, could feed a var in to set this
    // add indicator line
    EVE_VERTEX2F((arc_centerx * 16) - (line_start_x), (arc_centery * 16) + (line_start_y));
    EVE_VERTEX2F((arc_centerx * 16) - (line_end_x), (arc_centery * 16) + (line_end_y));

    // restore previous graphics context
    EVE_RESTORE_CONTEXT();
 
}

void steeringAngle(uint16_t input_x, uint16_t input_y, EVE_ASSET_PROPS *image, uint8_t value){

    //total width 72 , height 36
    //----------------------------------------

    // save graphics context
    EVE_SAVE_CONTEXT(); 
    
    // add arc line gauge into widget
    arc_line_gauge(input_x, input_y, 36, 12, 0xEEEE00, value);

    // draw widget bitmap in the centre of the arc
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(image->Handle);

#ifdef STEERING_ROTATE // not normally defined

    EVE_CMD_LOADIDENTITY();
    // centre roatation in the middle of the image, use value - 128 (0 full left lock, 255 full right lock) to determine rotation angle
    // scale to 95% orignal size
    EVE_CMD_ROTATEAROUND((image->Width/2), (image->Height/2), DEG2FURMAN((value - 128)), ((65536 * 95)/100));
    // Configure transform matrix of bitmap
    EVE_CMD_SETMATRIX();

#endif // STEERING_ROTATE 

    // place bitmap on screen
    EVE_VERTEX2F(((input_x - (image->Width/2)) * 16), ((input_y - (image->Height/2)) * 16));

    // restore graphics context
    EVE_RESTORE_CONTEXT();

}

void arc_point_gauge(uint16_t arc_centerx, uint16_t arc_centery, uint16_t arc_radius, uint16_t arc_thickness, uint32_t arc_active_color, uint8_t arc_value_user)
{
    
    uint16_t arc_min_limit = 35;
    uint16_t arc_max_limit = 146;
    uint16_t arc_range = (arc_max_limit - arc_min_limit);
    uint16_t arc_value = arc_min_limit + (((arc_value_user) * arc_range) / 255);

    // if this is 0 the point shadow will be the same size as the arc_thickness (increment this value to grow the shadow around the indicator point)
    uint8_t shadow_size = 2;
 
    int16_t arc_active_x = 0;
    int16_t arc_active_y = 0;
 
    int16_t arc_start_x = 0;
    int16_t arc_start_y = 0;
 
    int16_t arc_end_x = 0;
    int16_t arc_end_y = 0;

    int16_t point_start_x = 0;
    int16_t point_start_y = 0;

    int16_t point_end_x = 0;
    int16_t point_end_y = 0;

    // Ensure the value is within limits
    if (arc_value > arc_max_limit)
       arc_value = arc_max_limit;
    if (arc_value < arc_min_limit)
       arc_value = arc_min_limit;

    // ensure that the arc thickness is even number so we can simplify the maths later for positining the end points and value indicator points
    if((arc_thickness % 2) != 0)
        arc_thickness ++;

    //--------------------------------------------------------------------------------------------------------
    // Process the angle data which will be used to make a uniform motion of the arc
    //--------------------------------------------------------------------------------------------------------
 
    // Calculate the coordinates of the starting point, the gauge arc and the point at the tip of the arc
 
    // for the arc gauge value itself
    // we want to draw this in the middle of the arc so we take the width and divide it by 2, then negate this from the raidus
    // multiply by 16 so we can feed this number directly int the VERTEX2F command with our desired pixel precision
    arc_active_x = CIRC_X(((arc_radius - (arc_thickness/2)) * 16), DEG2FURMAN(arc_value));
    arc_active_y = CIRC_Y(((arc_radius - (arc_thickness/2)) * 16), DEG2FURMAN(arc_value));
    
    // for the starting angle 
    arc_start_x = CIRC_X((arc_radius * 32), DEG2FURMAN(arc_min_limit));
    arc_start_y = CIRC_Y((arc_radius * 32), DEG2FURMAN(arc_min_limit));
 
    // for the finishing angle 
    arc_end_x = CIRC_X((arc_radius * 32), DEG2FURMAN(arc_max_limit));
    arc_end_y = CIRC_Y((arc_radius * 32), DEG2FURMAN(arc_max_limit));

    // for the rounded ends
    point_start_x = CIRC_X(((arc_radius - (arc_thickness/2)) * 16), DEG2FURMAN(arc_min_limit));
    point_start_y = CIRC_Y(((arc_radius - (arc_thickness/2)) * 16), DEG2FURMAN(arc_min_limit));
    point_end_x = CIRC_X(((arc_radius - (arc_thickness/2)) * 16), DEG2FURMAN(arc_max_limit));
    point_end_y = CIRC_Y(((arc_radius - (arc_thickness/2)) * 16), DEG2FURMAN(arc_max_limit));

    //--------------------------------------------------------------------------------------------------------
    // Write to the alpha buffer and disable writing colours to the screen to make an invisible arc
    //--------------------------------------------------------------------------------------------------------

    // save current graphics context
    EVE_SAVE_CONTEXT();

    // scissor to area of guage
    EVE_SCISSOR_SIZE((arc_radius * 2) + 1, (arc_radius * 2) + 1);
    EVE_SCISSOR_XY((arc_centerx - arc_radius), (arc_centery - arc_radius));

    // set desried pixel precision format
    //EVE_VERTEX_FORMAT(4);
 
    // disable colours, but leave alpha enabled
    EVE_COLOR_MASK(0, 0, 0, 1);
    // clear colour and set alpha
    EVE_CLEAR(1, 0, 0);
    EVE_COLOR_A(255);
 
    // begin drawing circles for our arc
    //-------------------------------------------------------------------------
    EVE_BEGIN(EVE_BEGIN_POINTS);
    // add to alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE(arc_radius * 16);
    EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));
 
    // remove from alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ZERO, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE((arc_radius - arc_thickness) * 16);
    EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));
    
    // edge strips to ensure nothing outwith the arc is coloured
    //-------------------------------------------------------------------------
    // adjust edge strip draw shape based on input angles
    if(arc_min_limit >= 135){
        EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_L);
        EVE_VERTEX2F((arc_centerx * 16), ((arc_centery + arc_radius) * 16));
    }
    else
        EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_B);

    EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));
    EVE_VERTEX2F(((arc_centerx * 16) - arc_start_x), ((arc_centery * 16) + arc_start_y));
    

    // adjust edge strip draw based on input angles
    if(arc_max_limit <= 225){
        EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_R);
        EVE_VERTEX2F((arc_centerx * 16), ((arc_centery + arc_radius) * 16));
    }
    else
        EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_B);
    
    EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));
    EVE_VERTEX2F(((arc_centerx * 16) - arc_end_x), ((arc_centery * 16) + arc_end_y));

    // if we need to add some rectangles to cover the areas that are missed when swapping from a bottom to a right or left edge strip:
    if(arc_min_limit >= 135){
        EVE_BEGIN(EVE_BEGIN_RECTS);
        EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));
        EVE_VERTEX2F(((arc_centerx - arc_radius) * 16), ((arc_centery + arc_radius) * 16));
    }
    if(arc_max_limit <= 225){
        EVE_BEGIN(EVE_BEGIN_RECTS);
        EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));
        EVE_VERTEX2F(((arc_centerx + arc_radius) * 16), ((arc_centery + arc_radius) * 16));
    }

    // add final shapes for rounded ends
    //-------------------------------------------------------------------------

    EVE_BEGIN(EVE_BEGIN_POINTS);
    // add to alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE((arc_thickness/2) * 16);
    // left side
    EVE_VERTEX2F(((arc_centerx * 16) - point_start_x) , ((arc_centery * 16) + point_start_y));
    // right side
    EVE_VERTEX2F(((arc_centerx * 16) - point_end_x) , ((arc_centery * 16) + point_end_y));
 
    // these circles are used to indicate current value;
    //-------------------------------------------------------------------------
    // remove from alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ZERO, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE(((arc_thickness/2) + shadow_size) * 16); //guage thickness + shadow_size increase
 
    // draw point based on current input value
    EVE_VERTEX2F((arc_centerx * 16) - (arc_active_x), (arc_centery * 16) + (arc_active_y));
 
    // add to alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_POINT_SIZE((arc_thickness/3) * 16); //this should be slighlty smaller than the guage thickness
 
    // draw point based on current input value
    EVE_VERTEX2F((arc_centerx * 16) - (arc_active_x), (arc_centery * 16) + (arc_active_y));
 
    //--------------------------------------------------------------------------------------------------------
    // Draw a circle which will fill the arc
    //--------------------------------------------------------------------------------------------------------
 
    // re-enable colours
    EVE_COLOR_MASK(1, 1, 1, 0);
    // blend in colour
    EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_ONE_MINUS_DST_ALPHA);
    // colour based on input colour
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_COLOR_RGB(((uint8_t)(arc_active_color >> 16)), ((uint8_t)(arc_active_color >> 8)), ((uint8_t)(arc_active_color)));
    EVE_POINT_SIZE(arc_radius * 16);
    EVE_VERTEX2F((arc_centerx * 16), (arc_centery * 16));
 
    //--------------------------------------------------------------------------------------------------------
    // Clean up to avoid affecting other items later in the display list
    //--------------------------------------------------------------------------------------------------------

    // restore previous graphics context
    EVE_RESTORE_CONTEXT();
 
}

void elevationWidget(uint16_t input_x, uint16_t input_y, uint8_t max, uint8_t min, uint8_t value){

    // total width 90, height 90
    //----------------------------------------

    // save graphics context
    EVE_SAVE_CONTEXT();

    // draw outline box
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_COLOR_RGB(0, 0, 0); // black
    EVE_COLOR_A(125);
    EVE_LINE_WIDTH(80); // 5
    EVE_VERTEX2F((input_x + 5) * 16, (input_y + 5) * 16);
    EVE_VERTEX2F((input_x + 85) * 16, (input_y + 85) * 16);

    // draw mountain
    EVE_COLOR_A(255);
    EVE_COLOR_RGB(255, 255, 255);
    EVE_LINE_WIDTH(16); // 1
    // outline
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    EVE_VERTEX2F((input_x + 35) * 16, (input_y + 60) * 16);
    EVE_VERTEX2F((input_x + 50) * 16, (input_y + 34) * 16);
    EVE_VERTEX2F((input_x + 56) * 16, (input_y + 40) * 16);
    EVE_VERTEX2F((input_x + 64) * 16, (input_y + 26) * 16);
    EVE_VERTEX2F((input_x + 80) * 16, (input_y + 60) * 16);
    EVE_VERTEX2F((input_x + 35) * 16, (input_y + 60) * 16);

    // snow tips
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    EVE_VERTEX2F((input_x + 61) * 16, (input_y + 40) * 16);
    EVE_VERTEX2F(((input_x + 63) * 16) + 8, ((input_y + 34) * 16) + 8); // need 0.5 addition here on X/Y
    EVE_VERTEX2F((input_x + 66) * 16, (input_y + 40) * 16); 
    EVE_BEGIN(EVE_BEGIN_LINE_STRIP);
    EVE_VERTEX2F((input_x + 49) * 16, (input_y + 43) * 16);
    EVE_VERTEX2F((input_x + 51) * 16, (input_y + 40) * 16); 
    EVE_VERTEX2F(((input_x + 53) * 16) + 8, (input_y + 43) * 16); // need 0.5 addition here on X

    // add max/min elevation text
    EVE_COLOR_RGB(175,175,175);
    EVE_CMD_TEXT((input_x + 43), (input_y + 5), 20, EVE_OPT_FORMAT, "%d (m)", max);
    EVE_CMD_TEXT((input_x + 43) , (input_y + 71), 20, EVE_OPT_FORMAT, "%d (m)", min);

    // add arc gauge into widget    
    arc_point_gauge((input_x + 53), (input_y + 45), 45, 10, 0xFFFFFF, value);

    // restore graphics context
    EVE_RESTORE_CONTEXT();

}

void tyreTemps(uint16_t input_x, uint16_t input_y, uint8_t img_handle, uint8_t cold_threshold, uint8_t hot_threshold, uint8_t left_front, uint8_t left_rear, uint8_t right_front, uint8_t right_rear){

    //total width 200, height 90
    //----------------------------------------

    // save graphics context
    EVE_SAVE_CONTEXT();

    // widget outline shape
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_COLOR_RGB(0, 0, 0); // black
    EVE_COLOR_A(120);
    EVE_LINE_WIDTH(80); // 5
    EVE_VERTEX2F((input_x + 5) * 16, (input_y + 5) * 16);
    EVE_VERTEX2F((input_x + 195) * 16, (input_y + 85) * 16);

    // individual tyre temp boxes
    EVE_COLOR_A(150);
    // LF
    if(left_front < cold_threshold){
        EVE_COLOR_RGB(0,190,255);// blue
    }
    else if(left_front > hot_threshold){
        EVE_COLOR_RGB(255,0,0);// Red
    }
    else{
        EVE_COLOR_RGB(50,50,50);// grey
    }

    EVE_VERTEX2F((input_x + 15) * 16, (input_y + 15) * 16);
    EVE_VERTEX2F((input_x + 65) * 16, (input_y + 35) * 16);

    // LR
    if(left_rear < cold_threshold){
        EVE_COLOR_RGB(0,190,255);// blue
    }
    else if(left_rear > hot_threshold){
        EVE_COLOR_RGB(255,0,0);// Red
    }
    else{
        EVE_COLOR_RGB(50,50,50);// grey
    }
    EVE_VERTEX2F((input_x + 15) * 16, (input_y + 55) * 16);
    EVE_VERTEX2F((input_x + 65) * 16, (input_y + 75) * 16);

    // LF
    if(right_front < cold_threshold){
        EVE_COLOR_RGB(0,190,255);// blue
    }
    else if(right_front > hot_threshold){
        EVE_COLOR_RGB(255,0,0);// Red
    }
    else{
        EVE_COLOR_RGB(50,50,50);// grey
    }

    EVE_VERTEX2F((input_x + 135) * 16, (input_y + 15) * 16);
    EVE_VERTEX2F((input_x + 185) * 16, (input_y + 35) * 16);

    // RR
    if(right_rear < cold_threshold){;
        EVE_COLOR_RGB(0,190,255);// blue
    }
    else if(right_rear > hot_threshold){
        EVE_COLOR_RGB(255,0,0);// Red
    }
    else{
        EVE_COLOR_RGB(50,50,50);// grey
    }

    EVE_VERTEX2F((input_x + 135) * 16, (input_y + 55) * 16);
    EVE_VERTEX2F((input_x + 185) * 16, (input_y + 75) * 16);

    // reset colour and alpha
    EVE_COLOR_RGB(255, 255, 255);
    EVE_COLOR_A(255);
    // draw icon for tyres 
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(img_handle);
    EVE_VERTEX2F((input_x + 80) * 16, (input_y + 5) * 16);

    // print numbers
    // print left front reading
    EVE_CMD_NUMBER((input_x + 40), (input_y + 25), 30, EVE_OPT_CENTER, left_front);

    // print left rear reading
    EVE_CMD_NUMBER((input_x + 40), (input_y + 65), 30, EVE_OPT_CENTER, left_rear);

    // print right rear reading
    EVE_CMD_NUMBER((input_x + 160), (input_y + 25), 30, EVE_OPT_CENTER, right_front);

    // print right rear reading
    EVE_CMD_NUMBER((input_x + 160), (input_y + 65), 30, EVE_OPT_CENTER, right_rear);

    // restore context
    EVE_RESTORE_CONTEXT();
}

void sectorWidget(uint16_t center_x, uint16_t center_y, uint16_t radius, uint16_t thickness, uint8_t font_handle, uint16_t sector_one_end, uint16_t sector_two_end, uint8_t current_sector, uint16_t arc_value_user)
{
    // Varibles for use in function
    uint16_t arc_min_limit = 0;
    uint16_t arc_max_limit = 360;
    uint16_t arc_value = arc_value_user;

    int16_t arc_active_x = 0;
    int16_t arc_active_y = 0;

    int16_t sector_one_x = 0;
    int16_t sector_one_y = 0;

    int16_t sector_two_x = 0;
    int16_t sector_two_y = 0;

    int16_t sector_one_angle = sector_one_end;
    int16_t sector_two_angle = sector_two_end;
 
    //--------------------------------------------------------------------------------------------------------
    // Process the angle data which will be used to make a uniform motion of the arc
    //--------------------------------------------------------------------------------------------------------

    // Ensure the value is within limits
    if (arc_value > arc_max_limit)
       arc_value = arc_max_limit;
    if (arc_value < arc_min_limit)
       arc_value = arc_min_limit;

    //---------------------------------------------------------
    // Calculate the coordinates of the active line and sector crossovers, the gauge arc and the point at the tip of the arc

    // for the arc gauge itself
    // same idea as the rev counter lines, where we multiply by 16 here to get a better number and + 8 to round
    arc_active_x = CIRC_X_DEG((radius * 16 + 8), arc_value);
    arc_active_y = CIRC_Y_DEG((radius * 16 + 8), arc_value);

    // for the starting angle (the minimum value of the arc as it is normally not desired to be a full 360 deg circle)
    sector_one_x = CIRC_X_DEG(radius, sector_one_angle);
    sector_one_y = CIRC_Y_DEG(radius, sector_one_angle);

    // for the finishing angle (the maximum value of the arc as it is normally not desired to be a full 360 deg circle)
    sector_two_x = CIRC_X_DEG(radius, sector_two_angle);
    sector_two_y = CIRC_Y_DEG(radius, sector_two_angle);

    //--------------------------------------------------------------------------------------------------------
    // Write to the stencil buffer and disable writing to the screen to make an invisible arc
    //--------------------------------------------------------------------------------------------------------

    // save graphics context
    EVE_SAVE_CONTEXT();

    // stencil the area of the widget
    EVE_SCISSOR_SIZE(((radius * 2) + 12), ((radius * 2) + 12)); // + 2 * line width of background box
    EVE_SCISSOR_XY(((center_x - radius) - 6), ((center_y - radius) - 6)); // top left (shift due to line width of background box)

    // draw background box
    EVE_COLOR_RGB(0,0,0);
    EVE_LINE_WIDTH(80); //5
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_COLOR_A(120);
    EVE_VERTEX2F(((center_x - radius) *16), ((center_y + radius) * 16)); // plus the line width used above
    EVE_VERTEX2F(((center_x + radius) *16), ((center_y - radius) * 16)); // minus the line width used above
    

    // firstly we want ot paint the fill and outline shapes into the alpah buffer, and use one stencil
    // disable all colours bar the alpha channel
    EVE_COLOR_MASK(0, 0, 0, 1);
    // clear stencil and colour
    EVE_CLEAR(1,1,0);

    // we want to add the fill into the alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    EVE_COLOR_A(255);
    //--------------------------------------------------------------------------------------------------------
    // Draw the edge strips ( or circle) which will fill in the arc
    //--------------------------------------------------------------------------------------------------------

    if (arc_value >= 360){

        EVE_BEGIN(EVE_BEGIN_POINTS);
        EVE_POINT_SIZE(radius * 16);
        EVE_VERTEX2F(((center_x) * 16), (center_y) * 16);

    }else{
        // These are drawn per quadrant as each edge strip will only work well on an angle up to 90 deg
        // 0 - 89 Deg
        if ((arc_value > 0) && (arc_value < 90))
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_B);
            EVE_VERTEX2F(((center_x) * 16), (center_y) * 16);
            EVE_VERTEX2F(((center_x * 16) - arc_active_x ), (center_y * 16) + arc_active_y);
        }
        else if (arc_value > 0){
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_B);
            EVE_VERTEX2F(((center_x) * 16), (center_y) * 16);
            EVE_VERTEX2F(((center_x - radius) * 16), (center_y) * 16);
        }

        // 90 - 179 deg
        if ((arc_value >= 90) && (arc_value < 180))
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_L);
            EVE_VERTEX2F(((center_x) * 16), (center_y) * 16);
            EVE_VERTEX2F(((center_x * 16) - arc_active_x ), (center_y * 16) + arc_active_y);
        }
        else if (arc_value > 90)
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_L);
            EVE_VERTEX2F(((center_x) * 16), (center_y) * 16);
            EVE_VERTEX2F(((center_x) * 16), (center_y - radius) * 16);
        }

        // 180 - 269 deg
        if ((arc_value >= 180) && (arc_value < 270))
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_A);
            EVE_VERTEX2F(((center_x - 1) * 16), (center_y) * 16);
            EVE_VERTEX2F(((center_x * 16) - arc_active_x), (center_y * 16) + arc_active_y);
        }
        else if (arc_value > 180)
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_A);
            EVE_VERTEX2F(((center_x - 1) * 16), (center_y) * 16);
            EVE_VERTEX2F(((center_x + radius + 1) * 16), (center_y) * 16);
        }

        // 270 - 359 deg
        if ((arc_value >= 270) && (arc_value < 360))
        {
            // Edge strip to draw the arc
            EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_R);
            EVE_VERTEX2F(((center_x) * 16), (center_y - 1) * 16);
            EVE_VERTEX2F((center_x * 16) - arc_active_x, (center_y * 16) + arc_active_y);
        }
    } 


    // draw the outer radius here and blend destination alpha to source alpha to maintain the aliased edge of the circle we are going to draw for the outline
    EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_SRC_ALPHA);
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE(radius * 16);
    EVE_VERTEX2F(((center_x) * 16), (center_y) * 16);

    //--------------------------------------------------------------------------------------------------------
    // Draw the outline section for the arc
    //--------------------------------------------------------------------------------------------------------

    // add a stencil value to the next shape so we can use this later to prevent overdraw
    EVE_STENCIL_OP(EVE_STENCIL_INCR, EVE_STENCIL_INCR);
    // set the alpha here to a value for the unfilled section of the arc
    EVE_COLOR_A(64);
    // add to alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ONE, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    // draw outer circle edge
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE(radius * 16);
    EVE_VERTEX2F(((center_x) * 16), (center_y) * 16);
    // stop the stencil incrementing
    EVE_STENCIL_OP(EVE_STENCIL_KEEP, EVE_STENCIL_KEEP);

    // draw the hollow centre circle and remove its value from the alpha buffer
    EVE_COLOR_A(255);
    // add to alpha buffer
    EVE_BLEND_FUNC(EVE_BLEND_ZERO, EVE_BLEND_ONE_MINUS_SRC_ALPHA);
    // draw outer circle edge
    EVE_BEGIN(EVE_BEGIN_POINTS);
    EVE_POINT_SIZE((radius - thickness) * 16);
    EVE_VERTEX2F(((center_x) * 16), (center_y) * 16);

    //--------------------------------------------------------------------------------------------------------
    // Add some lines to show where the sector cross-over points are
    //--------------------------------------------------------------------------------------------------------

    // draw sector threshold lines
    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_LINE_WIDTH(16);
    // sector one line
    EVE_VERTEX2F(((center_x - sector_one_x)  * 16), ((center_y + sector_one_y) * 16));
    EVE_VERTEX2F((center_x * 16), (center_y  * 16));
    // sector two line
    EVE_VERTEX2F(((center_x - sector_two_x)  * 16), ((center_y + sector_two_y) * 16));
    EVE_VERTEX2F((center_x * 16), (center_y * 16));

    //--------------------------------------------------------------------------------------------------------
    // Draw edge strips to colour the desired sections of the arc
    //--------------------------------------------------------------------------------------------------------

    // only draw where the stencil equals one to preven overdraw of the outline
    EVE_STENCIL_FUNC(EVE_TEST_EQUAL, 1, 255);
    // re-enable colours, but disable alpha
    EVE_COLOR_MASK(1,1,1,0);

    // blend the following shapes colours into the alpha buffer values we have defined earlier
    EVE_BLEND_FUNC(EVE_BLEND_DST_ALPHA, EVE_BLEND_ONE_MINUS_DST_ALPHA);

    // Draw edge strip for the first sector
    EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_L);
    EVE_COLOR_RGB(255,0,0); //red
    EVE_VERTEX2F(((center_x - sector_one_x) * 16), ((center_y + sector_one_y) * 16));
    EVE_VERTEX2F((center_x * 16),(center_y * 16));
    // shift x by one so that when value = 1 it hits this sector
    EVE_VERTEX2F(((center_x + 1)  * 16),((center_y + radius + 1) * 16));

    // Draw edge strip for the second sector
    EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_A);
    EVE_COLOR_RGB(0,190,255); //blue
    EVE_VERTEX2F(((center_x - sector_one_x) * 16), ((center_y + sector_one_y) * 16));
    EVE_VERTEX2F(((center_x + 1 )* 16), (center_y * 16));
    // shift x by one so it doesnt touch the first edge strip
    EVE_VERTEX2F(((center_x - sector_two_x + 1) * 16), ((center_y + sector_two_y) * 16));

    // Draw edge strip for the third sector
    EVE_BEGIN(EVE_BEGIN_EDGE_STRIP_R);
    EVE_COLOR_RGB(225,225,0); //yellow
    EVE_VERTEX2F(((center_x - sector_two_x) * 16), ((center_y + sector_two_y) * 16));
    EVE_VERTEX2F((center_x * 16),(center_y * 16));
    // shift x by one so it doesnt touch the first edge strip
    EVE_VERTEX2F(((center_x + 1) * 16),((center_y + radius + 1) * 16));


    //--------------------------------------------------------------------------------------------------------
    // Add start/finish line on top of arc, and print sector number
    //--------------------------------------------------------------------------------------------------------
    
    // Revert to always drawing for the subsequent items
    EVE_STENCIL_FUNC(EVE_TEST_ALWAYS, 0, 255); 
    // set the belnd function back to the default
    EVE_BLEND_FUNC(EVE_BLEND_SRC_ALPHA, EVE_BLEND_ONE_MINUS_SRC_ALPHA); 

    // add point in the middle of the gauge that the number will print on top of
    EVE_BEGIN(EVE_BEGIN_POINTS);
    // colour this white
    EVE_COLOR_RGB(255,255,255);
    EVE_POINT_SIZE((radius - (thickness * 2)) * 16);
    EVE_VERTEX2F((center_x * 16), (center_y * 16));

    // black
    EVE_COLOR_RGB(0,0,0);
    EVE_POINT_SIZE((radius - (thickness * 2) - 2) * 16);
    EVE_VERTEX2F((center_x * 16), (center_y * 16));

    EVE_BEGIN(EVE_BEGIN_LINES);
    EVE_COLOR_RGB(255,255,255);
    // draw start finish line marker 
    EVE_LINE_WIDTH(16); //1
    EVE_VERTEX2F((center_x * 16), ((center_y + radius - thickness) * 16));
    EVE_VERTEX2F((center_x  * 16), (center_y + radius) * 16);

    //finally add sector number
    EVE_CMD_NUMBER(center_x, center_y, font_handle, EVE_OPT_CENTER, current_sector);

    //--------------------------------------------------------------------------------------------------------
    // Clean up to avoid affecting other items later in the display list
    //--------------------------------------------------------------------------------------------------------

    EVE_CLEAR(0, 1, 0);              // Clear the stencil buffer to ensure it does not affect other items on the screen

    // restore graphics context
    EVE_RESTORE_CONTEXT();
}

void lapAndSectorTimes(uint16_t input_x, uint16_t input_y, uint32_t color_sector_1, uint32_t color_sector_2, uint32_t sector_time_1, uint32_t sector_time_2, uint32_t laptime){

    //total width 200, height 90
    //----------------------------------------

    // variables to take the input laptime in milliseconds and translate this into min/sec/msec
    uint32_t total_milliseconds = laptime; 
    // Calculate minutes
    uint8_t minutes = total_milliseconds / 60000;
    // Calculate remaining milliseconds after minutes
    uint32_t remaining_ms_after_minutes = total_milliseconds % 60000;
    // Calculate seconds from the remaining milliseconds
    uint8_t seconds = remaining_ms_after_minutes / 1000;
    // Calculate final remaining milliseconds
    uint16_t milliseconds = remaining_ms_after_minutes % 1000;


    //--------------------------------------------------------------------------------------------------------
    // Draw background and fill shapes
    //--------------------------------------------------------------------------------------------------------

    // save context
    EVE_SAVE_CONTEXT();

    // draw outline box
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_COLOR_RGB(0, 0, 0); // black
    EVE_COLOR_A(120);
    EVE_LINE_WIDTH(80); // 5
    EVE_VERTEX2F((input_x + 5) * 16, (input_y + 5) * 16);
    EVE_VERTEX2F((input_x + 195) * 16, (input_y + 85) * 16);

    // draw some boxes that will be used to indicate sector time positive or negative deltas
    // if we dont currentl have a time, just draw a normal shadow box
    // check we have a valid time to check if we want to draw this box
    EVE_COLOR_A(150);
    if (sector_time_1 > 0){   
        // sector 1
        EVE_COLOR_RGB(((uint8_t)(color_sector_1 >> 16)), ((uint8_t)(color_sector_1 >> 8)), ((uint8_t)(color_sector_1)));
        EVE_VERTEX2F((input_x + 47) * 16, (input_y + 65) * 16);
        EVE_VERTEX2F((input_x + 87) * 16, (input_y + 76) * 16);
    }else{
        // else just draw a shadow box
        // sector 1
        EVE_COLOR_RGB(50,50,50); //grey
        EVE_VERTEX2F((input_x + 47) * 16, (input_y + 65) * 16);
        EVE_VERTEX2F((input_x + 87) * 16, (input_y + 76) * 16);
    }
    if(sector_time_2 > 0){
        // dont need to add a COLOR_A command here, becuase sector 1 will always be valid before sector 2 so the COLOR_A will have already been inserted
        // sector 2
        EVE_COLOR_RGB(((uint8_t)(color_sector_2 >> 16)), ((uint8_t)(color_sector_2 >> 8)), ((uint8_t)(color_sector_2)));
        EVE_VERTEX2F((input_x + 141) * 16, (input_y + 65) * 16);
        EVE_VERTEX2F((input_x + 181) * 16, (input_y + 76) * 16);
    }else{
        // else just draw a shadow box
        // sector 1
        EVE_COLOR_RGB(50,50,50);//grey
        EVE_VERTEX2F((input_x + 141) * 16, (input_y + 65) * 16);
        EVE_VERTEX2F((input_x + 181) * 16, (input_y + 76) * 16);
    }

    // draw an outline for the total laptimes 
    EVE_COLOR_RGB(50,50,50);// grey
    EVE_VERTEX2F((input_x + 14) * 16, (input_y + 14) * 16);
    EVE_VERTEX2F((input_x + 186) * 16, (input_y + 46) * 16);

    //--------------------------------------------------------------------------------------------------------
    // Add times into the widget
    //--------------------------------------------------------------------------------------------------------

    EVE_COLOR_A(255);
    EVE_COLOR_RGB(255,255,255); // white

    // add overall laptime
    EVE_CMD_TEXT((input_x + 100), (input_y + 30), 31, EVE_OPT_CENTER | EVE_OPT_FORMAT, "%d:%.2d.%.3d", minutes, seconds, milliseconds);

    // add sector times, but only if there is a valid sector time, if there isnt then print dash marks
    
    //----------------------------------------
    // sector 1
    // add label
    EVE_CMD_TEXT((input_x + 18), (input_y + 63), 26, EVE_OPT_FORMAT, "S1: ");
    // if we have a valid time
    if(sector_time_1 > 0){
        // re-use prevsiously declared variables, esnure we do this after the total lap time has been printed
        total_milliseconds = sector_time_1; 
        // Calculate minutes
        minutes = total_milliseconds / 60000;
        // Calculate remaining milliseconds after minutes
        remaining_ms_after_minutes = total_milliseconds % 60000;
        // Calculate seconds from the remaining milliseconds
        seconds = remaining_ms_after_minutes / 1000;
        // Calculate final remaining milliseconds
        milliseconds = remaining_ms_after_minutes % 1000;

        EVE_CMD_TEXT((input_x + 46), (input_y + 62), 21, EVE_OPT_FORMAT, "%.2d.%.3d", seconds, milliseconds);

    }
    else{
        // manually draw dashed indicators to signify there isnt a valid time
        // these are placed mnaullay to account for the differen in font widths for the numbers and dash symbol
        EVE_BEGIN(EVE_BEGIN_BITMAPS);
        EVE_BITMAP_HANDLE(21); // font 21
        EVE_CELL(45); // ASCII value for (-)
        EVE_VERTEX2F((input_x + 48) * 16, (input_y + 62) * 16);
        EVE_VERTEX2F((input_x + 56) * 16, (input_y + 62) * 16);
        EVE_VERTEX2F((input_x + 67) * 16, (input_y + 62) * 16);
        EVE_VERTEX2F((input_x + 75) * 16, (input_y + 62) * 16);
        EVE_VERTEX2F((input_x + 83) * 16, (input_y + 62) * 16);
        EVE_CELL(46);// ASCII value for (.)
        EVE_VERTEX2F((input_x + 62) * 16, (input_y + 62) * 16);
    }

    //----------------------------------------
    // sector 2
    // add label
    EVE_CMD_TEXT((input_x + 112), (input_y + 63), 26, EVE_OPT_FORMAT, "S2: ");
    // if we have a valid time
    if(sector_time_2 > 0){
        // re-use prevsiously declared variables, esnure we do this after the sector 1 time has been printed
        total_milliseconds = sector_time_2; 
        // Calculate minutes
        minutes = total_milliseconds / 60000;
        // Calculate remaining milliseconds after minutes
        remaining_ms_after_minutes = total_milliseconds % 60000;
        // Calculate seconds from the remaining milliseconds
        seconds = remaining_ms_after_minutes / 1000;
        // Calculate final remaining milliseconds
        milliseconds = remaining_ms_after_minutes % 1000;

        EVE_CMD_TEXT((input_x + 182), (input_y + 62), 21, EVE_OPT_RIGHTX | EVE_OPT_FORMAT, "%.2d.%.3d", seconds, milliseconds);

    }else
    {
        // manually draw dashed indicators to signify there isnt a valid time
        // these are placed mnaullay to account for the differen in font widths for the numbers and dash symbol
        EVE_BEGIN(EVE_BEGIN_BITMAPS);
        EVE_BITMAP_HANDLE(21); // font 21
        EVE_CELL(45); // ASCII value for (-)
        EVE_VERTEX2F((input_x + 142) * 16, (input_y + 62) * 16);
        EVE_VERTEX2F((input_x + 150) * 16, (input_y + 62) * 16);
        EVE_VERTEX2F((input_x + 161) * 16, (input_y + 62) * 16);
        EVE_VERTEX2F((input_x + 169) * 16, (input_y + 62) * 16);
        EVE_VERTEX2F((input_x + 177) * 16, (input_y + 62) * 16);
        EVE_CELL(46);// ASCII value for (.)
        EVE_VERTEX2F((input_x + 156) * 16, (input_y + 62) * 16);

    }

    // restore grpahics context
    EVE_RESTORE_CONTEXT();

}

void batteryIndicator(uint16_t input_x, uint16_t input_y, uint8_t img_handle, uint32_t color, uint8_t value){

    //total width 90, height 90
    //----------------------------------------

    // save graphics context
    EVE_SAVE_CONTEXT();

    // draw outline
    EVE_BEGIN(EVE_BEGIN_RECTS);
    EVE_COLOR_RGB(0, 0, 0); // black
    EVE_COLOR_A(120);
    EVE_LINE_WIDTH(80); // 5
    EVE_VERTEX2F((input_x + 5) * 16, (input_y + 5) * 16);
    EVE_VERTEX2F((input_x + 85) * 16, (input_y + 85) * 16);

    // draw battery 
    // if the battery isnt full, then we can also draw a shadow of the full battery behind the bitmpap cell we wish to display
    // colour based on input
    EVE_COLOR_RGB(((uint8_t)(color >> 16)), ((uint8_t)(color >> 8)), ((uint8_t)(color)));
    EVE_BEGIN(EVE_BEGIN_BITMAPS);
    EVE_BITMAP_HANDLE(img_handle);

    // if we arent rendering the full cell
    if (value < 95)
    {
        // render shadow bitmap
        EVE_COLOR_A(30);
        // choose the cell that represents the "full" bitmap
        EVE_CELL(19);
        // place bitmap on screen
        EVE_VERTEX2F((input_x + 26) * 16, (input_y + 9) * 16);
    }    

    // set or reset alpha for the level indication
    EVE_COLOR_A(255);
    // select which bitmap cell to render based on current battery percentage
    if (value < 5)
    {
        EVE_CELL(0);
    }
    else if ((value >= 5) && (value  < 95))
    {
        EVE_CELL((value/5));
    }
    else if (value  >= 95)
    {
        EVE_CELL(19);
    }

    // place bitmap on screen
    EVE_VERTEX2F((input_x + 26) * 16, (input_y + 9) * 16);

    // restore graphics context
    EVE_RESTORE_CONTEXT();

}

void configure_bitmaps(void){

    EVE_LIB_BeginCoProList();
    EVE_CMD_DLSTART();

    // Configure images
    //----------------------------------------------------------------
    EVE_BEGIN(EVE_BEGIN_BITMAPS);

    // Configure images and fonts for use in the screen
    EVE_BITMAP_HANDLE(Carbon_Fiber_800x480_asset.Handle);
    EVE_CMD_SETBITMAP(Carbon_Fiber_800x480_asset.RAM_G_Start, Carbon_Fiber_800x480_asset.Format, Carbon_Fiber_800x480_asset.Width, Carbon_Fiber_800x480_asset.Height);

    EVE_BITMAP_HANDLE(LED_32x32_asset.Handle);
    EVE_CMD_SETBITMAP(LED_32x32_asset.RAM_G_Start, LED_32x32_asset.Format, LED_32x32_asset.Width, LED_32x32_asset.Height);

    EVE_BITMAP_HANDLE(Trackmap_96x60_asset.Handle);
    EVE_CMD_SETBITMAP(Trackmap_96x60_asset.RAM_G_Start, Trackmap_96x60_asset.Format, Trackmap_96x60_asset.Width, Trackmap_96x60_asset.Height);

    //this bitmap has 2 cells
    EVE_BITMAP_HANDLE(Arrows_96x192_asset.Handle);
    EVE_CMD_SETBITMAP(Arrows_96x192_asset.RAM_G_Start, Arrows_96x192_asset.Format, Arrows_96x192_asset.Width, Arrows_96x192_asset.CellHeight);

    EVE_BITMAP_HANDLE(Widget_Gear_152x152_asset.Handle);
    EVE_CMD_SETBITMAP(Widget_Gear_152x152_asset.RAM_G_Start, Widget_Gear_152x152_asset.Format, Widget_Gear_152x152_asset.Width, Widget_Gear_152x152_asset.Height);

    EVE_BITMAP_HANDLE(Widget_RPM_152x56_asset.Handle);
    EVE_CMD_SETBITMAP(Widget_RPM_152x56_asset.RAM_G_Start, Widget_RPM_152x56_asset.Format, Widget_RPM_152x56_asset.Width, Widget_RPM_152x56_asset.Height);

    EVE_BITMAP_HANDLE(Widget_Speed_104x56_asset.Handle);
    EVE_CMD_SETBITMAP(Widget_Speed_104x56_asset.RAM_G_Start, Widget_Speed_104x56_asset.Format, Widget_Speed_104x56_asset.Width, Widget_Speed_104x56_asset.Height);

    EVE_BITMAP_HANDLE(Bottom_Bar_800x8_asset.Handle);
    EVE_CMD_SETBITMAP(Bottom_Bar_800x8_asset.RAM_G_Start, Bottom_Bar_800x8_asset.Format, Bottom_Bar_800x8_asset.Width, Bottom_Bar_800x8_asset.Height);

    EVE_BITMAP_HANDLE(Car_Overhead_44x80_asset.Handle);
    EVE_CMD_SETBITMAP(Car_Overhead_44x80_asset.RAM_G_Start, Car_Overhead_44x80_asset.Format, Car_Overhead_44x80_asset.Width, Car_Overhead_44x80_asset.Height);

    // This bitmap has 20 cells
    EVE_BITMAP_HANDLE(Battery_Cells_40x720_asset.Handle);
    EVE_CMD_SETBITMAP(Battery_Cells_40x720_asset.RAM_G_Start, Battery_Cells_40x720_asset.Format, Battery_Cells_40x720_asset.Width, Battery_Cells_40x720_asset.CellHeight);

    // This bitmap has 2 cells
    EVE_BITMAP_HANDLE(Oil_Water_Icons_32x64_asset.Handle);
    EVE_CMD_SETBITMAP(Oil_Water_Icons_32x64_asset.RAM_G_Start, Oil_Water_Icons_32x64_asset.Format, Oil_Water_Icons_32x64_asset.Width, Oil_Water_Icons_32x64_asset.CellHeight);

    EVE_BITMAP_HANDLE(Steering_Wheel_32x32_asset.Handle);
    EVE_CMD_SETBITMAP(Steering_Wheel_32x32_asset.RAM_G_Start, Steering_Wheel_32x32_asset.Format, Steering_Wheel_32x32_asset.Width, Steering_Wheel_32x32_asset.Height);

    EVE_BITMAP_HANDLE(Side_Gauge_L_136x104_asset.Handle);
    EVE_CMD_SETBITMAP(Side_Gauge_L_136x104_asset.RAM_G_Start, Side_Gauge_L_136x104_asset.Format, Side_Gauge_L_136x104_asset.Width, Side_Gauge_L_136x104_asset.Height);

    EVE_BITMAP_HANDLE(Side_Gauge_R_136x104_asset.Handle);
    EVE_CMD_SETBITMAP(Side_Gauge_R_136x104_asset.RAM_G_Start, Side_Gauge_R_136x104_asset.Format, Side_Gauge_R_136x104_asset.Width, Side_Gauge_R_136x104_asset.Height);

    EVE_BITMAP_HANDLE(Side_Gauge_L_Fill_136x104_asset.Handle);
    EVE_CMD_SETBITMAP(Side_Gauge_L_Fill_136x104_asset.RAM_G_Start, Side_Gauge_L_Fill_136x104_asset.Format, Side_Gauge_L_Fill_136x104_asset.Width, Side_Gauge_L_Fill_136x104_asset.Height);

    EVE_BITMAP_HANDLE(Side_Gauge_R_Fill_136x104_asset.Handle);
    EVE_CMD_SETBITMAP(Side_Gauge_R_Fill_136x104_asset.RAM_G_Start, Side_Gauge_R_Fill_136x104_asset.Format, Side_Gauge_R_Fill_136x104_asset.Width, Side_Gauge_R_Fill_136x104_asset.Height);

    EVE_BITMAP_HANDLE(PIT_SC_Widget_80x36_asset.Handle);
    EVE_CMD_SETBITMAP(PIT_SC_Widget_80x36_asset.RAM_G_Start, PIT_SC_Widget_80x36_asset.Format, PIT_SC_Widget_80x36_asset.Width, PIT_SC_Widget_80x36_asset.Height);

    // Configure custom fonts
    EVE_CMD_SETFONT2(eurostile_150_L8.Handle, eurostile_150_L8.RAM_G_Start, 48); // use 48 here as we only converted the 0-9 chars

    // Send display to EVE
    EVE_DISPLAY();
    EVE_CMD_SWAP();
    EVE_LIB_EndCoProList();
    EVE_LIB_AwaitCoProEmpty();

}

// ######################################################################################################################################################################################################
// #######################################################                           Main display loop code                           ###################################################################
// ######################################################################################################################################################################################################

void eve_display(void)
{

    // varibles for determining FPS 
    //----------------------------------------------------------------
    uint32_t currentMs = 0;
    uint32_t previousMs = 0;
    // Desired framerate
    const uint32_t frameMs = 40;
    // Clock for working out FPS update timing (1 per second)
    uint32_t clockMs = 0; 
    uint32_t currentFrames = 0;
    uint32_t previousFrames = 0;
    uint32_t FPS = 0;
    uint32_t DLCount = 0;
    uint32_t DLPS = 0;

    // screen data varibles
    //----------------------------------------------------------------

    // laptime arrays
    uint32_t laptime_msec [] = {0, 38, 157, 198, 337, 398, 577, 758, 817, 958, 997, 1118, 1297, 1337, 1398, 1557, 1678, 1797, 1958, 1977, 2337, 2358, 2517, 2697, 2718, 2878, 2977, 3078, 3197, 3257, 3278, 3516, 3518, 3717, 3718, 3997, 3998, 4217, 4238, 4257, 4478, 4537, 4678, 4737, 4878, 4897, 5077, 5119, 5417, 5439, 5639, 5717, 5839, 6017, 6039, 6237, 6279, 6397, 6577, 6678, 6857, 7037, 7038, 7238, 7277, 7357, 7478, 7537, 7917, 7918, 8097, 8118, 8257, 8438, 8497, 8598, 8697, 8918, 9057, 9318, 9377, 9478, 9777, 9878, 10036, 10217, 10398, 10537, 10678, 10817, 10838, 11198, 11237, 11277, 11537, 11598, 11837, 11839, 12057, 12199, 12237, 12457, 12639, 12877, 13039, 13177, 13198, 13477, 13678, 13697, 13998, 14117, 14198, 14337, 14397, 14438, 14598, 14677, 14878, 14957, 15318, 15377, 15518, 15637, 15838, 15877, 16137, 16158, 16317, 16438, 16477, 16758, 16817, 17078, 17117, 17357, 17438, 17557, 17758, 17857, 17958, 18117, 18158, 18359, 18417, 18599, 18837, 18839, 18999, 19057, 19199, 19397, 19479, 19597, 19759, 19857, 20037, 20078, 20457, 20518, 20617, 20937, 20958, 21118, 21157, 21278, 21337, 21438, 21477, 21678, 21777, 21878, 21977, 22197, 22358, 22437, 22558, 22637, 22758, 22917, 22958, 23158, 23318, 23437, 23478, 23617, 23678, 23997, 24038, 24197, 24238, 24438, 24457, 25257, 25359, 25639, 25657, 25857, 25959, 26117, 26199, 26377, 26599, 26737, 26998, 27017, 27277, 27278, 27437, 27478, 27638, 27717, 27917, 28038, 28157, 28278, 28337, 28417, 28438, 28657, 28678, 28877, 28958, 29137, 29278, 29438, 29577, 29718, 29878, 29977, 30078, 30157, 30278, 30377, 30438, 30537, 30638, 30838, 30917, 31158, 31177, 31359, 31437, 31557, 31559, 31719, 31737, 31957, 32039, 32217, 32239, 32439, 32677, 32879, 32897, 33079, 33239, 33257, 33478, 33517, 33678, 33697, 33918, 34037, 34297, 34358, 34377, 34518, 34657, 34758, 35078, 35157, 35278, 35317, 35518, 35597, 35797, 35798, 35998, 36158, 36217, 36318, 36436, 36477, 36478, 36678, 36717, 36878, 37158, 37257, 37437, 37597, 37598, 37857, 38077, 38079, 38297, 38319, 38597, 38679, 38857, 39079, 39097, 39239, 39399, 39477, 39657, 39679, 39879, 40079, 40117, 40238, 40417, 40478, 40657, 40817, 40878, 41137, 41358, 41517, 41718, 41877, 41918, 42198, 42217, 42358, 42437, 42457, 42558, 42677, 42758, 42916, 43118, 43157, 43318, 43517, 43598, 43697, 43958, 44057, 44118, 44437, 44478, 44517, 44719, 44777, 45077, 45079, 45257, 45319, 45639, 45717, 45919, 46037, 46199, 46337, 46479, 46537, 46639, 46717, 46839, 46937, 46998, 47137, 47317, 47358, 47477, 47638, 47677, 48238, 48317, 48477, 48598, 48697, 48838, 48957, 49177, 49238, 49456, 49556, 49638, 49797, 49997, 50078, 50177, 50238, 50357, 50518, 50577, 50758, 50817, 51037, 51038, 51357, 51479, 51617, 51719, 51877, 52057, 52119, 52279, 52517, 52559, 52617, 52917, 52959, 53239, 53277, 53477, 53479, 53657, 53758, 53857, 54037, 54078, 54317, 54398, 54557, 54598, 54737, 54798, 55158, 55177, 55438, 55598, 55677, 55877, 55998, 56136, 56198, 56417, 56438, 56597, 56798, 57078, 57097, 57398, 57477, 57638, 57677, 57857, 57919, 58157, 58279, 58357, 58479, 58737, 58839, 59079, 59157, 59239, 59439, 59537, 59599, 59737, 59959, 60017, 60297, 60319, 60577, 60797, 60798, 60998, 61057, 61397, 61398, 61617, 61757, 61798, 61917, 62198, 62217, 62397, 62438, 62616, 62678, 62716, 62917, 63038, 63157, 63398, 63557, 63718, 63857, 63918, 64197, 64198, 64457, 64557, 64639, 64857, 64919, 65177, 65239, 65439, 65637, 65797, 65799, 65999, 66077, 66279, 66297, 66439, 66637, 66639, 66799, 66937, 66999, 67277, 67318, 67478, 67557, 67697, 67918, 67937, 68198, 68377, 68478, 68758, 68777, 68977, 68998, 69137, 69358, 69397, 69558, 69597, 69677, 69838, 69937, 69998, 70198, 70237, 70397, 70438, 70557, 70598, 70777, 70937, 71039, 71239, 71297, 71479, 71677, 71799, 71999, 72117, 72319, 72517, 72677, 72719, 72957, 73079, 73277, 73399, 73517, 73717, 73719, 73879, 73937, 74078, 74297, 74358, 74518, 74697, 74718, 74857, 75118, 75157, 75317, 75318, 75577, 75598, 75836, 75998, 76057, 76238, 76437, 76438, 76677, 77278, 77437, 77479, 77737, 77759, 77959, 78057, 78119, 78359, 78377, 78519, 78537, 78717, 78719, 78959, 79017, 79239, 79417, 79439, 79657, 79837, 79879, 80197, 80239, 80399, 80517, 80679, 80697, 80917, 80918, 81118, 81137, 81278, 81337, 81518, 81717, 81718, 81878, 81897, 82078, 82157, 82278, 82376, 82478, 82557, 82617, 82718, 82937, 83078, 83278, 83417, 83558, 83757, 83878, 83997, 84159, 84217, 84377, 84637, 84639, 84677, 85039, 85057, 85277, 85477, 85479, 85679, 85897, 85919, 86097, 86119, 86319, 86437, 86559, 86657, 86759, 86917, 86959, 87137, 87319, 87397, 87637, 87678, 87838, 87937, 87998, 88238, 88317, 88398, 88617, 88718, 88936, 88998, 89318, 89337, 89518, 89537, 89757, 89918, 89977, 90158, 90397, 90478, 90557, 90657, 90719, 90999, 91037, 91239, 91297, 91537, 91559, 91857, 91999, 92277, 92279, 92477, 92479, 92839, 92877, 93039, 93117, 93417, 93519, 93717, 93839, 93917, 94119, 94237, 94399, 94417, 94617, 94758, 94837, 94998, 95037, 95237, 95358, 95516, 95777, 95798, 95957, 95998, 96117, 96278, 96417, 96438, 96637, 96677, 96798, 96937, 97157, 97239, 97317, 97517, 97599, 97757, 97759, 97959, 97977, 98119, 98211};
    uint32_t sector_times [] = {0, 0};

    // car data arrays
    uint16_t RPM [] = {11195, 11106, 10634, 10472, 10543, 10574, 10658, 10742, 10775, 10854, 10854, 10854, 11042, 11084, 11148, 11172, 11191, 11261, 11356, 11362, 11473, 11479, 11510, 11546, 11550, 11516, 11490, 11463, 11256, 11152, 11115, 11797, 11803, 12046, 12047, 11298, 11295, 10598, 10531, 10582, 11176, 11208, 11284, 11278, 11262, 11270, 11349, 11367, 10347, 10272, 10096, 9949, 9719, 9491, 9463, 9300, 9266, 9237, 9192, 9167, 9384, 9602, 9603, 9953, 10020, 10158, 10366, 10329, 10091, 10090, 10006, 9996, 9768, 9472, 9351, 9143, 8890, 8324, 7960, 7276, 7130, 6879, 6534, 6417, 6327, 6224, 6121, 6048, 5974, 6077, 6092, 6799, 6843, 6889, 7185, 7255, 7739, 7743, 8312, 8682, 8765, 9244, 9640, 10263, 10687, 11395, 11503, 11162, 10916, 10958, 11629, 11722, 11785, 11085, 10783, 10576, 10856, 10976, 11281, 11326, 11529, 11497, 11420, 11090, 10533, 10555, 10703, 10715, 10660, 10618, 10723, 11480, 11387, 10976, 10966, 10908, 10888, 10613, 10149, 9738, 9318, 9060, 8993, 8686, 8659, 8576, 8714, 8715, 8609, 8604, 8592, 8783, 8862, 9049, 9306, 9461, 9746, 9811, 10479, 10586, 10756, 11307, 11343, 11532, 11559, 11643, 11269, 10628, 10688, 10996, 11137, 11281, 11387, 11624, 11797, 11410, 10817, 10749, 10644, 10911, 10980, 11226, 11300, 11515, 11589, 11519, 11488, 10620, 10509, 10589, 10610, 10770, 10780, 11205, 11259, 11350, 11355, 11407, 11433, 11515, 11558, 11624, 11707, 11703, 11696, 11699, 11747, 11747, 10745, 10487, 10461, 10456, 10444, 10437, 10406, 10375, 10442, 10534, 10558, 10308, 10284, 11025, 11326, 10917, 10594, 10010, 9907, 9803, 9863, 10103, 10348, 10603, 10994, 10654, 10445, 10485, 10526, 10902, 10527, 9384, 9360, 9135, 8963, 8699, 8695, 8656, 8640, 8448, 8376, 8304, 8295, 8474, 8684, 8863, 8900, 9276, 9498, 9568, 10431, 10511, 10842, 10913, 11734, 11472, 10898, 10764, 10794, 11018, 11207, 11345, 10886, 10717, 10457, 10581, 11218, 10851, 9924, 9919, 9100, 8534, 8188, 7596, 7096, 6922, 6918, 6767, 6638, 6104, 5923, 5775, 5507, 5268, 5267, 5364, 5447, 5448, 5729, 5757, 6005, 6078, 6438, 6886, 6955, 7502, 8026, 8175, 8520, 8562, 9101, 10100, 10190, 10476, 10681, 10751, 10812, 10866, 10887, 10391, 9967, 10464, 11092, 10712, 10614, 10183, 10163, 10015, 9923, 9900, 9783, 9664, 9583, 9454, 9289, 9342, 9559, 9612, 9633, 9743, 10033, 10268, 10413, 10798, 10847, 10971, 11615, 11625, 11676, 11676, 11315, 11189, 11623, 11351, 10647, 10832, 11085, 11266, 11452, 11510, 11613, 11428, 11138, 10661, 10364, 10515, 10710, 10754, 10884, 11060, 11072, 11249, 11233, 11200, 11175, 10941, 10607, 11057, 11888, 12119, 11318, 10950, 10649, 10658, 10669, 10674, 10933, 11093, 10847, 10514, 10438, 10203, 10052, 9488, 9485, 9326, 9265, 9214, 9176, 9262, 9359, 9393, 9599, 10043, 10121, 10258, 10966, 11065, 11561, 11530, 11366, 11364, 10979, 10761, 10420, 9800, 9659, 8548, 8172, 8016, 7976, 7770, 7679, 7369, 7377, 7486, 7892, 7947, 8087, 8171, 8575, 8757, 9460, 9527, 10072, 10761, 11405, 11435, 11908, 11859, 11760, 11654, 11164, 10995, 11295, 11449, 11565, 11747, 11546, 11466, 11120, 11026, 10928, 11810, 11539, 11368, 10885, 10109, 10188, 10568, 10598, 9603, 8755, 8751, 8037, 7878, 6962, 6959, 6488, 6186, 6098, 5980, 5700, 5687, 5567, 5539, 5479, 5458, 5515, 5814, 5994, 6197, 6607, 6746, 6887, 7633, 7961, 9015, 9019, 9951, 10311, 10606, 11205, 11375, 10751, 10601, 11031, 10643, 10330, 10326, 10883, 10999, 11298, 11251, 10878, 10611, 10608, 10874, 11106, 11210, 11495, 11537, 11733, 11568, 11276, 10814, 10818, 10868, 11128, 11275, 11379, 11396, 11580, 11599, 11658, 11752, 11756, 11771, 11600, 11250, 10545, 10508, 10485, 10113, 10295, 11042, 11233, 11259, 11268, 10811, 10403, 10143, 10687, 10803, 11168, 11042, 10964, 10142, 9652, 8812, 7980, 7308, 7131, 6984, 6909, 6499, 6246, 6123, 5915, 5913, 5757, 5794, 5884, 6181, 6264, 6767, 6598, 6578, 7058, 7960, 8064, 8490, 8493, 9252, 9314, 10033, 10522, 10739, 11403, 12027, 12030, 12007, 11948, 10964, 10704, 11016, 11043, 11254, 11361, 11428, 11765, 11655, 10785, 10767, 10592, 10590, 10787, 10836, 11025, 11127, 11140, 11281, 11397, 11424, 11534, 11548, 11511, 11057, 10434, 10441, 10521, 10521, 10643, 10643, 10643, 10688, 10826, 10938, 10939, 10963, 10958, 10907, 10972, 11072, 11117, 11163, 11189, 11208, 11241, 11294, 11328, 11367, 11269, 11170, 11727, 12066, 11764, 11354, 11334, 11279, 11189, 11188, 11183, 11135, 11168, 11567, 11930, 11934, 10534, 9528, 9427, 8719, 8632, 8281, 8163, 8040, 7966, 7888, 7722, 7678, 7737, 7797, 7858, 8047, 8079, 8389, 8525, 8609, 8765, 8770, 8776, 8840, 8870, 8989, 9023, 9605, 9630, 9869, 9915, 10453, 10847, 10980, 11390, 11270, 11229, 11171, 11097, 11051, 11191, 11271, 11699, 11542, 10893, 10834, 11082, 11200, 11589, 11592, 11746, 11748, 10485, 10524, 10691, 10724, 10850, 10893, 10643, 10489, 10414, 10219, 10180, 10126, 10122, 10079, 10049, 10124, 10276, 10317, 10528, 10656, 10872, 11229, 11258, 11322, 11339, 11472, 11652, 11699, 11706, 11086, 10962, 10585, 10684, 10841, 10900, 10964, 11129, 11197, 11380, 11382, 11482, 11492, 11573, 11630};
    uint16_t speed [] = {257, 258, 259, 259, 262, 263, 266, 269, 270, 272, 272, 274, 277, 277, 278, 280, 282, 283, 285, 285, 288, 288, 289, 290, 290, 291, 290, 288, 286, 285, 285, 275, 275, 267, 267, 263, 263, 237, 234, 233, 217, 212, 199, 197, 191, 189, 168, 163, 148, 147, 141, 139, 137, 134, 134, 132, 131, 129, 127, 125, 127, 129, 129, 135, 135, 136, 138, 139, 143, 143, 139, 139, 137, 135, 135, 134, 129, 118, 113, 104, 103, 101, 97, 95, 92, 89, 86, 86, 86, 83, 83, 85, 86, 87, 93, 95, 103, 103, 109, 113, 114, 121, 126, 140, 150, 150, 150, 165, 175, 176, 186, 190, 192, 198, 200, 202, 202, 205, 213, 215, 224, 224, 225, 227, 231, 231, 235, 235, 234, 233, 232, 227, 225, 215, 212, 193, 186, 179, 167, 163, 158, 152, 151, 149, 148, 146, 142, 142, 137, 137, 137, 138, 139, 142, 146, 148, 151, 152, 167, 169, 172, 180, 181, 191, 192, 194, 196, 200, 201, 207, 210, 213, 215, 220, 224, 226, 230, 231, 232, 240, 242, 245, 249, 251, 252, 254, 255, 261, 262, 265, 266, 269, 269, 280, 281, 285, 285, 287, 288, 289, 289, 290, 292, 293, 294, 294, 295, 295, 295, 295, 297, 297, 297, 297, 298, 299, 298, 296, 296, 291, 290, 289, 288, 279, 272, 255, 244, 232, 225, 217, 208, 202, 193, 183, 177, 173, 168, 150, 146, 135, 135, 130, 128, 126, 126, 122, 122, 118, 117, 114, 114, 114, 116, 117, 118, 123, 130, 131, 137, 139, 145, 146, 159, 162, 169, 171, 172, 180, 183, 186, 185, 184, 182, 178, 160, 157, 148, 148, 134, 120, 117, 111, 107, 106, 106, 100, 98, 91, 85, 83, 81, 78, 78, 75, 73, 73, 75, 75, 80, 81, 86, 92, 93, 99, 103, 106, 113, 114, 123, 132, 134, 142, 149, 152, 158, 164, 166, 167, 168, 164, 159, 154, 153, 148, 147, 143, 142, 142, 140, 139, 138, 135, 131, 131, 129, 129, 129, 131, 135, 138, 140, 147, 148, 150, 158, 160, 170, 170, 178, 181, 191, 194, 201, 204, 208, 212, 216, 218, 221, 223, 225, 226, 227, 230, 234, 235, 237, 240, 241, 248, 248, 247, 247, 245, 241, 239, 234, 233, 219, 213, 208, 197, 183, 177, 170, 165, 159, 150, 148, 142, 141, 135, 135, 131, 130, 127, 125, 126, 128, 128, 130, 135, 136, 138, 147, 148, 157, 157, 159, 159, 157, 156, 151, 142, 140, 131, 128, 118, 115, 112, 111, 104, 104, 103, 101, 102, 104, 106, 112, 114, 127, 128, 135, 144, 155, 155, 161, 163, 168, 169, 172, 173, 181, 185, 187, 191, 193, 194, 189, 185, 180, 169, 167, 165, 158, 147, 144, 127, 126, 115, 105, 105, 96, 94, 83, 83, 79, 77, 76, 74, 68, 68, 65, 64, 63, 62, 62, 65, 66, 68, 71, 73, 76, 84, 87, 93, 93, 103, 107, 110, 124, 128, 139, 142, 152, 160, 166, 166, 174, 177, 185, 186, 192, 199, 199, 202, 206, 208, 218, 220, 223, 225, 229, 235, 235, 239, 243, 246, 251, 251, 253, 253, 255, 257, 258, 260, 260, 261, 263, 264, 264, 266, 265, 259, 258, 256, 255, 240, 227, 219, 200, 196, 184, 175, 170, 154, 144, 127, 115, 105, 102, 99, 97, 94, 92, 90, 86, 86, 86, 85, 81, 82, 82, 88, 92, 93, 95, 99, 102, 113, 113, 121, 122, 137, 147, 149, 156, 156, 156, 167, 194, 200, 201, 208, 209, 215, 218, 220, 224, 224, 226, 227, 232, 232, 236, 237, 241, 244, 244, 246, 249, 249, 253, 254, 256, 258, 260, 260, 263, 263, 266, 266, 266, 267, 271, 272, 272, 275, 275, 277, 277, 278, 278, 279, 280, 281, 282, 283, 284, 285, 283, 280, 278, 276, 266, 252, 248, 236, 216, 216, 213, 186, 185, 172, 160, 160, 147, 133, 132, 124, 123, 117, 116, 114, 112, 110, 109, 109, 107, 104, 105, 108, 108, 110, 113, 115, 124, 124, 125, 123, 122, 123, 123, 129, 129, 134, 135, 142, 147, 148, 151, 162, 166, 170, 176, 179, 185, 187, 196, 198, 204, 205, 212, 216, 221, 221, 224, 224, 229, 228, 226, 224, 215, 212, 208, 205, 203, 199, 198, 197, 197, 193, 191, 191, 192, 193, 197, 200, 202, 205, 205, 211, 213, 216, 219, 224, 225, 227, 228, 229, 232, 236, 237, 238, 242, 243, 247, 247, 251, 251, 253, 254};
    uint8_t gears [] = {6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 5, 5, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6};
    uint8_t throttle [] = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 99, 99, 99, 38, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5, 10, 19, 25, 37, 50, 51, 72, 72, 72, 74, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 14, 20, 22, 45, 46, 48, 58, 61, 62, 63, 72, 78, 78, 82, 85, 93, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 100, 99, 99, 99, 99, 99, 99, 99, 99, 99, 46, 6, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 6, 31, 34, 42, 58, 65, 67, 72, 75, 81, 83, 96, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 100, 99, 99, 99, 99, 99, 99, 99, 99, 100, 100, 100, 100, 100, 100, 100, 99, 99, 99, 99, 100, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 96, 94, 71, 40, 32, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 6, 7, 21, 34, 46, 48, 71, 76, 76, 87, 87, 92, 92, 99, 99, 99, 99, 99, 99, 99, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 11, 12, 23, 25, 44, 50, 59, 71, 71, 72, 75, 78, 86, 87, 90, 99, 99, 99, 99, 99, 99, 99, 99, 46, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 6, 33, 45, 47, 53, 59, 63, 74, 76, 77, 86, 88, 98, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 79, 39, 9, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 5, 10, 15, 25, 37, 42, 53, 60, 62, 64, 74, 76, 82, 76, 47, 47, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 29, 34, 47, 55, 70, 77, 96, 98, 98, 99, 82, 81, 76, 78, 83, 84, 90, 92, 96, 99, 99, 99, 35, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 8, 14, 19, 23, 32, 32, 32, 45, 52, 64, 65, 70, 73, 75, 82, 84, 92, 95, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 99, 99, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 99, 99, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 6, 16, 26, 30, 42, 50, 51, 58, 73, 74, 81, 82, 91, 92, 96, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 99, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 94, 49, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 17, 24, 47, 51, 55, 62, 67, 67, 51, 35, 24, 20, 28, 31, 54, 55, 65, 65, 75, 83, 83, 86, 95, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 98, 80, 4, 3, 0, 0, 0, 0, 1, 6, 14, 25, 26, 39, 49, 57, 74, 76, 90, 99, 99, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
    uint8_t brake [] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t DRS [] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    // 0 is full left lock, 255 is full right lock
    uint8_t steering [] = {128, 128, 128, 128, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 127, 127, 127, 127, 127, 127, 127, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 118, 108, 98, 88, 78, 68, 78, 88, 98, 108, 118, 128, 138, 148, 158, 168, 158, 148, 138, 133, 128, 127, 127, 128, 129, 129, 128, 128, 127, 127, 127, 128, 118, 108, 98, 88, 78, 68, 58, 48, 40, 48, 58, 68, 78, 88, 99, 108, 118, 128, 128, 128, 128, 128, 128, 128, 128, 110, 100, 95, 90, 94, 99, 105, 114, 120, 128, 128, 128, 128, 129, 129, 129, 129, 131, 131, 131, 131, 131, 130, 129, 129, 129, 128, 128, 128, 127, 127, 127, 128, 128, 128, 129, 129, 131, 138, 148, 158, 156, 165, 174, 174, 180, 185, 178, 170, 160, 152, 138, 132, 128, 128, 128, 129, 129, 129, 129, 129, 131, 131, 131, 131, 131, 129, 129, 129, 129, 129, 129, 128, 128, 127, 127, 127, 127, 127, 126, 126, 126, 128, 128, 130, 130, 130, 130, 133, 140, 144, 144, 148, 148, 153, 158, 153, 153, 144, 144, 136, 136, 136, 132, 130, 128, 128, 127, 127, 128, 128, 128, 128, 129, 129, 129, 129, 128, 128, 126, 126, 126, 126, 126, 128, 127, 127, 127, 128, 128, 128, 128, 129, 129, 120, 118, 108, 98, 88, 78, 68, 62, 64, 68, 78, 88, 98, 108, 118, 120, 124, 128, 129, 129, 128, 128, 127, 127, 128, 128, 128, 128, 129, 129, 129, 129, 128, 128, 128, 128, 128, 127, 127, 127, 127, 128, 128, 128, 128, 128, 130, 134, 147, 159, 170, 180, 192, 205, 215, 221, 210, 199, 185, 177, 163, 156, 142, 136, 130, 129, 128, 128, 129, 129, 130, 130, 129, 129, 128, 128, 127, 127, 127, 127, 128, 128, 129, 129, 128, 128, 122, 111, 101, 98, 94, 88, 88, 88, 92, 98, 98, 100, 110, 120, 120, 125, 126, 126, 126, 126, 127, 127, 127, 127, 128, 128, 128, 129, 129, 129, 129, 130, 130, 130, 130, 130, 129, 129, 129, 129, 129, 128, 128, 128, 128, 127, 127, 127, 127, 126, 126, 127, 127, 127, 127, 128, 128, 128, 128, 128, 129, 129, 129, 128, 120, 110, 100, 90, 80, 70, 65, 70, 80, 90, 100, 105, 115, 120, 126, 129, 129, 129, 129, 130, 129, 129, 129, 128, 128, 127, 127, 130, 135, 140, 150, 160, 170, 180, 170, 166, 155, 145, 136, 130, 127, 126, 124, 118, 110, 100, 95, 94, 98, 104, 112, 120, 126, 126, 127, 127, 127, 128, 128, 128, 128, 129, 129, 129, 129, 130, 130, 130, 130, 130, 129, 129, 129, 129, 128, 128, 128, 120, 105, 90, 75, 60, 55, 40, 33, 22, 22, 22, 33, 48, 55, 67, 80, 88, 94, 104, 101, 120, 128, 128, 129, 129, 129, 129, 129, 129, 129, 129, 129, 129, 130, 130, 130, 130, 130, 131, 131, 131, 131, 131, 131, 135, 135, 135, 135, 135, 135, 133, 133, 133, 133, 130, 130, 130, 130, 129, 128, 128, 128, 128, 127, 127, 127, 125, 125, 126, 126, 127, 127, 128, 128, 128, 128, 129, 129, 129, 128, 128, 128, 129, 129, 129, 130, 130, 131, 131, 131, 140, 151, 165, 177, 190, 195, 195, 178, 170, 155, 140, 132, 130, 130, 129, 129, 129, 129, 129, 128, 128, 128, 128, 127, 127, 127, 127, 128, 128, 128, 128, 127, 127, 126, 126, 120, 120, 110, 110, 105, 105, 105, 117, 117, 120, 120, 125, 125, 127, 127, 128, 128, 129, 129, 130, 130, 130, 129, 129, 129, 128, 128, 128, 127, 127, 127, 126, 126, 126, 127, 127, 127, 127, 128, 128, 128, 128, 129, 129, 129, 129, 128, 128, 128, 128, 127, 127, 127, 127, 128, 128, 128, 129, 133, 135, 145, 155, 157, 162, 170, 180, 190, 190, 180, 165, 155, 140, 132, 129, 128, 127, 120, 109, 100, 92, 84, 75, 70, 88, 100, 111, 119, 125, 128, 128, 128, 129, 129, 129, 128, 128, 128, 127, 127, 127, 127, 128, 128, 128, 129, 129, 129, 129, 128, 128, 127, 124, 120, 110, 101, 95, 86, 76, 68, 68, 72, 72, 76, 80, 85, 90, 94, 94, 100, 100, 107, 112, 118, 122, 126, 128, 129, 129, 129, 130, 130, 130, 130, 130, 130};
    // air fuel ratio graph data (0 - 28 range)
    uint8_t AFR [] = {12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 17, 18, 19, 20, 20, 20, 20, 20, 20, 19, 18, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 22, 22, 22, 22, 23, 24, 24, 24, 24, 24, 24, 24, 23, 22, 21, 21, 21, 21, 21, 19, 19, 19, 19, 18, 18, 18, 18, 18, 17, 16, 15, 14, 13, 13, 13, 13, 13, 12, 12, 12, 12, 11, 11, 11, 10, 10, 10, 10, 9, 8, 7, 6, 5, 4, 4, 4, 4, 4, 4, 5, 6, 7, 7, 7, 7, 7, 7, 6, 5, 4, 3, 2, 1, 1, 1, 1, 1, 1, 2, 3, 4, 5, 5, 5, 5, 5, 5, 5, 6, 7, 8, 8, 8, 8, 8, 9, 10, 11, 12, 12, 12, 12, 12, 12, 11, 10, 9, 8, 8, 8, 7, 6, 5, 4, 3, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 4, 4, 4, 4, 5, 6, 7, 7, 7, 7, 7, 7, 6, 5, 5, 5, 5, 5, 6, 7, 8, 9, 9, 9, 9, 10, 10, 10, 10, 10, 11, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 16, 17, 18, 18, 18, 18, 18, 18, 17, 16, 16, 16, 16, 16, 17, 18, 19, 19, 19, 19, 19, 20, 21, 22, 22, 22, 22, 21, 20, 19, 19, 19, 19, 19, 18, 17, 16, 15, 15, 15, 15, 15, 15, 16, 17, 17, 17, 17, 17, 17, 18, 19, 20, 21, 22, 22, 22, 22, 22, 22, 23, 24, 24, 24, 24, 24, 23, 23, 23, 22, 22, 22, 22, 22, 21, 20, 19, 19, 19, 19, 19, 19, 20, 21, 22, 23, 24, 24, 24, 24, 24, 23, 22, 21, 20, 20, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 9, 9, 9, 9, 10, 11, 12, 12, 12, 13, 14, 15, 15, 15, 15, 15, 14, 13, 12, 11, 11, 11, 11, 11, 11, 10, 9, 8, 7, 7, 7, 7, 7, 6, 5, 4, 3, 2, 2, 2, 2, 2, 2, 3, 4, 5, 6, 6, 6, 6, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 7, 7, 7, 7, 6, 5, 4, 4, 4, 4, 4, 4, 5, 6, 7, 8, 9, 9, 9, 9, 9, 10, 11, 12, 13, 13, 13, 13, 13, 12, 11, 10, 10, 10, 10, 10, 11, 12, 12, 13, 14, 14, 14, 14, 14, 14, 15, 16, 17, 18, 18, 18, 18, 18, 17, 16, 15, 14, 13, 12, 11, 11, 11, 11, 10, 9, 8, 7, 6, 6, 6, 6, 6, 5, 4, 3, 2, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 5, 5, 5, 5, 6, 7, 8, 9, 9, 9, 9, 7, 8, 6, 6, 6, 6, 6, 7, 8, 9, 10, 11, 11, 11, 11, 11, 12, 12, 12, 13, 14, 15, 16, 17, 18, 18, 18, 18, 18, 17, 16, 15, 15, 15, 15, 15, 15, 14, 13, 12, 11, 10, 10, 10, 10, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 20, 20, 20, 21, 22, 23, 24, 24, 24, 24, 24, 24, 24, 24, 23, 22, 21, 21, 21, 21, 20, 19, 18, 18, 18, 18, 18, 17, 16, 15, 14, 14, 14, 14, 14, 13, 12, 11, 10, 10, 10, 10, 10, 9, 8, 7, 7, 7, 7, 8, 9, 10, 11, 13, 14, 14, 14, 14, 13, 12, 11, 10, 10, 10, 10, 10, 10, 11, 12, 13, 14, 15, 15, 15, 15, 16, 17, 18, 18, 18, 18, 18, 17, 16, 16, 16, 16, 16, 17, 18, 19, 20, 20, 20, 20, 20, 20, 19, 18, 17, 17, 17, 17, 18, 19, 20, 21, 22, 23, 24, 24, 24, 24, 24, 23, 22, 21, 20, 20, 20, 20, 19, 18, 17, 16, 15, 14, 13, 12, 12, 12, 12, 12, 12, 13, 14, 14, 14, 14, 14, 13, 11, 10, 9, 8, 8, 8, 8, 7, 6, 5, 4, 3, 2, 1, 1, 1, 1, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 9, 9, 10, 11, 12};
    // Intake Air Temps, this is a 2D array holding 6 values per column
    // only 106 entires
    int8_t IAT [] [6] = {{9, 8, 9, 10, 9, 8}, {9, 9, 10, 9, 8, 7}, {8, 10, 11, 10, 7, 8}, {7, 9, 10, 9, 7, 7}, {8, 8, 9, 8, 7, 6}, {7, 7, 8, 7, 6, 7}, {6, 7, 7, 6, 7, 8}, {5, 6, 7, 7, 6, 8}, {5, 5, 6, 6, 5, 9}, {4, 5, 5, 7, 4, 8}, {3, 4, 5, 8, 5, 7}, {2, 4, 6, 7, 4, 8}, {1, 3, 7, 6, 3, 7}, {1, 2, 6, 5, 2, 6}, {2, 1, 5, 4, 1, 5}, {3, 0, 4, 3, 0, 4}, {4, 1, 3, 2, 0, 3}, {3, 1, 2, 2, 1, 2}, {2, 0, 2, 1, 1, 2}, {1, 0, 3, 0, 0, 1}, {1, 0, 3, 0, 0, 0}, {0, 0, 2, 0, 0, 0}, {0, 1, 1, 1, 1, 0}, {1, 2, 1, 2, 1, 1}, {1, 3, 2, 1, 2, 2}, {1, 3, 3, 2, 3, 1}, {2, 3, 4, 2, 2, 2}, {3, 4, 5, 3, 2, 3}, {4, 4, 6, 4, 2, 3}, {5, 4, 7, 5, 3, 4}, {6, 5, 8, 5, 4, 5}, {6, 6, 7, 6, 5, 6}, {6, 7, 8, 7, 6, 6}, {7, 7, 9, 7, 6, 7}, {8, 6, 10, 6, 7, 8}, {9, 6, 10, 5, 8, 8}, {9, 6, 11, 5, 9, 8}, {9, 7, 12, 5, 10, 8}, {10, 8, 13, 6, 11, 9}, {10, 8, 14, 7, 12, 10}, {11, 9, 15, 8, 13, 11}, {12, 10, 16, 9, 14, 11}, {13, 10, 17, 10, 15, 12}, {14, 11, 18, 11, 16, 13}, {14, 11, 18, 11, 16, 14}, {14, 12, 17, 12, 15, 14}, {13, 13, 16, 13, 15, 15}, {12, 13, 15, 12, 14, 14}, {11, 13, 14, 11, 13, 13}, {10, 12, 13, 10, 13, 12}, {9, 11, 12, 9, 12, 11}, {9, 10, 11, 8, 11, 9}, {8, 9, 10, 9, 10, 8}, {8, 8, 9, 9, 9, 7}, {7, 7, 9, 8, 9, 7}, {7, 6, 9, 7, 9, 6}, {8, 6, 8, 6, 10, 6}, {8, 5, 8, 6, 11, 5}, {9, 4, 9, 6, 12, 4}, {10, 3, 8, 7, 11, 3}, {11, 4, 7, 8, 10, 2}, {10, 5, 6, 9, 9, 1}, {9, 6, 6, 8, 9, 2}, {9, 7, 7, 7, 8, 3}, {10, 7, 8, 6, 7, 3}, {10, 7, 9, 5, 7, 4}, {9, 8, 10, 4, 6, 5}, {8, 8, 11, 3, 5, 6}, {7, 8, 11, 2, 5, 7}, {6, 9, 10, 1, 6, 8}, {5, 9, 9, 0, 7, 9}, {4, 9, 8, 0, 8, 9}, {5, 10, 7, 1, 9, 9}, {6, 9, 7, 2, 9, 10}, {7, 7, 8, 3, 9, 11}, {8, 6, 9, 4, 10, 12}, {9, 6, 10, 5, 10, 13}, {9, 7, 9, 6, 9, 14}, {10, 8, 8, 7, 8, 14}, {10, 9, 7, 8, 7, 14}, {11, 9, 8, 9, 8, 14}, {12, 10, 8, 10, 8, 15}, {13, 11, 9, 10, 8, 16}, {14, 12, 10, 11, 9, 17}, {15, 12, 11, 12, 10, 18}, {16, 13, 11, 13, 11, 18}, {17, 14, 12, 14, 12, 18}, {18, 14, 13, 15, 13, 17}, {18, 14, 12, 16, 14, 16}, {17, 15, 11, 15, 13, 15}, {17, 15, 10, 14, 12, 15}, {16, 16, 11, 13, 11, 15}, {15, 17, 12, 12, 10, 14}, {15, 18, 13, 11, 9, 13}, {13, 17, 12, 10, 9, 13}, {12, 16, 11, 9, 8, 12}, {11, 15, 10, 8, 7, 11}, {10, 14, 9, 7, 6, 10}, {10, 13, 8, 6, 5, 9}, {10, 12, 9, 7, 4, 8}, {9, 11, 10, 8, 5, 7}, {8, 10, 11, 9, 5, 6}, {7, 9, 12, 10, 6, 5}, {6, 9, 11, 10, 7, 6}, {7, 8, 10, 9, 8, 7}, {8, 9, 9, 8, 9, 8}};

    // traction arrays
    // cornering
    int16_t accX [] = {6, 4, -1, -3, -1, 2, 0, -7, -19, -21, -11, -8, -12, -19, -23, -21, -17, -13, -16, -24, -21, -17, -16, -15, -12, -26, -26, -29, -34, -31, -25, -20, -17, -2, 57, 54, 67, 92, 106, 118, 133, 148, 180, 170, 205, 244, 268, 323, 334, 386, 427, 423, 489, 502, 489, 467, 402, 378, 309, 244, 169, 47, -63, -124, -181, -258, -278, -301, -297, -272, -253, -170, -130, -80, 11, 76, 192, 208, 251, 336, 352, 366, 394, 416, 392, 361, 370, 340, 342, 329, 318, 290, 264, 256, 203, 206, 210, 139, 141, 137, 132, 144, 148, 155, 174, 183, 184, 192, 187, 192, 190, 175, 165, 127, 106, 92, 64, 46, 25, -6, -30, -49, -66, -90, -99, -156, -173, -187, -211, -222, -233, -233, -264, -282, -235, -253, -295, -333, -329, -324, -399, -415, -471, -518, -494, -497, -476, -475, -461, -391, -357, -294, -261, -253, -194, -178, -165, -169, -166, -147, -144, -133, -139, -149, -126, -120, -117, -102, -87, -71, -54, -29, -9, 0, 4, 10, 12, 13, 16, 15, 12, 14, 14, 8, 0, -1, -1, -1, -2, -5, -16, -34, -47, -59, -119, -161, -177, -215, -278, -300, -318, -336, -334, -304, -272, -267, -234, -175, -151, -118, -79, -58, -29, -26, -20, -30, -41, -37, -37, -44, -53, -54, -46, -44, -37, -27, -14, 30, 90, 154, 181, 202, 237, 276, 294, 301, 293, 269, 252, 294, 315, 343, 376, 412, 437, 432, 455, 442, 387, 353, 309, 254, 220, 178, 158, 115, 80, 67, 75, 75, 79, 124, 132, 127, 130, 144, 148, 131, 113, 96, 31, 13, -8, -49, -75, -98, -111, -121, -142, -317, -341, -346, -401, -440, -464, -489, -513, -568, -427, -412, -424, -358, -315, -282, -250, -222, -147, -107, -93, -69, -71, -67, -53, -43, -32, -24, -19, -16, -5, 19, 34, 51, 64, 78, 98, 124, 157, 206, 216, 234, 275, 284, 341, 361, 350, 372, 419, 459, 481, 491, 505, 457, 450, 460, 429, 398, 360, 322, 306, 274, 262, 232, 215, 212, 158, 149, 144, 95, 80, 60, 47, 29, 2, -17, -34, -49, -58, -58, -61, -64, -61, -47, -37, -30, -21, -15, -9, 0, 8, 12, 0, -10, -30, -50, -51, -47, -36, -27, -18, -7, 0, 36, 73, 110, 151, 222, 270, 326, 368, 383, 389, 401, 409, 387, 328, 318, 285, 255, 316, 320, 331, 328, 329, 331, 315, 323, 313, 263, 252, 194, 129, 60, -47, -166, -277, -370, -439, -467, -462, -447, -422, -345, -260, -191, -114, -73, -54, -27, 3, 46, 134, 200, 236, 291, 329, 349, 346, 334, 310, 222, 147, 106, 43, -7, -39, -73, -105, -134, -171, -183, -190, -191, -181, -171, -134, -100, -36, 41, 98, 136, 182, 249, 269, 334, 432, 427, 434, 454, 464, 466, 413, 428, 369, 273, 255, 220, 177, 152, 121, 121, 90, 71, 56, 47, 47, 37, 35, 39, 29, 21, 23, 25, 16, 4, -2, -13, -23, -38, -46, -52, -49, -44, -38, -33, -30, -26, -8, 9, 18, 15, 12, 10, 5, 3, 2, -4, -13, -20, -22, -21, -21, -27, -44, -47, -57, -98, -115, -123, -130, -144, -151, -131, -132, -128, -93, -73, -70, -71, -59, -49, -57, -55, -70, -101, -131, -185, -233, -262, -283, -320, -392, -402, -414, -435, -399, -384, -368, -352, -320, -253, -230, -190, -141, -125, -92, -78, -65, -57, -54, -44, -36, -41, -40, -40, -34, -30, -22, -2, 3, 9, 19, 28, 49, 54, 69, 90, 92, 101, 157, 201, 236, 280, 302, 352, 389, 406, 454, 423, 382, 345, 288, 259, 186, 120, 91, 33, 2, -9, -9, -8, -8, -2, 2, -2, -6, -3, 2, 3, 3, 2, 5, 16, 20, 22, 27, 34, 43, 47, 49, 42, 27, 19, 14, 9, -3, -65, -73, -80, -91, -89, -84, -108, -140, -172, -131, -162, -219, -265, -326, -370, -429, -458, -482, -506, -477, -413, -347, -263, -190, -91, 13, 91, 134, 184, 221, 242, 237, 248, 266, 249, 260, 292, 288, 266, 266, 281, 258, 238, 226, 213, 199, 166, 155, 133, 99, 77, 59, 44, 26, -4, -18, -28, -40, -39, -36, -36, -25, -13, 6, 49, 91, 150, 212, 297, 356, 434, 531, 569, 611, 613, 603, 582, 529, 521, 457, 398, 402, 352, 373, 372, 370, 408, 393, 396, 404, 375, 364, 323, 319, 297, 244, 223, 201, 150, 123, 104, 83, 45, 32, 18, 6, 1};
    // braking/accelerating
    int16_t accY [] = {53, 62, 50, 64, 81, 80, 75, 71, 68, 62, 60, 60, 62, 63, 63, 61, 56, 50, 42, 38, 31, 28, 25, 26, 15, -16, -47, -67, -67, -103, -142, -180, -180, -140, -102, -221, -378, -501, -428, -360, -349, -356, -304, -243, -304, -417, -430, -332, -208, -162, -117, -95, -77, -66, -60, -59, -62, -66, -42, -3, 35, 77, 101, 103, 77, 55, 53, 52, 4, -42, -79, -68, -55, -44, -67, -125, -181, -191, -165, -127, -99, -74, -70, -72, -76, -66, -40, -28, -42, -35, -10, 54, 85, 111, 121, 133, 137, 132, 124, 127, 129, 157, 202, 206, 137, 120, 149, 224, 203, 175, 144, 149, 162, 179, 136, 94, 90, 130, 147, 122, 77, 57, 52, 71, 73, 63, 30, 1, -33, -53, -75, -116, -152, -239, -303, -339, -308, -270, -236, -205, -176, -130, -88, -55, -56, -65, -93, -105, -80, -28, 17, 43, 74, 97, 98, 89, 110, 139, 158, 144, 128, 167, 207, 194, 145, 120, 145, 151, 139, 131, 127, 118, 107, 105, 116, 115, 92, 96, 137, 154, 134, 93, 90, 84, 73, 73, 79, 86, 88, 83, 75, 65, 61, 59, 61, 55, 49, 37, 29, 25, 27, 30, 28, 24, 20, 18, 11, 5, 15, 27, 27, 12, 6, 18, 18, -16, -56, -92, -101, -86, -61, -78, -139, -247, -333, -381, -354, -301, -306, -334, -356, -358, -387, -374, -313, -275, -293, -296, -252, -173, -139, -101, -94, -96, -102, -96, -82, -69, -68, -47, -19, 13, 29, 67, 111, 161, 160, 152, 148, 167, 208, 219, 197, 151, 131, 174, 194, 181, 103, 34, -25, -80, -208, -301, -315, -242, -230, -282, -336, -316, -264, -193, -156, -137, -139, -159, -159, -141, -98, -77, -71, -63, -54, -46, -19, 9, 48, 61, 84, 101, 119, 148, 157, 159, 152, 160, 182, 191, 203, 231, 238, 229, 191, 168, 158, 119, 73, 9, -43, -89, -122, -114, -101, -103, -112, -106, -82, -63, -55, -57, -68, -79, -70, -48, -21, 7, 38, 69, 102, 112, 114, 115, 141, 167, 170, 158, 166, 185, 185, 165, 150, 148, 137, 124, 119, 125, 132, 129, 115, 93, 73, 72, 80, 94, 94, 87, 80, 71, 62, 32, 9, -27, -60, -90, -100, -93, -138, -201, -264, -286, -297, -309, -317, -324, -309, -281, -241, -200, -167, -133, -120, -91, -71, -58, -72, -60, -25, 17, 39, 54, 75, 97, 115, 128, 136, 135, 103, 68, 9, -19, -76, -135, -193, -206, -188, -196, -234, -224, -171, -103, -86, -62, -43, -22, 2, 31, 77, 118, 180, 226, 235, 221, 195, 184, 145, 115, 102, 114, 109, 93, 97, 120, 141, 141, 115, 79, 17, -49, -145, -209, -232, -186, -166, -174, -213, -237, -249, -239, -218, -197, -197, -194, -177, -159, -122, -100, -78, -81, -84, -88, -84, -79, -64, -52, -28, 3, 32, 51, 56, 61, 65, 95, 155, 172, 154, 129, 146, 171, 199, 237, 251, 231, 207, 203, 195, 181, 174, 176, 176, 176, 182, 181, 174, 137, 115, 108, 134, 154, 143, 121, 106, 113, 120, 100, 88, 86, 94, 92, 67, 50, 41, 45, 50, 56, 61, 59, 53, 45, 38, 36, 27, -32, -94, -132, -110, -165, -252, -345, -376, -386, -364, -311, -245, -244, -284, -344, -355, -327, -294, -217, -145, -74, -66, -71, -76, -81, -56, -38, -47, -58, -43, 36, 93, 124, 107, 83, 79, 159, 240, 255, 199, 184, 229, 264, 231, 165, 82, 93, 134, 198, 182, 159, 138, 130, 132, 136, 120, 100, 72, 64, 84, 110, 114, 94, 76, 76, 73, 64, 56, 51, 54, 58, 60, 59, 60, 62, 61, 58, 59, 61, 42, 29, 40, 59, 57, 54, 61, 69, 54, 35, 27, 22, 27, 38, 49, 49, 38, 27, 10, -25, -55, -67, -112, -217, -321, -360, -345, -333, -333, -333, -332, -309, -285, -263, -271, -278, -283, -256, -229, -183, -148, -100, -77, -72, -69, -53, -42, -46, -48, -14, 24, 51, 64, 94, 129, 144, 107, 46, -12, -22, -9, 29, 58, 89, 103, 123, 134, 138, 115, 117, 144, 194, 224, 234, 200, 156, 144, 166, 172, 145, 121, 116, 104, 94, 79, 71, 65, 59, 15, -35, -94, -115, -122, -112, -101, -96, -88, -66, -47, -47, -62, -63, -32, 4, 50, 77, 90, 74, 57, 88, 130, 152, 130, 113, 130, 119, 92, 53, 55, 65, 76, 80, 77, 75, 82, 95, 100, 95, 77, 63, 44, 26};
    
    // lap data arrays
    uint16_t distance [] = {0, 3, 11, 14, 25, 29, 42, 56, 60, 71, 74, 83, 97, 100, 105, 117, 127, 136, 149, 150, 179, 181, 194, 208, 210, 223, 231, 239, 248, 253, 255, 273, 273, 288, 288, 308, 308, 323, 324, 325, 338, 342, 349, 353, 360, 361, 369, 371, 383, 384, 392, 395, 400, 406, 407, 414, 416, 420, 426, 430, 436, 442, 442, 450, 451, 454, 459, 461, 477, 477, 484, 484, 490, 496, 499, 502, 506, 513, 517, 524, 526, 529, 537, 539, 543, 548, 552, 555, 559, 562, 562, 571, 572, 573, 580, 581, 588, 588, 595, 599, 601, 608, 615, 625, 632, 637, 638, 651, 661, 662, 678, 684, 689, 696, 700, 702, 711, 716, 728, 733, 755, 759, 768, 775, 788, 791, 808, 809, 819, 827, 830, 847, 851, 866, 869, 881, 885, 891, 900, 904, 909, 915, 917, 925, 928, 935, 944, 945, 951, 953, 958, 966, 969, 974, 980, 984, 992, 994, 1012, 1015, 1019, 1036, 1037, 1045, 1047, 1054, 1057, 1063, 1065, 1076, 1082, 1088, 1094, 1108, 1118, 1123, 1131, 1136, 1144, 1154, 1157, 1171, 1182, 1190, 1193, 1203, 1207, 1230, 1234, 1245, 1248, 1263, 1265, 1327, 1335, 1357, 1359, 1375, 1383, 1396, 1402, 1417, 1435, 1446, 1467, 1469, 1490, 1490, 1503, 1507, 1520, 1526, 1543, 1553, 1563, 1573, 1578, 1584, 1586, 1603, 1605, 1621, 1628, 1641, 1652, 1663, 1672, 1681, 1691, 1697, 1703, 1707, 1713, 1718, 1721, 1726, 1731, 1739, 1742, 1751, 1752, 1758, 1761, 1765, 1765, 1771, 1771, 1778, 1781, 1787, 1787, 1794, 1801, 1808, 1809, 1815, 1821, 1821, 1830, 1831, 1838, 1839, 1848, 1854, 1866, 1869, 1870, 1877, 1884, 1890, 1906, 1910, 1916, 1918, 1927, 1930, 1938, 1938, 1946, 1951, 1953, 1956, 1960, 1961, 1961, 1966, 1967, 1971, 1978, 1980, 1984, 1987, 1988, 1993, 1997, 1997, 2002, 2002, 2008, 2010, 2015, 2021, 2021, 2025, 2030, 2032, 2038, 2038, 2045, 2053, 2054, 2059, 2066, 2069, 2077, 2085, 2087, 2100, 2110, 2117, 2126, 2133, 2134, 2146, 2147, 2152, 2155, 2156, 2160, 2164, 2168, 2173, 2181, 2182, 2188, 2195, 2198, 2202, 2211, 2215, 2218, 2231, 2232, 2234, 2243, 2246, 2260, 2260, 2269, 2272, 2289, 2293, 2305, 2311, 2321, 2329, 2338, 2341, 2347, 2352, 2360, 2366, 2370, 2379, 2391, 2393, 2401, 2412, 2415, 2453, 2459, 2470, 2478, 2485, 2494, 2502, 2516, 2520, 2533, 2539, 2543, 2551, 2561, 2565, 2569, 2572, 2577, 2584, 2586, 2593, 2596, 2604, 2604, 2615, 2620, 2625, 2628, 2634, 2640, 2642, 2648, 2657, 2659, 2661, 2673, 2675, 2687, 2689, 2698, 2698, 2706, 2710, 2714, 2721, 2722, 2731, 2734, 2739, 2740, 2745, 2746, 2757, 2757, 2765, 2769, 2772, 2777, 2781, 2785, 2787, 2795, 2796, 2802, 2810, 2822, 2823, 2837, 2840, 2848, 2850, 2858, 2861, 2874, 2880, 2884, 2891, 2904, 2910, 2923, 2926, 2931, 2940, 2944, 2947, 2953, 2962, 2964, 2974, 2975, 2982, 2988, 2989, 2994, 2995, 3003, 3003, 3008, 3011, 3012, 3014, 3019, 3019, 3023, 3023, 3026, 3027, 3028, 3032, 3034, 3036, 3041, 3044, 3048, 3051, 3053, 3060, 3060, 3068, 3071, 3073, 3081, 3083, 3094, 3096, 3104, 3113, 3121, 3121, 3131, 3135, 3145, 3146, 3154, 3165, 3165, 3174, 3182, 3185, 3202, 3205, 3215, 3220, 3229, 3243, 3245, 3262, 3274, 3281, 3301, 3302, 3316, 3318, 3327, 3343, 3346, 3358, 3360, 3366, 3378, 3385, 3390, 3405, 3407, 3419, 3422, 3430, 3433, 3445, 3454, 3460, 3471, 3474, 3483, 3493, 3498, 3507, 3512, 3518, 3524, 3528, 3530, 3536, 3539, 3544, 3548, 3550, 3555, 3555, 3559, 3560, 3563, 3568, 3570, 3574, 3578, 3579, 3583, 3590, 3591, 3596, 3596, 3605, 3606, 3615, 3622, 3625, 3632, 3641, 3641, 3653, 3686, 3695, 3698, 3713, 3714, 3726, 3732, 3736, 3750, 3752, 3761, 3762, 3773, 3773, 3789, 3793, 3808, 3820, 3821, 3836, 3849, 3852, 3874, 3877, 3889, 3897, 3909, 3910, 3926, 3926, 3941, 3943, 3953, 3957, 3971, 3986, 3986, 3998, 4000, 4014, 4020, 4029, 4037, 4045, 4051, 4056, 4063, 4081, 4092, 4108, 4119, 4129, 4145, 4154, 4163, 4174, 4177, 4187, 4202, 4202, 4205, 4223, 4224, 4234, 4243, 4243, 4251, 4259, 4260, 4266, 4267, 4273, 4277, 4281, 4284, 4287, 4292, 4293, 4298, 4303, 4305, 4313, 4314, 4319, 4322, 4324, 4332, 4335, 4338, 4345, 4349, 4356, 4358, 4370, 4370, 4377, 4378, 4387, 4393, 4396, 4403, 4414, 4418, 4422, 4427, 4430, 4445, 4447, 4458, 4461, 4475, 4476, 4494, 4502, 4519, 4520, 4532, 4532, 4555, 4557, 4567, 4572, 4590, 4596, 4607, 4614, 4618, 4629, 4636, 4645, 4646, 4656, 4664, 4668, 4677, 4679, 4690, 4697, 4705, 4720, 4722, 4731, 4733, 4741, 4750, 4759, 4760, 4773, 4776, 4783, 4792, 4807, 4812, 4818, 4831, 4837, 4848, 4848, 4862, 4863, 4873, 4879};
    uint16_t corner_distance [] = {407, 466, 541, 700, 958, 1400, 1761, 2000, 2183, 2609, 2769, 2856, 3021, 3583, 3815, 4298, 4369, 4634, 4752};
    uint8_t corner_numbers [] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    uint16_t sector_thresholds [] = {1510, 3450};
    // elevation numbers have been nomralised to an 8 bit value
    uint8_t elevation [] = {56, 56, 56, 56, 56, 59, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 59, 56, 56, 56, 56, 59, 61, 61, 61, 61, 59, 56, 56, 56, 53, 51, 51, 51, 51, 51, 51, 49, 47, 47, 47, 42, 42, 33, 33, 37, 37, 42, 42, 42, 42, 42, 42, 37, 37, 42, 47, 47, 47, 49, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 53, 56, 56, 56, 56, 56, 59, 61, 61, 63, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 63, 61, 61, 61, 61, 63, 66, 66, 70, 70, 70, 70, 75, 77, 80, 80, 80, 77, 75, 70, 70, 70, 75, 75, 75, 75, 75, 75, 77, 80, 80, 80, 85, 85, 89, 89, 89, 94, 94, 99, 99, 103, 103, 108, 108, 103, 103, 108, 108, 103, 99, 99, 94, 94, 94, 89, 89, 75, 75, 75, 75, 69, 66, 63, 61, 59, 56, 56, 56, 59, 61, 61, 61, 61, 63, 66, 69, 70, 75, 75, 75, 75, 75, 75, 75, 75, 75, 77, 80, 80, 80, 80, 83, 85, 87, 89, 89, 89, 89, 89, 89, 89, 89, 89, 91, 94, 94, 99, 99, 103, 108, 118, 132, 132, 136, 136, 151, 155, 174, 179, 188, 203, 217, 221, 231, 231, 240, 245, 245, 245, 245, 245, 245, 243, 240, 240, 240, 245, 245, 250, 250, 250, 250, 245, 245, 245, 250, 250, 250, 245, 245, 250, 250, 250, 250, 250, 250, 250, 252, 255, 255, 252, 250, 250, 250, 250, 250, 250, 250, 248, 245, 245, 245, 245, 245, 240, 240, 236, 236, 231, 221, 221, 217, 212, 212, 203, 203, 198, 198, 188, 184, 184, 174, 170, 151, 132, 127, 122, 132, 136, 141, 146, 146, 151, 151, 151, 151, 151, 151, 151, 146, 141, 136, 132, 132, 118, 103, 94, 94, 89, 89, 94, 99, 99, 99, 99, 103, 103, 103, 103, 103, 103, 99, 94, 89, 89, 89, 89, 94, 99, 103, 109, 113, 115, 118, 118, 118, 118, 118, 118, 118, 118, 120, 122, 122, 122, 122, 122, 125, 125, 122, 122, 127, 127, 127, 127, 127, 129, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 132, 129, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 125, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 122, 125, 127, 127, 127, 127, 129, 132, 132, 132, 129, 127, 127, 127, 127, 127, 127, 127, 127, 122, 118, 113, 107, 103, 103, 101, 99, 99, 99, 99, 97, 94, 94, 94, 94, 94, 94, 94, 94, 91, 91, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 89, 85, 85, 83, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 83, 85, 85, 85, 83, 80, 80, 80, 80, 80, 80, 80, 78, 75, 72, 70, 70, 70, 70, 70, 70, 70, 70, 70, 70, 69, 68, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 68, 70, 70, 70, 70, 70, 70, 68, 66, 66, 66, 61, 51, 51, 51, 56, 56, 59, 61, 59, 56, 59, 61, 63, 66, 66, 66, 66, 66, 63, 61, 61, 61, 61, 61, 60, 59, 58, 57, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 54, 52, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 52, 54, 56, 56, 56, 56, 61, 61, 61, 61, 61, 61, 63, 66, 66, 66, 63, 61, 56, 56, 51, 47, 42, 37, 23, 18, 9, 4, 4, 4, 4, 0, 0, 0, 4, 4, 9, 13, 18, 23, 28, 33, 37, 37, 37, 37, 37, 36, 34, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 34, 35, 36, 37, 40, 42, 40, 37, 37, 37, 40, 44, 47, 47, 49, 51, 51, 51, 53, 56, 53, 51, 51, 51, 51, 59, 48, 47, 47, 47, 48, 49, 50, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 52, 53, 54, 55, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56};
    // these have not been nomralised and are the raw max/min from the dataset
    uint8_t elevation_limits [] = {230, 176};

    // 0 is left, 1 is right
    uint8_t corner_direction [] = {0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0};
    // normalised so they start at 0 (nomially first corner is 40 degrees, so -40 from every value), manually calculted values.
    uint16_t corner_angle [] = {0, 116, 108, 159, 215, 116, 72, 137, 72, 157, 210, 180, 223, 336, 240, 270, 180, 270, 328, 0};

    // battery and tyre temps
    uint8_t battery_level = 100;
    uint8_t left_front = 95;
    uint8_t left_rear = 93;
    uint8_t right_front = 95;
    uint8_t right_rear = 94;

    // oil & water indicator bools
    bool oilWarning = false;
    bool waterWarning = false;

    // pit & SC (saftey car) bools
    bool pitIn = false;
    bool safteyCar = false; // we are probably not going to signal the safety car, but we do want the ability to

    // fuel and boost values
    int8_t fuel = 100;
    int8_t boost = 100;

    // logic variables
    //----------------------------------------------------------------
    uint16_t array_max = 742; // number of entries, not indicies max
    uint8_t upcomming_corner = 1;
    uint8_t num_of_corners = 19;
    uint8_t print_distance_to_corner = 55;
    uint8_t current_sector = 1;
    uint8_t sector_max = 3;
    uint16_t lap_distance = 4927;
    uint16_t redline = 11500;
    uint8_t pit_stop_fuel_level = 15;
    uint16_t count = 0;

    // Configure background images and other images for use in the example
    // we could perfrom these calls to CMD_SETBITMAP in the main display list as well
    configure_bitmaps();

    while(1)
    {
        //--------------------------------------------------------------------------------------------------------
        // Construct display list to send to EVE
        //--------------------------------------------------------------------------------------------------------

        EVE_LIB_BeginCoProList();
        EVE_CMD_DLSTART();
        EVE_CLEAR_COLOR_RGB(0, 0, 0);
        EVE_CLEAR(1, 1, 1);

        // Draw background image and bottom bar 
        //----------------------------------------------------------------

        EVE_BEGIN(EVE_BEGIN_BITMAPS);
        // We want to darken this image slightly, so we use a COLOR_RGB command here to do this.
        EVE_COLOR_RGB(80, 80, 140); // slight purple tint
        // Draw the background image if it can be drawn.
        EVE_BITMAP_HANDLE(Carbon_Fiber_800x480_asset.Handle);
        // Place BG carbon fiber at 0,0
        EVE_VERTEX2F(0*16, 0*16);
        
        //reset colour to white
        EVE_COLOR_RGB(255, 255, 255);
        EVE_BITMAP_HANDLE(Bottom_Bar_800x8_asset.Handle);
        // Place bottom bar at 0,380
        EVE_VERTEX2F(0*16, 365*16);
        // end bitmaps - important to call end here as the widget functions will use save/restore blocks and we may accidentally
        // alter the bitmap properties for the above bitmaps if we dont call end() here.
        EVE_END();

        // Add widgets onto screen
        //----------------------------------------------------------------   

        // Draw acceleration data
        tractionCircle(14,14, (array_max-1), count, accX, accY);

        // Draw Oil temp warning light (cell 0 in the bitmap so 0 is passed into the function after the bitmap properties)
        iconIndicator(160, 100, &Oil_Water_Icons_32x64_asset, 0, 0xEE0000, oilWarning);

        // Value range 0-6, this represents the sequence of lights illuminating
        // If we are braking theres no need for the shift light to have a value
        if(brake[count] == 1){
            shiftLight(150, 10, 0);
        // Else display shift light based on defined RPM ranges
        } else{
            if (RPM[count] < 9900)
                shiftLight(150, 10, 0);
            else if ((RPM[count] >= 9900) && (RPM[count]  < 10100))
                shiftLight(150, 10, 1);
            else if ((RPM[count] >= 10100) && (RPM[count]  < 10400))
                shiftLight(150, 10, 2);
            else if ((RPM[count] >= 10400) && (RPM[count]  < 10700))
                shiftLight(150, 10, 3);
            else if ((RPM[count] >= 10700) && (RPM[count]  < 11100))
                shiftLight(150, 10, 4);
            else if ((RPM[count] >= 11100) && (RPM[count]  < 11400))
                shiftLight(150, 10, 5);
            else if (RPM[count]  >= 11400)
                shiftLight(150, 10, 6);
        }  

        // Draw coolant temp warning light (cell 0 in the bitmap so 1 is passed into the function after the bitmap properties)
        iconIndicator(640, 100, &Oil_Water_Icons_32x64_asset, 1, 0xEE6600, waterWarning); 

        // Draw track map OR corner arrow
        // Check if were close to a corner, if we are, print the corner direction icon
        if((corner_distance[upcomming_corner-1] - distance[count]) <= print_distance_to_corner){

            // If a left hand corner
            if(corner_direction[upcomming_corner-1] == 0){
                // Bitmap has cells so divide height by 2
                // Use first cell
                cornerIndicator(676, 14, upcomming_corner, 0, &Arrows_96x192_asset);
            } else {
                // Use second cell
                cornerIndicator(676, 14, upcomming_corner, 1, &Arrows_96x192_asset);
            }       

        } else {
            // Print track map
            // If we are past the last corner, print as if were approaching the first corner, else print for the upcoming corner
            if (distance[count] > corner_distance[num_of_corners-1])
                trackMap(676, 14, corner_angle[0], &Trackmap_96x60_asset);
            else
                trackMap(676, 14, corner_angle[upcomming_corner-1], &Trackmap_96x60_asset);
        }

        // Draw rev counter
        revCounter(14, 74, 422, 34, 115, 130, (redline/100), 28, (RPM[count]/100)); 

        // Draw fuel indicator, colour red if near empty
        if (fuel <= pit_stop_fuel_level)
            fuelIndicator(650, 140, &Side_Gauge_R_136x104_asset, &Side_Gauge_R_Fill_136x104_asset, 0xFF0000, fuel);
        else
            fuelIndicator(650, 140, &Side_Gauge_R_136x104_asset, &Side_Gauge_R_Fill_136x104_asset, 0x00CC00, fuel);

        // Draw boost indicator
        boostIndicator(15, 140, &Side_Gauge_L_136x104_asset, &Side_Gauge_L_Fill_136x104_asset, boost);

        // Draw pit and saftey car indicator
        pitAndScWidget(271, 158, &PIT_SC_Widget_80x36_asset, pitIn, safteyCar);

        // Draw DRS indicator
        drsIcon(370, 155, DRS[count]);   
        
        // Draw Intake Air Temp bargraph
        // there are only 106 entries in the IAT array
        iatGraph(241, 201, IAT[count % 106]);

        // Draw speed widget
        speedWidget(200, 235, Widget_Speed_104x56_asset.Handle, 31, "km/h", speed[count]);
             
        // Draw RPM widget and colour when we hit redline
        if(RPM[count] > redline)
            rpmWidget(152,300, Widget_RPM_152x56_asset.Handle, 31, "RPM", 0xFF0000, RPM[count]);
        else
            rpmWidget(152,300, Widget_RPM_152x56_asset.Handle, 31, "RPM", 0xFFFFFF, RPM[count]);

        // Draw gear widget
        gearWidget(324, 204, Widget_Gear_152x152_asset.Handle, eurostile_150_L8.Handle, gears[count]);

        // Draw Air Fuel Ratio graph
        // the graph plot expects a value range of 0-24
        afrGraph(495, 201, AFR[count]);

        // Draw throttle pos
        verticalBarGauge(495, 241, 0x00CC00, "Throttle", throttle[count]);

        // Draw brake pos
        verticalBarGauge(560, 241, 0xFF0000, "Brake", (brake[count]) * 100);

        // Draw steering angle
        steeringAngle(660, 340, &Steering_Wheel_32x32_asset, steering[count]);

        // Draw elevation
        elevationWidget(25, 380, elevation_limits[0], elevation_limits[1], elevation[count]);

        // Print tyre tempratues
        tyreTemps(135, 380, Car_Overhead_44x80_asset.Handle, 94, 96, left_front, left_rear, right_front, right_rear);

        // Print sector gauge
        sectorWidget(400, 425, 40, 10, 30, ((sector_thresholds[0]*360)/distance[array_max-1]), ((sector_thresholds[1]*360)/distance[array_max-1]), current_sector, ((distance[count]*360)/distance[array_max-1]));

        // Print lap and sector times
        // This function can take a colour to print for sector times delta indication, but as we are only using one lap of data we will send RED for sector 1 and Green for sector 2
        lapAndSectorTimes(465, 380, 0xFF0000, 0x00BB00, sector_times[0], sector_times[1], laptime_msec[count]);

        // Draw battery deployment indicator
        // Colour based on input value
        if(battery_level > 40)
            batteryIndicator(685, 380, Battery_Cells_40x720_asset.Handle, 0xFFFFFF, battery_level); //colour white
        if((battery_level <= 40)  &&  (battery_level >=21))
            batteryIndicator(685, 380, Battery_Cells_40x720_asset.Handle, 0xEE9900, battery_level); //colour orange
        if(battery_level < 21)
            batteryIndicator(685, 380, Battery_Cells_40x720_asset.Handle, 0xEE0000, battery_level); //colour red

        // Print FPS and screen update info on screen
        //----------------------------------------------------------------
        EVE_CMD_TEXT(0, 0, 20, EVE_OPT_FORMAT, "%d FPS", FPS);
        EVE_CMD_TEXT(800, 0, 20, EVE_OPT_FORMAT | EVE_OPT_RIGHTX, "%d DL/sec", DLPS);
        
        // Send display to EVE
        EVE_DISPLAY();
        EVE_DISPLAY(); // per BRT_TN_005
        EVE_CMD_SWAP();
        EVE_LIB_EndCoProList();

        //--------------------------------------------------------------------------------------------------------
        // Perform logic for screen updates
        //--------------------------------------------------------------------------------------------------------

        // Increment count through data arrays
        count ++;

        // Increment corner numnber if we are past the corner, but if we are not at the last corner 
        if (upcomming_corner != 19){
            if (distance[count] > corner_distance[upcomming_corner-1]){
                // Update upcomming corner number
                upcomming_corner ++;
            }
        }

        // As we pass through the sectors increment the sector variable (if were not in the last sector), and the sector times
        // There are only 3 sectors on the lap
        if (current_sector != sector_max){
            if (distance[count] > sector_thresholds[current_sector-1]){
                // Sector times
                if(current_sector == 1){
                    // If were in the first sector, the sector time is just the current lap time as we pass the threshold
                    sector_times[current_sector-1] = laptime_msec[count-1];
                }else{
                    // If were not in the first sector we take the current laptime and negate the last sector time
                    sector_times[current_sector-1] = (laptime_msec[count-1] - sector_times[current_sector-2]);
                }
                // Sector count increment
                current_sector ++;
            }
        }

        // non data array logic 
        //----------------------------------------------------------------

        // Determine if we need to signal a pit stop
        // Make this flash by swithcing the bool state every x counts through the data arrays
        if((fuel <= pit_stop_fuel_level) && (count % 10 == 0)){
            pitIn = !pitIn;
        }
    
        // Use some fuel every x amount of data points, reset the varible once it gets close to 0
        if(count % 35 == 0){
            // Reset fuel when we are close to running out, and cancel pit in indicator
            if (fuel <= 3){
                fuel = 100;
                pitIn = false;
            }
            else{
                fuel--;
            }
        }

        // Determine boost pressure from throttle usage
        if (throttle[count] > 30 && (boost != 100)){
            // The more throttle applied the faster the boost will rise
            if (throttle[count] < 50)
                boost ++;
            else if ((throttle[count] >= 50) && (throttle[count] <= 90))
                boost = boost + 2;
            else if (throttle[count] > 90)
                boost = boost + 3;

            // Ensure we dont go above 100
            if (boost >= 100)
                boost = 100;

            } else if (throttle [count] == 0 && (boost != 0)){
                // Lose boost pressure
                if (boost < 30)
                    boost--;
                else
                    boost = boost - 4;

                // Ensure we dont go below 0
                if (boost <= 0)
                    boost = 0;
        }  

        // We dont have data to trigger the oil or water temprature warning lights
        // so lets just turn them on and off randomly
        if(count % 225 == 0)
            oilWarning = !oilWarning;
        if(count % 125 == 0)
            waterWarning = !waterWarning;

        // We dont have data for battery deployemnt, so we can use some logic here to estimate it
        // if were braking were probably regenerating the battery
        if(brake[count] == 1)
            if (battery_level < 99)
            battery_level = battery_level + 2; //add some juice         

        // If were on at least 99% throttle, were probably deploying the battery
        if(throttle[count] > 90)
            if (battery_level > 0)
                battery_level --; // use that extra power

        // We dont have data for tyre temps, so base this on g-force acceleration data
        if (accX[count] < -500)
            left_front ++;
        if (accX[count] > 570)
            right_front ++;
        // Hard braking might result in some rear tyre temp increase
        if (accY[count] < -415){
            left_rear ++;
            right_rear ++;
        }
        // Maybe the tyres cool off some when we are accelerating hard
        if (accY[count] > 238){
            left_front --;
            right_front --;
            left_rear --;
            right_rear --;
        }

        // End of lap logic 
        //----------------------------------------------------------------

        // Reset count, corner number. current sector, and sector times, if we start a new lap
        if (count == array_max){
            // Reset logic variables
            count = 0;
            upcomming_corner = 1;
            current_sector = 1;

            // For the (number of sectors - 1), reset the sector time variables
            for(int i = 0; i < (sector_max - 1); i++){
                sector_times[i] = 0;
            }
        }

        // Screen render time logic
        //----------------------------------------------------------------

        // Await here for a synchronised next screen update
        while (1)
        {
            currentMs = platform_get_time();
            if ((uint32_t)(currentMs - previousMs) >= frameMs) break;
        }
        
        // Update varibles for screen FPS and DL/sec calculations
        previousMs = currentMs;
        DLCount ++;

        if ((currentMs - clockMs) >= 1000){
            // Work out elspaed time
            clockMs = currentMs;
            // Work out frame rate
            previousFrames = currentFrames;
            currentFrames = EVE_LIB_MemRead32(EVE_REG_FRAMES);
            // FPS calc
            FPS = currentFrames - previousFrames;
            // Update varible for current data updates/sec
            DLPS = DLCount;
            DLCount = 0;
            DEBUG_PRINTF("FPS: %d DL/sec: %d\n", FPS, DLPS);
        } 

    }
}

// ######################################################################################################################################################################################################
// #######################################################                        Application Code begins here                        ###################################################################
// ######################################################################################################################################################################################################

void eve_example(void)
{

    EVE_Init();                 // Initialise the display

    flash_full_speed();         // Ensure flash can enter Full speed

    set_asset_props();          // Configure asset properties for custom assets used in application

    eve_display_load_assets();  // Load assets into RAM_G

    eve_display();              // Run Application

}