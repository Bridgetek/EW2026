#include "ov5640.h"
#include "ov5640_regs.h"
#include <stdbool.h>
#include "eve.h"

// Need GNU extensions
#define IM_MAX(a, b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define IM_MIN(a, b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

#define BLANK_LINES 8
#define DUMMY_LINES 6

#define BLANK_COLUMNS 0
#define DUMMY_COLUMNS 16

#define SENSOR_WIDTH 2624
#define SENSOR_HEIGHT 1964

#define ACTIVE_SENSOR_WIDTH (SENSOR_WIDTH - BLANK_COLUMNS - (2 * DUMMY_COLUMNS))
#define ACTIVE_SENSOR_HEIGHT (SENSOR_HEIGHT - BLANK_LINES - (2 * DUMMY_LINES))

#define DUMMY_WIDTH_BUFFER 16
#define DUMMY_HEIGHT_BUFFER 8

#define HSYNC_TIME 252
#define VYSNC_TIME 24

static uint16_t hts_target = 0;

static int16_t readout_x = 0;
static int16_t readout_y = 0;

static uint16_t readout_w = ACTIVE_SENSOR_WIDTH;
static uint16_t readout_h = ACTIVE_SENSOR_HEIGHT;

extern uint8_t g_using_line_event;

int regVideo[] =
{
// input clock 24Mhz
// PCLK 42Mhz
0x3035, 0x41, // PLL
0x3036, 0x69, // PLL
0x3c07, 0x07, // lightmeter 1 threshold[7:0]
0x3820, 0x41, // flip
0x3821, 0x07, // mirror
0x3814, 0x31, // timing X inc
0x3815, 0x31, // timing Y inc
0x3800, 0x00, // HS
0x3801, 0x00, // HS
0x3802, 0x00, // VS
0x3803, 0xfa, // VS
0x3804, 0x0a, // HW (HE)
0x3805, 0x3f, // HW (HE)
0x3806, 0x06, // VH (VE)
0x3807, 0xa9, // VH (VE)
0x3808, 0x05, // DVPHO
0x3809, 0x00, // DVPHO
0x380a, 0x02, // DVPVO
0x380b, 0xd0, // DVPVO
0x380c, 0x07, // HTS
0x380d, 0x64, // HTS
0x380e, 0x02, // VTS
0x380f, 0xe4, // VTS
0x3813, 0x04, // timing V offset
0x3618, 0x00,
0x3612, 0x29,
0x3709, 0x52,
0x370c, 0x03,
0x3a02, 0x02, // 60Hz max exposure
0x3a03, 0xe0, // 60Hz max exposure
0x3a08, 0x00, // B50 step
0x3a09, 0x6f, // B50 step
0x3a0a, 0x00, // B60 step
0x3a0b, 0x5c, // B60 step
0x3a0e, 0x06, // 50Hz max band
0x3a0d, 0x08, // 60Hz max band
0x3a14, 0x02, // 50Hz max exposure
0x3a15, 0xe0, // 50Hz max exposure
0x4004, 0x02, // BLC line number
0x3002, 0x1c, // reset JFIFO, SFIFO, JPG
0x3006, 0xc3, // disable clock of JPEG2x, JPEG
0x4713, 0x03, // JPEG mode 3
0x4407, 0x14, // Quantization sacle
0x460b, 0x37,
0x460c, 0x20,
0x4837, 0x16, // MIPI global timing
0x3824, 0x04, // PCLK manual divider
0x5001, 0x83, // SDE on, CMX on, AWB on
0x3503, 0x00, // AEC/AGC on
};

int regCapture[] =
{
// YUV Capture
// 2592 x 1944 3.75fps
// 24 MHz input clock, 42Mhz PCLK
0x3035, 0x71, // PLL
0x3036, 0x69, // PLL
0x3c07, 0x07, // lightmeter 1 threshold[7:0]
0x3820, 0x40, // flip
0x3821, 0x06, // mirror
0x3814, 0x11, // timing X inc
0x3815, 0x11, // timing Y inc
0x3800, 0x00, // HS
0x3801, 0x00, // HS
0x3802, 0x00, // VS
0x3803, 0x00, // VS
0x3804, 0x0a, // HW (HE)
0x3805, 0x3f, // HW (HE)
0x3806, 0x07, // VH (VE)
0x3807, 0x9f, // VH (VE)
0x3808, 0x0a, // DVPHO
0x3809, 0x20, // DVPHO
0x380a, 0x07, // DVPVO
0x380b, 0x98, // DVPVO
0x380c, 0x0b, // HTS
0x380d, 0x1c, // HTS
0x380e, 0x07, // VTS
0x380f, 0xb0, // VTS
0x3813, 0x04, // timing V offset
0x3618, 0x04,
0x3612, 0x2b,
0x3709, 0x12,
0x370c, 0x00,
0x4004, 0x06, // BLC line number
0x3002, 0x1c, // reset JFIFO, SFIFO, JPG
0x3006, 0xc3, // disable clock of JPEG2x, JPEG
0x4713, 0x02, // JPEG mode 2
0x4407, 0x14, // Quantization sacle
0x460b, 0x37,
0x460c, 0x20,
0x4837, 0x2c, // MIPI global timing
0x3824, 0x01, // PCLK manual divider
0x5001, 0x83, // SDE on, CMX on, AWB on, scale off
0x3503, 0x03, // AEC/AGC off
};

