#pragma once
#include <string>

class PointF;

//Radius will always be positive
class CircleF
{
public:
	CircleF();
	CircleF(const CircleF& copy);
	CircleF(float ix, float iy, float ir);
	~CircleF();
	//Pass in an array of integers and the first 3 will be used as the x, y, r respectively
	CircleF(float iArray[]);

	//Add dx, dy to x, y
	void Offset(float dx, float dy);
	//Add dr to r (result will be the absolute value)
	void Expand(float dr);
	//Sets r to product of r with scale
	void Scale(float scale);
	//Sets x, y, r to product of x, y, r with scale
	void ScaleAll(float scale);
	//Set x, y, r = 0
	void Zero();
	//Set x, y, r to nx, ny, nr
	void Set(float nx, float ny, float nr);
	//Set x, y to nx, ny
	void SetCoordinates(float nx, float ny);
	//Set r to nr
	void SetRadius(float nr);
	//Add dx, dy, dr to x, y, r
	void Add(float dx, float dy, float dr);
	CircleF *operator&();
	CircleF &operator=(const CircleF &rhs);
	bool operator==(const CircleF &rhs) const;
	bool operator!=(const CircleF &rhs) const;
	//Checks to see if this CircleF intersects with collisionCircle
	//edgeIntersection - if false then Intersects will ignore intersection of edges
	bool Intersects(const CircleF &collisionCircle, bool edgeIntersection = false) const;
	//Checks to see if this CircleF intersects with collisionCircle after offsetting from the center
	//edgeIntersection - if false then Intersects will ignore intersection of edges
	bool Intersects(const CircleF &collisionCircle, const PointF &offsetCenter, bool edgeIntersection = false) const;
	//Checks to see if this CircleF after offsetting intersects with collisionCircle after offsetting from the center
	//edgeIntersection - if false then Intersects will ignore intersection of edges
	bool Intersects(const PointF &thisCenter, const CircleF &collisionCircle, const PointF &offsetCenter, bool edgeIntersection = false) const;
	std::string ToString() const;
	std::string ToCSV() const;

	float x, y, r;
};

