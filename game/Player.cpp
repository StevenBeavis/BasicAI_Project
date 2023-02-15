#include "stdafx.h"
#include "Player.h"


CPlayer::CPlayer(Sint16 x, Sint16 y, char* pFileBitmap, Uint32 time) : CSprite(x, y, 0, 0, time)
{
	LoadAnimation(pFileBitmap, "walkR", CSprite::Sheet(13, 21).Row(9).From(0).To(8));
	LoadAnimation(pFileBitmap, "walkD", CSprite::Sheet(13, 21).Row(10).From(0).To(8));
	LoadAnimation(pFileBitmap, "walkL", CSprite::Sheet(13, 21).Row(11).From(0).To(8));
	LoadAnimation(pFileBitmap, "walkU", CSprite::Sheet(13, 21).Row(12).From(0).To(8));
	LoadAnimation(pFileBitmap, "attackR", CSprite::Sheet(13, 21).Row(13).From(0).To(5));
	LoadAnimation(pFileBitmap, "attackD", CSprite::Sheet(13, 21).Row(14).From(0).To(5));
	LoadAnimation(pFileBitmap, "attackL", CSprite::Sheet(13, 21).Row(15).From(0).To(5));
	LoadAnimation(pFileBitmap, "attackU", CSprite::Sheet(13, 21).Row(16).From(0).To(5));
	LoadImage(pFileBitmap, "idleR", CSprite::Sheet(13, 21).Tile(0, 13));
	LoadImage(pFileBitmap, "idleD", CSprite::Sheet(13, 21).Tile(0, 14));
	LoadImage(pFileBitmap, "idleL", CSprite::Sheet(13, 21).Tile(0, 15));
	LoadImage(pFileBitmap, "idleU", CSprite::Sheet(13, 21).Tile(0, 16));

	health = 100;
	state = NONE;
	ChangeState(IDLE, DOWN);
}

CPlayer::CPlayer(Sint16 x, Sint16 y, CGraphics* pGraphics, Uint32 time) : CSprite(x, y, 0, 0, time)
{
	LoadAnimation(pGraphics, "walkR", CSprite::Sheet(13, 21).Row(9).From(0).To(8));
	LoadAnimation(pGraphics, "walkD", CSprite::Sheet(13, 21).Row(10).From(0).To(8));
	LoadAnimation(pGraphics, "walkL", CSprite::Sheet(13, 21).Row(11).From(0).To(8));
	LoadAnimation(pGraphics, "walkU", CSprite::Sheet(13, 21).Row(12).From(0).To(8));
	LoadAnimation(pGraphics, "attackR", CSprite::Sheet(13, 21).Row(13).From(0).To(5));
	LoadAnimation(pGraphics, "attackD", CSprite::Sheet(13, 21).Row(14).From(0).To(5));
	LoadAnimation(pGraphics, "attackL", CSprite::Sheet(13, 21).Row(15).From(0).To(5));
	LoadAnimation(pGraphics, "attackU", CSprite::Sheet(13, 21).Row(16).From(0).To(5));
	LoadImage(pGraphics, "idleR", CSprite::Sheet(13, 21).Tile(0, 13));
	LoadImage(pGraphics, "idleD", CSprite::Sheet(13, 21).Tile(0, 14));
	LoadImage(pGraphics, "idleL", CSprite::Sheet(13, 21).Tile(0, 15));
	LoadImage(pGraphics, "idleU", CSprite::Sheet(13, 21).Tile(0, 16));

	health = 100;
	state = NONE;
	ChangeState(IDLE, DOWN);
}

CPlayer::~CPlayer(void)
{
}

void CPlayer::OnAttacked()
{
	health -= 1.0f;
	if (health < 0) health = 0;
}

void CPlayer::OnUpdate(Uint32 time, Uint32 deltaTime)
{
	CSprite::OnUpdate(time, deltaTime);
}

void CPlayer::ChangeState(STATE newState, DIR newDir)
{
	if (newState == state && newDir == dir)
		return;		// no state change
	if (newState == state && newDir == NO_DIR)
		return;		// also no state change

	state = newState;
	if (newDir != NO_DIR)
		dir = newDir;

	switch (state)
	{
	case WALK:
		switch (dir)
		{
			case UP: SetAnimation("walkU", 8); SetVelocity(0, SPEED); break;
			case DOWN: SetAnimation("walkD", 8); SetVelocity(0, -SPEED); break;
			case LEFT: SetAnimation("walkL", 8); SetVelocity(-SPEED, 0); break;
			case RIGHT: SetAnimation("walkR", 8); SetVelocity(SPEED, 0); break;
		}
		break;

	case ATTACK:
		SetVelocity(0, 0);
		switch (dir)
		{
			case UP: SetAnimation("attackU", 8); break;
			case DOWN: SetAnimation("attackD", 8); break;
			case LEFT: SetAnimation("attackL", 8); break;
			case RIGHT: SetAnimation("attackR", 8); break;
		}
		break;

	case IDLE:
		SetVelocity(0, 0);
		switch (dir)
		{
			case UP: SetImage("idleU"); break;
			case DOWN: SetImage("idleD"); break;
			case LEFT: SetImage("idleL"); break;
			case RIGHT: SetImage("idleR"); break;
		}
		break;
	}
}

void CPlayer::Input(DIR newDir, bool bAttack)
{
	STATE newState;
	if (bAttack)
		newState = ATTACK;
	else if (newDir != NO_DIR)
		newState = WALK;
	else
		newState = IDLE;

	ChangeState(newState, newDir);
}