/* Initialization sequence for OV5640 */
static const uint16_t OV5640_Common[][2] =
{
  {OV5640_SCCB_SYSTEM_CTRL1, 0x11},
  {OV5640_SYSTEM_CTROL0, 0x82},
  {OV5640_SCCB_SYSTEM_CTRL1, 0x03},
  {0x3630, 0x36},
  {0x3631, 0x0e},
  {0x3632, 0xe2},
  {0x3633, 0x12},
  {0x3621, 0xe0},
  {0x3704, 0xa0},
  {0x3703, 0x5a},
  {0x3715, 0x78},
  {0x3717, 0x01},
  {0x370b, 0x60},
  {0x3705, 0x1a},
  {0x3905, 0x02},
  {0x3906, 0x10},
  {0x3901, 0x0a},
  {0x3731, 0x12},
  {0x3600, 0x08},
  {0x3601, 0x33},
  {0x302d, 0x60},
  {0x3620, 0x52},
  {0x371b, 0x20},
  {0x471c, 0x50},
  {OV5640_AEC_CTRL13, 0x43},
  {OV5640_AEC_GAIN_CEILING_HIGH, 0x00},
  {OV5640_AEC_GAIN_CEILING_LOW, 0xf8},
  {0x3635, 0x13},
  {0x3636, 0x03},
  {0x3634, 0x40},
  {0x3622, 0x01},
  {OV5640_5060HZ_CTRL01, 0x34},
  {OV5640_5060HZ_CTRL04, 0x28},
  {OV5640_5060HZ_CTRL05, 0x98},
  {OV5640_LIGHTMETER1_TH_HIGH, 0x00},
  {OV5640_LIGHTMETER1_TH_LOW, 0x00},
  {OV5640_LIGHTMETER2_TH_HIGH, 0x01},
  {OV5640_LIGHTMETER2_TH_LOW, 0x2c},
  {OV5640_SAMPLE_NUMBER_HIGH, 0x9c},
  {OV5640_SAMPLE_NUMBER_LOW, 0x40},
  {OV5640_TIMING_TC_REG20, 0x06},
  {OV5640_TIMING_TC_REG21, 0x00},
  {OV5640_TIMING_X_INC, 0x31},
  {OV5640_TIMING_Y_INC, 0x31},
  {OV5640_TIMING_HS_HIGH, 0x00},
  {OV5640_TIMING_HS_LOW, 0x00},
  {OV5640_TIMING_VS_HIGH, 0x00},
  {OV5640_TIMING_VS_LOW, 0x04},
  {OV5640_TIMING_HW_HIGH, 0x0a},
  {OV5640_TIMING_HW_LOW, 0x3f},
  {OV5640_TIMING_VH_HIGH, 0x07},
  {OV5640_TIMING_VH_LOW, 0x9b},
  {OV5640_TIMING_DVPHO_HIGH, 0x03},
  {OV5640_TIMING_DVPHO_LOW, 0x20},
  {OV5640_TIMING_DVPVO_HIGH, 0x02},
  {OV5640_TIMING_DVPVO_LOW, 0x58},
  /* For 800x480 resolution: OV5640_TIMING_HTS=0x790, OV5640_TIMING_VTS=0x440 */
  {OV5640_TIMING_HTS_HIGH, 0x07},
  {OV5640_TIMING_HTS_LOW, 0x90},
  {OV5640_TIMING_VTS_HIGH, 0x04},
  {OV5640_TIMING_VTS_LOW, 0x40},
  {OV5640_TIMING_HOFFSET_HIGH, 0x00},
  {OV5640_TIMING_HOFFSET_LOW, 0x10},
  {OV5640_TIMING_VOFFSET_HIGH, 0x00},
  {OV5640_TIMING_VOFFSET_LOW, 0x06},
  {0x3618, 0x00},
  {0x3612, 0x29},
  {0x3708, 0x64},
  {0x3709, 0x52},
  {0x370c, 0x03},
  {OV5640_AEC_CTRL02, 0x03},
  {OV5640_AEC_CTRL03, 0xd8},
  {OV5640_AEC_B50_STEP_HIGH, 0x01},
  {OV5640_AEC_B50_STEP_LOW, 0x27},
  {OV5640_AEC_B60_STEP_HIGH, 0x00},
  {OV5640_AEC_B60_STEP_LOW, 0xf6},
  {OV5640_AEC_CTRL0E, 0x03},
  {OV5640_AEC_CTRL0D, 0x04},
  {OV5640_AEC_MAX_EXPO_HIGH, 0x03},
  {OV5640_AEC_MAX_EXPO_LOW, 0xd8},
  {OV5640_BLC_CTRL01, 0x02},
  {OV5640_BLC_CTRL04, 0x02},
  {OV5640_SYSREM_RESET00, 0x00},
  {OV5640_SYSREM_RESET02, 0x1c},
  {OV5640_CLOCK_ENABLE00, 0xff},
  {OV5640_CLOCK_ENABLE02, 0xc3},
  {OV5640_MIPI_CONTROL00, 0x58},
  {0x302e, 0x00},
  {OV5640_POLARITY_CTRL, 0x22},
  {OV5640_FORMAT_CTRL00, 0x6F},
  {OV5640_FORMAT_MUX_CTRL, 0x01},
  {OV5640_JPG_MODE_SELECT, 0x03},
  {OV5640_JPEG_CTRL07, 0x04},
  {0x440e, 0x00},
  {0x460b, 0x35},
  {0x460c, 0x23},
  {OV5640_PCLK_PERIOD, 0x22},
  {0x3824, 0x02},
  {OV5640_ISP_CONTROL00, 0xa7},
  {OV5640_ISP_CONTROL01, 0xa3},
  {OV5640_AWB_CTRL00, 0xff},
  {OV5640_AWB_CTRL01, 0xf2},
  {OV5640_AWB_CTRL02, 0x00},
  {OV5640_AWB_CTRL03, 0x14},
  {OV5640_AWB_CTRL04, 0x25},
  {OV5640_AWB_CTRL05, 0x24},
  {OV5640_AWB_CTRL06, 0x09},
  {OV5640_AWB_CTRL07, 0x09},
  {OV5640_AWB_CTRL08, 0x09},
  {OV5640_AWB_CTRL09, 0x75},
  {OV5640_AWB_CTRL10, 0x54},
  {OV5640_AWB_CTRL11, 0xe0},
  {OV5640_AWB_CTRL12, 0xb2},
  {OV5640_AWB_CTRL13, 0x42},
  {OV5640_AWB_CTRL14, 0x3d},
  {OV5640_AWB_CTRL15, 0x56},
  {OV5640_AWB_CTRL16, 0x46},
  {OV5640_AWB_CTRL17, 0xf8},
  {OV5640_AWB_CTRL18, 0x04},
  {OV5640_AWB_CTRL19, 0x70},
  {OV5640_AWB_CTRL20, 0xf0},
  {OV5640_AWB_CTRL21, 0xf0},
  {OV5640_AWB_CTRL22, 0x03},
  {OV5640_AWB_CTRL23, 0x01},
  {OV5640_AWB_CTRL24, 0x04},
  {OV5640_AWB_CTRL25, 0x12},
  {OV5640_AWB_CTRL26, 0x04},
  {OV5640_AWB_CTRL27, 0x00},
  {OV5640_AWB_CTRL28, 0x06},
  {OV5640_AWB_CTRL29, 0x82},
  {OV5640_AWB_CTRL30, 0x38},
  {OV5640_CMX1, 0x1e},
  {OV5640_CMX2, 0x5b},
  {OV5640_CMX3, 0x08},
  {OV5640_CMX4, 0x0a},
  {OV5640_CMX5, 0x7e},
  {OV5640_CMX6, 0x88},
  {OV5640_CMX7, 0x7c},
  {OV5640_CMX8, 0x6c},
  {OV5640_CMX9, 0x10},
  {OV5640_CMXSIGN_HIGH, 0x01},
  {OV5640_CMXSIGN_LOW, 0x98},
  {OV5640_CIP_SHARPENMT_TH1, 0x08},
  {OV5640_CIP_SHARPENMT_TH2, 0x30},
  {OV5640_CIP_SHARPENMT_OFFSET1, 0x10},
  {OV5640_CIP_SHARPENMT_OFFSET2, 0x00},
  {OV5640_CIP_DNS_TH1, 0x08},
  {OV5640_CIP_DNS_TH2, 0x30},
  {OV5640_CIP_DNS_OFFSET1, 0x08},
  {OV5640_CIP_DNS_OFFSET2, 0x16},
  {OV5640_CIP_CTRL, 0x08},
  {OV5640_CIP_SHARPENTH_TH1, 0x30},
  {OV5640_CIP_SHARPENTH_TH2, 0x04},
  {OV5640_CIP_SHARPENTH_OFFSET1, 0x06},
  {OV5640_GAMMA_CTRL00, 0x01},
  {OV5640_GAMMA_YST00, 0x08},
  {OV5640_GAMMA_YST01, 0x14},
  {OV5640_GAMMA_YST02, 0x28},
  {OV5640_GAMMA_YST03, 0x51},
  {OV5640_GAMMA_YST04, 0x65},
  {OV5640_GAMMA_YST05, 0x71},
  {OV5640_GAMMA_YST06, 0x7d},
  {OV5640_GAMMA_YST07, 0x87},
  {OV5640_GAMMA_YST08, 0x91},
  {OV5640_GAMMA_YST09, 0x9a},
  {OV5640_GAMMA_YST0A, 0xaa},
  {OV5640_GAMMA_YST0B, 0xb8},
  {OV5640_GAMMA_YST0C, 0xcd},
  {OV5640_GAMMA_YST0D, 0xdd},
  {OV5640_GAMMA_YST0E, 0xea},
  {OV5640_GAMMA_YST0F, 0x1d},
  {OV5640_SDE_CTRL0, 0x02},
  {OV5640_SDE_CTRL3, 0x40},
  {OV5640_SDE_CTRL4, 0x10},
  {OV5640_SDE_CTRL9, 0x10},
  {OV5640_SDE_CTRL10, 0x00},
  {OV5640_SDE_CTRL11, 0xf8},
  {OV5640_GMTRX00, 0x23},
  {OV5640_GMTRX01, 0x14},
  {OV5640_GMTRX02, 0x0f},
  {OV5640_GMTRX03, 0x0f},
  {OV5640_GMTRX04, 0x12},
  {OV5640_GMTRX05, 0x26},
  {OV5640_GMTRX10, 0x0c},
  {OV5640_GMTRX11, 0x08},
  {OV5640_GMTRX12, 0x05},
  {OV5640_GMTRX13, 0x05},
  {OV5640_GMTRX14, 0x08},
  {OV5640_GMTRX15, 0x0d},
  {OV5640_GMTRX20, 0x08},
  {OV5640_GMTRX21, 0x03},
  {OV5640_GMTRX22, 0x00},
  {OV5640_GMTRX23, 0x00},
  {OV5640_GMTRX24, 0x03},
  {OV5640_GMTRX25, 0x09},
  {OV5640_GMTRX30, 0x07},
  {OV5640_GMTRX31, 0x03},
  {OV5640_GMTRX32, 0x00},
  {OV5640_GMTRX33, 0x01},
  {OV5640_GMTRX34, 0x03},
  {OV5640_GMTRX35, 0x08},
  {OV5640_GMTRX40, 0x0d},
  {OV5640_GMTRX41, 0x08},
  {OV5640_GMTRX42, 0x05},
  {OV5640_GMTRX43, 0x06},
  {OV5640_GMTRX44, 0x08},
  {OV5640_GMTRX45, 0x0e},
  {OV5640_GMTRX50, 0x29},
  {OV5640_GMTRX51, 0x17},
  {OV5640_GMTRX52, 0x11},
  {OV5640_GMTRX53, 0x11},
  {OV5640_GMTRX54, 0x15},
  {OV5640_GMTRX55, 0x28},
  {OV5640_BRMATRX00, 0x46},
  {OV5640_BRMATRX01, 0x26},
  {OV5640_BRMATRX02, 0x08},
  {OV5640_BRMATRX03, 0x26},
  {OV5640_BRMATRX04, 0x64},
  {OV5640_BRMATRX05, 0x26},
  {OV5640_BRMATRX06, 0x24},
  {OV5640_BRMATRX07, 0x22},
  {OV5640_BRMATRX08, 0x24},
  {OV5640_BRMATRX09, 0x24},
  {OV5640_BRMATRX20, 0x06},
  {OV5640_BRMATRX21, 0x22},
  {OV5640_BRMATRX22, 0x40},
  {OV5640_BRMATRX23, 0x42},
  {OV5640_BRMATRX24, 0x24},
  {OV5640_BRMATRX30, 0x26},
  {OV5640_BRMATRX31, 0x24},
  {OV5640_BRMATRX32, 0x22},
  {OV5640_BRMATRX33, 0x22},
  {OV5640_BRMATRX34, 0x26},
  {OV5640_BRMATRX40, 0x44},
  {OV5640_BRMATRX41, 0x24},
  {OV5640_BRMATRX42, 0x26},
  {OV5640_BRMATRX43, 0x28},
  {OV5640_BRMATRX44, 0x42},
  {OV5640_LENC_BR_OFFSET, 0xce},
  {0x5025, 0x00},
  {OV5640_AEC_CTRL0F, 0x30},
  {OV5640_AEC_CTRL10, 0x28},
  {OV5640_AEC_CTRL1B, 0x30},
  {OV5640_AEC_CTRL1E, 0x26},
  {OV5640_AEC_CTRL11, 0x60},
  {OV5640_AEC_CTRL1F, 0x14},
  {OV5640_SYSTEM_CTROL0, 0x02},
};
static uint8_t ov5640_WR_Reg(uint16_t reg, uint8_t data)
{
    Camera_WriteRegb2(&hcamera, reg, data);
    return 0;
}

