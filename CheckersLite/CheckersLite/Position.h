#pragma once

namespace Checkers
{
	struct Position
	{
		int X, Y;

		Position(int x = 0, int y = 0)
		{
			X = x;
			Y = y;
		}

		Position operator +(const Position& position) const
		{
			Position ret;
			ret.X = X + position.X;
			ret.Y = Y + position.Y;
			return ret;
		}
		Position operator -(const Position& position) const
		{
			Position ret;
			ret.X = X - position.X;
			ret.Y = Y - position.Y;
			return ret;
		}
	};
}
