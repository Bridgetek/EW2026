/**
 * @file eve.h
 * @brief Header file for EVE graphics library
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */
#include <stddef.h>
#include <wchar.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef EVE_H
#define EVE_H

#include "eve_bt820.h"

#define eve_debug_break() __builtin_trap()
#define eve_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)

#if defined(DEBUG) // This flag is set from compiler flags
#define eve_printf_debug(fmt, ...) eve_printf(fmt, ##__VA_ARGS__)
#else
#define eve_printf_debug(fmt, ...) (void)0
#endif

#define eve_assert(cond)                                                                                                           \
	do                                                                                                                             \
	{                                                                                                                              \
		if (!(cond))                                                                                                               \
		{                                                                                                                          \
			const char *str = #cond;                                                                                               \
			const char *sf = __FILE__;                                                                                             \
			eve_printf("EVE Assert Failed: %s (in file '%s' on line '%i')\n", str ? str : "<NULL>", sf ? sf : "<NULL>", __LINE__); \
			eve_debug_break();                                                                                                     \
		}                                                                                                                          \
	} while (false)

#define eve_assert_ex(cond, ex)                                                                                                                             \
	do                                                                                                                                                      \
	{                                                                                                                                                       \
		if (!(cond))                                                                                                                                        \
		{                                                                                                                                                   \
			const char *str = #cond;                                                                                                                        \
			const char *sf = __FILE__;                                                                                                                      \
			eve_printf("EVE Assert Failed: %s (%s) (in file '%s' on line '%i')\n", ex ? ex : "<NULL>", str ? str : "<NULL>", sf ? sf : "<NULL>", __LINE__); \
			eve_debug_break();                                                                                                                              \
		}                                                                                                                                                   \
	} while (false)
#define eve_assert_do(cond) eve_assert(cond)

#ifndef NOMINMAX
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif
#endif

#define EVE_FT800 0x10800
#define EVE_FT801 0x10801
#define EVE_FT810 0x20810
#define EVE_FT811 0x20811
#define EVE_FT812 0x20812
#define EVE_FT813 0x20813
#define EVE_BT880 0x20880
#define EVE_BT881 0x20881
#define EVE_BT882 0x20882
#define EVE_BT883 0x20883
#define EVE_BT815 0x30815
#define EVE_BT816 0x30816
#define EVE_BT817 0x40817
#define EVE_BT818 0x40818
#define EVE_BT820 0x50820
#define EVE1 1
#define EVE2 2
#define EVE3 3
#define EVE4 4
#define EVE5 5
#define EVE_DL_STATE_STACK_SIZE 16
#define EVE_DL_STATE_STACK_MASK 15
#define DO_PRAGMA_(x) _Pragma(#x)
#define eve_pragma_warning(msg) DO_PRAGMA_("GCC warning \"" msg "\"")
#define eve_pragma_error(msg) _Pragma("GCC error \"" msg "\"")

#define EVE_GPU_NUMCHAR_PERFONT (128)
#define EVE_GPU_FONT_TABLE_SIZE (148)
#define EVE_CORE_RESET (0x68)
#define RAM_ERR_REPORT_MAX 128
#define EVE_DL_STATE phost->DlState[phost->DlStateIndex]

#define EVE_CMD_STRING_MAX 511
#define SQ(v) ((v) * (v))
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#define ALIGN_TWO_POWER_N(Value, alignval) (((Value) + (alignval - 1)) & (~(alignval - 1)))

/* ========================================================================== */
/*                              TYPE DEFINITIONS                              */
/* ========================================================================== */