static uint8_t ov5640_RD_Reg(uint16_t reg, uint8_t *data)
{

    return Camera_ReadRegb2(&hcamera, reg, data);
}

int ov5640_reset(void)
{
    // Reset all registers
    int ret = ov5640_WR_Reg(SCCB_SYSTEM_CTRL_1, 0x11);
    ret |= ov5640_WR_Reg(SYSTEM_CTROL0, 0x82);

    // Delay 5 ms
    Camera_delay(5);

    // Write default regsiters
    for (int i = 0; ov5640_default_regs[i][0]; i++)
    {
        ret |= ov5640_WR_Reg((ov5640_default_regs[i][0] << 8) | (ov5640_default_regs[i][1] << 0), ov5640_default_regs[i][2]);
    }

    // Delay 300 ms
    Camera_delay(300);
    //ov5640_WR_Reg(0x3036, 0x60);
    //ov5640_WR_Reg(0x3037, 0x13);
    //ov5640_WR_Reg(0x4740, 0x21);
    if (g_using_line_event){
        ov5640_WR_Reg(0x3035, 0x11); // /4 system divider
        ov5640_WR_Reg(0x3036, 0x64); // PLL multiplier ~ 100
        ov5640_WR_Reg(0x3108, 0x01); // /2 root divider
    }else{
        ov5640_WR_Reg(0x3035, 0x11); // /4 system divider
        ov5640_WR_Reg(0x3036, 0x46); // PLL multiplier ~ 70
        ov5640_WR_Reg(0x3108, 0x01); // /2 root divider
    }

#if 0
    // Write preview table
    ov5640_WR_Reg(0x3035, 0x21); // /4 system divider
    ov5640_WR_Reg(0x3036, 0x46); // PLL multiplier ~ 70
    ov5640_WR_Reg(0x3108, 0x01); // /2 root divider

    for (int i=0; i<sizeof(OV5640_Common)/sizeof(int); i+=2)
	{
		ov5640_WR_Reg(OV5640_Common[i], OV5640_Common[i+1]);
	}
#endif
    return ret;
}

