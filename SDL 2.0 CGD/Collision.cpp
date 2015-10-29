#include "Collision.h"
#include "Helper_Methods.h"
#include "PhysicsObject.h"
#include <limits>
#include <algorithm>

using namespace Phys;

static const float percent = 0.8f;
static const float slop = 0.03f;

std::map<Collision::CGroup, Collision::CRule> Collision::mCollisionRules;

Collision::Collision(PhysicsObject* a_, PhysicsObject* b_)
	: a(a_), b(b_), mCollideA(false), mCollideB(false), mResolveA(false), mResolveB(false)
{
	mResolveThisFrame = false;
	mPenetration = 0;
	mContacts = NULL;	// new Vec2[0];
	mContactIndex = -1;

	mShapeA = a->GetShape();
	mShapeB = b->GetShape();

	auto groupA = a->COLLISION_GROUP;
	auto groupB = b->COLLISION_GROUP;
	auto& ruleA = mCollisionRules[groupA];
	auto& ruleB = mCollisionRules[groupB];
	if (std::find(ruleA.collide.begin(), ruleA.collide.end(), groupB) != ruleA.collide.end())
		mCollideA = true;
	if (std::find(ruleB.collide.begin(), ruleB.collide.end(), groupA) != ruleB.collide.end())
		mCollideB = true;
	if (std::find(ruleA.resolve.begin(), ruleA.resolve.end(), groupB) != ruleA.resolve.end())
		mResolveA = true;
	if (std::find(ruleB.resolve.begin(), ruleB.resolve.end(), groupA) != ruleB.resolve.end())
		mResolveB = true;

	//add sub collisions if needed for concave polygons
	ConcavePolygon *cpa = nullptr, *cpb = nullptr;
	if (a->GetShape()->TYPE == 2)
		cpa = static_cast<ConcavePolygon*>(a->GetShape());
	if (b->GetShape()->TYPE == 2)
		cpb = static_cast<ConcavePolygon*>(b->GetShape());

	if (cpa != nullptr && cpb != nullptr) {
		auto shapesA = cpa->GetSubShapes();
		auto shapesB = cpb->GetSubShapes();
		for (unsigned int i = 0; i < shapesA.size(); i++) {
			for (unsigned int j = 0; i < shapesB.size(); j++) {
				mSubCollisions.push_back(new Collision(*this, shapesA[i], shapesB[j]));
			}
		}
	}
	else if (cpa != nullptr) {
		auto shapesA = cpa->GetSubShapes();
		for (unsigned int i = 0; i < shapesA.size(); i++) {
			mSubCollisions.push_back(new Collision(*this, shapesA[i], b->GetShape()));
		}
	}
	else if (cpb != nullptr) {
		auto shapesB = cpb->GetSubShapes();
		for (unsigned int i = 0; i < shapesB.size(); i++) {
			mSubCollisions.push_back(new Collision(*this, a->GetShape(), shapesB[i]));
		}
	}

	mSubCollisionDetected.resize(mSubCollisions.size());
}

Collision::Collision(const Collision& c, Shape* sa, Shape* sb)
	: a(c.a), b(c.b), mCollideA(c.mCollideA), mCollideB(c.mCollideB), mResolveA(c.mResolveA), mResolveB(c.mResolveB), mShapeA(sa), mShapeB(sb)
{
	mResolveThisFrame = false;
	mPenetration = 0;
	mContacts = NULL;	// new Vec2[0];
	mContactIndex = -1;
}

Collision::~Collision()
{
	for (auto c : mSubCollisions)
		delete c;
}

/*This function will find the separating axis based on an array of possible choices.*/
bool Collision::SeparatingAxis(const LineArray& lines1, const LineArray& lines2)
{
	//treat lines(n) as if it were lines1 and lines2 joined into one array
	auto lines = [&lines1, &lines2](unsigned int n) {
		return (n < lines1.size() ? lines1[n] : lines2[n - lines1.size()]);
	};

	float overlap = std::numeric_limits<float>::max();
	Projection proj1, proj2;
	int linesSize = lines1.size() + lines2.size();
	for (int i = 0; i < linesSize; i++)
	{
		Vec2 axis = lines(i).normal;
		proj1 = mShapeA->ProjectOnto(axis);
		proj2 = mShapeB->ProjectOnto(axis);
		float over = proj1.GetOverlap(proj2);
		if (over < 0)
		{
			return false;
		}
		else if (over < overlap)
		{
			overlap = over;
			mContactIndex = i;
		}
	}
	mEdge = lines(mContactIndex);
	mPenetration = overlap;
	mNormal = CorrectDirection(mEdge.normal);
	return true;
}

