#include "User.h"
#include "Board.h"

#include <SFML/window.hpp>
#include <SFML/graphics.hpp>


extern sf::RenderWindow window;  //Game Window


void User::Update(Checkers::CBoard* pGame)
{
	if ( sf::Mouse::isButtonPressed(sf::Mouse::Left) && !_bMousePreviouslyPressed )
	{
		sf::Vector2i mousePos = sf::Mouse::getPosition( window );

		// Convert to units to be used in the game
		mousePos.x /= 64;
		mousePos.y /= 64;

		// Handle the selected block
		if (_pSelectedPiece == nullptr)
		{
			Checkers::CPiece* piece = pGame->GetPieceAt(mousePos.x, mousePos.y);

			// Valid piece?
			if ( piece != nullptr && piece->eColor == this->color)
				_pSelectedPiece = piece;
		}
		else // Piece is selected
		{
			Checkers::Position position = { mousePos.x, mousePos.y };

			// Attempt to apply the move, then deselect the piece.
			// If the move failed, we just simply try again the next time around!
			pGame->ApplyMove(_pSelectedPiece, position);
			_pSelectedPiece = nullptr;
		}
	}

	// Detect clicks
	_bMousePreviouslyPressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
}


User::User() : Checkers::Player( PT_User )
{
	_bMousePreviouslyPressed = false;
	_pSelectedPiece = nullptr;
}


User::~User()
{
}