// HTS (Horizontal Time) is the readout width plus the HSYNC_TIME time. However, if this value gets
// too low the OV5640 will crash. The minimum was determined empirically with testing...
// Additionally, when the image width gets too large we need to slow down the line transfer rate by
// increasing HTS so that DCMI_DMAConvCpltUser() can keep up with the data rate.
//
// WARNING! IF YOU CHANGE ANYTHING HERE RETEST WITH **ALL** RESOLUTIONS FOR THE AFFECTED MODE!
static int calculate_hts(uint16_t width)
{
    uint16_t hts = hts_target;

    if ((hcamera.pixformat == PIXFORMAT_GRAYSCALE) || (hcamera.pixformat == PIXFORMAT_BAYER) || (hcamera.pixformat == PIXFORMAT_JPEG))
    {
        if (width <= 1280)
            hts = IM_MAX((width * 2) + 8, hts_target);
    }
    else
    {
        if (width > 640)
            hts = IM_MAX((width * 2) + 8, hts_target);
    }

    return IM_MAX(hts + HSYNC_TIME, (SENSOR_WIDTH + HSYNC_TIME) / 2); // Fix to prevent crashing.
}

// VTS (Vertical Time) is the readout height plus the VYSNC_TIME time. However, if this value gets
// too low the OV5640 will crash. The minimum was determined empirically with testing...
//
// WARNING! IF YOU CHANGE ANYTHING HERE RETEST WITH **ALL** RESOLUTIONS FOR THE AFFECTED MODE!
static int calculate_vts(uint16_t readout_height)
{
    return IM_MAX(readout_height + VYSNC_TIME, (SENSOR_HEIGHT + VYSNC_TIME) / 8); // Fix to prevent crashing.
}

