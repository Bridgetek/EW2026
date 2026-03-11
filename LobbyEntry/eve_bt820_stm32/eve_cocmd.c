/**
 * @file eve_cocmd.c
 * @brief EVE coprocessor commands implementation
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */
#include <stdarg.h>

#include "eve.h"
#include "eve_bt820.h"

#define EVE_COCMD_UNSUPPORTED(cmd, res) eve_pragma_warning("Coprocessor command " #cmd " is not supported on this platform")(res)

void EVE_CoCmd_d(EVE_HalContext *phost, uint32_t cmd)
{
	EVE_Cmd_wr32(phost, cmd);
}
void EVE_CoCmd_dd(EVE_HalContext *phost, uint32_t cmd, uint32_t d0)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_ddd(EVE_HalContext *phost, uint32_t cmd, uint32_t d0, uint32_t d1)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr32(phost, d1);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_dddd(EVE_HalContext *phost, uint32_t cmd, uint32_t d0, uint32_t d1, uint32_t d2)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr32(phost, d1);
	EVE_Cmd_wr32(phost, d2);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_ddddd(EVE_HalContext *phost, uint32_t cmd, uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr32(phost, d1);
	EVE_Cmd_wr32(phost, d2);
	EVE_Cmd_wr32(phost, d3);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_dddddd(EVE_HalContext *phost, uint32_t cmd,
					  uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr32(phost, d1);
	EVE_Cmd_wr32(phost, d2);
	EVE_Cmd_wr32(phost, d3);
	EVE_Cmd_wr32(phost, d4);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_ddww(EVE_HalContext *phost, uint32_t cmd,
					uint32_t d0, uint16_t w1, uint16_t w2)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_ddwww(EVE_HalContext *phost, uint32_t cmd,
					 uint32_t d0, uint16_t w1, uint16_t w2, uint16_t w3)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, 0);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_ddwwz(EVE_HalContext *phost, uint32_t cmd,
					 uint32_t d0, uint16_t w1, uint16_t w2, const char *s)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_ddwwz_s(EVE_HalContext *phost, uint32_t cmd,
					   uint32_t d0, uint16_t w1, uint16_t w2, const char *s, uint32_t len)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wrString(phost, s, len);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_dddwwww(EVE_HalContext *phost, uint32_t cmd,
					   uint32_t d0, uint32_t d1,
					   uint16_t w2, uint16_t w3, uint16_t w4, uint16_t w5)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr32(phost, d1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_dwwdd(EVE_HalContext *phost, uint32_t cmd,
					 uint16_t w0, uint16_t w1, uint32_t d2, uint32_t d3)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr32(phost, d2);
	EVE_Cmd_wr32(phost, d3);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_dwwdwwd(EVE_HalContext *phost, uint32_t cmd,
					   uint16_t w0, uint16_t w1, uint32_t d2,
					   uint16_t w3, uint16_t w4, uint32_t d5)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr32(phost, d2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr32(phost, d5);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_dwwwwdw(EVE_HalContext *phost, uint32_t cmd,
					   uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
					   uint32_t d4, uint16_t w5)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr32(phost, d4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wr16(phost, 0);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_dwwwwdww(EVE_HalContext *phost, uint32_t cmd,
						uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
						uint32_t d4, uint16_t w5, uint16_t w6)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr32(phost, d4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wr16(phost, w6);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_dwwww(EVE_HalContext *phost, uint32_t cmd,
					 uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_dwwwww(EVE_HalContext *phost, uint32_t cmd,
					  uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3, uint16_t w4)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, 0);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_dwwwwd(EVE_HalContext *phost, uint32_t cmd,
					  uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3, uint32_t d4)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr32(phost, d4);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_dwwwwww(EVE_HalContext *phost, uint32_t cmd,
					   uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3, uint16_t w4, uint16_t w5)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_dwwwwwww(EVE_HalContext *phost, uint32_t cmd,
						uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
						uint16_t w4, uint16_t w5, uint16_t w6)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wr16(phost, w6);
	EVE_Cmd_wr16(phost, 0);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_dwwwwwwww(EVE_HalContext *phost, uint32_t cmd,
						 uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
						 uint16_t w4, uint16_t w5, uint16_t w6, uint16_t w7)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wr16(phost, w6);
	EVE_Cmd_wr16(phost, w7);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_dwwwwwwz(EVE_HalContext *phost, uint32_t cmd,
						uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
						uint16_t w4, uint16_t w5, const char *s)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_dwwwwwwz_s(EVE_HalContext *phost, uint32_t cmd,
						  uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3,
						  uint16_t w4, uint16_t w5, const char *s, uint32_t len)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr16(phost, w0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr16(phost, w5);
	EVE_Cmd_wrString(phost, s, len);
	EVE_Cmd_endFunc(phost);
}
void EVE_CoCmd_ddwwwwdd(EVE_HalContext *phost, uint32_t cmd,
						uint32_t d0, uint16_t w1, uint16_t w2, uint16_t w3, uint16_t w4,
						uint32_t d5, uint32_t d6)
{
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, cmd);
	EVE_Cmd_wr32(phost, d0);
	EVE_Cmd_wr16(phost, w1);
	EVE_Cmd_wr16(phost, w2);
	EVE_Cmd_wr16(phost, w3);
	EVE_Cmd_wr16(phost, w4);
	EVE_Cmd_wr32(phost, d5);
	EVE_Cmd_wr32(phost, d6);
	EVE_Cmd_endFunc(phost);
}
bool EVE_CoCmd_getMatrix(EVE_HalContext *phost, int32_t *m)
{
	uint16_t resAddr;
	int i;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_GETMATRIX);
	resAddr = EVE_Cmd_moveWp(phost, 6 * 4);
	EVE_Cmd_endFunc(phost);
	if (m)
	{
		if (!EVE_Cmd_waitFlush(phost))
			return false;
		EVE_Hal_startTransfer(phost, EVE_TRANSFER_READ, RAM_CMD + resAddr);
		for (i = 0; i < 6; ++i)
			m[i] = EVE_Hal_transfer32(phost, 0);
		EVE_Hal_endTransfer(phost);
	}
	return true;
}
void EVE_CoCmd_setRotate(EVE_HalContext *phost, uint32_t r)
{
	const bool swapXY = r & 0x2;

	if (swapXY)
	{
		phost->Width = EVE_Hal_rd16(phost, REG_VSIZE);
		phost->Height = EVE_Hal_rd16(phost, REG_HSIZE);
	}
	else
	{
		phost->Width = EVE_Hal_rd16(phost, REG_HSIZE);
		phost->Height = EVE_Hal_rd16(phost, REG_VSIZE);
	}

	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_SETROTATE);
	EVE_Cmd_wr32(phost, r);
	EVE_Cmd_endFunc(phost);

	EVE_Hal_flush(phost);
}
uint32_t EVE_CoCmd_calibrate(EVE_HalContext *phost)
{
	uint16_t resAddr;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_CALIBRATE);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);
	phost->DlPrimitive = 0;
	if (!EVE_Cmd_waitFlush(phost))
		return 0;
	return EVE_Hal_rd32(phost, RAM_CMD + resAddr);
}
uint32_t EVE_CoCmd_calibrateSub(EVE_HalContext *phost, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	uint16_t resAddr;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_CALIBRATESUB);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, w);
	EVE_Cmd_wr16(phost, h);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);
	phost->DlPrimitive = 0;
	if (!EVE_Cmd_waitFlush(phost))
		return 0;
	return EVE_Hal_rd32(phost, RAM_CMD + resAddr);
}
void EVE_CoCmd_dl(EVE_HalContext *phost, uint32_t dl)
{
	EVE_CoCmd_d(phost, dl);
}
void EVE_CoDlImpl_resetDlState(EVE_HalContext *phost);
void EVE_CoDlImpl_resetCoState(EVE_HalContext *phost);
#define EVE_CO_SCRATCH_HANDLE (1 ? phost->CoScratchHandle : 15)
void EVE_CoCmd_dlStart(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_DLSTART);
	EVE_CoDlImpl_resetDlState(phost);
}
void EVE_CoCmd_swap(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_SWAP);
}
void EVE_CoCmd_interrupt(EVE_HalContext *phost, uint32_t ms)
{
	EVE_CoCmd_dd(phost, CMD_INTERRUPT, ms);
}
void EVE_CoCmd_coldStart(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_COLDSTART);
	EVE_CoDlImpl_resetCoState(phost);
}
bool EVE_CoCmd_getMatrix(EVE_HalContext *phost, int32_t *m);
void EVE_CoCmd_setRotate(EVE_HalContext *phost, uint32_t r);
void EVE_CoCmd_sync(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_SYNC);
}
void EVE_CoCmd_clearCache(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_CLEARCACHE);
}
void EVE_CoCmd_nop(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_NOP);
}
#define EVE_CoCmd_hsf(phost, hsf) EVE_COCMD_UNSUPPORTED(CMD_HSF, false)
#define EVE_CoCmd_apiLevel(phost, level) EVE_COCMD_UNSUPPORTED(CMD_APILEVEL, false)
void EVE_CoCmd_wait(EVE_HalContext *phost, uint32_t us)
{
	EVE_CoCmd_dd(phost, CMD_WAIT, us);
}
void EVE_CoCmd_return(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_RETURN);
}
void EVE_CoCmd_callList(EVE_HalContext *phost, uint32_t a)
{
	EVE_CoCmd_dd(phost, CMD_CALLLIST, a);
}
void EVE_CoCmd_newList(EVE_HalContext *phost, uint32_t a)
{
	EVE_CoCmd_dd(phost, CMD_NEWLIST, a);
}
void EVE_CoCmd_endList(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_ENDLIST);
}
void EVE_CoCmd_copyList(EVE_HalContext *phost, uint32_t dst)
{
	EVE_CoCmd_dd(phost, CMD_COPYLIST, dst);
}
void EVE_CoCmd_renderTarget(EVE_HalContext *phost, uint32_t dst,
                            uint16_t fmt, int16_t w, int16_t h)
{
	phost->CoRenderTargetDst = dst;
	phost->CoRenderTargetFmt = fmt;
	phost->CoRenderTargetW = w;
	phost->CoRenderTargetH = h;

	EVE_CoCmd_ddwww(phost, CMD_RENDERTARGET, dst, fmt, w, h);
}
#define EVE_CoCmd_pclkFreq(phost, ftarget, rounding) EVE_COCMD_UNSUPPORTED(CMD_PCLKFREQ, 0)
bool EVE_CoCmd_memCrc(EVE_HalContext *phost, uint32_t ptr, uint32_t num, uint32_t *result);
bool EVE_CoCmd_regRead(EVE_HalContext *phost, uint32_t ptr, uint32_t *result);
void EVE_CoCmd_memWrite(EVE_HalContext *phost, uint32_t ptr, uint32_t num)
{
	EVE_CoCmd_ddd(phost, CMD_MEMWRITE, ptr, num);
}
void EVE_CoCmd_memWrite32(EVE_HalContext *phost, uint32_t ptr, uint32_t value)
{
	EVE_CoCmd_dddd(phost, CMD_MEMWRITE, ptr, 4, value);
}
void EVE_CoCmd_memSet(EVE_HalContext *phost, uint32_t ptr, uint32_t value, uint32_t num)
{
	EVE_CoCmd_dddd(phost, CMD_MEMSET, ptr, value, num);
}
void EVE_CoCmd_memZero(EVE_HalContext *phost, uint32_t ptr, uint32_t num)
{
	EVE_CoCmd_ddd(phost, CMD_MEMZERO, ptr, num);
}
void EVE_CoCmd_memCpy(EVE_HalContext *phost, uint32_t dest, uint32_t src, uint32_t num)
{
	EVE_CoCmd_dddd(phost, CMD_MEMCPY, dest, src, num);
}
void EVE_CoCmd_append(EVE_HalContext *phost, uint32_t ptr, uint32_t num)
{
	EVE_CoCmd_ddd(phost, CMD_APPEND, ptr, num);
}
void EVE_CoCmd_apbRead(EVE_HalContext *phost, uint32_t dest, uint32_t addr)
{
	EVE_CoCmd_ddd(phost, CMD_APBREAD, dest, addr);
}
void EVE_CoCmd_apbWrite(EVE_HalContext *phost, uint32_t addr, uint32_t v)
{
	EVE_CoCmd_ddd(phost, CMD_APBWRITE, addr, v);
}
void EVE_CoCmd_regWrite(EVE_HalContext *phost, uint32_t dst, uint32_t value)
{
	EVE_CoCmd_ddd(phost, CMD_REGWRITE, dst, value);
}
void EVE_CoCmd_result(EVE_HalContext *phost, uint32_t dst)
{
	EVE_CoCmd_dd(phost, CMD_RESULT, dst);
}
void EVE_CoCmd_waitChange(EVE_HalContext *phost, uint32_t a)
{
	EVE_CoCmd_dd(phost, CMD_WAITCHANGE, a);
}
void EVE_CoCmd_waitCond(EVE_HalContext *phost, uint32_t a,
						uint32_t func, uint32_t ref, uint32_t mask)
{
	EVE_CoCmd_ddddd(phost, CMD_WAITCOND, a, func, ref, mask);
}
void EVE_CoCmd_fence(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_FENCE);
}
void EVE_CoCmd_workArea(EVE_HalContext *phost, uint32_t ptr)
{
	EVE_CoCmd_dd(phost, CMD_WORKAREA, ptr);
}
void EVE_CoCmd_skipCond(EVE_HalContext *phost, uint32_t a,
						uint32_t func, uint32_t ref, uint32_t mask, uint32_t num)
{
	EVE_CoCmd_dddddd(phost, CMD_SKIPCOND, a, func, ref, mask, num);
}
void EVE_CoCmd_snapshot(EVE_HalContext *phost, uint32_t ptr)
{
	EVE_CoCmd_dd(phost, CMD_SNAPSHOT, ptr);
}

