#include "Camera.h"
#include "Global.h"

Camera::Camera()
	: position(0, 0, RENDER_WIDTH, RENDER_HEIGHT)
{
}

Camera::Camera(float x, float y, float w, float h)
	: position(x, y, w, h)
{
}

Camera::Camera(const Camera &copy)
	: position(copy.position)
{
}

Camera::~Camera()
{
}

void Camera::Center(const RectF &rect, float offsetX, float offsetY)
{
	position.SetCoordinates(rect.x - position.w / 2 + offsetX, rect.y - position.h / 2 + offsetY);
}

void Camera::Center(float xy, bool isHorizontal, float offsetXY)
{
	if (isHorizontal)
		position.x = xy - position.w / 2 + offsetXY;
    else
		position.y = xy - position.h / 2 + offsetXY;
}