/*These three functions will handle collision detection for
all three possible collision cases using separatingAxis()*/
bool Collision::CircleVsCircle()
{
	Circle* circleA = static_cast<Circle*>(mShapeA);
	Circle* circleB = static_cast<Circle*>(mShapeB);
	Vec2 n = b->mPosition.Minus(a->mPosition);
	float overlap = circleA->RADIUS + circleB->RADIUS - n.Magnitude();
	if (overlap < 0)
	{
		return false;
	}
	mNormal = n.Unit();
	mContacts = &GetContacts(circleA);
	mPenetration = overlap;
	return true;
}

bool Collision::PolyVsPoly()
{
	Polygon* polyA = static_cast<Polygon*>(mShapeA);
	Polygon* polyB = static_cast<Polygon*>(mShapeB);

	if (SeparatingAxis(polyA->Edges(), polyB->Edges()))
	{
		mContacts = &GetContacts(polyA, polyB);
		return true;
	}

	return false;
}

bool Collision::PolyVsCircle()
{
	int typeA = a->GetShape()->TYPE;
	Polygon* poly = (Polygon*)((typeA == 0) ? mShapeB : mShapeA);
	Circle* circle = (Circle*)((typeA == 0) ? mShapeA : mShapeB);
	Vec2 center = (typeA == 0) ? a->mPosition : b->mPosition;
	Vec2 lastAxis = poly->ClosestVert(center).Minus(center).Unit();

	static LineArray lastAxisLineArr(1);
	lastAxisLineArr[0] = Line(lastAxis);

	if (SeparatingAxis(poly->Edges(), lastAxisLineArr))
	{
		mContacts = &GetContacts(poly, circle);
		return true;
	}
	return false;
}

/*The following three functions will fill contacts
with the collision points between the two Bodyects*/
Vec2Array& Collision::GetContacts(Phys::Circle* circleA)
{
	Vec2 point = a->mPosition.Plus(mNormal.Times(circleA->RADIUS));
	static Vec2Array c(1);
	c[0] = point;
	return c;
}

Vec2Array& Collision::GetContacts(Polygon* polyA, Polygon* polyB)
{
	Vec2Array const *vertices;
	if (mContactIndex < static_cast<int>(polyA->mMap.size()))
	{
		vertices = &polyB->Vertices();
	}
	else
	{
		vertices = &polyA->Vertices();
	}
	return mEdge.ClosestPoints(*vertices);
}

Vec2Array& Collision::GetContacts(Polygon* poly, Phys::Circle* circle)
{
	int typeA = a->GetShape()->TYPE;
	//Transform t = (typeA == 0) ? b->GetTransform() : a->GetTransform();
	Vec2 center = (typeA == 0) ? a->mPosition : b->mPosition;
	Vec2 nrm = (typeA == 0) ? mNormal.Anti() : mNormal;
	static Vec2Array c(1);
	if (mContactIndex < static_cast<int>(poly->mMap.size()))
	{
		c[0] = center.Minus(nrm.Times(circle->RADIUS));
	}
	else
	{
		c[0] = poly->ClosestVert(center);
	}
	return c;
}

/*This function will return true if a collision is possible*/
bool Collision::Possible() const
{
	if (a->IsNotAlive() || b->IsNotAlive())
		return false;

	//if we only want AABB detection, do the actual box overlap detection in Detect()
	if (a->DetectAABB() || b->DetectAABB())
		return true;

	const AABB& hitboxA = mShapeA->GetAABB();
	const AABB& hitboxB = mShapeB->GetAABB();
	return hitboxA.Overlaps(hitboxB);
}