void EVE_CoCmd_inflate(EVE_HalContext *phost, uint32_t ptr)
{
	EVE_CoCmd_ddd(phost, CMD_INFLATE2, ptr, 0);
}
bool EVE_CoCmd_inflate_progMem(EVE_HalContext *phost, uint32_t dst, const uint8_t *src, uint32_t size);
bool EVE_CoCmd_getPtr(EVE_HalContext *phost, uint32_t *result);
void EVE_CoCmd_loadImage(EVE_HalContext *phost, uint32_t ptr, uint32_t options)
{
	EVE_CoCmd_ddd(phost, CMD_LOADIMAGE, ptr, options);
	if ((options & OPT_FS) || (options & OPT_FLASH))
	{
		EVE_Cmd_wr32(phost, CMD_NOP);
	}
}
bool EVE_CoCmd_getImage_format(EVE_HalContext *phost, uint32_t *format);
bool EVE_CoCmd_loadImage_progMem(EVE_HalContext *phost, uint32_t dst, const uint8_t *src, uint32_t size, uint32_t *format);
bool EVE_CoCmd_loadImage_Mem(EVE_HalContext *phost, uint32_t dst, const uint8_t *src, uint32_t size, uint32_t *format);
bool EVE_CoCmd_loadImage_MediaFifo(EVE_HalContext *phost, uint32_t dst);
bool EVE_CoCmd_getProps(EVE_HalContext *phost, uint32_t *ptr, uint32_t *w, uint32_t *h);
void EVE_CoCmd_textDim(EVE_HalContext *phost, uint32_t dimensions,
					   int16_t font, uint16_t options, const char *s)
{
	EVE_CoCmd_ddwwz(phost, CMD_TEXTDIM, dimensions, font, options, s);
}
void EVE_CoCmd_textDim_s(EVE_HalContext *phost, uint32_t dimensions,
						 int16_t font, uint16_t options, const char *s, uint32_t len)
{
	EVE_CoCmd_ddwwz_s(phost, CMD_TEXTDIM, dimensions, font, options, s, len);
}
#define EVE_CoCmd_snapshot2(phost, fmt, ptr, x, y, w, h) EVE_COCMD_UNSUPPORTED(CMD_SNAPSHOT2, false)
void EVE_CoCmd_mediaFifo(EVE_HalContext *phost, uint32_t ptr, uint32_t size)
{
	EVE_CoCmd_ddd(phost, CMD_MEDIAFIFO, ptr, size);
}
void EVE_CoCmd_videoStart(EVE_HalContext *phost)
{
	EVE_CoCmd_dd(phost, CMD_VIDEOSTART, OPT_MEDIAFIFO);
}
void EVE_CoCmd_videoStart_ex(EVE_HalContext *phost, uint32_t options)
{
	EVE_CoCmd_dd(phost, CMD_VIDEOSTART, options);
}
void EVE_CoCmd_videoFrame(EVE_HalContext *phost, uint32_t dst, uint32_t ptr)
{
	EVE_CoCmd_ddd(phost, CMD_VIDEOFRAME, dst, ptr);
}
void EVE_CoCmd_flashErase(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_FLASHERASE);
}
bool EVE_CoCmd_flashErase_flush(EVE_HalContext *phost);
void EVE_CoCmd_flashWrite(EVE_HalContext *phost, uint32_t ptr, uint32_t num)
{
	EVE_CoCmd_ddd(phost, CMD_FLASHWRITE, ptr, num);
}
void EVE_CoCmd_flashRead(EVE_HalContext *phost, uint32_t dest, uint32_t src, uint32_t num)
{
	EVE_CoCmd_dddd(phost, CMD_FLASHREAD, dest, src, num);
}
bool EVE_CoCmd_flashRead_flush(EVE_HalContext *phost, uint32_t dest, uint32_t src, uint32_t num);
void EVE_CoCmd_flashUpdate(EVE_HalContext *phost, uint32_t dest, uint32_t src, uint32_t num)
{
	EVE_CoCmd_dddd(phost, CMD_FLASHUPDATE, dest, src, num);
}
void EVE_CoCmd_flashDetach(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_FLASHDETACH);
}
uint32_t EVE_CoCmd_flashAttach(EVE_HalContext *phost);
uint32_t EVE_CoCmd_flashFast(EVE_HalContext *phost, uint32_t *result);
void EVE_CoCmd_flashSpiDesel(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_FLASHSPIDESEL);
}
void EVE_CoCmd_flashSpiTx(EVE_HalContext *phost, uint32_t num)
{
	EVE_CoCmd_dd(phost, CMD_FLASHSPITX, num);
}
void EVE_CoCmd_flashSpiRx(EVE_HalContext *phost, uint32_t ptr, uint32_t num)
{
	EVE_CoCmd_ddd(phost, CMD_FLASHSPIRX, ptr, num);
}
void EVE_CoCmd_flashSource(EVE_HalContext *phost, uint32_t ptr)
{
	EVE_CoCmd_dd(phost, CMD_FLASHSOURCE, ptr);
}
void EVE_CoCmd_appendF(EVE_HalContext *phost, uint32_t ptr, uint32_t num)
{
	EVE_CoCmd_ddd(phost, CMD_APPENDF, ptr, num);
}
void EVE_CoCmd_videoStartF(EVE_HalContext *phost)
{
	EVE_CoCmd_dd(phost, CMD_VIDEOSTART, OPT_FLASH);
}
bool EVE_CoCmd_loadImage_flash(EVE_HalContext *phost, uint32_t dst, uint32_t src, uint32_t *format);
bool EVE_CoCmd_inflate_flash(EVE_HalContext *phost, uint32_t dst, uint32_t src);

