/**
 * @file eve_stm32.c
 * @brief EVE mcu implementation for STM32
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */
#include "stm32h7xx_hal.h"

#include "eve.h"
#include "eve_bt820.h"

static bool qspiInWriteSession = false;
static uint32_t qspiTotalBytes = 0;
static uint32_t qspiStartAddress = 0;

enum imageResolution
{
	RES_160X120 = 32 * 1024,
	RES_320X240 = 32 * 1024,
	RES_640X480 = 64 * 1024,
	RES_800x600 = 100 * 1024,
	RES_1024x768 = 128 * 1024,
	RES_1280x960 = 32 * 1024
};
#define WRITEBUF_SIZE (RES_800x600)
#define hspi hqspi
uint8_t qspiWriteCache[WRITEBUF_SIZE];
QSPI_CommandTypeDef sCommand = {0};
extern QSPI_HandleTypeDef hqspi;
void *SpiHandle = NULL;
char dddbuff[100];

void EVE_Mcu_release()
{
}
uint32_t EVE_millis()
{
	return HAL_GetTick();
}
void EVE_sleep(uint32_t ms)
{
	HAL_Delay(ms);
}
void EVE_Mcu_initialize()
{
	HAL_Init();
	HAL_Delay(500);
}
void EVE_Millis_initialize()
{
}
void EVE_Millis_release()
{
}
void EVE_HalImpl_initialize()
{
}
void EVE_HalImpl_release()
{
	HAL_SPI_DeInit(&hspi);
}
bool EVE_HalImpl_defaults(EVE_HalParameters *parameters, size_t deviceIdx)
{
	return true;
};
void setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls)
{
	phost->SpiChannels = numchnls;
}
bool EVE_HalImpl_open(EVE_HalContext *phost, const EVE_HalParameters *parameters)
{
	setSPI(phost, EVE_SPI_SINGLE_CHANNEL);
	phost->Status = EVE_STATUS_OPENED;
	phost->SpiChannels = 0;
	SpiHandle = &hspi;
	++g_HalPlatform.OpenedDevices;
	return true;
}
void EVE_HalImpl_close(EVE_HalContext *phost)
{
	phost->Status = EVE_STATUS_CLOSED;
	--g_HalPlatform.OpenedDevices;
	SpiHandle = NULL;
}
void EVE_HalImpl_idle(EVE_HalContext *phost)
{
}
void EVE_Hal_startTransfer(EVE_HalContext *phost, EVE_TRANSFER_T rw,
						   uint32_t addr)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);
	HAL_StatusTypeDef ret = HAL_TIMEOUT;
	if (rw == EVE_TRANSFER_READ)
	{
		memset(&sCommand, 0, sizeof(sCommand));
		sCommand.InstructionMode = QSPI_INSTRUCTION_NONE;
		sCommand.Instruction = 0;
		sCommand.AddressSize = QSPI_ADDRESS_32_BITS;
		if (phost->SpiChannels == EVE_SPI_SINGLE_CHANNEL)
		{
			sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
			sCommand.DataMode = QSPI_DATA_1_LINE;
		}
		else
		{
			sCommand.AddressMode = QSPI_ADDRESS_4_LINES;
			sCommand.DataMode = QSPI_DATA_4_LINES;
		}
		sCommand.Address = addr;
		sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
		sCommand.DummyCycles = 0;
		sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
		sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
		ret = HAL_OK;
		phost->Status = EVE_STATUS_READING;
	}
	else
	{
		qspiInWriteSession = true;
		qspiTotalBytes = 0;
		qspiStartAddress = addr;
		ret = HAL_OK;
		phost->Status = EVE_STATUS_WRITING;
	}
	if (ret != HAL_OK)
	{
		printf("Error: HAL_SPI_Transmit failed %d\r\n", __LINE__);
	}
}
void EVE_Hal_endTransfer(EVE_HalContext *phost)
{
	if (phost->Status == EVE_STATUS_WRITING)
	{
		HAL_StatusTypeDef ret = HAL_TIMEOUT;
		QSPI_CommandTypeDef sCommand = {0};
		sCommand.InstructionMode = QSPI_INSTRUCTION_NONE;
		sCommand.AddressSize = QSPI_ADDRESS_32_BITS;
		sCommand.Address = qspiStartAddress | 0x80000000;
		if (phost->SpiChannels == EVE_SPI_SINGLE_CHANNEL)
		{
			sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
			sCommand.DataMode = QSPI_DATA_1_LINE;
		}
		else
		{
			sCommand.AddressMode = QSPI_ADDRESS_4_LINES;
			sCommand.DataMode = QSPI_DATA_4_LINES;
		}
		sCommand.NbData = qspiTotalBytes;
		sCommand.DummyCycles = 0;
		sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
		sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
		sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
		ret = HAL_QSPI_Command(&hqspi, &sCommand, HAL_MAX_DELAY);
		ret = HAL_QSPI_Transmit(&hqspi, qspiWriteCache, HAL_MAX_DELAY);
		if (ret != HAL_OK)
		{
			printf("Error: HAL_QSPI_Transmit failed %d\r\n", __LINE__);
			qspiInWriteSession = false;
		}
		qspiInWriteSession = false;
		qspiTotalBytes = 0;
	}
	eve_assert(
		phost->Status == EVE_STATUS_READING || phost->Status == EVE_STATUS_WRITING);
	phost->Status = EVE_STATUS_OPENED;
}
void EVE_Hal_flush(EVE_HalContext *phost)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);
}
static inline bool rdBuffer(EVE_HalContext *phost, uint8_t *buffer, uint32_t size)
{
	HAL_StatusTypeDef ret = HAL_TIMEOUT;
	bool readOk = false;
	const uint32_t pollingBytes = sCommand.Address >= RAM_CMD ? 8 : EVE_POLLING_BYTES;
	uint8_t countTimout = 0;
	memset(phost->SpiRdBuf, 0, pollingBytes);
	sCommand.NbData = pollingBytes + size;
	ret = HAL_QSPI_Command(&hqspi, &sCommand, HAL_MAX_DELAY);
	do
	{
		ret = HAL_QSPI_Receive(&hqspi, phost->SpiRdBuf, HAL_MAX_DELAY);
		for (uint8_t i = 0; i < pollingBytes; i++)
		{
			if (phost->SpiRdBuf[i] == 0x01)
			{
				memcpy(buffer, &phost->SpiRdBuf[i + 1], size);
				readOk = true;
				return true;
			}
		}
		countTimout++;
		if (countTimout == 2 && readOk)
		{
			eve_printf_debug("Retry 2 times\n");
		}
		if (countTimout == 3 && readOk)
		{
			eve_printf_debug("Retry 3 times\n");
		}
		if (countTimout >= EVE_READ_TIMEOUT)
		{
			eve_printf_debug("Read buffer ends with error\n");
			assert(0 && "EVE read timeout exceeded");
			return false;
		}
	} while (readOk == false);
}
static inline bool wrBuffer(EVE_HalContext *phost, const uint8_t *buffer, uint32_t size)
{
	HAL_StatusTypeDef ret = HAL_TIMEOUT;
	if (!qspiInWriteSession)
		return false;
	memcpy(&qspiWriteCache[0] + qspiTotalBytes, buffer, size);
	qspiTotalBytes += size;
	return true;
}
uint32_t EVE_Hal_transfer32(EVE_HalContext *phost, uint32_t value)
{
	uint8_t buffer[4];
	if (phost->Status == EVE_STATUS_READING)
	{
		rdBuffer(phost, buffer, 4);
		return (uint32_t)buffer[0] | (uint32_t)buffer[1] << 8 | (uint32_t)buffer[2] << 16 | (uint32_t)buffer[3] << 24;
	}
	else
	{
		buffer[0] = value & 0xFF;
		buffer[1] = (value >> 8) & 0xFF;
		buffer[2] = (value >> 16) & 0xFF;
		buffer[3] = value >> 24;
		wrBuffer(phost, buffer, 4);
		return 0;
	}
}
void EVE_Hal_transferProgMem(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size)
{
	EVE_Hal_transferMem(phost, result, buffer, size);
}
void EVE_Hal_transferMem(EVE_HalContext *phost, uint8_t *result, const uint8_t *buffer, uint32_t size)
{
	if (!size)
		return;
	if (result && buffer)
	{
		eve_debug_break();
	}
	else if (result)
	{
		rdBuffer(phost, result, size);
	}
	else if (buffer)
	{
		wrBuffer(phost, buffer, size);
	}
}