/* Enumerations */
typedef enum EVE_SPI_CHANNELS_T
{
	EVE_SPI_SINGLE_CHANNEL = 0x00,
	EVE_SPI_DUAL_CHANNEL = 0x01,
	EVE_SPI_QUAD_CHANNEL = 0x02,
} EVE_SPI_CHANNELS_T;
typedef enum EVE_SPI_DUMMY_BYTES_T
{
	EVE_SPI_ONE_DUMMY_BYTE = 0x00,
	EVE_SPI_TWO_DUMMY_BYTES = 0x04,
} EVE_SPI_DUMMY_BYTES_T;
typedef enum EVE_PLL_SOURCE_T
{
	EVE_INTERNAL_OSC = 0x48,
	EVE_EXTERNAL_OSC = 0x44,
} EVE_PLL_SOURCE_T;
typedef enum EVE_PLL_FREQ_T
{
	EVE_PLL_48M = 0x62,
	EVE_PLL_36M = 0x61,
	EVE_PLL_24M = 0x64,
} EVE_PLL_FREQ_T;
typedef enum EVE_POWER_MODE_T
{
	EVE_ACTIVE_M = 0x00,
	EVE_STANDBY_M = 0x41,
	EVE_SLEEP_M = 0x42,
	EVE_POWERDOWN_M = 0x50,
} EVE_POWER_MODE_T;
typedef enum EVE_81X_PLL_FREQ_T
{
	EVE_SYSCLK_DEFAULT = 0x00,
	EVE_SYSCLK_84M = 0x07,
	EVE_SYSCLK_72M = 0x06,
	EVE_SYSCLK_60M = 0x05,
	EVE_SYSCLK_48M = 0x04,
	EVE_SYSCLK_36M = 0x03,
	EVE_SYSCLK_24M = 0x02,
} EVE_81X_PLL_FREQ_T;
typedef enum EVE_81X_ROM_AND_ADC_T
{
	EVE_MAIN_ROM = 0x80,
	EVE_RCOSATAN_ROM = 0x40,
	EVE_SAMPLE_ROM = 0x20,
	EVE_JABOOT_ROM = 0x10,
	EVE_J1BOOT_ROM = 0x08,
	EVE_ADC = 0x01,
	EVE_POWER_ON_ROM_AND_ADC = 0x00,
} EVE_81X_ROM_AND_ADC_T;
typedef enum EVE_81X_GPIO_DRIVE_STRENGTH_T
{
	EVE_5MA = 0x00,
	EVE_10MA = 0x01,
	EVE_15MA = 0x02,
	EVE_20MA = 0x03,
} EVE_81X_GPIO_DRIVE_STRENGTH_T;
typedef enum EVE_81X_GPIO_GROUP_T
{
	EVE_GPIO0 = 0x00,
	EVE_GPIO1 = 0x04,
	EVE_GPIO2 = 0x08,
	EVE_GPIO3 = 0x0C,
	EVE_GPIO4 = 0x10,
	EVE_DISP = 0x20,
	EVE_DE = 0x24,
	EVE_VSYNC_HSYNC = 0x28,
	EVE_PCLK = 0x2C,
	EVE_BACKLIGHT = 0x30,
	EVE_R_G_B = 0x34,
	EVE_AUDIO_L = 0x38,
	EVE_INT_N = 0x3C,
	EVE_TOUCHWAKE = 0x40,
	EVE_SCL = 0x44,
	EVE_SDA = 0x48,
	EVE_SPI_MISO_MOSI_IO2_IO3 = 0x4C,
} EVE_81X_GPIO_GROUP_T;

typedef enum EVE_82X_SPI_PREFIX_T
{
	EVE_82X_SPI_ACTIVE = 0x00,
	EVE_82X_SPI_COMMAND = 0xFF,
} EVE_82X_SPI_PREFIX_T;
typedef enum EVE_82X_SPI_COMMANDS_T
{
	EVE_82X_ACTIVE = 0x00,
	EVE_82X_STANDBY = 0xE0,
	EVE_82X_SLEEP = 0xE1,
	EVE_82X_POWERDOWN = 0xE2,
	EVE_82X_TESTCFG = 0xE3,
	EVE_82X_SETPLLSP1 = 0xE4,
	EVE_82X_SETPLLLOCK1 = 0xE5,
	EVE_82X_SETCLKDIV = 0xE6,
	EVE_82X_RESET = 0xE7,
	EVE_82X_SETBOOTCFG = 0xE8,
	EVE_82X_BOOTCFGEN = 0xE9,
	EVE_82X_PSW_ON_DELAY = 0xEA,
	EVE_82X_SETDDRTYPE = 0xEB,
	EVE_82X_SETGPREG = 0xEC,
} EVE_82X_SPI_COMMANDS_T;
typedef enum EVE_82X_RESET_OPTIONS_T
{
	EVE_82X_RESET_PULSE = 0x00,
	EVE_82X_RESET_ACTIVE = 0x02,
	EVE_82X_RESET_REMOVAL = 0x20,
} EVE_82X_RESET_OPTIONS_T;