void EVE_CoCmd_inflate2(EVE_HalContext *phost, uint32_t ptr, uint32_t options)
{
	EVE_CoCmd_ddd(phost, CMD_INFLATE2, ptr, options);
}
bool EVE_CoCmd_getImage(EVE_HalContext *phost, uint32_t *source, uint32_t *fmt, uint32_t *w, uint32_t *h, uint32_t *palette);
#define EVE_CoCmd_fontCache(phost, font, ptr, num) EVE_COCMD_UNSUPPORTED(CMD_FONTCACHE, false)
#define EVE_CoCmd_fontCacheQuery(phost, total, used) EVE_COCMD_UNSUPPORTED(CMD_FONTCACHEQUERY, false)
void EVE_CoCmd_bgColor(EVE_HalContext *phost, uint32_t c)
{
	if (phost->CoBgColor != c)
	{
		EVE_CoCmd_dd(phost, CMD_BGCOLOR, c);
		phost->CoBgColor = c;
	}
}
void EVE_CoCmd_fgColor(EVE_HalContext *phost, uint32_t c)
{
	if (phost->CoFgColor != c)
	{
		EVE_CoCmd_dd(phost, CMD_FGCOLOR, c);
		phost->CoFgColor = c;
	}
}
bool EVE_CoCmd_bitmapTransform(EVE_HalContext *phost, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t tx0, int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2, uint16_t *result);
void EVE_CoCmd_loadIdentity(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_LOADIDENTITY);
	phost->CoBitmapTransform = false;
}
void EVE_CoCmd_translate(EVE_HalContext *phost, int32_t tx, int32_t ty)
{
	EVE_CoCmd_ddd(phost, CMD_TRANSLATE, tx, ty);
	phost->CoBitmapTransform = true;
}
void EVE_CoCmd_scale(EVE_HalContext *phost, int32_t sx, int32_t sy)
{
	EVE_CoCmd_ddd(phost, CMD_SCALE, sx, sy);
	phost->CoBitmapTransform = true;
}
void EVE_CoCmd_rotate(EVE_HalContext *phost, int32_t a)
{
	EVE_CoCmd_dd(phost, CMD_ROTATE, a);
	phost->CoBitmapTransform = true;
}
void EVE_CoCmd_setMatrix(EVE_HalContext *phost)
{
	if (phost->CoBitmapTransform || EVE_DL_STATE.BitmapTransform)
	{
		EVE_CoCmd_d(phost, CMD_SETMATRIX);
		EVE_DL_STATE.BitmapTransform = phost->CoBitmapTransform;
	}
}
void EVE_CoCmd_setFont(EVE_HalContext *phost, uint32_t font, uint32_t ptr)
{
	EVE_CoCmd_dddd(phost, CMD_SETFONT2, font, ptr, 0);
	EVE_DL_STATE.Handle = (uint8_t)font;
}
void EVE_CoCmd_gradColor(EVE_HalContext *phost, uint32_t c)
{
	EVE_CoCmd_dd(phost, CMD_GRADCOLOR, c);
}
void EVE_CoCmd_setBase(EVE_HalContext *phost, uint32_t base)
{
	EVE_CoCmd_dd(phost, CMD_SETBASE, base);
}
void EVE_CoCmd_setFont2(EVE_HalContext *phost, uint32_t font, uint32_t ptr, uint32_t firstchar)
{
	EVE_CoCmd_dddd(phost, CMD_SETFONT2, font, ptr, firstchar);
	EVE_DL_STATE.Handle = (uint8_t)font;
}
void EVE_CoCmd_setScratch(EVE_HalContext *phost, uint32_t handle)
{
	EVE_CoCmd_dd(phost, CMD_SETSCRATCH, handle);
	phost->CoScratchHandle = (uint8_t)handle;
}
void EVE_CoCmd_romFont(EVE_HalContext *phost, uint32_t font, uint32_t romslot)
{
	EVE_CoCmd_ddd(phost, CMD_ROMFONT, font, romslot);
	EVE_DL_STATE.Handle = (uint8_t)font;
}
void EVE_CoCmd_setBitmap(EVE_HalContext *phost, uint32_t source, uint16_t fmt, uint16_t w, uint16_t h)
{
	EVE_CoCmd_ddwww(phost, CMD_SETBITMAP, source, fmt, w, h);
}
void EVE_CoCmd_rotateAround(EVE_HalContext *phost, int32_t x, int32_t y, int32_t a, int32_t s)
{
	EVE_CoCmd_ddddd(phost, CMD_ROTATEAROUND, x, y, a, s);
	phost->CoBitmapTransform = true;
}
void EVE_CoCmd_resetFonts(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_RESETFONTS);
}
void EVE_CoCmd_fillWidth(EVE_HalContext *phost, uint32_t s)
{
	EVE_CoCmd_dd(phost, CMD_FILLWIDTH, s);
}

