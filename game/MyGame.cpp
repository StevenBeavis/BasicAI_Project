#include "stdafx.h"
#include "MyGame.h"

#pragma warning(disable:4244)

char* CMyGame::m_tileLayout[12] =
{
	"XXXXXXXXXXXXXXXXXXXX",
	"X                  X",
	"X                  X",
	"X                  X",
	"X                  X",
	"X                  X",
	"X                  X",
	"X                  X",
	"X                  X",
	"X                  X",
	"X                  X",
	"XXXXXXXXXXXXXXXXXXXX",
};

float Coords[][2] = {
	{ 140, 138 },
	{ 140, 338 },
	{ 140, 538 },
	{ 140, 738 },
	{ 340, 138 },
	{ 340, 338 },
	{ 340, 538 },
	{ 340, 738 },
	{ 540, 138 },
	{ 540, 338 },
	{ 540, 538 },
	{ 540, 738 },
	{ 740, 138 },
	{ 740, 338 },
	{ 740, 538 },
	{ 740, 738 },
	{ 940, 138 },
	{ 940, 338 },
	{ 940, 538 },
	{ 940, 738 },
	{ 1140, 138 },
	{ 1140, 338 },
	{ 1140, 538 },
	{ 1140, 738 },
};

int Connections[][2] = {
	{ 0, 1 },
	{ 0, 4 },
	{ 1, 2 },
	{ 1, 10 },
	{ 2, 3 },
	{ 2, 9 },
	{ 3, 6 },
	{ 4, 5 },
	{ 4, 8 },
	{ 6, 7 },
	{ 7, 10 },
	{ 8, 9 },
	{ 9, 10 },
	{ 10, 11 },
	{ 9, 13 },
	{ 10, 14 },
	{ 12, 13 },
	{ 13, 14 },
	{ 14, 15 },
	{ 14, 19 },
	{ 16, 17 },
	{ 13, 22 },
	{ 21, 14 },
	{ 16, 17 },
	{ 16, 12 },
	{ 16, 20 },
	{ 21, 20 },
	{ 21, 22 },
	{ 23, 22 },
	{ 19, 18 },
	{ 18, 23 },
};

bool PathFind(vector<NODE>& graph, int nStart, int nGoal, vector<int>& path)
{
	list<int> open;

	// mark all nodes in the graph as unvisited
	for (unsigned i = 0; i < graph.size(); i++)
		graph[i].open = false;

	// open the Start node
	graph[nStart].costSoFar = 0;
	graph[nStart].nConnection = -1;
	graph[nStart].open = true;
	open.push_back(nStart);

	while (open.size() > 0)
	{
		// Find the element with the smallest costSoFar
		// iMin is the iterator (pointer) to its position in the opn list
		list<int>::iterator iCurrent = min_element(open.begin(), open.end(), [graph](int i, int j) -> bool {
			return graph[i].costSoFar < graph[j].costSoFar;
			});
		int curNode = *iCurrent;
		float coastSoFar = graph[curNode].costSoFar;

		// If the end node found, then terminate
		if (curNode == nGoal)
			break;

		// Otherwise, visit all the connections
		for (CONNECTION conn : graph[curNode].conlist)
		{
			int endNode = conn.nEnd;
			float newCostSoFar = coastSoFar + conn.cost;

			// for open nodes, ignore if the current route worse then the route already found
			if (graph[endNode].open && graph[endNode].costSoFar <= newCostSoFar)
				continue;

			// Wow, we've found a better route!
			graph[endNode].costSoFar = newCostSoFar;
			graph[endNode].nConnection = curNode;

			// if unvisited yet, add to the open list
			if (!graph[endNode].open)
			{
				graph[endNode].open = true;
				open.push_back(endNode);
			}

			// in Dijkstra, this should never be a closed node
		}

		// We can now close the current graph...
		graph[curNode].closed = true;
		open.erase(iCurrent);
	}

	// Collect the path from the generated graph data
	if (open.size() == 0)
		return false;		// path not found!

	int i = nGoal;
	while (graph[i].nConnection >= 0)
	{
		path.push_back(i);
		i = graph[i].nConnection;
	}
	path.push_back(i);

	reverse(path.begin(), path.end());
	return true;
}

