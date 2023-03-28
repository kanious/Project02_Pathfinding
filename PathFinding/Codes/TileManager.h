#ifndef _TILEMANAGER_H_
#define _TILEMANAGER_H_

#include "Base.h"
#include "Enums.h"
#include "glm\vec3.hpp"
#include "Tile.h"

class Tile;
class SceneDungeon;
class BGObject;

// A class that generates and manages tiles
class TileManager : public Engine::CBase
{
	SINGLETON(TileManager)
private:
	typedef struct sAStarNode
	{
		_float			fCost;
		_uint			iIndex;
		sAStarNode*		pParent;
	}NODE;

private:
	SceneDungeon*			m_pScene;
	std::vector<Tile*>		m_vecTiles;
	std::vector<BGObject*>	m_vecTileObjs;

	_uint					m_iMapWidth;
	_uint					m_iMapHeight;

	std::list<NODE*>		m_openList;
	std::list<NODE*>		m_closeList;
	std::list<_uint>		m_bestList;

	_uint					m_iStartIdx;
	_uint					m_iEndIdx;

	_bool					m_bPathSetting;
	_bool					m_bMoving;

private:
	explicit TileManager();
	~TileManager();
public:
	// Call instead of destructor to manage class internal data
	void Destroy();
	// Initialize
	RESULT Ready(SceneDungeon* pScene);
	// Add tile objects
	void AddTileObjects(BGObject* obj);
	// Notice player moving is end
	void SetMovingEnd();
	// Set new goal()
	void SetGoal(_uint idx);

private:
	// Load Tile information from BMP file
	void GenerateTilesFromBMPFile();
	// Add Tile Node to vector
	void AddTileNode(_uint idx, eTileType type, glm::vec3 vPos);
	// Connect nodes
	void ConnectNeighbors();

public:
	// Start to find the closest path
	void PathfindingStart(_uint startIdx, _uint endIdx);
private:
	// Clear all lists
	void ClearAllLists();
	// Reset all tile information
	void ResetAllTiles();
	// Make A* Route
	void MakeRoute();
	// Check if the index is already in the list
	_bool ListCheck(_uint idx);
	// For diagonal neighbors, Check If neighbor nodes are traversable
	_bool DirCheck(_uint idx, Tile::TILE_DIR eDir);
	// Make new node
	NODE* MakeNode(_uint idx, NODE* pParent);
	// Sort function
	static _bool Compare(const NODE* pNode1, const NODE* pNode2);
	// Draw the best lists to gameobject
	void DrawPathNode();

public:
	// Move player from the start to the goal
	void MovePlayer();
};

#endif //_TILEMANAGER_H_