#include "Board.h"
#include "Piece.h"
#include "Position.h"
#include "Player.h"
#include "User.h"
#include "NPC.h"


namespace Checkers
{
	std::vector< Position > CBoard::NormalPossibleMoves;
	std::vector< Position > CBoard::KingPossibleMoves;

	void CBoard::Update()
	{
		// Safety Check
		if (_pCurrentTurn == nullptr)
			return;

		_pCurrentTurn->Update(this);
	}

	void CBoard::Reset(Player::PlayerType RedType, Player::PlayerType BlackType)
	{
		// Get rid of the old players and pieces on the board
		ClearBoard();

		// Get new players
		if (RedType == Player::PT_User)
			_pRedPlayer = new User();
		else
			_pRedPlayer = new NPC();

		if (BlackType == Player::PT_User)
			_pBlackPlayer = new User();
		else
			_pBlackPlayer = new NPC();

		// Colors
		_pRedPlayer->color = CPiece::C_Red;
		_pBlackPlayer->color = CPiece::C_Black;

		// This toggle will help align the pieces on the black blocks only
		int toggle = 1;

		// Set up the new board
		for (int j = 0; j < 3; ++j)
		{

			for (int i = toggle; i < BOARD_WIDTH; i += 2)
			{
				_board[i][j] = new CPiece();
				_board[i][j]->Position.X = i;
				_board[i][j]->Position.Y = j;
			}

			toggle ^= 1;
		}

		toggle = 0;
		for (int j = BOARD_HEIGHT - 1; j >= 5; --j)
		{
			for (int i = toggle; i < BOARD_WIDTH; i += 2)
			{
				_board[i][j] = new CPiece();
				_board[i][j]->eColor = CPiece::C_Red;

				_board[i][j]->Position.X = i;
				_board[i][j]->Position.Y = j;
			}

			toggle ^= 1;
		}

		// Red always goes first
		_pCurrentTurn = _pRedPlayer;
	}

	bool CBoard::ApplyMove(CPiece* pPiece, const Position& MoveTo)
	{
		if (!isValidMove(pPiece, MoveTo))
			return false;

		Position curPos = pPiece->Position;
		Position deltaMove = MoveTo - pPiece->Position;

		// Is there a Jump that needs to be performed?
		if (isJumpAvailable())
		{
			if (abs(deltaMove.X) == 1)
				return false; // MUST make your jump!
		}

		// Apply the move
		_board[curPos.X][curPos.Y] = nullptr;
		_board[MoveTo.X][MoveTo.Y] = pPiece;

		pPiece->Position = MoveTo;

		// Will be set to false if there are more jumps the player needs to take
		bool isTurnOver = true;

		// If there was a piece jumped we need to grab it and remove it
		if (abs(deltaMove.X) == 2)
		{
			deltaMove.X /= 2;
			deltaMove.Y /= 2;

			// remove the piece that was jumped
			delete _board[curPos.X + deltaMove.X][curPos.Y + deltaMove.Y];
			_board[curPos.X + deltaMove.X][curPos.Y + deltaMove.Y] = nullptr;

			// If there's another jump that is available for this piece, the player needs to continue
			if (isJumpAvailable(pPiece))
				isTurnOver = false;
		}

		// Check for KING
		if ((pPiece->Position.Y == 0 && pPiece->eColor == CPiece::C_Red) ||
			(pPiece->Position.Y == 7 && pPiece->eColor == CPiece::C_Black))
		{
			if (!pPiece->bIsKing)
			{// KING!
				// This ends the players turn
				isTurnOver = true;
				pPiece->bIsKing = true;
			}
		}

		// Turn will not be over if there are more jumps in the sequence
		if (!isTurnOver)
			return true;

		// Next player's turn
		if (_pCurrentTurn == _pBlackPlayer)
			_pCurrentTurn = _pRedPlayer;
		else
			_pCurrentTurn = _pBlackPlayer;

		return true;
	}

