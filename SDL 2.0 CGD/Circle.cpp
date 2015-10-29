#include "Circle.h"
#include "Point.h"
#include "Conversion.h"
#include "Independent.h"

CircleF::CircleF(){ Zero(); }
CircleF::CircleF(const CircleF& copy){ Set(copy.x, copy.y, copy.r); }
CircleF::CircleF(float ix, float iy, float ir){ Set(ix, iy, ir); }
CircleF::~CircleF(){  }
CircleF::CircleF(float iArray[]){ Set(iArray[0], iArray[1], iArray[2]); }

void CircleF::Offset(float dx, float dy){ x += dx; y += dy; }
void CircleF::Expand(float dr){ r = Absolute(r + dr); }
void CircleF::Scale(float scale){ scale = abs(scale); r *= scale; }
void CircleF::ScaleAll(float scale){ x *= scale; y *= scale; Scale(scale); }
void CircleF::Zero(){ x = 0; y = 0; r = 0; }
void CircleF::Set(float nx, float ny, float nr){ x = nx; y = ny; r = abs(nr); }
void CircleF::SetCoordinates(float nx, float ny){ x = nx; y = ny; }
void CircleF::SetRadius(float nr){ r = abs(nr); }
void CircleF::Add(float dx, float dy, float dr){ Offset(dx, dy); Scale(dr); }
CircleF *CircleF::operator&(){ return this; }
CircleF &CircleF::operator=(const CircleF &rhs){ if (this != &rhs) { Set(rhs.x, rhs.y, rhs.r); } return *this; }
bool CircleF::operator==(const CircleF &rhs) const{ return x == rhs.x && y == rhs.y && r == rhs.r; }
bool CircleF::operator!=(const CircleF &rhs) const{ return x != rhs.x || y != rhs.y || r != rhs.r; }
bool CircleF::Intersects(const CircleF &collisionCircle, bool edgeIntersection) const
{	
	//Calculate total radius squared
    float totalRadiusSquared = r + collisionCircle.r;
    totalRadiusSquared = totalRadiusSquared * totalRadiusSquared;

    //If the distance between the centers of the circles is less than the sum of their radii
	if (edgeIntersection) {
		if(DistanceSquared(x, y, collisionCircle.x, collisionCircle.y) <= totalRadiusSquared)
		{
			//The circles have collided
			return true;
		}
	}
	else {
		if(DistanceSquared(x, y, collisionCircle.x, collisionCircle.y) < totalRadiusSquared)
		{
			//The circles have collided
			return true;
		}		
	}

    //If not
    return false;
}
bool CircleF::Intersects(const CircleF &collisionCircle, const PointF &offsetCenter, bool edgeIntersection) const
{
	return Intersects(CircleF(collisionCircle.x - offsetCenter.x, collisionCircle.y - offsetCenter.y, collisionCircle.r), edgeIntersection);
}
bool CircleF::Intersects(const PointF &thisCenter, const CircleF &collisionCircle, const PointF &offsetCenter, bool edgeIntersection) const
{
	return CircleF(x - thisCenter.x, y - thisCenter.y, r).Intersects(collisionCircle, offsetCenter, edgeIntersection);
}
std::string CircleF::ToString() const { return "(" + ::ToString(x) + ", " + ::ToString(y) + ", " + ::ToString(r) + ")"; }
std::string CircleF::ToCSV() const { return ::ToString(x) + "," + ::ToString(y) + "," + ::ToString(r); }
