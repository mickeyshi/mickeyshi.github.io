#pragma once
#include "Rect.h"

struct Camera
{
	RectF position;

	//0, 0, RENDER_WIDTH, RENDER_HEIGHT
	Camera();
	Camera(float x, float y, float w, float h);
	Camera(const Camera &copy);
	~Camera();
	//Center the camera around the given rectangle and offset the x and y afterwards
	void Center(const RectF &rect, float offsetX = 0, float offsetY = 0);
	//Center the camera around either the coordinate thats given and offset the x or y afterwards
	void Center(float xy, bool isHorizontal, float offsetXY = 0);
};
