#pragma once


#include "Player.h"


class NPC : public Checkers::Player
{
	struct Move
	{
		Checkers::CPiece* piece;
		Checkers::Position moveTo;
		int rank;
	};

public:
	// Put the depth at what you wish, the higher the smarter the AI
	// 5 begins to be quite slow, at least on my PC, but it's one smart guy!
	Move RunAI( Checkers::CBoard* pGame,int depth = 3 );
	void Update( Checkers::CBoard* pGame );

	NPC();
	virtual ~NPC();
};

