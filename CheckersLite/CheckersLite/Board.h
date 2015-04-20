#pragma once

#include <vector>
#include "Piece.h"
#include "Player.h"

// Forward Declaration
struct Position;

// Defines
#define BOARD_WIDTH		8
#define BOARD_HEIGHT	8

namespace Checkers
{
	class CBoard
	{
		static std::vector< Position > NormalPossibleMoves;	// Possible 'delta' moves that a normal piece can make
		static std::vector< Position > KingPossibleMoves;	// Possible 'delta' moves that a king piece can make

	public:
		CPiece*	_board[BOARD_WIDTH][BOARD_HEIGHT];

		// Players
		Player*	_pCurrentTurn;
		Player*	_pBlackPlayer;
		Player*	_pRedPlayer;


	public:
		// Sets the game and the pieces to their initial starting places
		void Reset( Player::PlayerType RedType = Player::PT_User,Player::PlayerType BlackType = Player::PT_NPC );

		// Updates the current player.  Should be called every frame
		void Update();

		// Attempts to make a move, if it is invalid this function will return false
		// and no move will be made
		bool ApplyMove(CPiece* pPiece, const Position& MoveTo);

		// Checks to see if the given move is valid
		bool isValidMove(const CPiece* pPiece, const Position& MoveTo) const;
		
		// Returns the piece at the given location
		// Returns null if piece is not at that location
		CPiece* GetPieceAt(int x, int y) const;

		// This function is a performance hit for AI.  Returns true when the
		// current player has a jump available that they need to take.
		// When 'pPiece' is null, it searches every piece on the board for jump
		bool isJumpAvailable( const CPiece* pPiece = nullptr) const;

		// Updates a list with all of the possible moves the given piece can perform
		// Does not account for Jumps, all moves that are seemingly possible are returned
		void GetAvailableMoves(const CPiece* pPiece, std::vector< Position >& Moves) const;

		// Returns all of the pieces that are active on the board
		void GetAllPieces(std::vector< CPiece* >& board,CPiece::Color color = CPiece::C_Count) const;

		// Clear all the pieces off of the board
		void ClearBoard();

		// Useful if you want to get information about the player to display :)
		const Player* const GetCurrentPlayer() const { return _pCurrentTurn; }

		// For use with AI
		CBoard* GetCopy() const;

		CBoard();
		virtual ~CBoard();
	};
}