void EVE_CoCmd_saveContext(EVE_HalContext *phost)
{
	uint8_t nextState;
	EVE_CoCmd_dl(phost, CMD_SAVECONTEXT);
	nextState = (phost->DlStateIndex + 1) & EVE_DL_STATE_STACK_MASK;
	phost->DlState[nextState] = phost->DlState[phost->DlStateIndex];
	phost->DlStateIndex = nextState;
}
void EVE_CoCmd_restoreContext(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_RESTORECONTEXT);
	phost->DlStateIndex = (phost->DlStateIndex - 1) & EVE_DL_STATE_STACK_MASK;
}
void EVE_CoCmd_gradient(EVE_HalContext *phost, int16_t x0, int16_t y0, uint32_t rgb0, int16_t x1, int16_t y1, uint32_t rgb1)
{
	EVE_CoCmd_dwwdwwd(phost, CMD_GRADIENT, x0, y0, rgb0, x1, y1, rgb1);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_cGradient(EVE_HalContext *phost, uint32_t shape,
						 int16_t x, int16_t y, int16_t w, int16_t h,
						 uint32_t rgb0, uint32_t rgb1)
{
	EVE_CoCmd_ddwwwwdd(phost, CMD_CGRADIENT, shape, x, y, w, h, rgb0, rgb1);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_glow(EVE_HalContext *phost,
					int16_t x, int16_t y, int16_t w, int16_t h)
{
	EVE_CoCmd_dwwww(phost, CMD_GLOW, x, y, w, h);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_arc(EVE_HalContext *phost, int16_t x, int16_t y, uint16_t r0, uint16_t r1, uint16_t a0, uint16_t a1)
{
	EVE_CoCmd_dwwwwww(phost, CMD_ARC, x, y, r0, r1, a0, a1);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_text(EVE_HalContext *phost, int16_t x, int16_t y, int16_t font, uint16_t options, const char *s, ...);
void EVE_CoCmd_text_s(EVE_HalContext *phost, int16_t x, int16_t y, int16_t font, uint16_t options, const char *s, uint32_t length);
void EVE_CoCmd_text_ex(EVE_HalContext *phost, int16_t x, int16_t y, int16_t font, uint16_t options, bool bottom, int16_t baseLine, int16_t capsHeight, int16_t xOffset, const char *s);
void EVE_CoCmd_button(EVE_HalContext *phost, int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char *s, ...);
void EVE_CoCmd_keys(EVE_HalContext *phost, int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char *s);
void EVE_CoCmd_progress(EVE_HalContext *phost, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t range)
{
	EVE_CoCmd_dwwwwwww(phost, CMD_PROGRESS, x, y, w, h, options, val, range);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_slider(EVE_HalContext *phost, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t range)
{
	EVE_CoCmd_dwwwwwww(phost, CMD_SLIDER, x, y, w, h, options, val, range);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_scrollbar(EVE_HalContext *phost, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t options, uint16_t val, uint16_t size, uint16_t range)
{
	EVE_CoCmd_dwwwwwwww(phost, CMD_SCROLLBAR, x, y, w, h, options, val, size, range);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_toggle(EVE_HalContext *phost, int16_t x, int16_t y, int16_t w, int16_t font, uint16_t options, uint16_t state, const char *s, ...);
void EVE_CoCmd_gauge(EVE_HalContext *phost, int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range)
{
	EVE_CoCmd_dwwwwwwww(phost, CMD_GAUGE, x, y, r, options, major, minor, val, range);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_clock(EVE_HalContext *phost, int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t h, uint16_t m, uint16_t s, uint16_t ms)
{
	EVE_CoCmd_dwwwwwwww(phost, CMD_CLOCK, x, y, r, options, h, m, s, ms);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_dial(EVE_HalContext *phost, int16_t x, int16_t y, int16_t r, uint16_t options, uint16_t val)
{
	EVE_CoCmd_dwwwww(phost, CMD_DIAL, x, y, r, options, val);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_number(EVE_HalContext *phost, int16_t x, int16_t y, int16_t font, uint16_t options, int32_t n);
void EVE_CoCmd_gradientA(EVE_HalContext *phost, int16_t x0, int16_t y0, uint32_t argb0, int16_t x1, int16_t y1, uint32_t argb1)
{
	EVE_CoCmd_dwwdwwd(phost, CMD_GRADIENTA, x0, y0, argb0, x1, y1, argb1);
	phost->DlPrimitive = 0;
}
uint32_t EVE_CoCmd_calibrate(EVE_HalContext *phost);
void EVE_CoCmd_spinner(EVE_HalContext *phost, int16_t x, int16_t y, uint16_t style, uint16_t scale)
{
	EVE_CoCmd_dwwww(phost, CMD_SPINNER, x, y, style, scale);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_stop(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_STOP);
}
void EVE_CoCmd_track(EVE_HalContext *phost, int16_t x, int16_t y, int16_t w, int16_t h, int16_t tag)
{
	EVE_CoCmd_dwwwww(phost, CMD_TRACK, x, y, w, h, tag);
}
void EVE_CoCmd_screenSaver(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_SCREENSAVER);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_sketch(EVE_HalContext *phost, int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t ptr, uint16_t format)
{
	EVE_CoCmd_dwwwwdw(phost, CMD_SKETCH, x, y, w, h, ptr, format);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_logo(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_LOGO);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_cSketch(EVE_HalContext *phost, int16_t x, int16_t y, uint16_t w, uint16_t h, uint32_t ptr, uint16_t format, uint16_t freq)
{
	EVE_CoCmd_sketch(phost, x, y, w, h, ptr, format);
}
void EVE_CoCmd_playVideo(EVE_HalContext *phost, uint32_t options)
{
	EVE_CoCmd_dd(phost, CMD_PLAYVIDEO, options);
	phost->DlPrimitive = 0;
}
#define EVE_CoCmd_animStart(phost, ch, aoptr, loop) EVE_COCMD_UNSUPPORTED(CMD_ANIMSTART, false)
void EVE_CoCmd_animStop(EVE_HalContext *phost, int32_t ch)
{
	EVE_CoCmd_dd(phost, CMD_ANIMSTOP, ch);
}
void EVE_CoCmd_animXY(EVE_HalContext *phost, int32_t ch, int16_t x, int16_t y)
{
	EVE_CoCmd_ddww(phost, CMD_ANIMXY, ch, x, y);
}
void EVE_CoCmd_animDraw(EVE_HalContext *phost, int32_t ch)
{
	EVE_CoCmd_dd(phost, CMD_ANIMDRAW, ch);
	phost->DlPrimitive = 0;
}
#define EVE_CoCmd_animFrame(phost, x, y, aoptr, frame) EVE_COCMD_UNSUPPORTED(CMD_ANIMFRAME, false)
void EVE_CoCmd_flashProgram(EVE_HalContext *phost, uint32_t dst, uint32_t src, uint32_t num)
{
	EVE_CoCmd_dddd(phost, CMD_FLASHPROGRAM, dst, src, num);
}
uint32_t EVE_CoCmd_calibrateSub(EVE_HalContext *phost, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void EVE_CoCmd_testCard(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_TESTCARD);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_animFrameRam(EVE_HalContext *phost, int16_t x, int16_t y, uint32_t aoptr, uint32_t frame)
{
	EVE_CoCmd_dwwdd(phost, CMD_ANIMFRAMERAM, x, y, aoptr, frame);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_animStartRam(EVE_HalContext *phost, int32_t ch, uint32_t aoptr, uint32_t loop)
{
	EVE_CoCmd_dddd(phost, CMD_ANIMSTARTRAM, ch, aoptr, loop);
}
void EVE_CoCmd_runAnim(EVE_HalContext *phost, uint32_t waitmask, uint32_t play)
{
	EVE_CoCmd_ddd(phost, CMD_RUNANIM, waitmask, play);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_enableRegion(EVE_HalContext *phost, uint32_t en)
{
	EVE_CoCmd_dd(phost, CMD_ENABLEREGION, en);
}
#define EVE_CoCmd_region(phost) EVE_COCMD_UNSUPPORTED(CMD_REGION, false)
#define EVE_CoCmd_endRegion(phost, x, y, w, h) EVE_COCMD_UNSUPPORTED(CMD_ENDREGION, false)
uint32_t EVE_CoCmd_fsDir(EVE_HalContext *phost, uint32_t dst, uint32_t num, const char *path);
uint32_t EVE_CoCmd_fsDir_s(EVE_HalContext *phost, uint32_t dst, uint32_t num, const char *path, uint32_t length);
uint32_t EVE_CoCmd_fsDir_ex(EVE_HalContext *phost, uint8_t *dst, uint32_t tmp, uint32_t num, const char *path);
uint32_t EVE_CoCmd_fsDir_ex_s(EVE_HalContext *phost, uint8_t *dst, uint32_t tmp, uint32_t num, const char *path, uint32_t length);
void EVE_CoCmd_fsOptions(EVE_HalContext *phost, uint32_t options)
{
	EVE_CoCmd_dd(phost, CMD_FSOPTIONS, options);
}
uint32_t EVE_CoCmd_fsRead(EVE_HalContext *phost, uint32_t dst, const char *filename);
uint32_t EVE_CoCmd_fsRead_s(EVE_HalContext *phost, uint32_t dst, const char *filename, uint32_t length);
uint32_t EVE_CoCmd_fsSize(EVE_HalContext *phost, const char *filename);
uint32_t EVE_CoCmd_fsSize_s(EVE_HalContext *phost, const char *filename, uint32_t length);
uint32_t EVE_CoCmd_fsSource(EVE_HalContext *phost, const char *filename);
uint32_t EVE_CoCmd_fsSource_s(EVE_HalContext *phost, const char *filename, uint32_t length);
uint32_t EVE_CoCmd_sdAttach(EVE_HalContext *phost, uint32_t options);
uint32_t EVE_CoCmd_sdBlockRead(EVE_HalContext *phost, uint32_t dst, uint32_t src, uint32_t count);
void EVE_CoCmd_loadAsset(EVE_HalContext *phost, uint32_t ptr, uint32_t options)
{
	EVE_CoCmd_ddd(phost, CMD_LOADASSET, ptr, options);
}
void EVE_CoCmd_loadPatch(EVE_HalContext *phost, uint32_t options)
{
	EVE_CoCmd_dd(phost, CMD_LOADPATCH, options);
}
void EVE_CoCmd_loadWav(EVE_HalContext *phost, uint32_t dst, uint32_t options)
{
	EVE_CoCmd_ddd(phost, CMD_LOADWAV, dst, options);
}
void EVE_CoCmd_playWav(EVE_HalContext *phost, uint32_t options)
{
	EVE_CoCmd_dd(phost, CMD_PLAYWAV, options);
}
void EVE_CoCmd_i2sStartup(EVE_HalContext *phost, uint32_t freq)
{
	EVE_CoCmd_dd(phost, CMD_I2SSTARTUP, freq);
}
void EVE_CoCmd_touchRd(EVE_HalContext *phost, uint32_t addr, uint32_t num)
{
	EVE_CoCmd_ddd(phost, CMD_TOUCHRD, addr, num);
}
void EVE_CoCmd_touchWr(EVE_HalContext *phost, uint32_t num)
{
	EVE_CoCmd_dd(phost, CMD_TOUCHWR, num);
}
void EVE_CoCmd_ddrStartup(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_DDRSTARTUP);
}
void EVE_CoCmd_ddrShutdown(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_DDRSHUTDOWN);
}
void EVE_CoCmd_graphicsFinish(EVE_HalContext *phost)
{
	EVE_CoCmd_d(phost, CMD_GRAPHICSFINISH);
}
void EVE_CoCmd_softBoot(EVE_HalContext *phost, uint32_t mode)
{
	EVE_CoCmd_dd(phost, CMD_SOFTBOOT, mode);
}
void EVE_CoCmd_watchdog(EVE_HalContext *phost, uint32_t init_val)
{
	EVE_CoCmd_dd(phost, CMD_WATCHDOG, init_val);
}

bool EVE_CoCmd_memCrc(EVE_HalContext *phost, uint32_t ptr, uint32_t num, uint32_t *result)
{
	uint16_t resAddr;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_MEMCRC);
	EVE_Cmd_wr32(phost, ptr);
	EVE_Cmd_wr32(phost, num);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);
	if (result)
	{
		if (!EVE_Cmd_waitFlush(phost))
			return false;
		*result = EVE_Hal_rd32(phost, RAM_CMD + resAddr);
	}
	return true;
}
bool EVE_CoCmd_regRead(EVE_HalContext *phost, uint32_t ptr, uint32_t *result)
{
	uint16_t resAddr;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_REGREAD);
	EVE_Cmd_wr32(phost, ptr);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);
	if (result)
	{
		if (!EVE_Cmd_waitFlush(phost))
			return false;
		*result = EVE_Hal_rd32(phost, RAM_CMD + resAddr);
	}
	return true;
}
bool EVE_CoCmd_inflate_progMem(EVE_HalContext *phost, uint32_t dst, const uint8_t *src, uint32_t size)
{
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_INFLATE2);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wr32(phost, 0);
	EVE_Cmd_wrProgMem(phost, src, (size + 3) & ~0x3UL);
	EVE_Cmd_endFunc(phost);
	return EVE_Cmd_waitFlush(phost);
}
bool EVE_CoCmd_getPtr(EVE_HalContext *phost, uint32_t *result)
{
	uint16_t resAddr;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_GETPTR);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);
	if (result)
	{
		if (!EVE_Cmd_waitFlush(phost))
			return false;
		*result = EVE_Hal_rd32(phost, RAM_CMD + resAddr);
	}
	return true;
}
bool EVE_CoCmd_getImage_format(EVE_HalContext *phost, uint32_t *format)
{
	uint16_t resAddr;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_GETIMAGE);
	resAddr = EVE_Cmd_moveWp(phost, 4 * 5);
	EVE_Cmd_endFunc(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	if (format)
		*format = EVE_Hal_rd32(phost, RAM_CMD + resAddr + 4);
	return true;
}
bool EVE_CoCmd_loadImage_progMem(EVE_HalContext *phost, uint32_t dst, const uint8_t *src, uint32_t size, uint32_t *format)
{
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_LOADIMAGE);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wr32(phost, OPT_NODL);
	EVE_Cmd_wrProgMem(phost, src, (size + 3) & ~0x3UL);
	EVE_Cmd_endFunc(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	return EVE_CoCmd_getImage_format(phost, format);
}
bool EVE_CoCmd_loadImage_Mem(EVE_HalContext *phost, uint32_t dst, const uint8_t *src, uint32_t size, uint32_t *format)
{
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_LOADIMAGE);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wr32(phost, OPT_NODL);
	EVE_Cmd_wrMem(phost, src, (size + 3) & ~0x3UL);
	EVE_Cmd_endFunc(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	return EVE_CoCmd_getImage_format(phost, format);
}
bool EVE_CoCmd_loadImage_MediaFifo(EVE_HalContext *phost, uint32_t dst)
{
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_LOADIMAGE);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wr32(phost, OPT_NODL | OPT_MEDIAFIFO | OPT_YCBCR);
	EVE_Cmd_endFunc(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	return true;
}
bool EVE_CoCmd_getProps(EVE_HalContext *phost, uint32_t *ptr, uint32_t *w, uint32_t *h)
{
	uint16_t resAddr;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_GETPROPS);
	resAddr = EVE_Cmd_moveWp(phost, 12);
	EVE_Cmd_endFunc(phost);
	if (ptr || w || h)
	{
		if (!EVE_Cmd_waitFlush(phost))
			return false;
		if (ptr)
			*ptr = EVE_Hal_rd32(phost, RAM_CMD + resAddr);
		if (w)
			*w = EVE_Hal_rd32(phost, RAM_CMD + resAddr + 4);
		if (h)
			*h = EVE_Hal_rd32(phost, RAM_CMD + resAddr + 8);
	}
	return true;
}
#if defined(EVE_FLASH_AVAILABLE)
bool EVE_CoCmd_flashErase_flush(EVE_HalContext *phost)
{
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	EVE_CoCmd_d(phost, CMD_FLASHERASE);
	return EVE_Cmd_waitFlush(phost);
}
bool EVE_CoCmd_flashRead_flush(EVE_HalContext *phost, uint32_t dest, uint32_t src, uint32_t num)
{
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	EVE_CoCmd_dddd(phost, CMD_FLASHREAD, dest, src, num);
	return EVE_Cmd_waitFlush(phost);
}
uint32_t EVE_CoCmd_flashAttach(EVE_HalContext *phost)
{
	uint32_t flashStatus;
	if (!EVE_Cmd_waitFlush(phost))
		return EVE_Hal_rd32(phost, REG_FLASH_STATUS);
	flashStatus = EVE_Hal_rd32(phost, REG_FLASH_STATUS);
	if (flashStatus != FLASH_STATUS_DETACHED)
		return flashStatus;
	EVE_CoCmd_d(phost, CMD_FLASHATTACH);
	EVE_Cmd_waitFlush(phost);
	return EVE_Hal_rd32(phost, REG_FLASH_STATUS);
}
uint32_t EVE_CoCmd_flashFast(EVE_HalContext *phost, uint32_t *result)
{
	uint16_t resAddr;
	uint32_t flashStatus;
	if (!EVE_Cmd_waitFlush(phost))
	{
		if (result)
			*result = 0xE000;
		return EVE_Hal_rd32(phost, REG_FLASH_STATUS);
	}
	flashStatus = EVE_Hal_rd32(phost, REG_FLASH_STATUS);
	if (flashStatus < FLASH_STATUS_BASIC)
	{
		if (result)
			*result = 0xE001;
		return flashStatus;
	}
	if (flashStatus > FLASH_STATUS_BASIC)
	{
		if (result)
			*result = 0;
		return flashStatus;
	}
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_STOP);
	EVE_Cmd_wr32(phost, CMD_FLASHFAST);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);
	EVE_Cmd_waitFlush(phost);
	if (result)
		*result = EVE_Hal_rd32(phost, RAM_CMD + resAddr);
	return EVE_Hal_rd32(phost, REG_FLASH_STATUS);
}
bool EVE_CoCmd_loadImage_flash(EVE_HalContext *phost, uint32_t dst, uint32_t src, uint32_t *format)
{
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_FLASHSOURCE);
	EVE_Cmd_wr32(phost, src);
	EVE_Cmd_wr32(phost, CMD_LOADIMAGE);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wr32(phost, OPT_FLASH | OPT_NODL);
	EVE_Cmd_wr32(phost, CMD_NOP);
	EVE_Cmd_endFunc(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	return EVE_CoCmd_getImage_format(phost, format);
}
bool EVE_CoCmd_inflate_flash(EVE_HalContext *phost, uint32_t dst, uint32_t src)
{
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_FLASHSOURCE);
	EVE_Cmd_wr32(phost, src);
	EVE_Cmd_wr32(phost, CMD_INFLATE2);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wr32(phost, OPT_FLASH);
	EVE_Cmd_endFunc(phost);
	return EVE_Cmd_waitFlush(phost);
}
#endif
bool EVE_CoCmd_getImage(EVE_HalContext *phost, uint32_t *source, uint32_t *fmt, uint32_t *w, uint32_t *h, uint32_t *palette)
{
	uint16_t resAddr;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_GETIMAGE);
	resAddr = EVE_Cmd_moveWp(phost, 4 * 5);
	EVE_Cmd_endFunc(phost);
	if (source || fmt || w || h || palette)
	{
		if (!EVE_Cmd_waitFlush(phost))
			return false;
		if (source)
			*source = EVE_Hal_rd32(phost, RAM_CMD + resAddr);
		if (fmt)
			*fmt = EVE_Hal_rd32(phost, RAM_CMD + resAddr + 4);
		if (w)
			*w = EVE_Hal_rd32(phost, RAM_CMD + resAddr + 8);
		if (h)
			*h = EVE_Hal_rd32(phost, RAM_CMD + resAddr + 12);
		if (palette)
			*palette = EVE_Hal_rd32(phost, RAM_CMD + resAddr + 16);
	}
	return true;
}
uint32_t EVE_CoCmd_fsDir(EVE_HalContext *phost, uint32_t dst, uint32_t num, const char *path)
{
	return EVE_CoCmd_fsDir_s(phost, dst, num, path, EVE_CMD_STRING_MAX);
}
uint32_t EVE_CoCmd_fsDir_s(EVE_HalContext *phost, uint32_t dst, uint32_t num, const char *path, uint32_t length)
{
	uint16_t resAddr;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_FSDIR);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wr32(phost, num);
	EVE_Cmd_wrString(phost, path, length);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return 0xFFFFFFFF;
	return EVE_Hal_rd32(phost, RAM_CMD + resAddr);
}
uint32_t EVE_CoCmd_fsDir_ex(EVE_HalContext *phost, uint8_t *dst, uint32_t tmp, uint32_t num, const char *path)
{
	return EVE_CoCmd_fsDir_ex_s(phost, dst, tmp, num, path, EVE_CMD_STRING_MAX);
}
uint32_t EVE_CoCmd_fsDir_ex_s(EVE_HalContext *phost, uint8_t *dst, uint32_t tmp, uint32_t num, const char *path, uint32_t length)
{
	uint32_t result = EVE_CoCmd_fsDir_s(phost, tmp, num, path, length);
	if (result == 0)
	{
		EVE_Hal_rdMem(phost, dst, tmp, num);
	}
	return result;
}
uint32_t EVE_CoCmd_fsRead(EVE_HalContext *phost, uint32_t dst, const char *filename)
{
	return EVE_CoCmd_fsRead_s(phost, dst, filename, EVE_CMD_STRING_MAX);
}
uint32_t EVE_CoCmd_fsRead_s(EVE_HalContext *phost, uint32_t dst, const char *filename, uint32_t length)
{
	uint16_t resAddr;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_FSREAD);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wrString(phost, filename, length);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return 0xFFFFFFFF;
	return EVE_Hal_rd32(phost, RAM_CMD + resAddr);
}
uint32_t EVE_CoCmd_fsSize(EVE_HalContext *phost, const char *filename)
{
	return EVE_CoCmd_fsSize_s(phost, filename, EVE_CMD_STRING_MAX);
}
uint32_t EVE_CoCmd_fsSize_s(EVE_HalContext *phost, const char *filename, uint32_t length)
{
	uint16_t resAddr;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_FSSIZE);
	EVE_Cmd_wrString(phost, filename, length);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return 0xFFFFFFFF;
	return EVE_Hal_rd32(phost, RAM_CMD + resAddr);
}
uint32_t EVE_CoCmd_fsSource(EVE_HalContext *phost, const char *filename)
{
	return EVE_CoCmd_fsSource_s(phost, filename, EVE_CMD_STRING_MAX);
}
uint32_t EVE_CoCmd_fsSource_s(EVE_HalContext *phost, const char *filename, uint32_t length)
{
	uint16_t resAddr;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_FSSOURCE);
	EVE_Cmd_wrString(phost, filename, length);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return 0xFFFFFFFF;
	return EVE_Hal_rd32(phost, RAM_CMD + resAddr);
}
uint32_t EVE_CoCmd_sdAttach(EVE_HalContext *phost, uint32_t options)
{
	uint16_t resAddr;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_SDATTACH);
	EVE_Cmd_wr32(phost, options);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return 0xFFFFFFFF;
	return EVE_Hal_rd32(phost, RAM_CMD + resAddr);
}
uint32_t EVE_CoCmd_sdBlockRead(EVE_HalContext *phost, uint32_t dst, uint32_t src, uint32_t count)
{
	uint16_t resAddr;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_SDBLOCKREAD);
	EVE_Cmd_wr32(phost, dst);
	EVE_Cmd_wr32(phost, src);
	EVE_Cmd_wr32(phost, count);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return 0xFFFFFFFF;
	return EVE_Hal_rd32(phost, RAM_CMD + resAddr);
}
bool EVE_CoCmd_loadImage_sdCard(EVE_HalContext *phost, uint32_t address, char *filename, uint32_t *format)
{
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_LOADIMAGE);
	EVE_Cmd_wr32(phost, address);
	EVE_Cmd_wr32(phost, OPT_FS | OPT_NODL);
	EVE_Cmd_endFunc(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	return true;
}
bool EVE_CoCmd_inflate_sdCard(EVE_HalContext *phost, uint32_t address, char *filename)
{
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_INFLATE2);
	EVE_Cmd_wr32(phost, address);
	EVE_Cmd_wr32(phost, OPT_FS | OPT_NODL);
	EVE_Cmd_endFunc(phost);
	if (!EVE_Cmd_waitFlush(phost))
		return false;
	return true;
}

