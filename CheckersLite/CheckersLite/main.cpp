#include <Windows.h>

#include <SFML/Graphics.hpp>
#include "Board.h"
#include "User.h"


// Macros
#define IS_EVEN( n )  ( (n)%2 == 0 )



// Global Data
sf::RenderWindow	window;		// The game window
Checkers::CBoard*	game;		// The main game logic

sf::Texture	redBlockTexture;	// Texture for the red and black 
sf::Texture	blackBlockTexture;	// blocks on the background
sf::Texture	redPieceTexture;	// Texture for the red piece
sf::Texture	blackPieceTexture;	// Texture for the black piece
sf::Texture redKingTexture;		// Texture for kinged red piece
sf::Texture blackKingTexture;	// Texture for kinged black piece



// Drawing
void DrawBoard( const sf::Sprite sprites[64] );
void DrawPieces();
void DrawHighlight(sf::RectangleShape& highlightSprite);
void DrawSelected(sf::RectangleShape& selectSprite);

int main()
{
	srand(unsigned(time(0)));

	// The window
	window.create(sf::VideoMode(512, 512), "Checkers Lite");

	// Load the textures
	redBlockTexture.loadFromFile("Textures/RedBlock.png");
	blackBlockTexture.loadFromFile("Textures/BlackBlock.png");
	redPieceTexture.loadFromFile("Textures/RedPiece.png");
	blackPieceTexture.loadFromFile("Textures/BlackPiece.png");
	redKingTexture.loadFromFile("Textures/KingRed.png");
	blackKingTexture.loadFromFile("Textures/KingBlack.png");


	const int nBlockCount = BOARD_WIDTH*BOARD_HEIGHT;

	// Need a sprite for each block
	sf::Sprite background[nBlockCount];

	// INITIALIZE ALL OF THE SPRITES
	for (int i = 0; i < nBlockCount; ++i)
	{
		// Set the texture for each sprite
		// Toggle the texture to draw for each sprite to get the checker effect
		if ( IS_EVEN( i / 8 ) )
		{
			if (i % 2 == 0)
				background[i].setTexture(redBlockTexture);
			else
				background[i].setTexture(blackBlockTexture);
		}
		else
		{
			if (i % 2 == 1)
				background[i].setTexture(redBlockTexture);
			else
				background[i].setTexture(blackBlockTexture);
		}

		// Scale it up a bit!
		background[i].setScale(2, 2);

		// Create the new position for this block
		sf::Vector2f position;

		position.x = (i % 8) * 64.0f;
		position.y = (i / 8) * 64.0f;

		background[i].setPosition(position);
	}

	// Create a highlight object
	sf::RectangleShape highlight;
	highlight.setSize(sf::Vector2f(64, 64));
	highlight.setFillColor(sf::Color(128, 128, 128, 100));

	// Create a select object
	sf::RectangleShape select;
	select.setSize(sf::Vector2f(64, 64));
	select.setFillColor(sf::Color(128, 128, 255, 100));


	// Start up the game
	game = new Checkers::CBoard();
	game->Reset();

	// The Window Loop
	while (window.isOpen())
	{
		// Events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// Update the game
		game->Update();

		// Clear the render target
		window.clear();

		// Render to back buffer
		DrawBoard( background );
		DrawPieces();
		DrawSelected( select );
		DrawHighlight( highlight );
		
		// Draw the back buffer
		window.display();
	}

	delete game;
	return 0;
}


void DrawHighlight(sf::RectangleShape& highlightSprite)
{
	sf::Vector2i pos = sf::Mouse::getPosition(window);

	sf::Vector2f hiPos;

	// Snap it's position by getting rid of the snap remainders
	pos.x /= 64;
	pos.y /= 64;

	// This gets it's snapped position now
	hiPos.x = pos.x * 64.0f;
	hiPos.y = pos.y * 64.0f;

	highlightSprite.setPosition(hiPos);

	// Super draw that highlight!
	window.draw(highlightSprite);
}
void DrawSelected(sf::RectangleShape& selectSprite)
{
	User* user = nullptr;
	
	// If the player is a user we want to grab them for their selected block
	if (game->GetCurrentPlayer()->type == User::PT_User)
		user = (User*)game->GetCurrentPlayer();
	else
		return;

	// Safety check
	if ( user->GetSelectedPiece() == nullptr )
		return;

	Checkers::Position pos = user->GetSelectedPiece()->Position;
	sf::Vector2f selectPos = { (float)(pos.X * 64), (float)(pos.Y * 64) };

	selectSprite.setPosition(selectPos);

	window.draw(selectSprite);
}

void DrawPieces()
{
	std::vector< sf::Sprite > sprites;
	std::vector< Checkers::CPiece* > pieces;

	game->GetAllPieces(pieces);
	sprites.reserve(pieces.size());

	// Get all of the sprites needed to draw the pieces
	for (unsigned i = 0; i < pieces.size(); ++i)
	{
		sf::Sprite sprite;

		// Get the appropriate texture for each sprite
		if (pieces[i]->eColor == Checkers::CPiece::C_Black)
		{
			if (pieces[i]->bIsKing)
				sprite.setTexture(blackKingTexture);
			else
				sprite.setTexture(blackPieceTexture);
		}
		else
		{
			if (pieces[i]->bIsKing)
				sprite.setTexture(redKingTexture);
			else
				sprite.setTexture(redPieceTexture);
		}

		sprite.setScale(2, 2);

		// Calculate it's position
		sf::Vector2f position;
		
		position.x = pieces[i]->Position.X * 64.0f;
		position.y = pieces[i]->Position.Y * 64.0f;

		sprite.setPosition(position);

		sprites.push_back(sprite);
	}
	

	for (unsigned i = 0; i < sprites.size(); ++i)
	{
		window.draw(sprites[i]);
	}
}

void DrawBoard(const sf::Sprite sprites[64])
{
	for (int i = 0; i < 64; ++i)
	{
		window.draw(sprites[i]);
	}
}