CMyGame::CMyGame(void) : 
	m_player(640, 384, "boy.png", 0),
	m_background("tile_5.png"),
	m_npc(544, 96, 64, 64, 0)
{
	m_npc.LoadAnimation("explosion.bmp", "walk", CSprite::Sheet(5, 5).Col(2).From(2).To(4));
	m_npc.LoadAnimation("explosion.bmp", "idle", CSprite::Sheet(5, 5).Col(2).From(0).To(1));
	m_npc.SetAnimation("idle", 4);

	// create graph structure - nodes
	for (float* coord : Coords)
		m_graph.push_back(NODE{ CVector(coord[0], coord[1]) });

	// create graph structure - connections
	for (int* conn : Connections)
	{
		int ind1 = conn[0];
		int ind2 = conn[1];
		NODE& node1 = m_graph[ind1];
		NODE& node2 = m_graph[ind2];
		float dist = Distance(node1.pos, node2.pos);

		node1.conlist.push_back(CONNECTION{ ind2, dist });
		node2.conlist.push_back(CONNECTION{ ind1, dist });
	}
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	if (!IsGameMode() || m_spiders.size() == 0) return;

	Uint32 t = GetTime();

	// Update AI Agents
	bool bAttack = IsKeyDown(SDLK_LCTRL) || IsKeyDown(SDLK_RCTRL);
	if (IsKeyDown(SDLK_w) || IsKeyDown(SDLK_UP)) m_player.Input(CPlayer::UP, bAttack);
	else if (IsKeyDown(SDLK_s) || IsKeyDown(SDLK_DOWN)) m_player.Input(CPlayer::DOWN, bAttack);
	else if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT)) m_player.Input(CPlayer::LEFT, bAttack);
	else if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT)) m_player.Input(CPlayer::RIGHT, bAttack);
	else m_player.Input(CPlayer::NO_DIR, bAttack);

	float health = 0;
	for (CSpider *pSpider : m_spiders)
	{
		health += pSpider->GetHealth();

		pSpider->SetEnemyPosition(m_player.GetPosition());

		if (Distance(pSpider->GetPosition(), m_player.GetPosition()) <= 64)
		{
			if (pSpider->GetState() == CSpider::ATTACK)
				m_player.OnAttacked();
			if (m_player.GetState() == CPlayer::ATTACK)
				pSpider->OnAttacked();
		}
		if (pSpider->HitTest(&m_npc))
		{
			pSpider->OnFire();
		}
		else
		{
			pSpider->NotOnFire();
		}
	}

	if (m_player.GetHealth() <= 0 || health <= 0)
		GameOver();

	m_player.Update(t);
	for (CSpider* pSpider : m_spiders)
		pSpider->Update(t);

	// NPC: follow the waypoints
	if (!m_waypoints.empty())
	{
		// If NPC not moving, start moving to the first waypoint
		if (m_npc.GetSpeed() < 1)
		{
			m_npc.SetSpeed(500);
			m_npc.SetAnimation("walk");
			m_npc.SetDirection(m_waypoints.front() - m_npc.GetPosition());
			m_npc.SetRotation(m_npc.GetDirection() - 90);
		}

		// Passed the waypoint?
		CVector v = m_waypoints.front() - m_npc.GetPosition();
		if (Dot(m_npc.GetVelocity(), v) < 0)
		{
			// Stop movement
			m_waypoints.pop_front();
			if (m_waypoints.empty())
				m_npc.SetAnimation("idle");
			m_npc.SetVelocity(0, 0);
			m_npc.SetRotation(0);
		}
	}

	m_npc.Update(t);
}

void DrawHealth(CGraphics* g, CVector pos, float w, float h, float health)
{
	if (health <= 0) return;
	g->DrawRect(CRectangle(pos.m_x - 1, pos.m_y - 1, w + 1, h + 1), CColor::Black());
	g->FillRect(CRectangle(pos.m_x, pos.m_y - 1, ceil(w * health / 100.f), h), health > 20 ? CColor::DarkGreen() : CColor::Red());
}

void CMyGame::OnDraw(CGraphics* g)
{
	// Draw background
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 3; j++)
			g->Blit(CVector(256.f*i, 256.f *j), m_background);

	m_player.Draw(g);
	DrawHealth(g, m_player.GetPosition() + CVector(-32, 32), 20, 4, m_player.GetHealth());
	for (CSpider* pSpider : m_spiders)
	{
		pSpider->Draw(g);
		DrawHealth(g, pSpider->GetPosition() + CVector(-32, 32), 20, 4, pSpider->GetHealth());
	}

	if (IsGameOver())
		if (m_player.GetHealth() <= 0)
			* g << font(48) << color(CColor::DarkRed()) << vcenter << center << "GAME OVER" << endl;
		else
			*g << font(48) << color(CColor::DarkBlue()) << vcenter << center << "YOU'RE A SPIDER KILLER!" << endl;