typedef enum EVE_MODE_T
{
	EVE_MODE_UNKNOWN = 0,
	EVE_MODE_I2C,
	EVE_MODE_SPI,
} EVE_MODE_T;

typedef enum EVE_STATUS_T
{
	EVE_STATUS_CLOSED = 0,
	EVE_STATUS_OPENED,
	EVE_STATUS_READING,
	EVE_STATUS_WRITING,
	EVE_STATUS_ERROR,
} EVE_STATUS_T;

typedef enum EVE_TRANSFER_T
{
	EVE_TRANSFER_NONE = 0,
	EVE_TRANSFER_READ,
	EVE_TRANSFER_WRITE,
} EVE_TRANSFER_T;

typedef enum EVE_BT820_T
{
	EVE_BT820_FT800 = EVE_FT800,
	EVE_BT820_FT801 = EVE_FT801,
	EVE_BT820_FT810 = EVE_FT810,
	EVE_BT820_FT811 = EVE_FT811,
	EVE_BT820_FT812 = EVE_FT812,
	EVE_BT820_FT813 = EVE_FT813,
	EVE_BT820_BT880 = EVE_BT880,
	EVE_BT820_BT881 = EVE_BT881,
	EVE_BT820_BT882 = EVE_BT882,
	EVE_BT820_BT883 = EVE_BT883,
	EVE_BT820_BT815 = EVE_BT815,
	EVE_BT820_BT816 = EVE_BT816,
	EVE_BT820_BT817 = EVE_BT817,
	EVE_BT820_BT818 = EVE_BT818,
	EVE_BT820_BT820 = EVE_BT820,
} EVE_BT820_T;

typedef enum EVE_HOST_T
{
	EVE_HOST_UNKNOWN = 0,
	EVE_HOST_EMBEDDED,
	EVE_HOST_NB
} EVE_HOST_T;

typedef enum EVE_DISPLAY_T
{
	EVE_DISPLAY_DEFAULT = 0,
	EVE_DISPLAY_QVGA_320x240_56Hz,
	EVE_DISPLAY_WQVGA_480x272_60Hz,
	EVE_DISPLAY_WVGA_800x480_74Hz,
	EVE_DISPLAY_WSVGA_1024x600_59Hz,
	EVE_DISPLAY_HDTV_1280x720_58Hz,
	EVE_DISPLAY_WXGA_1280x800_57Hz,
	EVE_DISPLAY_FHD_1920x1080_60Hz,
	EVE_DISPLAY_WUXGA_1920x1200_90Hz,
	EVE_DISPLAY_WUXGA_1920x1200_111Hz,
	EVE_DISPLAY_HVGA_320x480_60Hz,
	EVE_DISPLAY_WVGA_800x480_W860_60Hz,
	EVE_DISPLAY_WQVGA_480x480_48Hz,
	EVE_DISPLAY_RIVERDI_IPS35_62Hz,
	EVE_DISPLAY_RIVERDI_IPS43_58Hz,
	EVE_DISPLAY_RIVERDI_IPS50_63Hz,
	EVE_DISPLAY_RIVERDI_IPS70_59Hz,
	EVE_DISPLAY_NB,
}EVE_DISPLAY_T;

/* Forward declarations */
typedef struct EVE_HalContext EVE_HalContext;

/* Function pointer types */
typedef bool (*EVE_Callback)(EVE_HalContext *phost);
typedef int (*EVE_CoCmdHook)(EVE_HalContext *phost, uint32_t cmd, uint32_t state);
typedef void (*EVE_ResetCallback)(EVE_HalContext *phost, bool fault);

