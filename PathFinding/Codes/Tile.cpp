#include "Tile.h"

USING(std)
USING(glm)
USING(Engine)

Tile::Tile()
	: m_iIdx(0), m_eType(TILE_NONE), m_vPos(vec3(0.f))
{
	memset(m_neighborNodes, 0, sizeof(m_neighborNodes));
}

Tile::~Tile()
{
}

// Call instead of destructor to manage class internal data
void Tile::Destroy()
{
}

// Initialize
RESULT Tile::Ready(_uint idx, eTileType type, vec3 vPos)
{
	m_iIdx = idx;
	m_eType = type;
	m_vPos = vPos;

	return PK_NOERROR;
}

// Create an instance
Tile* Tile::Create(_uint idx, eTileType type, vec3 vPos)
{
	Tile* pInstance = new Tile();
	if (PK_NOERROR != pInstance->Ready(idx, type, vPos))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
