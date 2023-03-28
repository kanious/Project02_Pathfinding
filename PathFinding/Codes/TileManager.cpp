#include <fstream>
#include <sstream>

#include "TileManager.h"
#include "OpenGLDefines.h"
#include "SceneDungeon.h"
#include "BGObject.h"
#include "Player.h"


SINGLETON_FUNCTION(TileManager)
USING(Engine)
USING(std)
USING(glm)

TileManager::TileManager()
	: m_pScene(nullptr), m_iMapWidth(0), m_iMapHeight(0), m_iStartIdx(0), m_iEndIdx(0)
	, m_bPathSetting(false)
{
	m_vecTiles.clear();
	m_vecTileObjs.clear();
}

TileManager::~TileManager()
{
}

// Call instead of destructor to manage class internal data
void TileManager::Destroy()
{
	ClearAllLists();

	for (int i = 0; i < m_vecTiles.size(); ++i)
		SafeDestroy(m_vecTiles[i]);
	m_vecTiles.clear();

	m_vecTileObjs.clear();
}

// Initialize
RESULT TileManager::Ready(SceneDungeon* pScene)
{
	if (nullptr == pScene)
		return PK_ERROR_NULLPTR;

	m_pScene = pScene;

	GenerateTilesFromBMPFile();
	ConnectNeighbors();
	m_pScene->LoadTileObjects(m_vecTiles);

	return PK_NOERROR;
}

void TileManager::AddTileObjects(BGObject* obj)
{
	m_vecTileObjs.push_back(obj);
}

void TileManager::SetMovingEnd()
{
	m_bMoving = false;
	m_bPathSetting = false;
	m_iStartIdx = m_iEndIdx;
	m_iEndIdx = INT_MAX;
	ClearAllLists();
	ResetAllTiles();
}

void TileManager::SetGoal(_uint idx)
{
	if (m_bMoving)
		return;

	if (idx >= m_vecTiles.size())
		return;
	
	if (TILE_WALL == m_vecTiles[idx]->m_eType ||
		TILE_START == m_vecTiles[idx]->m_eType)
		return;

	m_iEndIdx = idx;
	ClearAllLists();
	ResetAllTiles();

	PathfindingStart(m_iStartIdx, m_iEndIdx);
}

// Load Tile information from BMP file
void TileManager::GenerateTilesFromBMPFile()
{
	// Load bitmap
	ifstream bmp;
	stringstream ss;
	ss << m_pScene->GetDataPath() << "Texture\\sub\\map.bmp";
	bmp.open(ss.str());
	if (!bmp.is_open())
	{
		cout << "Map bitmap loading failed" << endl;
		bmp.close();
		return;
	}

	bmp.seekg(0);

	_uchar info[54];
	bmp.read((_char*)&info, sizeof(info));
	m_iMapWidth = *(int*)&info[18];
	m_iMapHeight = *(int*)&info[22];
	_int pixelCount = m_iMapWidth * m_iMapHeight;

	_uchar r, g, b;
	for (_int i = 0; i < pixelCount; ++i)
	{
		bmp.read((_char*)&b, 1);
		bmp.read((_char*)&g, 1);
		bmp.read((_char*)&r, 1);

		_int rr(r);
		_int gg(g);
		_int bb(b);

		// Tile Type
		eTileType type = TILE_NONE;
		if (0 == rr && 0 == gg && 0 == bb)
			type = TILE_WALL;
		else if (255 == rr && 255 == gg && 255 == bb)
			type = TILE_FLOOR;
		else
		{
			if (255 == rr)
				type = TILE_GOAL;
			else if (255 == gg)
				type = TILE_START;
			else if (255 == bb)
				type = TILE_PATH;
		}

		// Tile Position
		_int xIdx, zIdx;
		xIdx = i % m_iMapWidth;
		zIdx = i / m_iMapWidth;
		vec3 vPos = vec3(xIdx * 5.f, 0.f, zIdx * 5.f);

		AddTileNode(i, type, vPos);
	}

	bmp.close();
}

// Add Tile Node to vector
void TileManager::AddTileNode(_uint idx, eTileType type, vec3 vPos)
{
	Tile* pTile = Tile::Create(idx, type, vPos);
	m_vecTiles.push_back(pTile);
}

