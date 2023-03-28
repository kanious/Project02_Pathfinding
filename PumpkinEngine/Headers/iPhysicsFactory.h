#ifndef _IPHYSICSFACTORY_H_
#define _IPHYSICSFACTORY_H_

#include <functional>
#include "Base.h"
#include "iShape.h"

NAMESPACE_BEGIN(Engine)

class iPhysicsWorld;
class iRigidBody;
class CRigidBodyDesc;

// Physics Factory interface class
class ENGINE_API iPhysicsFactory : public CBase
{
protected:
	explicit iPhysicsFactory() {}
	virtual ~iPhysicsFactory() {}
	virtual void Destroy() = 0;

public:
	virtual iPhysicsWorld* CreateWorld(std::function<void(void)> callback) = 0;
	virtual iRigidBody* CreateRigidBody(const CRigidBodyDesc& desc, eShapeType shape) = 0;
};

NAMESPACE_END

#endif //_IPHYSICSFACTORY_H_