bool EVE_CoCmd_bitmapTransform(EVE_HalContext *phost, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t tx0, int32_t ty0, int32_t tx1, int32_t ty1, int32_t tx2, int32_t ty2, uint16_t *result)
{
	uint32_t resAddr;
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_BITMAP_TRANSFORM);
	EVE_Cmd_wr32(phost, x0);
	EVE_Cmd_wr32(phost, y0);
	EVE_Cmd_wr32(phost, x1);
	EVE_Cmd_wr32(phost, y1);
	EVE_Cmd_wr32(phost, x2);
	EVE_Cmd_wr32(phost, y2);
	EVE_Cmd_wr32(phost, tx0);
	EVE_Cmd_wr32(phost, ty0);
	EVE_Cmd_wr32(phost, tx1);
	EVE_Cmd_wr32(phost, ty1);
	EVE_Cmd_wr32(phost, tx2);
	EVE_Cmd_wr32(phost, ty2);
	resAddr = EVE_Cmd_moveWp(phost, 4);
	EVE_Cmd_endFunc(phost);
	EVE_DL_STATE.BitmapTransform = true;
	if (result)
	{
		if (!EVE_Cmd_waitFlush(phost))
			return false;
		*result = EVE_Hal_rd16(phost, RAM_CMD + resAddr);
	}
	return true;
}