// Connect nodes
void TileManager::ConnectNeighbors()
{
	for (int i = 0; i < m_vecTiles.size(); ++i)
	{
		Tile* curTile = m_vecTiles[i];

		_uint idx = curTile->m_iIdx;
		_int xIdx = idx % m_iMapWidth;
		_int zIdx = idx / m_iMapWidth;

		// ZP
		if (zIdx + 1 < m_iMapHeight)
		{
			_uint nIdx = ((zIdx + 1) * m_iMapWidth) + xIdx;
			curTile->m_neighborNodes[Tile::ZP] = m_vecTiles[nIdx];
		}

		// ZP_XP
		if (zIdx + 1 < m_iMapHeight && xIdx + 1 < m_iMapWidth)
		{
			_uint nIdx = ((zIdx + 1) * m_iMapWidth) + (xIdx + 1);
			curTile->m_neighborNodes[Tile::ZP_XP] = m_vecTiles[nIdx];
		}

		// XP
		if (xIdx + 1 < m_iMapWidth)
		{
			_uint nIdx = (zIdx * m_iMapWidth) + (xIdx + 1);
			curTile->m_neighborNodes[Tile::XP] = m_vecTiles[nIdx];
		}

		// ZN_XP
		if (zIdx - 1 >= 0 && xIdx + 1 < m_iMapWidth)
		{
			_uint nIdx = ((zIdx - 1) * m_iMapWidth) + (xIdx + 1);
			curTile->m_neighborNodes[Tile::ZN_XP] = m_vecTiles[nIdx];
		}

		// ZN
		if (zIdx - 1 >= 0)
		{
			_uint nIdx = ((zIdx - 1) * m_iMapWidth) + xIdx;
			curTile->m_neighborNodes[Tile::ZN] = m_vecTiles[nIdx];
		}

		// ZN_XN
		if (zIdx - 1 >= 0 && xIdx - 1 >= 0)
		{
			_uint nIdx = ((zIdx - 1) * m_iMapWidth) + (xIdx - 1);
			curTile->m_neighborNodes[Tile::ZN_XN] = m_vecTiles[nIdx];
		}

		// XN
		if (xIdx - 1 >= 0)
		{
			_uint nIdx = (zIdx * m_iMapWidth) + (xIdx - 1);
			curTile->m_neighborNodes[Tile::XN] = m_vecTiles[nIdx];
		}

		// ZP_XN
		if (zIdx + 1 < m_iMapHeight && xIdx - 1 >= 0)
		{
			_uint nIdx = ((zIdx + 1) * m_iMapWidth) + (xIdx - 1);
			curTile->m_neighborNodes[Tile::ZP_XN] = m_vecTiles[nIdx];
		}
	}
}

// Start to find the closest path
void TileManager::PathfindingStart(_uint startIdx, _uint endIdx)
{
	if (startIdx == endIdx)
		return;

	m_iStartIdx = startIdx;
	m_iEndIdx = endIdx;

	MakeRoute();
}

// Clear all lists
void TileManager::ClearAllLists()
{
	list<NODE*>::iterator iter;
	for (iter = m_openList.begin(); iter != m_openList.end(); ++iter)
		delete* iter;
	m_openList.clear();

	for (iter = m_closeList.begin(); iter != m_closeList.end(); ++iter)
		delete* iter;
	m_closeList.clear();

	m_bestList.clear();
}

// Reset all tile information
void TileManager::ResetAllTiles()
{
	for (int i = 0; i < m_vecTileObjs.size(); ++i)
	{
		if (TILE_WALL != m_vecTileObjs[i]->GetTileType())
		{
			m_vecTiles[i]->m_eType = TILE_FLOOR;
			m_vecTileObjs[i]->ChangeTileType(TILE_FLOOR);
		}
	}

	if (m_iStartIdx < m_vecTileObjs.size())
	{
		m_vecTiles[m_iStartIdx]->m_eType = TILE_START;
		m_vecTileObjs[m_iStartIdx]->ChangeTileType(TILE_START);
	}
	if (m_iEndIdx < m_vecTileObjs.size())
	{
		m_vecTiles[m_iEndIdx]->m_eType = TILE_GOAL;
		m_vecTileObjs[m_iEndIdx]->ChangeTileType(TILE_GOAL);
	}
}

// Make A* Route
void TileManager::MakeRoute()
{
	NODE* pFirstNode = new NODE;
	pFirstNode->iIndex = m_iStartIdx;
	pFirstNode->pParent = nullptr;
	pFirstNode->fCost = 0.f;
	m_closeList.push_back(pFirstNode);

	NODE* pCurNode = nullptr;
	while (true)
	{
		Tile* pNeighbor = nullptr;
		for (int i = 0; i < Tile::TILE_DIR_END; ++i)
		{
			pNeighbor = m_vecTiles[pFirstNode->iIndex]->m_neighborNodes[i];
			if (nullptr == pNeighbor)
				continue;

			if (TILE_WALL != pNeighbor->m_eType &&
				DirCheck(pFirstNode->iIndex, (Tile::TILE_DIR)i) &&
				ListCheck(pNeighbor->m_iIdx))
			{
				pCurNode = MakeNode(pNeighbor->m_iIdx, pFirstNode);
				m_openList.push_back(pCurNode);
			}
		}

		if (m_openList.size() == 0)
			return;

		m_openList.sort(Compare);

		list<NODE*>::iterator iter = m_openList.begin();
		pFirstNode = (*iter);

		m_closeList.push_back(*iter);
		m_openList.erase(iter);

		if (pFirstNode->iIndex == m_iEndIdx)
		{
			while (true)
			{
				m_bestList.push_back(pFirstNode->iIndex);
				pFirstNode = pFirstNode->pParent;
				if (pFirstNode->iIndex == m_iStartIdx)
					break;
			}
			m_bestList.reverse();
			m_bPathSetting = true;
			DrawPathNode();
			break;
		}
	}
}