	bool CBoard::isValidMove(const CPiece* pPiece, const Position& MoveTo) const
	{
		// Safety check 1
		if (pPiece == nullptr)
			return false;

		// Safety check 2: Don't go out of bounds
		if (MoveTo.X < 0 || MoveTo.X >= BOARD_WIDTH ||
			MoveTo.Y < 0 || MoveTo.Y >= BOARD_HEIGHT)
			return false;

		// Make sure there's not already a piece where they're moving to
		if (_board[MoveTo.X][MoveTo.Y] != nullptr)
			return false;

		// Make sure it's the correct piece
		if (pPiece->eColor != _pCurrentTurn->color)
			return false;

		// The direction moved
		Position deltaMove = MoveTo - pPiece->Position;

		// Make sure it's a diagonal move
		if (abs(deltaMove.X) != abs(deltaMove.Y))
			return false;

		// Are they trying to move backwards?
		if (deltaMove.Y < 0 && pPiece->eColor == CPiece::C_Black)
		{
			// Not allowed to do that unless you are KING!
			if (!pPiece->bIsKing)
				return false;
		}
		else if (deltaMove.Y > 0 && pPiece->eColor == CPiece::C_Red)
		{
			// Not allowed to do that unless you are KING!
			if (!pPiece->bIsKing)
				return false;
		}

		// At this point if they only moved 1 space, it's a success
		if (abs(deltaMove.X) == 1)
			return true;

		// If they moved 2 spaces, do some Piece jumping checks
		if (abs(deltaMove.X) == 2)
		{
			Position deltaMoveHalf = { deltaMove.X / 2, deltaMove.Y / 2 };
			Position jumpPosition = pPiece->Position + deltaMoveHalf;

			// See if there is a piece at block that was jumped
			if (_board[jumpPosition.X][jumpPosition.Y] == nullptr)
				return false;  // Cannot jump an empty block

			// See if the piece that jumped was of the opposite color
			if (_board[jumpPosition.X][jumpPosition.Y]->eColor == pPiece->eColor)
				return false;  // Cannot jump your own piece
		}

		// Move is Valid
		return true;
	}

	bool CBoard::isJumpAvailable(const CPiece* pPiece) const
	{
		// If pPiece is valid, we will only search that
		if (pPiece)
		{
			// Now find it's possible moves
			std::vector< Position > moves;

			GetAvailableMoves(pPiece, moves);

			// See if there is a jump in these moves
			for (unsigned i = 0; i < moves.size(); ++i)
			{
				// Check for jump
				if (abs(moves[i].X - pPiece->Position.X) == 2)
					return true;  // Go ahead and return true to stop all of the looping
			}
		}
		else
		{
			// Check all of the pieces owned by the current player's turn for jumps
			for (int i = 0; i < BOARD_WIDTH; ++i)
			{
				for (int j = 0; j < BOARD_HEIGHT; ++j)
				{
					// Find a piece
					CPiece* piece = _board[i][j];
					if (piece != nullptr)
					{
						// See if it's the current color's turn
						if (piece->eColor == _pCurrentTurn->color)
						{
							// Now find it's possible moves
							std::vector< Position > moves;

							GetAvailableMoves(piece, moves);

							// See if there is a jump in these moves
							for (unsigned k = 0; k < moves.size(); ++k)
							{
								// Check for jump
								if (abs(moves[k].X - piece->Position.X) == 2)
									return true;  // Go ahead and return true to stop all of the looping
							}
						}
					}
				}
			}
		}

		return false;
	}

	void CBoard::ClearBoard()
	{
		// Clear the board
		for (int i = 0; i < BOARD_WIDTH; ++i)
		{
			for (int j = 0; j < BOARD_HEIGHT; ++j)
			{
				delete _board[i][j];
				_board[i][j] = nullptr;
			}
		}

		delete _pRedPlayer;
		delete _pBlackPlayer;

		_pRedPlayer = nullptr;
		_pBlackPlayer = nullptr;
		_pCurrentTurn = nullptr;
	}

	void CBoard::GetAvailableMoves(const CPiece* pPiece, std::vector< Position >& Moves) const
	{
		// Safety check
		if (pPiece == nullptr)
			return;

		// Multiplier for the 'Y' in case the piece is red
		int multY = 1;

		// Need to inverse the 'Y' if the piece is red
		if (pPiece->eColor == CPiece::C_Red)
			multY = -1;


		if (pPiece->bIsKing)
		{
			// Check for possible moves
			for (unsigned i = 0; i < KingPossibleMoves.size(); ++i)
			{
				// Add valid moves to the 'In vector'
				if (isValidMove(pPiece, pPiece->Position + KingPossibleMoves[i]))
					Moves.push_back(pPiece->Position + KingPossibleMoves[i]);
			}
		}
		else
		{
			// Check for possible moves
			for (unsigned i = 0; i < NormalPossibleMoves.size(); ++i)
			{
				// Account for red pieces
				Position pos = { NormalPossibleMoves[i].X, multY*NormalPossibleMoves[i].Y };

				// Add valid moves to the 'In vector'
				if (isValidMove(pPiece, pPiece->Position + pos))
					Moves.push_back(pPiece->Position + pos);
			}
		}
	}