/*This function will choose which collision detection function to call
if the AABBs do collide*/
bool Collision::Detect()
{
	//if we only want AABB detection:
	if (a->DetectAABB() || b->DetectAABB()) {
		const AABB& hitboxA = mShapeA->GetAABB();
		const AABB& hitboxB = mShapeB->GetAABB();
		return hitboxA.Overlaps(hitboxB);
	}

	int typeA = mShapeA->TYPE;
	int typeB = mShapeB->TYPE;
	if (typeA == 1 && typeB == 1) {
		return PolyVsPoly();
	}
	else if (typeA + typeB == 1) {
		return PolyVsCircle();
	}
	else if (typeA == 0 && typeB == 0) {
		return CircleVsCircle();
	}
	else if (mSubCollisions.size() > 0) {
		//do sub collisions
		bool detected = false;
		for (unsigned int i = 0; i < mSubCollisions.size(); i++) {
			Collision* c = mSubCollisions[i];
			if (c->Possible() && c->Detect()) {
				detected = true;
				mSubCollisionDetected[i] = true;
			}
			else  {
				mSubCollisionDetected[i] = false;
			}
		}
		return detected;
	}
	return false;
}

void Collision::MinimizePenetration(float dt)
{
	static const int MAX_STEPS = 10;
	static const float PENETRATION_THRESHOLD = 0.1f;
	static const float RV_THRESHOLD = 50.f;

	//don't do this if the relative velocity of both objects in the direction of the normal is too low
	Vec2 rv = RelativeVelocity(Vec2(0, 0), Vec2(0, 0));
	if (-rv.Dot(mNormal) < RV_THRESHOLD)	//if rv dot normal is negative, the objects are moving towards each other
		return;

	Vec2 origPosA = a->mPosition;
	Vec2 origPosB = b->mPosition;
	//float origOrientA = a->mOrient;
	//float origOrientB = b->mOrient;

	//the time at which penetration is smallest (but above 0)
	float time = 0.f;
	float minPenetration = mPenetration;

	for (int nSteps = 0; nSteps < MAX_STEPS && mPenetration > PENETRATION_THRESHOLD; nSteps++) {
		//do a binary search for the point in time where penetration is 0
		float newTime = time - dt / static_cast<float>(2 << nSteps);
		if (mResolveA) {
			//a->mPosition = origPosA.Plus(mNormal.Times(mNormal.Dot(a->mVelocity.Times(newTime))));
			a->mPosition = origPosA.Plus(a->mVelocity.Times(newTime));
			//a->mOrient = origOrientA + a->mAngVelocity * newTime;
			mShapeA->RecalcParams(a->GetTransform());
		}
		if (mResolveB) {
			//b->mPosition = origPosB.Plus(mNormal.Times(mNormal.Dot(b->mVelocity.Times(newTime))));
			b->mPosition = origPosB.Plus(b->mVelocity.Times(newTime));
			//b->mOrient = origOrientB + b->mAngVelocity * newTime;
			mShapeB->RecalcParams(b->GetTransform());
		}

		if (Detect() && mPenetration < minPenetration) {
			minPenetration = mPenetration;
			time = newTime;
		}
	}
}

/*In the event of a collision, this function will handle
the linear and angular velocities of the two bodies*/
void Collision::Resolve()
{
	//if we have sub collisions, resolve through them instead
	if (mSubCollisions.size() > 0) {
		for (unsigned int i = 0; i < mSubCollisions.size(); i++) {
			Collision* c = mSubCollisions[i];
			if (mSubCollisionDetected[i])
				c->Resolve();
		}
		return;
	}

	//reference: http://chrishecker.com/images/e/e7/Gdmphys3.pdf

	float e = std::min(a->Restitution(), b->Restitution());
	float sf = std::min(a->StaticFriction(), b->StaticFriction());
	float df = std::min(a->DynamicFriction(), b->DynamicFriction());

	int numContacts = mContacts->size();

	for (int i = 0; i < numContacts; i++)
	{
		Vec2 radA = (*mContacts)[i].Minus(a->mPosition);
		Vec2 radB = (*mContacts)[i].Minus(b->mPosition);

		FixRadii(radA, radB, mNormal);

		Vec2 rv = RelativeVelocity(radA, radB);

		float nVel = rv.Dot(mNormal);
		if (nVel <= 0)
		{
			//calculate impulse
			float invMassSum = (mResolveA ? a->mInvMass : 0.f) + (mResolveB ? b->mInvMass : 0.f) +
				(mResolveA ? a->mInvIn : 0.f)*Square(radA.Cross(mNormal)) + (mResolveB ? b->mInvIn : 0.f)*Square(radB.Cross(mNormal));
			float j = -(1 + e)*nVel / invMassSum / float(numContacts);
			Vec2 impulse = mNormal.Times(j);

			//calcuate tangent impulse
			Vec2 tangent = rv.Minus(mNormal.Times(rv.Dot(mNormal))).Unit();
			float jt = -tangent.Dot(rv) / invMassSum / float(numContacts);
			Vec2 tangentImpulse = (abs(jt) < j * sf) ? tangent.Times(jt) : tangent.Times(-j * df);

			if (mResolveA) {
				Vec2 jTotal = impulse.Anti().Plus(tangentImpulse.Anti());
				a->ApplyImpulse(jTotal, radA);
			}
			if (mResolveB) {
				Vec2 jTotal = impulse.Plus(tangentImpulse);
				b->ApplyImpulse(jTotal, radB);
			}
		}
	}

	//velocities should be recalculated for any subsequent collisions
	a->FlushImpulse();
	b->FlushImpulse();
}