// Check if the index is already in the list
_bool TileManager::ListCheck(_uint idx)
{
	list<NODE*>::iterator iter;
	for (iter = m_openList.begin(); iter != m_openList.end(); ++iter)
	{
		if ((*iter)->iIndex == idx)
			return false;
	}

	for (iter = m_closeList.begin(); iter != m_closeList.end(); ++iter)
	{
		if ((*iter)->iIndex == idx)
			return false;
	}

	return true;
}

// For diagonal neighbors, Check If neighbor nodes are traversable
_bool TileManager::DirCheck(_uint idx, Tile::TILE_DIR eDir)
{
	Tile* pTile = m_vecTiles[idx];

	switch (eDir)
	{
	case Tile::ZP:
	case Tile::XP:
	case Tile::ZN:
	case Tile::XN:
		return true;

	case Tile::ZP_XP:
	{
		if (nullptr == pTile->m_neighborNodes[Tile::ZP] ||
			TILE_WALL == pTile->m_neighborNodes[Tile::ZP]->m_eType ||
			nullptr == pTile->m_neighborNodes[Tile::XP] ||
			TILE_WALL == pTile->m_neighborNodes[Tile::XP]->m_eType)
			return false;
		else
			return true;
	}

	case Tile::ZN_XP:
	{
		if (nullptr == pTile->m_neighborNodes[Tile::ZN] ||
			TILE_WALL == pTile->m_neighborNodes[Tile::ZN]->m_eType ||
			nullptr == pTile->m_neighborNodes[Tile::XP] ||
			TILE_WALL == pTile->m_neighborNodes[Tile::XP]->m_eType)
			return false;
		else
			return true;
	}

	case Tile::ZN_XN:
	{
		if (nullptr == pTile->m_neighborNodes[Tile::ZN] ||
			TILE_WALL == pTile->m_neighborNodes[Tile::ZN]->m_eType ||
			nullptr == pTile->m_neighborNodes[Tile::XN] ||
			TILE_WALL == pTile->m_neighborNodes[Tile::XN]->m_eType)
			return false;
		else
			return true;
	}

	case Tile::ZP_XN:
	{
		if (nullptr == pTile->m_neighborNodes[Tile::ZP] ||
			TILE_WALL == pTile->m_neighborNodes[Tile::ZP]->m_eType ||
			nullptr == pTile->m_neighborNodes[Tile::XN] ||
			TILE_WALL == pTile->m_neighborNodes[Tile::XN]->m_eType)
			return false;
		else
			return true;
	}
		
	}

	return false;
}

// Make new node
TileManager::NODE* TileManager::MakeNode(_uint idx, NODE* pParent)
{
	NODE* pNode = new NODE;
	pNode->iIndex = idx;
	pNode->pParent = pParent;

	vec3 vPos = m_vecTiles[idx]->m_vPos;
	vec3 vParentPos = m_vecTiles[pParent->iIndex]->m_vPos;
	vec3 vEndPos = m_vecTiles[m_iEndIdx]->m_vPos;

	_float fDist1 = distance(vPos, vParentPos);
	_float fDist2 = distance(vPos, vEndPos);

	pNode->fCost = fDist1 + fDist2;

	return pNode;
}

// Sort function
_bool TileManager::Compare(const NODE* pNode1, const NODE* pNode2)
{
	return pNode1->fCost < pNode2->fCost;
}

// Draw the best lists to gameobject
void TileManager::DrawPathNode()
{
	list<_uint>::iterator iter;
	for (iter = m_bestList.begin(); iter != m_bestList.end(); ++iter)
	{
		_uint index = (*iter);
		if (TILE_FLOOR == m_vecTileObjs[index]->GetTileType())
			m_vecTileObjs[index]->ChangeTileType(TILE_PATH);
	}
}

// Move player from the start to the goal
void TileManager::MovePlayer()
{
	if (!m_bPathSetting || m_bMoving)
		return;

	Player* pPlayer = m_pScene->GetPlayer();
	if (nullptr == pPlayer)
		return;

	list<_uint>::iterator iter;
	for (iter = m_bestList.begin(); iter != m_bestList.end(); ++iter)
	{
		Tile* tile = m_vecTiles[(*iter)];
		pPlayer->AddTargetPos(tile->m_vPos);
	}
	pPlayer->StartMoving();
	m_bMoving = true;
}

