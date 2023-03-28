#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <queue>
#include "GameObject.h"
#include "EngineStruct.h"
#include "Enums.h"

namespace Engine
{
	class CMesh;
	class CInputDevice;
	class CAnimation;
}
class Animator;

// Character class
class Player : public Engine::CGameObject
{
private:
	Engine::CMesh*				m_pMesh;
	Engine::CInputDevice*		m_pInputDevice;
	glm::vec3					m_vOriginPosition;
	glm::vec3					m_vVelocity;
	_float						m_fSpeed;
	_float						m_fRotSpeed;

	Animator*					m_pAnimator;
	_bool						m_bMoving;

	std::queue<glm::vec3>		m_queuePos;
	glm::vec3					m_vTargetPos;

private:
	explicit Player();
	virtual ~Player();

public:
	glm::vec3 GetVelocity()					{ return m_vVelocity; }
	glm::vec3 GetOriginPosition()			{ return m_vOriginPosition; }
	Animator* GetAnimator()					{ return m_pAnimator; }
	// Notify mesh instance of targeting status
	void SetTarget(_bool value);
	// Add moving positions
	void AddTargetPos(glm::vec3 vPos);
	// Start moving
	void StartMoving();
private:
	// Check movement
	void MovingCheck(const _float& dt);

public:
	// Basic Update Function
	virtual void Update(const _float& dt);
	// Basic Render Function
	virtual void Render();
private:
	// Call instead of destructor to manage class internal data
	virtual void Destroy();
	// Initialize
	RESULT Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale);
public:
	// Create an instance
	static Player* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale);
};


#endif //_PLAYER_H_