uint32_t EVE_Hal_transferString(EVE_HalContext *phost, const char *str,
								uint32_t index, uint32_t size, uint32_t padMask)
{
	if (!size)
	{
		eve_assert(padMask == 0x3);
		EVE_Hal_transfer32(phost, 0);
		return 4;
	}
	eve_assert(size <= EVE_CMD_STRING_MAX);
	uint32_t transferred = 0;
	if (phost->Status == EVE_STATUS_WRITING)
	{
		transferred += (uint32_t)strnlen(str, size) + 1;
		eve_assert(str[transferred - 1] == '\0');
		wrBuffer(phost, str, transferred);
		if (transferred & padMask)
		{
			uint32_t pad = 4 - (transferred & padMask);
			uint8_t padding[4] = {0};
			wrBuffer(phost, padding, pad);
			transferred += pad;
			eve_assert(!(transferred & 0x3));
		}
	}
	else
	{
		eve_debug_break();
	}
	return transferred;
}
void EVE_Hal_hostCommand5(EVE_HalContext *phost, uint8_t cmd, uint8_t param0, uint8_t param1, uint8_t param2)
{
	eve_assert(phost->Status == EVE_STATUS_OPENED);
	uint8_t prefix = cmd == EVE_82X_ACTIVE ? EVE_82X_SPI_ACTIVE : EVE_82X_SPI_COMMAND;
	uint8_t hcmd[5] = {0, 0, 0, 0, 0};
	hcmd[0] = prefix;
	hcmd[1] = cmd;
	hcmd[2] = param0;
	hcmd[3] = param1;
	hcmd[4] = param2;
	HAL_StatusTypeDef ret = HAL_TIMEOUT;
	QSPI_CommandTypeDef sCommand = {0};
	sCommand.InstructionMode = QSPI_INSTRUCTION_NONE;
	sCommand.Instruction = 0;
	sCommand.AddressMode = QSPI_ADDRESS_NONE;
	sCommand.AddressSize = QSPI_ADDRESS_32_BITS;
	sCommand.Address = 0;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode = QSPI_DATA_1_LINE;
	sCommand.DummyCycles = 0;
	sCommand.NbData = 5;
	sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;
	sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	ret = HAL_QSPI_Command(&hqspi, &sCommand, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
	ret = HAL_QSPI_Transmit(&hqspi, hcmd, HAL_QSPI_TIMEOUT_DEFAULT_VALUE);
	if (ret != HAL_OK)
	{
		printf("Error: HAL_SPI_Transmit failed\r\n");
	}
}
bool EVE_Hal_powerCycle(EVE_HalContext *phost, bool up)
{
#define EVE_PD_GPIO_Port GPIOB
#define EVE_PD_Pin GPIO_PIN_10

	if (up)
	{
		HAL_GPIO_WritePin(EVE_PD_GPIO_Port, EVE_PD_Pin, GPIO_PIN_RESET);
		HAL_Delay(200);
		setSPI(phost, EVE_SPI_SINGLE_CHANNEL);
		HAL_Delay(200);
		HAL_GPIO_WritePin(EVE_PD_GPIO_Port, EVE_PD_Pin, GPIO_PIN_SET);
		HAL_Delay(200);
	}
	else
	{
		HAL_GPIO_WritePin(EVE_PD_GPIO_Port, EVE_PD_Pin, GPIO_PIN_SET);
		HAL_Delay(20);
		HAL_GPIO_WritePin(EVE_PD_GPIO_Port, EVE_PD_Pin, GPIO_PIN_RESET);
		HAL_Delay(20);
		setSPI(phost, EVE_SPI_SINGLE_CHANNEL);
	}
	return true;
}
void EVE_Hal_setSPI(EVE_HalContext *phost, EVE_SPI_CHANNELS_T numchnls)
{
	int32_t syscfg;
	if (numchnls > EVE_SPI_QUAD_CHANNEL)
		return;
	syscfg = EVE_Hal_rd32(phost, REG_SYS_CFG);
	if (numchnls == EVE_SPI_DUAL_CHANNEL)
	{
		syscfg |= SPI_WIDTH_2bit;
	}
	else if (numchnls == EVE_SPI_QUAD_CHANNEL)
	{
		syscfg |= SPI_WIDTH_4bit;
	}
	EVE_Hal_wr32(phost, REG_SYS_CFG, syscfg);
	setSPI(phost, numchnls);
}
bool EVE_UtilImpl_bootupDisplayGpio(EVE_HalContext *phost)
{
	return true;
}

void EVE_mcu_reset(void)
{
    eve_printf_debug("MCU reset requested via watchdog\n");
    HAL_Delay(100);
    
    __disable_irq();       
    NVIC_SystemReset();     
}
