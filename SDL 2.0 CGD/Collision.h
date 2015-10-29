#pragma once

#include "Body.h"
#include "Line.h"
#include <map>

class PhysicsObject;

namespace Phys {

class Collision
{
public:
	PhysicsObject *a, *b;
	Line mEdge;
	Vec2 mNormal;
	Vec2Array* mContacts;
	float mPenetration;
	int mContactIndex;
	bool mCollideA, mCollideB, mResolveA, mResolveB;
	bool mResolveThisFrame;

	//for use during concave polygon collisions
	std::vector<Collision*> mSubCollisions;
	std::vector<bool> mSubCollisionDetected;
	Shape *mShapeA, *mShapeB;
	Collision(const Collision& c, Shape* sa, Shape* sb);
	~Collision();

	Collision(PhysicsObject* a_, PhysicsObject* b_);

	/*This function will find the separating axis based on an array of possible choices.*/
	bool SeparatingAxis(const LineArray& lines1, const LineArray& lines2);

	/*These three functions will handle collision detection for
	all three possible collision cases using separatingAxis()*/
	bool CircleVsCircle();
	bool PolyVsPoly();
	bool PolyVsCircle();

	/*The following three functions will fill contacts
	with the collision points between the two Bodyects*/
	Vec2Array& GetContacts(Circle* circleA);

	Vec2Array& GetContacts(Polygon* polyA, Polygon* polyB);

	Vec2Array& GetContacts(Polygon* poly, Circle* circle);

	/*This function will return true if a collision is possible*/
	bool Possible() const;

	/*This function will choose which collision detection function to call
	if the AABBs do collide*/
	bool Detect();

	//reverse time for the two objects until the penetration is close to zero
	void MinimizePenetration(float dt);

	/*In the event of a collision, this function will handle
	the linear and angular velocities of the two Bodyects*/
	void Resolve();

	/*This function will push the Bodyects out of each other to prevent sinking*/
	void PositionalCorrection();

	/*This is a helper function which will make sure that the normal points
	from a to b*/
	Vec2 CorrectDirection(const Vec2& v) const;

	/*This is a helper function that will fix the direction of rotation*/
	static void FixRadii(Vec2& radA, Vec2& radB, const Vec2& n);

	/*This is a helper function that returns the relative velocity of b.vel - a.vel*/
	Vec2 RelativeVelocity(const Vec2& radA, const Vec2& radB);

	static Collision* MakeCollisionIfNeeded(PhysicsObject* a, PhysicsObject* b);

	enum CGroup {
		PLAYER, PLAYER_BOX, PLATFORMS, SOFT_PLATFORMS, WALL, PILLAR, NPCS, NPCS_INVULNERABLE, ENEMY_PROJECTILES, FRIENDLY_PROJECTILES, SENSORS, SHOOT_ONLY, DEFAULT
	};

	struct CRule {
		std::vector<CGroup> collide;
		std::vector<CGroup> resolve;
        CRule() {}
        CRule(const std::vector<CGroup> &col, const std::vector<CGroup> &res) : collide(col), resolve(res) {}
	};

	static void InitCollisionRules();

private:
	static std::map<CGroup, CRule> mCollisionRules;
};

}