/* Structure definitions */
typedef struct EVE_Gpu_Fonts
{
	uint8_t FontWidth[EVE_GPU_NUMCHAR_PERFONT];
	uint32_t FontBitmapFormat;
	uint32_t FontLineStride;
	uint32_t FontWidthInPixels;
	uint32_t FontHeightInPixels;
	uint32_t PointerToFontGraphicsData;
} EVE_Gpu_Fonts;

typedef struct EVE_Gpu_FontsExt
{
	uint32_t Signature;
	uint32_t Size;
	uint32_t Format;
	uint32_t Swizzle;
	uint32_t LayoutWidth;
	uint32_t LayoutHeight;
	uint32_t PixelWidth;
	uint32_t PixelHeight;
	uint32_t StartOfGraphicData;
	uint32_t OffsetGlyphData;
} EVE_Gpu_FontsExt;
typedef struct EVE_CMD_SYS_PLL_FREQ
{
	uint8_t pllNs : 7;
	uint8_t pllCks : 1;
} EVE_CMD_SYS_PLL_FREQ;
typedef struct EVE_CMD_SYS_CLK_DIV
{
	uint8_t freq : 4;
	uint8_t sysPllCps : 3;
	uint8_t rev : 1;
} EVE_CMD_SYS_CLK_DIV;
typedef struct EVE_CMD_BOOT_CFG
{
	uint8_t source : 3;
	uint8_t rev : 1;
	uint8_t watchdog : 1;
	uint8_t audio : 1;
	uint8_t jt : 1;
	uint8_t ddr : 1;
} EVE_CMD_BOOT_CFG;
typedef struct EVE_CMD_BOOT_CFG_EN
{
	uint8_t enable : 1;
	uint8_t rev : 4;
	uint8_t gpReg : 1;
	uint8_t ddrType : 1;
	uint8_t boot : 1;
} EVE_CMD_BOOT_CFG_EN;
typedef struct EVE_CMD_DDR_TYPE
{
	uint8_t size : 3;
	uint8_t type : 2;
	uint8_t speed : 3;
} EVE_CMD_DDR_TYPE;
typedef struct EVE_CMD_GPREG
{
	uint8_t mode : 1;
	uint8_t rev : 7;
} EVE_CMD_GPREG;

typedef struct EVE_DeviceInfo
{
	char DisplayName[256];
	char SerialNumber[16];
	EVE_HOST_T Host;
	bool Opened;
} EVE_DeviceInfo;

typedef struct EVE_HalParameters
{
	void *UserContext;
	EVE_Callback CbCmdWait;
	uint32_t DeviceIdx;
	uint8_t SpiCsPin;
	uint8_t PowerDownPin;
} EVE_HalParameters;

typedef struct EVE_HalDlState
{
	uint32_t PaletteSource;
	uint32_t ColorRGB;
	int16_t LineWidth;
	int16_t PointSize;
	uint16_t ScissorX;
	uint16_t ScissorY;
	uint16_t ScissorWidth;
	uint16_t ScissorHeight;
	uint8_t ColorA;
	uint8_t Handle;
	uint8_t Cell;
	uint8_t VertexFormat;
	bool BitmapTransform;
} EVE_HalDlState;

