#ifndef _BGOBJECT_H_
#define _BGOBJECT_H_

#include "GameObject.h"
#include "EngineStruct.h"
#include "Enums.h"

namespace Engine
{
	class CMesh;
}

// Static Object class
class BGObject : public Engine::CGameObject
{
private:
	Engine::CMesh*					m_pMesh;
	_bool							m_bTransparent;
	_uint							m_iTileIdx;
	eTileType						m_eTileType;

private:
	explicit BGObject();
	virtual ~BGObject();

public:
	// Basic Update Function
	virtual void Update(const _float& dt);
	// Basic Render Function
	virtual void Render();
private:
	// Call instead of destructor to manage class internal data
	virtual void Destroy();

public:
	_uint GetTileIdx()				{ return m_iTileIdx; }
	eTileType GetTileType()			{ return m_eTileType; }
	// Change tile type
	void ChangeTileType(eTileType eType);

private:
	// Initialize
	RESULT Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, _uint tileIdx, eTileType eType);
public:
	// Create an instance
	static BGObject* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, _uint tileIdx, eTileType eType);
};

#endif //_BGOBJECT_H_