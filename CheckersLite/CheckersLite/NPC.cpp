#include "NPC.h"
#include "Position.h"
#include "Board.h"

#include <Windows.h>

int Evaluate(Checkers::CBoard* pGame);


NPC::NPC() : Player( Player::PT_NPC )
{
}


NPC::~NPC()
{
}


void NPC::Update(Checkers::CBoard* pGame)
{
	Move bestMove = RunAI(pGame);

	pGame->ApplyMove(bestMove.piece, bestMove.moveTo);

	// So we can actually observe the AI's movement
	//Sleep(700);
}

NPC::Move NPC::RunAI( Checkers::CBoard* pGame,int depth )
{

	Checkers::CBoard* copy = nullptr;
	Move retMove = { nullptr };

	std::vector< Checkers::CPiece* > pieces;

	pGame->GetAllPieces(pieces, pGame->GetCurrentPlayer()->color);

	for (unsigned i = 0; i < pieces.size(); ++i)
	{
		std::vector< Checkers::Position > moves;
		pGame->GetAvailableMoves(pieces[i], moves);

		for (unsigned j = 0; j < moves.size(); ++j)
		{
			// If the move is a jump, we need to enforce it
			if (abs(moves[j].X - pieces[i]->Position.X) == 2)
			{
				retMove.piece = pieces[i];
				retMove.moveTo = moves[j];
			}

			// Need a copy of the game to test on
			copy = pGame->GetCopy();
			
			Checkers::CPiece* piece = copy->GetPieceAt(pieces[i]->Position.X, pieces[i]->Position.Y);

			copy->ApplyMove(piece, moves[j]);

			int rank = 0;
			
			if (depth > 0)
				rank = RunAI(copy, depth - 1).rank;
			else
				rank = Evaluate(copy);

			// See if this move beats the best
			if (color == Checkers::CPiece::C_Black)
			{
				if (rank < retMove.rank)
				{
					retMove.piece = pGame->GetPieceAt(pieces[i]->Position.X, pieces[i]->Position.Y);
					retMove.moveTo = moves[j];
					retMove.rank = rank;
				}
			}
			else
			{
				if (rank > retMove.rank)
				{
					retMove.piece = pGame->GetPieceAt(pieces[i]->Position.X, pieces[i]->Position.Y);
					retMove.moveTo = moves[j];
					retMove.rank = rank;
				}
			}

			// In case there is no move selected as the best move yet ( first iteration )
			if (retMove.piece == nullptr)
			{
				retMove.piece = pGame->GetPieceAt(pieces[i]->Position.X, pieces[i]->Position.Y);
				retMove.moveTo = moves[j];
				retMove.rank = rank;
			}


			delete copy;
			copy = nullptr;
		}
	}


	return retMove;
}


int Evaluate(Checkers::CBoard* pGame)
{
	std::vector< Checkers::CPiece* > pieces;

	pGame->GetAllPieces(pieces);

	int score = 0;

	for (unsigned i = 0; i < pieces.size(); ++i)
	{
		if (pieces[i]->eColor == Checkers::CPiece::C_Black)
		{
			if (pieces[i]->bIsKing)
				score -= 200;
			else
				score -= 50;
		}
		else
		{
			if (pieces[i]->bIsKing)
				score += 200;
			else
				score += 50;
		}
	}

	return score;
}