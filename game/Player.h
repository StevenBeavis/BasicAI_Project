#pragma once

class CPlayer : public CSprite
{
public:
	enum STATE { NONE, IDLE, WALK, ATTACK };
	enum DIR { NO_DIR, UP, DOWN, LEFT, RIGHT };

	float SPEED = 100;

private:
	STATE state;
	DIR dir;
	float health;

public:
	CPlayer(Sint16 x, Sint16 y, char* pFileBitmap, Uint32 time);
	CPlayer(Sint16 x, Sint16 y, CGraphics* pGraphics, Uint32 time);
	~CPlayer(void);

	void OnAttacked();
	void OnUpdate(Uint32 time, Uint32 deltaTime);

	void ChangeState(STATE newState, DIR newDir);
	STATE GetState() { return state; }
	DIR GetDir() { return dir; }

	float GetHealth() { return health; }
	void SetHealth(float h) { health = h; }

	void Input(DIR newDir, bool bAttack);
};