static int ov5640_set_pixformat(pixformat_t pixformat)
{
    uint8_t reg;
    int ret = 0;
    // Not a multiple of 8. The JPEG encoder on the OV5640 can't handle this.
    if ((pixformat == PIXFORMAT_JPEG) && ((dvp_cam_resolution[hcamera.framesize][0] % 8) || (dvp_cam_resolution[hcamera.framesize][1] % 8)))
    {
        return -1;
    }

    // Readout speed too fast. The DCMI_DMAConvCpltUser() line callback overhead is too much to handle the line transfer speed.
    // If we were to slow the pixclk down these resolutions would work. As of right now, the image shakes and scrolls with
    // the current line transfer speed. Note that there's an overhead to the DCMI_DMAConvCpltUser() function. It's not the
    // memory copy operation that's too slow. It's that there's too much overhead in the DCMI_DMAConvCpltUser() method
    // to even have time to start the line transfer. If it were possible to slow the line readout speed of the OV5640
    // this would enable these resolutions below. However, there's nothing in the datasheet that when modified does this.
    if (((pixformat == PIXFORMAT_GRAYSCALE) || (pixformat == PIXFORMAT_BAYER) || (pixformat == PIXFORMAT_JPEG)) && ((hcamera.framesize == FRAMESIZE_QQCIF) || (hcamera.framesize == FRAMESIZE_QQSIF) || (hcamera.framesize == FRAMESIZE_HQQQVGA) || (hcamera.framesize == FRAMESIZE_HQQVGA)))
    {
        return -1;
    }

    switch (pixformat)
    {
    case PIXFORMAT_GRAYSCALE:
        ret |= ov5640_WR_Reg(FORMAT_CONTROL, 0x10);
        ret |= ov5640_WR_Reg(FORMAT_CONTROL_MUX, 0x00);
        break;
    case PIXFORMAT_RGB565:
        ret |= ov5640_WR_Reg(FORMAT_CONTROL, 0x61);
        ret |= ov5640_WR_Reg(FORMAT_CONTROL_MUX, 0x01);
        break;
    case PIXFORMAT_YUV422:
        ret |= ov5640_WR_Reg(FORMAT_CONTROL, 0x30);
        ret |= ov5640_WR_Reg(FORMAT_CONTROL_MUX, 0x00);
        break;
    case PIXFORMAT_YUV420:
        ret |= ov5640_WR_Reg(FORMAT_CONTROL, 0x40);
        ret |= ov5640_WR_Reg(FORMAT_CONTROL_MUX, 0x00);
        break;
    case PIXFORMAT_BAYER:
        ret |= ov5640_WR_Reg(FORMAT_CONTROL, 0x00);
        ret |= ov5640_WR_Reg(FORMAT_CONTROL_MUX, 0x01);
        break;
    case PIXFORMAT_JPEG:
        ret |= ov5640_WR_Reg(FORMAT_CONTROL, 0x30);
        ret |= ov5640_WR_Reg(FORMAT_CONTROL_MUX, 0x00);
        break;
    default:
        return -1;
    }

    ret |= ov5640_RD_Reg(TIMING_TC_REG_21, &reg);
    ret |= ov5640_WR_Reg(TIMING_TC_REG_21, (reg & 0xDF) | ((pixformat == PIXFORMAT_JPEG) ? 0x20 : 0x00));

    ret |= ov5640_RD_Reg(SYSTEM_RESET_02, &reg);
    ret |= ov5640_WR_Reg(SYSTEM_RESET_02, (reg & 0xE3) | ((pixformat == PIXFORMAT_JPEG) ? 0x00 : 0x1C));

    ret |= ov5640_RD_Reg(CLOCK_ENABLE_02, &reg);
    ret |= ov5640_WR_Reg(CLOCK_ENABLE_02, (reg & 0xD7) | ((pixformat == PIXFORMAT_JPEG) ? 0x28 : 0x00));

    /*ret |= ov5640_WR_Reg(OV5640_JPEG_CTRL00, 0x81);
    ret |= ov5640_WR_Reg(OV5640_JPEG_CTRL07, 0x0C);
    ret |= ov5640_WR_Reg(OV5640_JPG_MODE_SELECT, 0x01);*/
    //ov5640_WR_Reg(0x4741, 0x04);

    //ret |= ov5640_WR_Reg(VFIFO_CTRL00, 0x40);
    if (hts_target)
    {
        uint16_t sensor_hts = calculate_hts(dvp_cam_resolution[hcamera.framesize][0]);

        ret |= ov5640_WR_Reg(TIMING_HTS_H, sensor_hts >> 8);
        ret |= ov5640_WR_Reg(TIMING_HTS_L, sensor_hts);
    }

    return ret;
}