static uint8_t countArgs(const char *str)
{
	uint8_t count = 0;
	const char *tmp = str;
	while ((tmp = strstr(tmp, "%")))
	{
		if (*(tmp + 1) == '%')
		{
			tmp += 2;
		}
		else
		{
			count++;
			tmp++;
		}
	}
	return count;
}
void EVE_CoCmd_text(EVE_HalContext *phost, int16_t x, int16_t y, int16_t font, uint16_t options, const char *s, ...)
{
	va_list args;
	uint8_t num;
	if (font > BITMAP_HANDLE_MASK)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}
	va_start(args, s);
	num = (options & OPT_FORMAT) ? (countArgs(s)) : (0);
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_TEXT);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
	for (uint8_t i = 0; i < num; i++)
		EVE_Cmd_wr32(phost, (uint32_t)va_arg(args, uint32_t));

	EVE_Cmd_endFunc(phost);
	va_end(args);
	phost->DlPrimitive = 0;
}

void EVE_CoCmd_text_s(EVE_HalContext *phost, int16_t x, int16_t y, int16_t font, uint16_t options, const char *s, uint32_t length)
{
	if (font > BITMAP_HANDLE_MASK)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_TEXT);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wrString(phost, s, length);
	EVE_Cmd_endFunc(phost);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_text_ex(EVE_HalContext *phost, int16_t x, int16_t y, int16_t font, uint16_t options, bool bottom, int16_t baseLine, int16_t capsHeight, int16_t xOffset, const char *s)
{
	int16_t yOffset;
	if (options & OPT_CENTERY)
		yOffset = baseLine - (capsHeight >> 1);
	else if (bottom)
		yOffset = baseLine;
	else
		yOffset = baseLine - capsHeight;
	EVE_CoCmd_text(phost, x - xOffset, y - yOffset, font, options & ~OPT_CENTERY, s);
}
void EVE_CoCmd_button(EVE_HalContext *phost, int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char *s, ...)
{
	va_list args;
	uint8_t num;
	if (font > BITMAP_HANDLE_MASK)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}
	va_start(args, s);
	num = (options & OPT_FORMAT) ? (countArgs(s)) : (0);
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_BUTTON);
	EVE_Cmd_wr32(phost, (((uint32_t)y << 16) | (x & 0xffff)));
	EVE_Cmd_wr32(phost, (((uint32_t)h << 16) | (w & 0xffff)));
	EVE_Cmd_wr32(phost, (((uint32_t)options << 16) | (font & 0xffff)));
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
	for (uint8_t i = 0; i < num; i++)
		EVE_Cmd_wr32(phost, (uint32_t)va_arg(args, uint32_t));

	EVE_Cmd_endFunc(phost);
	va_end(args);
	phost->DlPrimitive = 0;
}

