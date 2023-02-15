#pragma once

#include "Player.h"
#include "Spider.h"

struct CONNECTION
{
	int nEnd;		// index of the destination node
	float cost;		// cost of the transition
};

struct NODE
{
	CVector pos;				// 2D position of the node
	list<CONNECTION> conlist;	// list of connections

	// variables used by A*
	float costSoFar;
	int nConnection;
	bool open, closed;
};

class CMyGame : public CGame
{
	// Define sprites and other instance variables here
	CPlayer m_player;
	CSpiderList m_spiders;
	CGraphics m_background;
	

	CSprite m_npc;					// Fire
	CSpriteList m_tiles;			// Tiles
	CSpriteList m_nodes;			// Nodes
	static char* m_tileLayout[12];	// Tiles layout

	vector<NODE> m_graph;
	list<CVector> m_waypoints;

public:
	CMyGame(void);
	~CMyGame(void);

	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartGame();
	virtual void OnStartLevel(Sint16 nLevel);
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y);
};
