#pragma once
#include "Shape.h"
#include "GameObject.h"
#include "ResetObject.h"

namespace Phys {

	class Body : virtual public IResetObject {
	public:
		Vec2 mPosition, mVelocity, mForce, mImpulse;
		float mOrient, mAngVelocity, mTorque, mAngularImpulse;

		Vec2 mCenterOfMass;

		bool mMove, mRotate;

		float mInvMass, mInvIn;
		float mMass;
		Material mMaterial;

	protected:

		Shape* mShape;

		//note: s will be deleted upon destruction
		Body(Shape* s, const Phys::Vec2& pos, const Material& m, bool move = true, bool rotate = false, bool detectAABB = false);
		virtual ~Body();
		virtual void ResetObject();
		bool mDetectAABB;

	public:
		float GetXPos() const { return mPosition.x; }
		float GetYPos() const { return mPosition.y; }

		bool DetectAABB() const { return mDetectAABB; }

		//Motion functions
		virtual void Step(float dt, float g, float aF);

		//Move the object without changing the velocity=(position-previous)
		void Displace(const Vec2& shift);

		//apply any outstanding impulses to velocity
		void FlushImpulse();

		const AABB& GetAABB() const;
		//This method will return the 'total velocity', combining both linear and angular terms
		Vec2 CombinedVelocity(const Vec2& r);
		void ApplyImpulse(const Vec2& impulse, const Vec2& contactVector);
		void ApplyForce(const Vec2& f, const Vec2& contactVector);
		Projection ProjectOnto(const Vec2& axis) const;

		virtual void OffsetCenter(const Vec2& offset);

		//accessors
		Transform GetTransform();
		float Restitution() const;
		float StaticFriction() const;
		float DynamicFriction() const;
		Shape* GetShape() const { return mShape; }

		//Miscellaneous
		void AttractTo(Vec2 point, float M);
		void FreezeRotation();
		void EnableRotation();
		void FreezeMotion();
		void EnableMotion();
	private:
		void Init();
		void InitializeMotionData(const Vec2& pos);
		void InitializeMassData(const Material& m);
		void GetMassData();
		void GetInertiaData();
		//Motion functions
		void ApplyGravity(float g, const Vec2& pointOfApplication = Vec2());
		void ApplyAirFriction(float fric);
	};

}	//namespace phys