void EVE_CoCmd_keys(EVE_HalContext *phost, int16_t x, int16_t y, int16_t w, int16_t h, int16_t font, uint16_t options, const char *s)
{
	if (font > BITMAP_HANDLE_MASK)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_KEYS);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, w);
	EVE_Cmd_wr16(phost, h);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
	EVE_Cmd_endFunc(phost);
	phost->DlPrimitive = 0;
}
void EVE_CoCmd_toggle(EVE_HalContext *phost, int16_t x, int16_t y, int16_t w, int16_t font, uint16_t options, uint16_t state, const char *s, ...)
{
	va_list args;
	uint8_t num;
	if (font > BITMAP_HANDLE_MASK)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}
	va_start(args, s);
	num = (options & OPT_FORMAT) ? (countArgs(s)) : (0);
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_TOGGLE);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, w);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wr16(phost, state);
	EVE_Cmd_wrString(phost, s, EVE_CMD_STRING_MAX);
	for (uint8_t i = 0; i < num; i++)
		EVE_Cmd_wr32(phost, (uint32_t)va_arg(args, uint32_t));

	EVE_Cmd_endFunc(phost);
	va_end(args);
	phost->DlPrimitive = 0;
}

void EVE_CoCmd_number(EVE_HalContext *phost, int16_t x, int16_t y, int16_t font, uint16_t options, int32_t n)
{
	if (font > BITMAP_HANDLE_MASK)
	{
		if (font != 63)
			eve_printf_debug("Invalid font handle specified: %i\n", (int)font);
		return;
	}
	EVE_Cmd_startFunc(phost);
	EVE_Cmd_wr32(phost, CMD_NUMBER);
	EVE_Cmd_wr16(phost, x);
	EVE_Cmd_wr16(phost, y);
	EVE_Cmd_wr16(phost, font);
	EVE_Cmd_wr16(phost, options);
	EVE_Cmd_wr32(phost, n);
	EVE_Cmd_endFunc(phost);
	phost->DlPrimitive = 0;
}