/*This function will push the Bodyects out of each other to prevent sinking*/
void Collision::PositionalCorrection()
{
	//sub collisions
	if (mSubCollisions.size() > 0) {
		for (unsigned int i = 0; i < mSubCollisions.size(); i++)
			if (mSubCollisionDetected[i])
				mSubCollisions[i]->PositionalCorrection();
		return;
	}

	Vec2 correction = mNormal.Times(std::max(mPenetration - slop, 0.0f) / (a->mInvMass + b->mInvMass)*percent);

	if (mResolveA)
		a->Displace(correction.Times(-a->mInvMass));
	if (mResolveB)
		b->Displace(correction.Times(b->mInvMass));
}

/*This is a helper function which will make sure that the normal points
from a to b*/
Vec2 Collision::CorrectDirection(const Vec2& v) const
{
	if (b->mPosition.Minus(a->mPosition).Dot(v) < 0)
	{
		return v.Anti();
	}
	else return v;
}

/*This is a helper function that will fix the direction of rotation*/
void Collision::FixRadii(Vec2& radA, Vec2& radB, const Vec2& n)
{
	if (n.Dot(radA) < 0)
	{
		radA.Negate();
	}
	if (n.Dot(radB) > 0)
	{
		radB.Negate();
	}
}

/*This is a helper function that returns the relative velocity of b->vel - a->vel*/
Vec2 Collision::RelativeVelocity(const Vec2& radA, const Vec2& radB)
{
	return b->CombinedVelocity(radB).Minus(a->CombinedVelocity(radA));
}

Collision* Collision::MakeCollisionIfNeeded(PhysicsObject* a, PhysicsObject* b)
{
	auto groupA = a->COLLISION_GROUP;
	auto groupB = b->COLLISION_GROUP;
	auto& ruleA = mCollisionRules[groupA];
	auto& ruleB = mCollisionRules[groupB];

	if (std::find(ruleA.collide.begin(), ruleA.collide.end(), groupB) != ruleA.collide.end() || (std::find(ruleB.collide.begin(), ruleB.collide.end(), groupA) != ruleB.collide.end()))
		return new Collision(a, b);
	else
		return nullptr;
}