typedef struct EVE_HalContext
{
	void *UserContext;
	void *LibraryContext;
	EVE_Callback CbCmdWait;
	EVE_ResetCallback CbCoprocessorReset;
	EVE_STATUS_T Status;
	int ReadTimeout;
	bool ReadTimeoutOccured;
	int TransferLength;
	uint8_t PCLK;
	uint32_t Width;
	uint32_t Height;
	EVE_SPI_CHANNELS_T SpiChannels;
	uint8_t SpiDummyBytes;
	uint8_t SpiCsPin;
	uint8_t PowerDownPin;
	uint32_t RamGSize;
	uint32_t RamGAvailable;
	uint8_t SpiWrBuf[0xFFFF];
	uint32_t SpiWrBufIndex;
	uint32_t SpiRamGAddr;
	uint8_t SpiRdBuf[65536 - 4];
	uint8_t CmdBuffer[4];
	uint8_t CmdBufferIndex;
	uint16_t CmdSpace;
	uint32_t MediaFifoAddress;
	uint32_t MediaFifoSize;

	ptrdiff_t LoadFileRemaining;
	EVE_HalDlState DlState[EVE_DL_STATE_STACK_SIZE];
	uint8_t DlStateIndex;
	uint8_t DlStateBase;
	uint8_t DlPrimitive;
	uint32_t CoFgColor;
	uint32_t CoBgColor;
	bool CoBitmapTransform;
	uint8_t CoScratchHandle;
	uint32_t CoRenderTargetDst;
	uint16_t CoRenderTargetFmt;
	int16_t CoRenderTargetW;
	int16_t CoRenderTargetH;
	bool DebugMessageVisible;
	uint8_t DebugBackup[RAM_ERR_REPORT_MAX];
	bool CmdFunc;
	bool CmdFault;
	bool CmdWaiting;
	uint32_t DDR_RamSize;
} EVE_HalContext;

typedef struct EVE_HalPlatform
{
	uint32_t OpenedDevices;
} EVE_HalPlatform;

typedef struct
{
	uint32_t TotalChannelNum;
} Gpu_HalInit_t;

typedef struct Fifo_t
{
	uint32_t fifo_buff;
	int32_t fifo_len;
	int32_t fifo_wp;
	uint32_t HW_Read_Reg;
} Fifo_t;

typedef struct
{
	uint32_t TotalChannelNum;
} Ft_Gpu_HalInit_t;

typedef struct EVE_BootupParameters
{
	EVE_81X_PLL_FREQ_T SystemClock;
	bool ExternalOsc;
	EVE_SPI_CHANNELS_T SpiChannels;
	uint8_t SpiDummyBytes;
} EVE_BootupParameters;

typedef struct EVE_ConfigParameters
{
	int16_t Width;
	int16_t Height;
	int16_t HCycle;
	int16_t HOffset;
	int16_t HSync0;
	int16_t HSync1;
	int16_t VCycle;
	int16_t VOffset;
	int16_t VSync0;
	int16_t VSync1;
	uint8_t PCLK;
	int8_t PCLKPol;
	uint8_t PixelsPerClock;
	uint32_t RefClkFreq;
	int16_t SwapchainFormat;
} EVE_ConfigParameters;

/* ========================================================================== */
/*                          EXTERNAL DECLARATIONS                             */
/* ========================================================================== */

extern EVE_HalPlatform g_HalPlatform;

/* ========================================================================== */
/*                          FUNCTION DECLARATIONS                             */
/* ========================================================================== */

/* HAL Implementation Functions */
void EVE_HalImpl_initialize();
void EVE_HalImpl_release();
bool EVE_HalImpl_defaults(EVE_HalParameters *parameters, size_t deviceIdx);
bool EVE_HalImpl_open(EVE_HalContext *phost, const EVE_HalParameters *parameters);
void EVE_HalImpl_close(EVE_HalContext *phost);
void EVE_HalImpl_idle(EVE_HalContext *phost);

/* HAL Transfer Functions */
void EVE_Hal_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw, uint32_t addr);
void EVE_Hal_endTransfer(EVE_HalContext *phost);
uint8_t EVE_Hal_transfer8(EVE_HalContext *phost, uint8_t value);
uint16_t EVE_Hal_transfer16(EVE_HalContext *phost, uint16_t value);
uint32_t EVE_Hal_transfer32(EVE_HalContext *phost, uint32_t value);
void EVE_Hal_flush(EVE_HalContext *phost);

/* MCU and Timing Functions */
void EVE_Mcu_initialize();
void EVE_Mcu_release();
void EVE_Millis_initialize();
void EVE_Millis_release();
uint32_t EVE_millis();
uint64_t EVE_millis64();
void EVE_sleep(uint32_t ms);

/* Utility Functions */
bool EVE_UtilImpl_bootupDisplayGpio(EVE_HalContext *phost);

