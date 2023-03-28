#ifndef _TILE_H_
#define _TILE_H_

#include "Base.h"
#include "Enums.h"
#include "glm\vec3.hpp"

class Tile : public Engine::CBase
{
public:
	enum TILE_DIR
	{
		ZP,
		ZP_XP,
		XP,
		ZN_XP,
		ZN,
		ZN_XN,
		XN,
		ZP_XN,
		TILE_DIR_END
	};

public:
	_uint			m_iIdx;
	eTileType		m_eType;
	glm::vec3		m_vPos;
	Tile*			m_neighborNodes[8];

private:
	explicit Tile();
	~Tile();

private:
	// Call instead of destructor to manage class internal data
	virtual void Destroy();
	// Initialize
	RESULT Ready(_uint idx, eTileType type, glm::vec3 vPos);
public:
	// Create an instance
	static Tile *Create(_uint idx, eTileType type, glm::vec3 vPos);
};

#endif //_TILE_H_