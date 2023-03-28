#include "BGObject.h"
#include "ComponentMaster.h"
#include "Layer.h"

#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Renderer.h"
#include "BoundingBox.h"
#include "OpenGLDefines.h"
#include "OpenGLDevice.h"
#include "SoundMaster.h"
#include "LightMaster.h"
#include "Shader.h"
#include "LightMaster.h"
#include "Light.h"


USING(Engine)
USING(glm)
USING(std)

BGObject::BGObject()
	: m_pMesh(nullptr), m_bTransparent(false), m_iTileIdx(0), m_eTileType(TILE_NONE)
{
	m_bDebug = false;
}

BGObject::~BGObject()
{
}

// Basic Update Function
void BGObject::Update(const _float& dt)
{
	if (m_bEnable)
	{
		CGameObject::Update(dt);

		if (nullptr != m_pRenderer)
			m_pRenderer->AddRenderObj(this, m_bTransparent);
	}
}

// Basic Render Function
void BGObject::Render()
{
	CGameObject::Render();
}

// Call instead of destructor to manage class internal data
void BGObject::Destroy()
{
	CGameObject::Destroy();
}

void BGObject::ChangeTileType(eTileType eType)
{
	m_eTileType = eType;

	switch (eType)
	{
	case TILE_NONE:
	case TILE_WALL:
		break;

	case TILE_FLOOR:
		m_pMesh->SetTexture("tile_white_Tex");
		break;

	case TILE_START:
		m_pMesh->SetTexture("tile_green_Tex");
		break;

	case TILE_GOAL:
		m_pMesh->SetTexture("tile_red_Tex");
		break;

	case TILE_PATH:
		m_pMesh->SetTexture("tile_blue_Tex");
		break;
	}
}

// Initialize
RESULT BGObject::Ready(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale, _uint tileIdx, eTileType eType)
{
	SetupGameObject(sTag, lTag, oTag);
	m_pLayer = pLayer;
	m_meshName = meshID;
	m_bTransparent = false;
	m_iTileIdx = tileIdx;
	m_eTileType = eType;

	//Clone.Mesh
 	m_pMesh = CloneComponent<CMesh*>(meshID);
	if (nullptr != m_pMesh)
	{
		AttachComponent("Mesh", m_pMesh);
		m_pMesh->SetTransform(m_pTransform);
		m_pBoundingBox = m_pMesh->GetBoundingBox();
		if (nullptr != m_pBoundingBox)
			m_pBoundingBox->SetTransform(m_pTransform);
		m_pMesh->SetWireFrame(false);
		m_pMesh->SetDebugBox(false);
		m_pMesh->SetTransparency(m_bTransparent);

		switch (eType)
		{
		case TILE_NONE:
		case TILE_WALL:
			break;

		case TILE_FLOOR:
			m_pMesh->SetTexture("tile_white_Tex");
			break;
	
		case TILE_START:
			m_pMesh->SetTexture("tile_green_Tex");
			break;
		
		case TILE_GOAL:
			m_pMesh->SetTexture("tile_red_Tex");
			break;
		
		case TILE_PATH:
			m_pMesh->SetTexture("tile_blue_Tex");
			break;
		}
	}

	if (nullptr != m_pTransform)
	{
		m_pTransform->SetPosRotScale(vPos, vRot, vScale);
		m_pTransform->Update(0);
	}

	return PK_NOERROR;
}

// Create an instance
BGObject* BGObject::Create(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale, _uint tileIdx, eTileType eType)
{
	BGObject* pInstance = new BGObject();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale, tileIdx, eType))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