/* HAL Core Functions */
void EVE_Hal_release();
size_t EVE_Hal_list();
void EVE_Hal_info(EVE_DeviceInfo *deviceInfo, size_t deviceIdx);
bool EVE_Hal_isDevice(EVE_HalContext *phost, size_t deviceIdx);
void EVE_Hal_defaults(EVE_HalParameters *parameters);
void EVE_Hal_defaultsEx(EVE_HalParameters *parameters, size_t deviceIdx);
bool EVE_Hal_open(EVE_HalContext *phost, const EVE_HalParameters *parameters);
void EVE_Hal_close(EVE_HalContext *phost);
void EVE_Hal_idle(EVE_HalContext *phost);
void EVE_Hal_transferMem(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size);
void EVE_Hal_transferProgMem(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size);
uint32_t EVE_Hal_transferString(EVE_HalContext *phost, const char *str, uint32_t index, uint32_t size, uint32_t padMask);
uint8_t EVE_Hal_rd8(EVE_HalContext *phost, uint32_t addr);
uint16_t EVE_Hal_rd16(EVE_HalContext *phost, uint32_t addr);
uint32_t EVE_Hal_rd32(EVE_HalContext *phost, uint32_t addr);
void EVE_Hal_rdMem(EVE_HalContext *phost, uint8_t *result, uint32_t addr, uint32_t size);
void EVE_Hal_wr8(EVE_HalContext *phost, uint32_t addr, uint8_t v);
void EVE_Hal_wr16(EVE_HalContext *phost, uint32_t addr, uint16_t v);
void EVE_Hal_wr32(EVE_HalContext *phost, uint32_t addr, uint32_t v);
void EVE_Hal_wrMem(EVE_HalContext *phost, uint32_t addr, const uint8_t *buffer, uint32_t size);
void EVE_Hal_wrProgMem(EVE_HalContext *phost, uint32_t addr, const uint8_t *buffer, uint32_t size);
void EVE_Hal_wrString(EVE_HalContext *phost, uint32_t addr, const char *str, uint32_t index, uint32_t size, uint32_t padMask);
void EVE_Hal_hostCommand(EVE_HalContext *phost, uint8_t *cmd);
void EVE_Hal_hostCommandExt3(EVE_HalContext *phost, uint32_t cmd);
void EVE_Hal_hostCommand5(EVE_HalContext *phost, uint8_t cmd, uint8_t param0, uint8_t param1, uint8_t param2);
bool EVE_Hal_powerCycle(EVE_HalContext *phost, bool up);
void EVE_Hal_setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls);
void EVE_Hal_restoreSPI(EVE_HalContext *phost);
uint32_t EVE_Hal_currentFrequency(EVE_HalContext *phost);
void EVE_Hal_displayMessage(EVE_HalContext *phost, const char *str, uint16_t size);
EVE_HalPlatform *EVE_Hal_initialize();

/* Host Control Functions */
void EVE_Host_powerModeSwitch(EVE_HalContext *phost, EVE_POWER_MODE_T pwrmode);
void EVE_Host_selectSysClk(EVE_HalContext *phost, EVE_81X_PLL_FREQ_T freq);
void EVE_Host_resetActive(EVE_HalContext *phost);
void EVE_Host_resetRemoval(EVE_HalContext *phost);
void EVE_Host_setPllSp1(EVE_HalContext *phost, EVE_CMD_SYS_PLL_FREQ val);
void EVE_Host_setClkDiv(EVE_HalContext *phost, EVE_CMD_SYS_CLK_DIV val);
void EVE_Host_setBootCfg(EVE_HalContext *phost, EVE_CMD_BOOT_CFG val);
void EVE_Host_bootCfgEn(EVE_HalContext *phost, EVE_CMD_BOOT_CFG_EN val);
void EVE_Host_setDdrType(EVE_HalContext *phost, EVE_CMD_DDR_TYPE val);
void EVE_Host_setGpReg(EVE_HalContext *phost, EVE_CMD_GPREG val);

