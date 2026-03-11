/**
 * @file eve_codl.c
 * @brief EVE coprocessor commands implementation
 * @author Bridgetek
 * @copyright MIT License (https://opensource.org/licenses/MIT)
 * @date 2026
 */
#include "eve.h"
#include "eve_bt820.h"

void EVE_CoDlImpl_resetDlState(EVE_HalContext *phost)
{
	phost->DlStateIndex = phost->DlStateBase;
	phost->DlState[phost->DlStateBase] = (EVE_HalDlState){
		.LineWidth = 16,
		.PointSize = 16,
		.ColorRGB = 0xFFFFFF,
		.ScissorWidth = (uint16_t)phost->Width,
		.ScissorHeight = (uint16_t)phost->Height,
		.ColorA = 0xFF,
		.Handle = 0x3F,
		.VertexFormat = 4,
	};
	phost->DlPrimitive = 0;
}
void EVE_CoDlImpl_resetCoState(EVE_HalContext *phost)
{
	phost->CoFgColor = 0x003870;
	phost->CoBgColor = 0x002040;
	phost->CoBitmapTransform = false;
	if (1)
	{
		phost->CoScratchHandle = 15;
	}
}
#define EVE_VERTEX2F_MIN -16384L
#define EVE_VERTEX2F_MAX 16383L
#define EVE_VERTEX2II_MIN 0UL
#define EVE_VERTEX2II_MAX 511UL
void EVE_CoDlImpl_resetDlState(EVE_HalContext *phost);
void EVE_CoDlImpl_resetCoState(EVE_HalContext *phost);
void EVE_CoDl_display(EVE_HalContext *phost)
{
	EVE_CoCmd_dl(phost, DISPLAY());
}
void EVE_CoDl_vertex2f(EVE_HalContext *phost, int16_t x, int16_t y)
{
	EVE_CoCmd_dl(phost, VERTEX2F(x, y));
}
void EVE_CoDl_vertex2ii(EVE_HalContext *phost, uint16_t x, uint16_t y, uint8_t handle, uint8_t cell)
{
	{
		EVE_CoCmd_dl(phost, VERTEX2II(x, y, handle, cell));
	}
}
void EVE_CoDl_bitmapSource(EVE_HalContext *phost, uint32_t addr)
{
	EVE_CoCmd_dl(phost, BITMAP_SOURCE_H(addr >> 24));
	EVE_CoCmd_dl(phost, BITMAP_SOURCE(addr));
}
void EVE_CoDl_bitmapSource_ex(EVE_HalContext *phost, uint32_t addr, bool flash)
{
	if (!flash)
		EVE_CoDl_bitmapSource(phost, addr);
}
void EVE_CoDl_clearColorRgb_ex(EVE_HalContext *phost, uint32_t c)
{
	EVE_CoCmd_dl(phost, CLEAR_COLOR_RGB(0, 0, 0) | (c & 0xFFFFFF));
}
void EVE_CoDl_clearColorRgb(EVE_HalContext *phost, uint8_t r, uint8_t g, uint8_t b)
{
	EVE_CoCmd_dl(phost, CLEAR_COLOR_RGB(r, g, b));
}
void EVE_CoDl_clearColorA(EVE_HalContext *phost, uint8_t alpha)
{
	EVE_CoCmd_dl(phost, CLEAR_COLOR_A(alpha));
}
void EVE_CoDl_clearColorArgb_ex(EVE_HalContext *phost, uint32_t c)
{
	EVE_CoDl_clearColorRgb_ex(phost, c);
	EVE_CoDl_clearColorA(phost, c >> 24);
}
void EVE_CoDl_tag(EVE_HalContext *phost, uint32_t s)
{
	EVE_CoCmd_dl(phost, TAG(s));
}
void EVE_CoDl_colorRgb_ex(EVE_HalContext *phost, uint32_t c)
{
	uint32_t rgb = c & 0xFFFFFF;
	if (rgb != EVE_DL_STATE.ColorRGB)
	{
		EVE_CoCmd_dl(phost, COLOR_RGB(0, 0, 0) | (c & 0xFFFFFF));
		EVE_DL_STATE.ColorRGB = rgb;
	}
}
void EVE_CoDl_colorRgb(EVE_HalContext *phost, uint8_t r, uint8_t g, uint8_t b)
{
	uint32_t rgb = ((uint32_t)r << 16) | ((uint32_t)g << 8) | ((uint32_t)b);
	EVE_CoDl_colorRgb_ex(phost, rgb);
}
void EVE_CoDl_colorA(EVE_HalContext *phost, uint8_t alpha)
{
	if (alpha != EVE_DL_STATE.ColorA)
	{
		EVE_CoCmd_dl(phost, COLOR_A(alpha));
		EVE_DL_STATE.ColorA = alpha;
	}
}
void EVE_CoDl_colorArgb_ex(EVE_HalContext *phost, uint32_t c)
{
	EVE_CoDl_colorRgb_ex(phost, c);
	EVE_CoDl_colorA(phost, c >> 24);
}
void EVE_CoDl_bitmapHandle(EVE_HalContext *phost, uint8_t handle)
{
	if (handle != EVE_DL_STATE.Handle)
	{
		EVE_CoCmd_dl(phost, BITMAP_HANDLE(handle));
		EVE_DL_STATE.Handle = handle;
	}
}
void EVE_CoDl_cell(EVE_HalContext *phost, uint8_t cell)
{
	if (cell != EVE_DL_STATE.Cell)
	{
		EVE_CoCmd_dl(phost, CELL(cell));
		EVE_DL_STATE.Cell = cell;
	}
}
void EVE_CoDl_bitmapLayout(EVE_HalContext *phost, uint8_t format, uint16_t linestride, uint16_t height)
{
	EVE_CoCmd_dl(phost, BITMAP_LAYOUT_H(linestride >> 10, height >> 9));
	EVE_CoCmd_dl(phost, BITMAP_LAYOUT(format, linestride, height));
}
void EVE_CoDl_bitmapSize(EVE_HalContext *phost, uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height)
{
	if(width > 511 || height > 511) 
		EVE_CoCmd_dl(phost, BITMAP_SIZE_H(width >> 9, height >> 9));
	EVE_CoCmd_dl(phost, BITMAP_SIZE(filter, wrapx, wrapy, min(511, width), min(511, height)));
}
void EVE_CoDl_alphaFunc(EVE_HalContext *phost, uint8_t func, uint8_t ref)
{
	EVE_CoCmd_dl(phost, ALPHA_FUNC(func, ref));
}
void EVE_CoDl_stencilFunc(EVE_HalContext *phost, uint8_t func, uint8_t ref, uint8_t mask)
{
	EVE_CoCmd_dl(phost, STENCIL_FUNC(func, ref, mask));
}
void EVE_CoDl_blendFunc(EVE_HalContext *phost, uint8_t src, uint8_t dst)
{
	EVE_CoCmd_dl(phost, BLEND_FUNC(src, dst));
}
void EVE_CoDl_blendFunc_default(EVE_HalContext *phost)
{
	EVE_CoDl_blendFunc(phost, SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
}
void EVE_CoDl_stencilOp(EVE_HalContext *phost, uint8_t sfail, uint8_t spass)
{
	EVE_CoCmd_dl(phost, STENCIL_OP(sfail, spass));
}
void EVE_CoDl_pointSize(EVE_HalContext *phost, int16_t size)
{
	if (size != EVE_DL_STATE.PointSize)
	{
		EVE_CoCmd_dl(phost, POINT_SIZE(size));
		EVE_DL_STATE.PointSize = size;
	}
}
void EVE_CoDl_lineWidth(EVE_HalContext *phost, int16_t width)
{
	if (width != EVE_DL_STATE.LineWidth)
	{
		EVE_CoCmd_dl(phost, LINE_WIDTH(width));
		EVE_DL_STATE.LineWidth = width;
	}
}
void EVE_CoDl_clearStencil(EVE_HalContext *phost, uint8_t s)
{
	EVE_CoCmd_dl(phost, CLEAR_STENCIL(s));
}
void EVE_CoDl_clearTag(EVE_HalContext *phost, uint32_t s)
{
	EVE_CoCmd_dl(phost, CLEAR_TAG(s));
}
void EVE_CoDl_stencilMask(EVE_HalContext *phost, uint8_t mask)
{
	EVE_CoCmd_dl(phost, STENCIL_MASK(mask));
}
void EVE_CoDl_tagMask(EVE_HalContext *phost, bool mask)
{
	EVE_CoCmd_dl(phost, TAG_MASK(mask));
}
void EVE_CoDl_bitmapTransformA(EVE_HalContext *phost, uint16_t v)
{
	EVE_CoCmd_dl(phost, BITMAP_TRANSFORM_A(v));
	EVE_DL_STATE.BitmapTransform = true;
}
void EVE_CoDl_bitmapTransformA_ex(EVE_HalContext *phost, bool p, uint16_t v)
{
	EVE_CoCmd_dl(phost, BITMAP_TRANSFORM_A_EXT(p, v));
	EVE_DL_STATE.BitmapTransform = true;
}
void EVE_CoDl_bitmapTransformB(EVE_HalContext *phost, uint16_t v)
{
	EVE_CoCmd_dl(phost, BITMAP_TRANSFORM_B(v));
	EVE_DL_STATE.BitmapTransform = true;
}
void EVE_CoDl_bitmapTransformB_ex(EVE_HalContext *phost, bool p, uint16_t v)
{
	EVE_CoCmd_dl(phost, BITMAP_TRANSFORM_B_EXT(p, v));
	EVE_DL_STATE.BitmapTransform = true;
}
void EVE_CoDl_bitmapTransformC(EVE_HalContext *phost, uint32_t v)
{
	EVE_CoCmd_dl(phost, BITMAP_TRANSFORM_C(v));
	EVE_DL_STATE.BitmapTransform = true;
}
void EVE_CoDl_bitmapTransformD(EVE_HalContext *phost, uint16_t v)
{
	EVE_CoCmd_dl(phost, BITMAP_TRANSFORM_D(v));
	EVE_DL_STATE.BitmapTransform = true;
}
void EVE_CoDl_bitmapTransformD_ex(EVE_HalContext *phost, bool p, uint16_t v)
{
	EVE_CoCmd_dl(phost, BITMAP_TRANSFORM_D_EXT(p, v));
	EVE_DL_STATE.BitmapTransform = true;
}
void EVE_CoDl_bitmapTransformE(EVE_HalContext *phost, uint16_t v)
{
	EVE_CoCmd_dl(phost, BITMAP_TRANSFORM_E(v));
	EVE_DL_STATE.BitmapTransform = true;
}
void EVE_CoDl_bitmapTransformE_ex(EVE_HalContext *phost, bool p, uint16_t v)
{
	EVE_CoCmd_dl(phost, BITMAP_TRANSFORM_E_EXT(p, v));
	EVE_DL_STATE.BitmapTransform = true;
}
void EVE_CoDl_bitmapTransformF(EVE_HalContext *phost, uint32_t v)
{
	EVE_CoCmd_dl(phost, BITMAP_TRANSFORM_F(v));
	EVE_DL_STATE.BitmapTransform = true;
}
void EVE_CoDl_bitmapTransform_ex(EVE_HalContext *phost, bool p, uint16_t a, uint16_t b, uint32_t c, uint16_t d, uint16_t e, uint32_t f)
{
	EVE_CoDl_bitmapTransformA_ex(phost, p, a);
	EVE_CoDl_bitmapTransformB_ex(phost, p, b);
	EVE_CoDl_bitmapTransformC(phost, c);
	EVE_CoDl_bitmapTransformD_ex(phost, p, d);
	EVE_CoDl_bitmapTransformE_ex(phost, p, e);
	EVE_CoDl_bitmapTransformF(phost, f);
	EVE_DL_STATE.BitmapTransform = true;
}
void EVE_CoDl_bitmapTransform_identity(EVE_HalContext *phost)
{
	if (EVE_DL_STATE.BitmapTransform)
	{
		EVE_CoDl_bitmapTransformA_ex(phost, 0, 256);
		EVE_CoDl_bitmapTransformB_ex(phost, 0, 0);
		EVE_CoDl_bitmapTransformC(phost, 0);
		EVE_CoDl_bitmapTransformD_ex(phost, 0, 0);
		EVE_CoDl_bitmapTransformE_ex(phost, 0, 256);
		EVE_CoDl_bitmapTransformF(phost, 0);
		EVE_DL_STATE.BitmapTransform = false;
	}
}
void EVE_CoDl_scissorXY(EVE_HalContext *phost, uint16_t x, uint16_t y)
{
	if (EVE_DL_STATE.ScissorX != x || EVE_DL_STATE.ScissorY != y)
	{
		EVE_CoCmd_dl(phost, SCISSOR_XY(x, y));
		EVE_DL_STATE.ScissorX = x;
		EVE_DL_STATE.ScissorY = y;
	}
}
void EVE_CoDl_scissorSize(EVE_HalContext *phost, uint16_t width, uint16_t height)
{
	if (EVE_DL_STATE.ScissorWidth != width || EVE_DL_STATE.ScissorHeight != height)
	{
		EVE_CoCmd_dl(phost, SCISSOR_SIZE(width, height));
		EVE_DL_STATE.ScissorWidth = width;
		EVE_DL_STATE.ScissorHeight = height;
	}
}
void EVE_CoDl_call(EVE_HalContext *phost, uint16_t dest)
{
	EVE_CoCmd_dl(phost, CALL(dest));
}
void EVE_CoDl_jump(EVE_HalContext *phost, uint16_t dest)
{
	EVE_CoCmd_dl(phost, JUMP(dest));
}
void EVE_CoDl_begin(EVE_HalContext *phost, uint8_t prim)
{
	uint8_t oldPrim = phost->DlPrimitive;
	switch (oldPrim)
	{
	case LINE_STRIP:
	case EDGE_STRIP_R:
	case EDGE_STRIP_L:
	case EDGE_STRIP_A:
	case EDGE_STRIP_B:
		oldPrim = 0;
		break;
	default:
		break;
	}
	if (prim != oldPrim)
	{
		EVE_CoCmd_dl(phost, BEGIN(prim));
		phost->DlPrimitive = prim;
	}
}
void EVE_CoDl_colorMask(EVE_HalContext *phost, bool r, bool g, bool b, bool a)
{
	EVE_CoCmd_dl(phost, COLOR_MASK(r, g, b, a));
}
void EVE_CoDl_end(EVE_HalContext *phost)
{
}
void EVE_CoDl_saveContext(EVE_HalContext *phost)
{
	EVE_CoCmd_saveContext(phost);
}
void EVE_CoDl_restoreContext(EVE_HalContext *phost)
{
	EVE_CoCmd_restoreContext(phost);
}
void EVE_CoDl_return(EVE_HalContext *phost)
{
	EVE_CoCmd_dl(phost, RETURN());
}
void EVE_CoDl_macro(EVE_HalContext *phost, uint16_t m)
{
	EVE_CoCmd_dl(phost, MACRO(m));
}
void EVE_CoDl_clear(EVE_HalContext *phost, bool c, bool s, bool t)
{
	EVE_CoCmd_dl(phost, CLEAR(c, s, t));
}
void EVE_CoDl_vertexFormat(EVE_HalContext *phost, uint8_t frac)
{
	if (frac != EVE_DL_STATE.VertexFormat)
	{
		EVE_CoCmd_dl(phost, VERTEX_FORMAT(frac));
		EVE_DL_STATE.VertexFormat = frac;
	}
}
void EVE_CoDl_paletteSource(EVE_HalContext *phost, uint32_t addr)
{
	if (addr != EVE_DL_STATE.PaletteSource)
	{
		EVE_CoCmd_dl(phost, PALETTE_SOURCE_H(addr >> 24));
		EVE_CoCmd_dl(phost, PALETTE_SOURCE(addr));
		EVE_DL_STATE.PaletteSource = addr;
	}
}
void EVE_CoDl_vertexTranslateX(EVE_HalContext *phost, int16_t x)
{
	EVE_CoCmd_dl(phost, VERTEX_TRANSLATE_X(x));
}
void EVE_CoDl_vertexTranslateY(EVE_HalContext *phost, int16_t y)
{
	EVE_CoCmd_dl(phost, VERTEX_TRANSLATE_Y(y));
}
void EVE_CoDl_nop(EVE_HalContext *phost)
{
	EVE_CoCmd_dl(phost, NOP());
}
void EVE_CoDl_bitmapExtFormat(EVE_HalContext *phost, uint16_t format)
{
	EVE_CoCmd_dl(phost, BITMAP_EXT_FORMAT(format));
}
void EVE_CoDl_bitmapSwizzle(EVE_HalContext *phost, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	EVE_CoCmd_dl(phost, BITMAP_SWIZZLE(r, g, b, a));
}
void EVE_CoDl_vertex2f_4(EVE_HalContext *phost, int16_t x, int16_t y)
{
	if (x > EVE_VERTEX2F_MAX || y > EVE_VERTEX2F_MAX)
	{
		EVE_CoDl_vertexFormat(phost, 3);
		EVE_CoDl_vertex2f(phost, x >> 1, y >> 1);
	}
	else
	{
		EVE_CoDl_vertexFormat(phost, 4);
		EVE_CoDl_vertex2f(phost, x, y);
	}
}
void EVE_CoDl_vertex2f_2(EVE_HalContext *phost, int16_t x, int16_t y)
{
	EVE_CoDl_vertexFormat(phost, 2);
	EVE_CoDl_vertex2f(phost, x, y);
}
void EVE_CoDl_vertex2f_0(EVE_HalContext *phost, int16_t x, int16_t y)
{
	EVE_CoDl_vertexFormat(phost, 0);
	EVE_CoDl_vertex2f(phost, x, y);
}
void EVE_CoDl_region(EVE_HalContext *phost, uint16_t y, uint16_t h, uint16_t dest)
{
	EVE_CoCmd_dl(phost, REGION(y, h, dest));
}
