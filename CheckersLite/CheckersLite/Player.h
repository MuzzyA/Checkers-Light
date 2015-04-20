#pragma once

#include "Piece.h"

namespace Checkers
{
	class CBoard;

	struct Player
	{
		enum PlayerType { PT_User, PT_NPC };

		PlayerType		type;
		CPiece::Color	color;

		virtual void Update(CBoard* pGame) = 0;

		Player( PlayerType t )
		{
			type = t;
			color = CPiece::C_Black;
		}
		virtual ~Player() = 0{}
	};
}