/* Command Buffer Functions */
uint16_t EVE_Cmd_rp(EVE_HalContext *phost);
uint16_t EVE_Cmd_wp(EVE_HalContext *phost);
uint16_t EVE_Cmd_space(EVE_HalContext *phost);
void EVE_Cmd_startFunc(EVE_HalContext *phost);
void EVE_Cmd_endFunc(EVE_HalContext *phost);
bool EVE_Cmd_wrMem(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size);
bool EVE_Cmd_wrProgMem(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size);
uint32_t EVE_Cmd_wrString(EVE_HalContext *phost, const char *str, uint32_t maxLength);
bool EVE_Cmd_wr8(EVE_HalContext *phost, uint8_t value);
bool EVE_Cmd_wr16(EVE_HalContext *phost, uint16_t value);
bool EVE_Cmd_wr32(EVE_HalContext *phost, uint32_t value);
uint16_t EVE_Cmd_moveWp(EVE_HalContext *phost, uint16_t bytes);
bool EVE_Cmd_waitFlush(EVE_HalContext *phost);
uint32_t EVE_Cmd_waitSpace(EVE_HalContext *phost, uint32_t size);
bool EVE_Cmd_waitLogo(EVE_HalContext *phost);
bool EVE_Cmd_waitRead32(EVE_HalContext *phost, uint32_t ptr, uint32_t value);
void EVE_Cmd_restore(EVE_HalContext *phost);

/* Utility File Functions */
bool EVE_Util_loadSdCard(EVE_HalContext *phost);
bool EVE_Util_sdCardReady(EVE_HalContext *phost);
bool EVE_Util_loadRawFile(EVE_HalContext *phost, uint32_t address, const char *filename);
bool EVE_Util_loadInflateFile(EVE_HalContext *phost, uint32_t address, const char *filename);
bool EVE_Util_loadCmdFile(EVE_HalContext *phost, const char *filename, uint32_t *transfered);
size_t EVE_Util_readFile(EVE_HalContext *phost, uint8_t *buffer, size_t size, const char *filename);
bool EVE_Util_loadMediaFile(EVE_HalContext *phost, const char *filename, uint32_t *transfered);
bool EVE_Util_loadMediaRam(EVE_HalContext *phost, const uint8_t *srcBuf, uint32_t size, uint32_t *transfered);
bool EVE_Util_loadFile(EVE_HalContext *phost, uint32_t address, uint32_t size, const char *filename, uint32_t *transfered);
void EVE_Util_closeFile(EVE_HalContext *phost);

/* Media FIFO Functions */
bool EVE_MediaFifo_set(EVE_HalContext *phost, uint32_t address, uint32_t size);
void EVE_MediaFifo_close(EVE_HalContext *phost);
uint32_t EVE_MediaFifo_rp(EVE_HalContext *phost);
uint32_t EVE_MediaFifo_wp(EVE_HalContext *phost);
uint32_t EVE_MediaFifo_space(EVE_HalContext *phost);
bool EVE_MediaFifo_wrMem(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size, uint32_t *transfered);
bool EVE_MediaFifo_waitFlush(EVE_HalContext *phost, bool orCmdFlush);
uint32_t EVE_MediaFifo_waitSpace(EVE_HalContext *phost, uint32_t size, bool orCmdFlush);

/* Bootup and Configuration Functions */
void EVE_Util_bootupDefaults(EVE_HalContext *phost, EVE_BootupParameters *bootup);
void EVE_Util_clearScreen(EVE_HalContext *phost);
bool EVE_Util_resetCoprocessor(EVE_HalContext *phost);
bool EVE_Util_bootupConfig(EVE_HalContext *phost);
bool EVE_Util_openDeviceInteractive(EVE_HalContext *phost, const char *flashFile);
bool EVE_Util_bootupConfigInteractive(EVE_HalContext *phost, EVE_DISPLAY_T display);
void EVE_Util_forceFault(EVE_HalContext *phost, const char *err);

EVE_BT820_T EVE_extendedChipId(int chipId);
int EVE_shortChipId(EVE_BT820_T chipId);
int EVE_gen(EVE_BT820_T chipId);
#endif /* EVE_H */
