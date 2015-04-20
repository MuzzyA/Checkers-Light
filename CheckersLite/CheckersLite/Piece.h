#pragma once

#include "Position.h"

namespace Checkers
{
	struct CPiece
	{
		enum Color { C_Red, C_Black, C_Count };

		bool		bIsKing;
		Color		eColor;
		Position	Position;

		CPiece()
		{
			bIsKing = false;
			eColor = C_Black;
			Position.X = 0;
			Position.Y = 0;
		}
	};
}