/*
	for (NODE n : m_graph)
		for (CONNECTION c : n.conlist)
			g->DrawLine(n.pos, m_graph[c.nEnd].pos, CColor::Black());
	m_nodes.for_each(&CSprite::Draw, g);
	m_tiles.for_each(&CSprite::Draw, g);*/
	m_npc.Draw(g);
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	// Create Nodes
	int i = 0;
	for (NODE n : m_graph)
	{
		stringstream s;
		s << i++;
		m_nodes.push_back(new CSpriteOval(n.pos, 12, CColor::White(), CColor::Black(), 0));
		m_nodes.push_back(new CSpriteText(n.pos, "arial.ttf", 14, s.str(), CColor::Black(), 0));
	}
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	StartGame();	// exits the menu mode and starts the game mode
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
	m_spiders.delete_all();

	m_spiders.push_back(new CSpider(640, 40, "spider64.png", 0));
	m_spiders.push_back(new CSpider(80, 192, "spider64.png", 0));
	m_spiders.push_back(new CSpider(80, 192, "spider64.png", 0));
	m_spiders.push_back(new CSpider(80, 192, "spider64.png", 0));
	m_spiders.push_back(new CSpider(80, 192, "spider64.png", 0));
	m_spiders.push_back(new CSpider(80, 192, "spider64.png", 0));
	m_spiders.push_back(new CSpider(1100, 576, "spider64.png", 0));
	m_spiders.push_back(new CSpider(1100, 40, "spider64.png", 0));
	m_spiders.push_back(new CSpider(1100, 40, "spider64.png", 0));
	m_spiders.push_back(new CSpider(1100, 40, "spider64.png", 0));
	m_spiders.push_back(new CSpider(1100, 40, "spider64.png", 0));
	m_spiders.push_back(new CSpider(1100, 40, "spider64.png", 0));
	m_spiders.push_back(new CSpider(80, 40, "spider64.png", 0));
	m_spiders.push_back(new CSpider(1100, 576, "spider64.png", 0));
	m_spiders.push_back(new CSpider(1100, 576, "spider64.png", 0));
	m_spiders.push_back(new CSpider(1100, 576, "spider64.png", 0));
	m_spiders.push_back(new CSpider(1100, 576, "spider64.png", 0));
	m_spiders.push_back(new CSpider(1100, 576, "spider64.png", 0));
	m_spiders.push_back(new CSpider(80, 576, "spider64.png", 0));
	m_spiders.push_back(new CSpider(80, 576, "spider64.png", 0));
	m_spiders.push_back(new CSpider(80, 576, "spider64.png", 0));
	m_spiders.push_back(new CSpider(80, 576, "spider64.png", 0));
	m_spiders.push_back(new CSpider(80, 576, "spider64.png", 0));
	
	m_player.SetPosition(640, 384);
	m_player.ChangeState(CPlayer::IDLE, CPlayer::DOWN);
	m_player.SetHealth(100);
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
}

// called when the game is over
void CMyGame::OnGameOver()
{
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();

	if (sym == SDLK_1) (*m_spiders.begin())->ChangeState(CSpider::IDLE);
	if (sym == SDLK_2) (*m_spiders.begin())->ChangeState(CSpider::PATROL);
	if (sym == SDLK_3) (*m_spiders.begin())->ChangeState(CSpider::CHASE);
	if (sym == SDLK_4) (*m_spiders.begin())->ChangeState(CSpider::ATTACK);
	if (sym == SDLK_5) (*m_spiders.begin())->ChangeState(CSpider::FLEE);
	if (sym == SDLK_6) (*m_spiders.begin())->ChangeState(CSpider::DIE);
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}

/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	CVector v(x, y);	// destination

	// check if the move is legal
	if (m_tileLayout[y / 64][x / 64] != ' ')
		return;	// cannot go into a wall!

	// find the first node: the closest to the NPC
	vector<NODE>::iterator iFirst =
		min_element(m_graph.begin(), m_graph.end(), [this](NODE& n1, NODE& n2) -> bool {
		return Distance(n1.pos, m_npc.GetPos()) < Distance(n2.pos, m_npc.GetPos());
			});

	// find the last node: the closest to the destination
	vector<NODE>::iterator iLast =
		min_element(m_graph.begin(), m_graph.end(), [v](NODE& n1, NODE& n2) -> bool {
		return Distance(n1.pos, v) < Distance(n2.pos, v);
			});

	int nFirst = iFirst - m_graph.begin();
	int nLast = iLast - m_graph.begin();


	// remove the current way points and reset the NPC
	if (!m_waypoints.empty())
	{
		m_waypoints.clear();
		m_npc.SetVelocity(0, 0);
	}

	// call the path finding algorithm to complete the waypoints
	vector<int> path;
	if (PathFind(m_graph, nFirst, nLast, path))
	{
		for (int i : path)
			m_waypoints.push_back(m_graph[i].pos);
		m_waypoints.push_back(v);
	}
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
