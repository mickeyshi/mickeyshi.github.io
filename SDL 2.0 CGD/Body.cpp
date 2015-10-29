#include "Body.h"
#include <algorithm>
#include <cmath>

using namespace std;

using namespace Phys;

const Vec2 VELOCITY_THRESHOLD(2.0f, 2.0f);

Body::Body(Shape* s, const Phys::Vec2& pos, const Material& m, bool move, bool rotate, bool detectAABB)
{
	mShape = s;
	mMove = move, mRotate = rotate, mDetectAABB = detectAABB;
	StoreValues<Body>(pos, m, mMove, mRotate, mDetectAABB, mVelocity, mForce, mCenterOfMass);
	Init();
}

Body::~Body()
{
	delete mShape;
}

void Body::Init()
{
	Phys::Vec2 pos;
	Material m;

	LoadValues<Body>(pos, m, mMove, mRotate, mDetectAABB, mVelocity, mForce, mCenterOfMass);

	InitializeMotionData(pos);
	InitializeMassData(m);
	if (!mMove && !mRotate)
		FreezeMotion();
	if (!mRotate)
		FreezeRotation();

	mAngularImpulse = 0.0;
}

void Body::ResetObject()
{
	if (mShape != NULL)
	{
		mShape->ResetObject();
	}
	Init();
}

void Body::InitializeMotionData(const Vec2& pos)
{
	mPosition = pos;
	mOrient = 0.0f;
	mAngVelocity = 0.0f;
	mTorque = 0.0f;
}

void Body::InitializeMassData(const Material& m)
{
	mMaterial = m;
	GetMassData();
	GetInertiaData();
}

void Body::GetMassData()
{
	mMass = mShape->GetArea() * mMaterial.mDensity;
	mInvMass = (mMass == 0) ? 0 : 1 / mMass;
}

void Body::GetInertiaData()
{
	float inertia = mShape->GetInertia(mMaterial.mDensity);
	mInvIn = (inertia == 0) ? 0 : 1 / inertia;
}

//* Motion functions *//
void Body::ApplyGravity(float g, const Vec2& pointOfApplication)
{
	float mass = (mInvMass == 0) ? 0 : 1 / mInvMass;
	ApplyForce(Vec2(0.0f, mass * g * mMaterial.mGravScale), pointOfApplication);
}

void Body::Step(float dt, float g, float aF)
{
	//apply forces
	if (mMove)
		mVelocity.Add(mForce.Times(mInvMass*dt));

	//apply impulse
	FlushImpulse();

	if (abs(mVelocity.x) < VELOCITY_THRESHOLD.x)
		mVelocity.x = 0.0f;
	if (abs(mVelocity.y) < VELOCITY_THRESHOLD.y)
		mVelocity.y = 0.0f;

	mPosition.Add(mVelocity.Times(dt));

	if (mRotate) {
		mAngVelocity += mTorque * mInvIn * dt;
		mOrient += mAngVelocity * dt;
	}
	else {
		mAngVelocity = 0.0f;
	}

	mForce = Vec2();
	mTorque = 0;

	mShape->RecalcParams(GetTransform());

	//setup forces for the next frame
	ApplyGravity(g, mCenterOfMass.Anti().Rotate(Transform(Vec2(), mOrient)));
	ApplyAirFriction(aF);
}

//* Move the object without changing the velocity=(mPosition-mPrevious) *//
void Body::Displace(const Vec2& shift)
{
	if (mMove) {
		mPosition.Add(shift);
		mShape->RecalcParams(GetTransform());
	}
	else if (!mMove && mRotate) {
		//emulate positional displacement using angular displacement
		Vec2 com = mCenterOfMass.Rotate(Transform(Vec2(), mOrient));
		Vec2 v1(com.Anti().Plus(shift));
		Vec2 v2(com.Anti());

		float sinTheta = v1.Cross(v2) / v1.Magnitude() / v2.Magnitude();
		float dtheta = asin(std::max(-1.0f, std::min(sinTheta, 1.0f)));	//account for floating point errors by clamping to exactly [-1.0f, 1.0f]

		mOrient -= dtheta;

		mShape->RecalcParams(GetTransform());
	}
}

void Body::FlushImpulse()
{
	mVelocity.Add(mImpulse.Times(mInvMass));
	mAngVelocity += mAngularImpulse * mInvIn;

	mImpulse = Vec2();
	mAngularImpulse = 0.f;
}

void Body::ApplyAirFriction(float fric)
{
	if (mAngVelocity > fric / 2)
		mAngVelocity -= fric;
	else if (mAngVelocity < -fric / 2)
		mAngVelocity += fric;
	else
		mAngVelocity = 0.0f;
}

const AABB& Body::GetAABB() const
{
	return mShape->GetAABB();
}

Vec2 Body::CombinedVelocity(const Vec2& r)
{
	return mVelocity.Plus(r.Ortho().Times(mAngVelocity));
}

/*This methodi will apply an impulse to both linear and angular motion*/
void Body::ApplyImpulse(const Vec2& impulse, const Vec2& contactVector)
{
	//hold off on changing the velocity until Step() is called to avoid changing the relative velocity
	//in the middle of Collision::Resolve
	if (mMove)
		mImpulse.Add(impulse);
	if (mRotate)
		mAngularImpulse += contactVector.Cross(impulse);
}

void Body::ApplyForce(const Vec2& f, const Vec2& contactVector)
{
	if (mMove)
		mForce.Add(f);
	if (mRotate)
		mTorque += contactVector.Cross(f);
}

/*This method will project the object's shape onto an axis*/
Projection Body::ProjectOnto(const Vec2& axis) const
{
	return mShape->ProjectOnto(axis);
}

void Body::OffsetCenter(const Vec2& offset)
{
	mShape->OffsetCenter(offset);
	mPosition.Add(offset);
	mCenterOfMass.Add(offset);
}

/*accessors*/
Transform Body::GetTransform()
{
	Transform t = Transform(mPosition, mOrient);
	return t;
}

float Body::Restitution() const
{
	return mMaterial.mRestitution;
}

float Body::StaticFriction() const
{
	return mMaterial.mStaticFriction;
}

float Body::DynamicFriction() const
{
	return mMaterial.mDynamicFriction;
}

/*Miscellaneous*/
void Body::AttractTo(Vec2 point, float M)
{
	Vec2 dir = point.Minus(mPosition);
	Vec2 f = dir.Unit().Times(M / dir.Magnitude());
	mForce.Add(f);
}

void Body::FreezeRotation()
{
	mInvIn = 0;
	mRotate = false;
}

void Body::EnableRotation()
{
	GetInertiaData();
	mRotate = true;
}

void Body::FreezeMotion()
{
	mInvMass = 0;
}

void Body::EnableMotion()
{
	GetMassData();
}