static int ov5640_set_framesize(framesize_t framesize)
{
    uint8_t reg;
    int ret = 0;
    uint16_t w = dvp_cam_resolution[framesize][0];
    uint16_t h = dvp_cam_resolution[framesize][1];

    // Not a multiple of 8. The JPEG encoder on the OV5640 can't handle this.
    if ((hcamera.pixformat == PIXFORMAT_JPEG) && ((w % 8) || (h % 8)))
    {
        return -1;
    }

    // Readout speed too fast. The DCMI_DMAConvCpltUser() line callback overhead is too much to handle the line transfer speed.
    // If we were to slow the pixclk down these resolutions would work. As of right now, the image shakes and scrolls with
    // the current line transfer speed. Note that there's an overhead to the DCMI_DMAConvCpltUser() function. It's not the
    // memory copy operation that's too slow. It's that there's too much overhead in the DCMI_DMAConvCpltUser() method
    // to even have time to start the line transfer. If it were possible to slow the line readout speed of the OV5640
    // this would enable these resolutions below. However, there's nothing in the datasheet that when modified does this.
    if (((hcamera.pixformat == PIXFORMAT_GRAYSCALE) || (hcamera.pixformat == PIXFORMAT_BAYER) || (hcamera.pixformat == PIXFORMAT_JPEG)) && ((framesize == FRAMESIZE_QQCIF) || (framesize == FRAMESIZE_QQSIF) || (framesize == FRAMESIZE_HQQQVGA) || (framesize == FRAMESIZE_HQQVGA)))
    {
        return -1;
    }

    // Generally doesn't work for anything.
    if (framesize == FRAMESIZE_QQQQVGA)
    {
        return -1;
    }

    // Invalid resolution.
    if ((w > ACTIVE_SENSOR_WIDTH) || (h > ACTIVE_SENSOR_HEIGHT))
    {
        return -1;
    }

    // Step 0: Clamp readout settings.

    readout_w = IM_MAX(readout_w, w);
    readout_h = IM_MAX(readout_h, h);

    int readout_x_max = (ACTIVE_SENSOR_WIDTH - readout_w) / 2;
    int readout_y_max = (ACTIVE_SENSOR_HEIGHT - readout_h) / 2;
    readout_x = IM_MAX(IM_MIN(readout_x, readout_x_max), -readout_x_max);
    readout_y = IM_MAX(IM_MIN(readout_y, readout_y_max), -readout_y_max);

    // Step 1: Determine readout area and subsampling amount.

    uint16_t sensor_div = 0;

    if ((w > (readout_w / 2)) || (h > (readout_h / 2)))
    {
        sensor_div = 1;
    }
    else
    {
        sensor_div = 2;
    }

    // Step 2: Determine horizontal and vertical start and end points.

    uint16_t sensor_w = readout_w + DUMMY_WIDTH_BUFFER;  // camera hardware needs dummy pixels to sync
    uint16_t sensor_h = readout_h + DUMMY_HEIGHT_BUFFER; // camera hardware needs dummy lines to sync

    uint16_t sensor_ws = IM_MAX(IM_MIN((((ACTIVE_SENSOR_WIDTH - sensor_w) / 4) + (readout_x / 2)) * 2, ACTIVE_SENSOR_WIDTH - sensor_w), -(DUMMY_WIDTH_BUFFER / 2)) + DUMMY_COLUMNS; // must be multiple of 2
    uint16_t sensor_we = sensor_ws + sensor_w - 1;

    uint16_t sensor_hs = IM_MAX(IM_MIN((((ACTIVE_SENSOR_HEIGHT - sensor_h) / 4) - (readout_y / 2)) * 2, ACTIVE_SENSOR_HEIGHT - sensor_h), -(DUMMY_HEIGHT_BUFFER / 2)) + DUMMY_LINES; // must be multiple of 2
    uint16_t sensor_he = sensor_hs + sensor_h - 1;

    // Step 3: Determine scaling window offset.

    float ratio = IM_MIN((readout_w / sensor_div) / ((float)w), (readout_h / sensor_div) / ((float)h));

    uint16_t w_mul = w * ratio;
    uint16_t h_mul = h * ratio;
    uint16_t x_off = ((sensor_w / sensor_div) - w_mul) / 2;
    uint16_t y_off = ((sensor_h / sensor_div) - h_mul) / 2;

    // Step 4: Compute total frame time.

    hts_target = sensor_w / sensor_div;

    uint16_t sensor_hts = calculate_hts(w);
    uint16_t sensor_vts = calculate_vts(sensor_h / sensor_div);

    uint16_t sensor_x_inc = (((sensor_div * 2) - 1) << 4) | (1 << 0); // odd[7:4]/even[3:0] pixel inc on the bayer pattern
    uint16_t sensor_y_inc = (((sensor_div * 2) - 1) << 4) | (1 << 0); // odd[7:4]/even[3:0] pixel inc on the bayer pattern

    // Step 5: Write regs.

    ret |= ov5640_WR_Reg(TIMING_HS_H, sensor_ws >> 8);
    ret |= ov5640_WR_Reg(TIMING_HS_L, sensor_ws);

    ret |= ov5640_WR_Reg(TIMING_VS_H, sensor_hs >> 8);
    ret |= ov5640_WR_Reg(TIMING_VS_L, sensor_hs);

    ret |= ov5640_WR_Reg(TIMING_HW_H, sensor_we >> 8);
    ret |= ov5640_WR_Reg(TIMING_HW_L, sensor_we);

    ret |= ov5640_WR_Reg(TIMING_VH_H, sensor_he >> 8);
    ret |= ov5640_WR_Reg(TIMING_VH_L, sensor_he);

    ret |= ov5640_WR_Reg(TIMING_DVPHO_H, w >> 8);
    ret |= ov5640_WR_Reg(TIMING_DVPHO_L, w);

    ret |= ov5640_WR_Reg(TIMING_DVPVO_H, h >> 8);
    ret |= ov5640_WR_Reg(TIMING_DVPVO_L, h);

    ret |= ov5640_WR_Reg(TIMING_HTS_H, 0x06);
    ret |= ov5640_WR_Reg(TIMING_HTS_L, 0x04);
    ret |= ov5640_WR_Reg(TIMING_VTS_H, 0x03);
    ret |= ov5640_WR_Reg(TIMING_VTS_L, 0xe0);

    ret |= ov5640_WR_Reg(TIMING_HOFFSET_H, x_off >> 8);
    ret |= ov5640_WR_Reg(TIMING_HOFFSET_L, x_off);

    ret |= ov5640_WR_Reg(TIMING_VOFFSET_H, y_off >> 8);
    ret |= ov5640_WR_Reg(TIMING_VOFFSET_L, y_off);

    ret |= ov5640_WR_Reg(TIMING_X_INC, sensor_x_inc);
    ret |= ov5640_WR_Reg(TIMING_Y_INC, sensor_y_inc);

    ret |= ov5640_RD_Reg(TIMING_TC_REG_20, &reg);
    ret |= ov5640_WR_Reg(TIMING_TC_REG_20, (reg & 0xFE) | (sensor_div > 1));

    ret |= ov5640_RD_Reg(TIMING_TC_REG_21, &reg);
    ret |= ov5640_WR_Reg(TIMING_TC_REG_21, (reg & 0xFE) | (sensor_div > 1));

    ret |= ov5640_WR_Reg(VFIFO_HSIZE_H, w >> 8);
    ret |= ov5640_WR_Reg(VFIFO_HSIZE_L, w);

    ret |= ov5640_WR_Reg(VFIFO_VSIZE_H, h >> 8);
    ret |= ov5640_WR_Reg(VFIFO_VSIZE_L, h);

    return ret;
}

static int ov5640_set_hmirror(int enable)
{
    uint8_t reg;
    int ret = ov5640_RD_Reg(TIMING_TC_REG_21, &reg);
    if (enable)
    {
        ret |= ov5640_WR_Reg(TIMING_TC_REG_21, reg | 0x06);
    }
    else
    {
        ret |= ov5640_WR_Reg(TIMING_TC_REG_21, reg & 0xF9);
    }
    return ret;
}

static int ov5640_set_vflip(int enable)
{
    uint8_t reg;
    int ret = ov5640_RD_Reg(TIMING_TC_REG_20, &reg);
    if (!enable)
    {
        ret |= ov5640_WR_Reg(TIMING_TC_REG_20, reg | 0x06);
    }
    else
    {
        ret |= ov5640_WR_Reg(TIMING_TC_REG_20, reg & 0xF9);
    }
    return ret;
}

int ov5640_init(framesize_t framesize, pixformat_t format)
{
    ov5640_reset();
    hcamera.framesize = framesize;
    hcamera.pixformat = format;

    ov5640_set_pixformat(hcamera.pixformat);
    ov5640_set_framesize(hcamera.framesize);
    ov5640_set_hmirror(0);
    ov5640_set_vflip(0);

    return 1;
}

