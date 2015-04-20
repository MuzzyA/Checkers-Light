#pragma once

#include "Player.h"

namespace Checkers
{
	struct CPiece;
}

class User : public Checkers::Player
{
	// Hack to get mouse clicks :)
	bool				_bMousePreviouslyPressed;

	Checkers::CPiece*	_pSelectedPiece;

public:

	void Update(Checkers::CBoard* pGame);

	void SetSelectedPiece(Checkers::CPiece* pPiece) { _pSelectedPiece = pPiece; }
	Checkers::CPiece* GetSelectedPiece() const { return _pSelectedPiece; }


	User();
	virtual ~User();
};

