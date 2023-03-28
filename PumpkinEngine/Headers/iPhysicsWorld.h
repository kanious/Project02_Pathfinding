#ifndef _IPHYSICSWORLD_H_
#define _IPHYSICSWORLD_H_

#include "Base.h"
#include "glm\vec3.hpp"

NAMESPACE_BEGIN(Engine)

class iRigidBody;
class CComponent;

// Physics World interface class
class ENGINE_API iPhysicsWorld : public CBase
{
protected:
	explicit iPhysicsWorld() {}
	virtual ~iPhysicsWorld() {}
	virtual void Destroy() = 0;

public:
	virtual void Update(const _float& dt) = 0;

public:
	virtual void SetGravity(const glm::vec3& gravity) = 0;
	virtual void AddBody(iRigidBody* body) = 0;
	virtual void RemoveBody(iRigidBody* body) = 0;
	virtual void RollDice(_uint count) = 0;
	virtual void SetCamera(CComponent* pCamera) = 0;
};

NAMESPACE_END

#endif //_IPHYSICSWORLD_H_ 