void Collision::InitCollisionRules()
{
	static bool initialized = false;
	if (initialized)
		return;

	/******************************************************************************************************************
										  Collision groups and rules explained:
	
	Collision detection occurs in two separate stages: detection and resolution. During detection, the physics engine 
	simply checks to see if two objects are touching each other, while during resolution, the physics engine decides
	how the two objects will interact with or bounce off each other.

	A single collision rule consists of three components: the group the rule applies to (call this group G for now), a 
	list of groups that will collide with the current group (call this list C), and a list of groups that will resolve 
	with the current group (R). R should be a subset of C.

	First, the collision detection stage happens. If an object in G collides with an object in any of the groups in C,
	a collision will be detected, and the object in G will be notified of the collision via PhysicsObject::Collided.
	However, note that the object in C doesn't necessarily have to have group G in its C group in order for a collision
	to be detected. For example, PLAYER's C group includes PLATFORMS, but PLATFORM's C group doesn't include PLAYER.
	This means that the player will be notified of the collision through Collided(), but not the platform, and either
	way the collision will still occur. Of course, if neither group includes each other in their respective C groups,
	no collision occurs and they pass right through each other. Also, note that objects in a group will not collide
	with other objects of the same group unless explicitly stated.

	Next comes collision resolution. If an object is in another object's R group, it means that the first object will
	be affected by the second object during collision resolution. For example, the NPCS group includes PLATFORMS in its
	R groups, meaning that platforms can affect the movements of NPCs. This does not have to be reciprocated by the other
	group; for example, PLATFORMS doesn't include NPCS in its R groups, therefore the physics of platforms can't be
	affected by collisions with NPCs. It's also possible to have a certain group in the C groups, but not the R groups.
	For example, PLAYER collides with NPCS, but doesn't resolve with them. This means that the player will be notified
	of collisions with NPCs, but will not be physically affected by them (meaning they'll pass right through, though
	other game mechanics might cause the player to take damage). 
	
	The lines in the set of rules below are formatted like this: rules[G] = CRule({C}, {R})
	******************************************************************************************************************/

	//if you add a new collision group, be sure to also add it to the list of constants exported to python in PyInit_CGD()

	std::map<CGroup, CRule> rules;
	rules[PLAYER] = CRule({ PLATFORMS, SOFT_PLATFORMS, WALL, PILLAR, NPCS, ENEMY_PROJECTILES, DEFAULT }, { PLATFORMS, SOFT_PLATFORMS, WALL, PILLAR, DEFAULT });
	rules[PLAYER_BOX] = CRule({ PLATFORMS, SOFT_PLATFORMS, WALL, PILLAR, DEFAULT }, { PLATFORMS, SOFT_PLATFORMS, WALL, PILLAR, DEFAULT });
	rules[PLATFORMS] = CRule( { PLAYER }, {} );
	rules[SOFT_PLATFORMS] = CRule( { PLAYER }, {} );
	rules[WALL] = CRule( { }, {} );
	rules[PILLAR] = CRule({ PLATFORMS, SOFT_PLATFORMS, WALL, FRIENDLY_PROJECTILES, ENEMY_PROJECTILES, DEFAULT }, { PLATFORMS, SOFT_PLATFORMS, WALL, FRIENDLY_PROJECTILES, ENEMY_PROJECTILES, DEFAULT });
	rules[NPCS] = CRule({ PLAYER, PLATFORMS, SOFT_PLATFORMS, WALL, PILLAR, FRIENDLY_PROJECTILES, DEFAULT }, { PLATFORMS, SOFT_PLATFORMS, WALL, PILLAR, DEFAULT });
	rules[NPCS_INVULNERABLE] = CRule({ PLATFORMS, SOFT_PLATFORMS, WALL, PILLAR }, { PLATFORMS, SOFT_PLATFORMS, PILLAR });
	rules[ENEMY_PROJECTILES] = CRule({ PLAYER, PLATFORMS, SOFT_PLATFORMS, WALL, PILLAR, SHOOT_ONLY, DEFAULT }, {});
	rules[FRIENDLY_PROJECTILES] = CRule({ PLATFORMS, SOFT_PLATFORMS, WALL, PILLAR, NPCS, SHOOT_ONLY, DEFAULT }, {});
	rules[SENSORS] = CRule({ PLATFORMS, SOFT_PLATFORMS, WALL, PILLAR, PLAYER, FRIENDLY_PROJECTILES, DEFAULT }, {});
	rules[SHOOT_ONLY] = CRule({ PLATFORMS, SOFT_PLATFORMS, WALL, FRIENDLY_PROJECTILES }, { PLATFORMS, SOFT_PLATFORMS, WALL, FRIENDLY_PROJECTILES });
	rules[DEFAULT] = CRule({ PLAYER, PLATFORMS, SOFT_PLATFORMS, WALL, PILLAR, NPCS, ENEMY_PROJECTILES, FRIENDLY_PROJECTILES, SENSORS, SHOOT_ONLY, DEFAULT }, { PLAYER, PLATFORMS, SOFT_PLATFORMS, WALL, PILLAR, NPCS, ENEMY_PROJECTILES, FRIENDLY_PROJECTILES, SHOOT_ONLY, DEFAULT });

	mCollisionRules = rules;
	initialized = true;
}