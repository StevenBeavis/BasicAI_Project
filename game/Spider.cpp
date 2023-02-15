#include "stdafx.h"
#include "Spider.h"




CSpider::CSpider(Sint16 x, Sint16 y, char* pFileBitmap, Uint32 time) : CSprite(x, y, 0, 0, time)
{
	LoadAnimation(pFileBitmap, "walk", CSprite::Sheet(4, 2).Col(0).From(0).To(1));
	LoadAnimation(pFileBitmap, "attack", CSprite::Sheet(4, 2).Col(1).From(0).To(1));
	LoadAnimation(pFileBitmap, "idle", CSprite::Sheet(4, 2).Col(2).From(0).To(1));
	LoadAnimation(pFileBitmap, "death", CSprite::Sheet(4, 2).Col(3).From(0).To(0));

	health = 100;
	state = NONE;
	ChangeState(IDLE);
}

CSpider::CSpider(Sint16 x, Sint16 y, CGraphics* pGraphics, Uint32 time) : CSprite(x, y, 0, 0, time)
{
	LoadAnimation(pGraphics, "walk", CSprite::Sheet(4, 2).Col(0).From(0).To(1));
	LoadAnimation(pGraphics, "attack", CSprite::Sheet(4, 2).Col(1).From(0).To(1));
	LoadAnimation(pGraphics, "idle", CSprite::Sheet(4, 2).Col(2).From(0).To(1));
	LoadAnimation(pGraphics, "death", CSprite::Sheet(4, 2).Col(3).From(0).To(0));
	SetAnimation("idle", 8);

	health = 100;
	state = NONE;
	ChangeState(IDLE);
}

CSpider::~CSpider(void)
{
    
}

void CSpider::OnAttacked()
{
	health -= 0.5f;
	if (health < 0) health = 0;
}
void CSpider::OnFire()
{
	
	health -= 0.5f;
	if (health < 0) health = 0;
	OnFirer = true;
	
}
void CSpider::NotOnFire()
{
	OnFirer = false;

}

void CSpider::OnUpdate(Uint32 time, Uint32 deltaTime)
{
	// State-dependent action
	switch (state)
	{
	case IDLE:
		health += 0.2f;
		break;
	case PATROL:	// take a random turn at a random frame, on average once every 60 frames
		if (rand() % 60 == 0)
			SetDirection(GetDirection() + (float)(rand() % 180 - 90));
		health -= 0.05f;
		break;
	case CHASE:
		SetDirection(enemyPosition - GetPosition());
		health -= 0.1f;
		break;
	case ATTACK:
		health -= 0.05f;
		break;
	case FLEE:
		SetDirection(GetPosition() - enemyPosition);
		health -= 0.1f;
		break;
	case DIE:
		break;
	}

	// Generic behaviour: bounce from the screen margins
	if (GetX() < 32 || GetX() > 1280-32) SetVelocity(Reflect(GetVelocity(), CVector(1, 0)));
	if (GetY() < 32 || GetY() > 768-32) SetVelocity(Reflect(GetVelocity(), CVector(0, 1)));

	// if in motion, rotate so that to follow the direction of the motion; otherwise reset rotation
	if (GetSpeed() > 0)
		SetRotation(GetDirection() - 90);
	else if (state != ATTACK)
		SetRotation(0);

	// Transitions
	float enemyDistance = Distance(enemyPosition, GetPosition());
	cout << state << " " << enemyDistance << endl;
	switch (state)
	{
	case IDLE:
		if (OnFirer == true) ChangeState(FLEE);
		if (health > 90 && OnFirer == false) ChangeState(PATROL);
		if (enemyDistance < 200 && health > 40 && OnFirer == false) ChangeState(CHASE);
		if (enemyDistance < 50 && OnFirer == false) ChangeState(ATTACK);
		if (health < 1) ChangeState(DIE);
		break;
	case PATROL:
		if (OnFirer == true) ChangeState(FLEE);
		if (enemyDistance < 200 && OnFirer == false) ChangeState(CHASE);
		if (health < 20 && OnFirer == false) ChangeState(IDLE);
		break;
	case CHASE:
		if (OnFirer == true) ChangeState(FLEE);
		if (enemyDistance > 250 && OnFirer == false) ChangeState(IDLE);
		if (enemyDistance < 50 && OnFirer == false) ChangeState(ATTACK);
		if (health < 30 && OnFirer == false) ChangeState(FLEE);
		break;
	case ATTACK:
		if (OnFirer == true) ChangeState(FLEE);
		if (enemyDistance > 64 && OnFirer == false) ChangeState(CHASE);
		if (health < 30 && OnFirer == false) ChangeState(FLEE);
		if (health < 1 && OnFirer == false) ChangeState(DIE);
		break;
	case FLEE:
		if (health < 10 && OnFirer == false) ChangeState(IDLE);
		if (enemyDistance > 250 && OnFirer == false) ChangeState(IDLE);
		break;
	case DIE:
		break;
	}

	CSprite::OnUpdate(time, deltaTime);
}

void CSpider::ChangeState(STATE newState)
{
	if (newState == state)
		return;		// No state change

	state = newState;

	switch (state)
	{
	case IDLE:
		SetVelocity(0, 0); 
		SetAnimation("idle", 4);
		break;
	case PATROL:
		SetDirection((float)(rand() % 360));	// turn to a random angle
		SetSpeed(PATROL_SPEED);
		SetAnimation("walk");
		break;
	case CHASE:
		SetDirection(enemyPosition - GetPosition());
		SetSpeed(CHASE_SPEED);
		SetAnimation("walk");
		break;
	case ATTACK:
		SetVelocity(0, 0);
		SetAnimation("attack");
		break;
	case FLEE:
		SetDirection(GetPosition() - enemyPosition);
		SetSpeed(CHASE_SPEED);
		SetAnimation("walk");
		break;
	case DIE:
		SetVelocity(0, 0);
		SetAnimation("death");
		break;
	}
}