	void CBoard::GetAllPieces(std::vector< CPiece* >& board,CPiece::Color color) const
	{
		// Should we get all of the pieces?
		if (color == CPiece::C_Count)
		{
			for (int i = 0; i < 8; ++i)
			{
				for (int j = 0; j < 8; ++j)
				{
					// Find all of the pieces, add them to the list
					if (_board[i][j] != nullptr)
						board.push_back(_board[i][j]);
				}
			}
		}
		else
		{
			// Only grab the piece from one side
			for (int i = 0; i < 8; ++i)
			{
				for (int j = 0; j < 8; ++j)
				{
					// Find all of the pieces that match the 'In' color, add them to the list
					if (_board[i][j] != nullptr && _board[i][j]->eColor == color )
						board.push_back(_board[i][j]);
				}
			}
		}
	}

	CPiece* CBoard::GetPieceAt(int x, int y) const
	{
		if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT)
			return nullptr;

		return _board[x][y];
	}


	CBoard* CBoard::GetCopy() const
	{
		CBoard* copy = new CBoard();

		copy->ClearBoard();

		// Leaving these commented out for now, because it doesn't seem like I need them
		// Players
		if (_pRedPlayer->type == Player::PT_User)
			copy->_pRedPlayer = new User();
		else
			copy->_pRedPlayer = new NPC();

		if (_pBlackPlayer->type == Player::PT_User)
			copy->_pBlackPlayer = new User();
		else
			copy->_pBlackPlayer = new NPC();

		// Colors
		copy->_pRedPlayer->color = CPiece::C_Red;
		copy->_pBlackPlayer->color = CPiece::C_Black;

		// Whose turn
		if (_pCurrentTurn->color == CPiece::C_Red)
			copy->_pCurrentTurn = copy->_pRedPlayer;
		else if (_pCurrentTurn->color == CPiece::C_Black)
			copy->_pCurrentTurn = copy->_pBlackPlayer;

		// Set up the new board
		for (int i = 0; i < BOARD_WIDTH; ++i)
		{
			for (int j = 0; j < BOARD_HEIGHT; ++j)
			{
				if ( _board[i][j] != nullptr )
				{
					copy->_board[i][j] = new CPiece();
					copy->_board[i][j]->Position.X = i;
					copy->_board[i][j]->Position.Y = j;
					copy->_board[i][j]->bIsKing = _board[i][j]->bIsKing;
					copy->_board[i][j]->eColor = _board[i][j]->eColor;
				}
			}
		}

		return copy;
	}

	// Constructor
	CBoard::CBoard()
	{
		// Default
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				_board[i][j] = nullptr;
			}
		}

		// Constant list of all of the moves possible for each normal piece

		if (NormalPossibleMoves.size() == 0)
		{
			NormalPossibleMoves.push_back(Position(1, 1));
			NormalPossibleMoves.push_back(Position(-1, 1));
			NormalPossibleMoves.push_back(Position(2, 2));
			NormalPossibleMoves.push_back(Position(-2, 2));
		}

		// Constant list of all of the moves possible for each king piece
		if (KingPossibleMoves.size() == 0)
		{
			KingPossibleMoves.push_back(Position(1, 1));
			KingPossibleMoves.push_back(Position(-1, 1));
			KingPossibleMoves.push_back(Position(1, -1));
			KingPossibleMoves.push_back(Position(-1, -1));
			KingPossibleMoves.push_back(Position(2, 2));
			KingPossibleMoves.push_back(Position(-2, 2));
			KingPossibleMoves.push_back(Position(2, -2));
			KingPossibleMoves.push_back(Position(-2, -2));
		}
	}

	// Destructor
	CBoard::~CBoard()
	{
		ClearBoard();
	}

}
