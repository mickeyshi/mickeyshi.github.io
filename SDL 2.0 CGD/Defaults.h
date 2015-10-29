#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif
#include <climits>
#include <cfloat>

struct Camera;
class Rect;
class RectF;
class PointF;
class Color;
class Texture;

//DST RECT ONLY
//Defaults_Evaluate will use source rect width
#define DEF_DST_W FLT_MAX
//DST RECT ONLY
//Defaults_Evaluate will use source rect height
#define DEF_DST_H -FLT_MAX
//DST RECT ONLY
//Defaults_Evaluate will set width/height so that the width to height ratio of source rect and dst rect are the same
//Example: src = Rect(0, 0, 100, 200); dst = RectF(0, 0, WRAP_CONTENT, 300)
//	then --> dst = RectF(0, 0, 150, 300)
//Same effect as DEF_DST_W and DEF_DST_H if both width and height are WRAP_CONTENT
#define WRAP_CONTENT -1.0f
//CENTER POINT ONLY
//Defaults_Evaluate will use half of dst rect width
#define DEF_CEN_X -FLT_MAX+1.0f
//CENTER POINT ONLY
//Defaults_Evaluate will use half of dst rect height
#define DEF_CEN_Y -FLT_MAX
//CENTER POINT ONLY
//Defaults_Evaluate will use dst rect width
#define FULL_CEN_X FLT_MAX-1.0f
//CENTER POINT ONLY
//Defaults_Evaluate will use dst rect height
#define FULL_CEN_Y FLT_MAX

//Fixed camera with position rect being RectF(0, 0, RENDER_WIDTH, RENDER_HEIGHT)
extern const Camera &STILL_CAMERA;
//(0, 0, RENDER_WIDTH, RENDER_HEIGHT)
//Is initialized by Defaults_Initialize() which is called by GlobalInitialize()
extern const RectF &DEFAULT_DST_RECT,
//Representation of (0, 0, src_rect width, src_rect height)  when constructing a GameObject
//Value - (0, 0, DEF_DST_W, DEF_DST_H)
//Assumption that you don't actually use this value for dst rect
//DO NOT USE UNLESS THE CLASS/FUNCTION YOU ARE USING IT FOR INTERPRETS IT
	DEFAULT_GAMEOBJECT_DST_RECT;
//Representation of (0, 0, texture width, texture height) when constructing a GameObject
//Value - (INT_MIN, INT_MIN, INT_MIN, INT_MIN)
//Assumption that you don't actually use this value for source rect
//DO NOT USE UNLESS THE CLASS/FUNCTION YOU ARE USING IT FOR INTERPRETS IT
extern const Rect DEFAULT_GAMEOBJECT_SRC_RECT;
//Representation of (dst.w / 2, dst.h / 2) for RectF::Intersects, IObject and GameObject constructor
//Value - (DEF_CEN_X, DEF_CEN_Y)
//Assumption that you don't actually use this value for center point
//DO NOT USE UNLESS THE CLASS/FUNCTION YOU ARE USING IT FOR INTERPRETS IT
extern const PointF DEFAULT_OBJECT_CENTER, POINT_ZERO;
//White
extern const Color DEFAULT_COLOR;
//0
extern const double DEFAULT_ANGLE;
//SDL_FLIP_NONE
extern const SDL_RendererFlip DEFAULT_FLIP;

//Called by GlobalInitialize()
void Defaults_Initialize();
//Called by GlobalUnload()
void Defaults_UnloadContent();

void Defaults_Evaluate(const RectF &dst, PointF &cen);

void Defaults_Evaluate(const Rect &src, RectF &dst);

void Defaults_Evaluate(const Texture &texture, Rect &src);