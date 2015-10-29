#include "Defaults.h"
#include "Global.h"
#include "Color.h"
#include "Rect.h"
#include "Point.h"
#include "Texture.h"
#include "Camera.h"
#include <climits>

Camera stillCamera;
const Camera &STILL_CAMERA = stillCamera;
RectF defaultRect = RectF();
const RectF &DEFAULT_DST_RECT = defaultRect, DEFAULT_GAMEOBJECT_DST_RECT = RectF(0.0f, 0.0f, DEF_DST_W, DEF_DST_H);
const Rect DEFAULT_GAMEOBJECT_SRC_RECT = Rect(INT_MIN, INT_MIN, INT_MIN, INT_MIN);
const PointF DEFAULT_OBJECT_CENTER = PointF(DEF_CEN_X, DEF_CEN_Y), POINT_ZERO = PointF(0, 0);
const Color DEFAULT_COLOR = Color(WHITE);
const double DEFAULT_ANGLE = 0;
const SDL_RendererFlip DEFAULT_FLIP = SDL_FLIP_NONE;

void Defaults_Initialize()
{
	stillCamera.position.Set(0, 0, static_cast<float>(RENDER_WIDTH), static_cast<float>(RENDER_HEIGHT));
	defaultRect.Set(0, 0, static_cast<float>(RENDER_WIDTH), static_cast<float>(RENDER_HEIGHT));
}
void Defaults_UnloadContent()
{
}

void Defaults_Evaluate(const RectF &dst, PointF &cen)
{
	#ifdef ABSTRACT_DEFAULTS
	if (cen.x == DEF_CEN_X)
		cen.x = dst.w / 2.0f;
	else if (cen.x == FULL_CEN_X)
		cen.x = dst.w;
	else if (cen.x == DEF_CEN_Y)
		cen.x = dst.h / 2.0f;
	else if (cen.x == FULL_CEN_Y)
		cen.x = dst.h;
	
	if (cen.y == DEF_CEN_Y)
		cen.y = dst.h / 2.0f;
	else if (cen.y == FULL_CEN_Y)
		cen.y = dst.h;
	else if (cen.y == DEF_CEN_X)
		cen.y = dst.w / 2.0f;
	else if (cen.y == FULL_CEN_X)
		cen.y = dst.w;
	#endif
}

void Defaults_Evaluate(const Rect &src, RectF &dst)
{
	#ifdef ABSTRACT_DEFAULTS
	if (dst.w == WRAP_CONTENT && dst.h == WRAP_CONTENT)
	{
		dst.w = DEF_DST_W;
		dst.h = DEF_DST_H;
	}
	if (dst.w == DEF_DST_W)
		dst.w = static_cast<float>(src.w);
	else if (dst.w == DEF_DST_H)
		dst.w = static_cast<float>(src.h);
	if (dst.h == DEF_DST_H)
		dst.h = static_cast<float>(src.h);
	else if (dst.h == DEF_DST_W)
		dst.h = static_cast<float>(src.w);
	
	if (dst.w == WRAP_CONTENT)
		dst.w = src.w * dst.h / src.h;
	else if (dst.h == WRAP_CONTENT)
		dst.h = src.h * dst.w / src.w;
	#endif
}

void Defaults_Evaluate(const Texture &texture, Rect &src)
{
	#ifdef ABSTRACT_DEFAULTS
	if (src == DEFAULT_GAMEOBJECT_SRC_RECT)
		src.Set(0, 0, texture.GetWidth(), texture.GetHeight());
	#endif
}