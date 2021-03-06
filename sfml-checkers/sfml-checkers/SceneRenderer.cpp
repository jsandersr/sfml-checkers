//---------------------------------------------------------------
//
// SceneRenderer.cpp
//

#include "SceneRenderer.h"
#include "Game.h"
#include "Log.h"

#include <assert.h>

SceneRenderer::SceneRenderer(sf::RenderTarget& target, Game* game)
	: m_renderTarget(&target)
	, m_game(game)
{
	BuildBoardBackground();
}

SceneRenderer::~SceneRenderer()
{
}

void SceneRenderer::Draw()
{
	m_renderTarget->clear();

	DrawBoardBackground();
	DrawBoardPieces();
}

void SceneRenderer::OnMouseClick(sf::Vector2i localPosition)
{
	// TODO:  SceneRenderer should not be handling input.

	float localX = static_cast<float>(localPosition.x);
	float localY = static_cast<float>(localPosition.y);
	LOG_DEBUG_OUTPUT_WINDOW("Mouse button Clicked: (" + std::to_string(localX) + " , "
		+ std::to_string(localY) + ") ");

	// Find the square that I clicked
	auto it = std::find_if(m_checkersSquares.begin(), m_checkersSquares.end(),
		[this, &localX, &localY](const CheckersSquare& currentSquare)
	{
		return currentSquare.m_square.getGlobalBounds().contains(localX, localY);
	});

	// Notify game of a move selection event
	m_game->OnMoveSelectionEvent(it->m_boardIndex);

	if (it == m_checkersSquares.end())
	{
		LOG_DEBUG_OUTPUT_WINDOW("Failed to find a valid location.");
	}
}

void SceneRenderer::BuildBoardBackground()
{
	std::vector<sf::Color> colorPalette(s_boardSize);

	bool colorToggle = true;
	std::generate_n(colorPalette.begin(), s_boardSize, [colorToggle] () mutable
	{
		// Alternate colors.
		if (colorToggle = !colorToggle)
			return sf::Color(209, 139, 71, 255);
		else
			return sf::Color(255, 228, 170, 255);
	});

	for (int row = 0; row < s_boardSize; ++row)
	{
		for (int col = 0; col < s_boardSize; ++col)
		{
			sf::RectangleShape currentSquare = sf::RectangleShape(sf::Vector2f(s_squareSize, s_squareSize));
			currentSquare.setPosition(row * s_squareSize, col * s_squareSize);
			currentSquare.setFillColor(colorPalette[col]);

			CheckersSquare square(currentSquare, m_game->GetBoardIndexFromRowCol(row, col));
			m_checkersSquares.push_back(square);
		}

		// Reverse the color palette to make the checker effect.
		std::reverse(colorPalette.begin(), colorPalette.end());
	}
}

void SceneRenderer::DrawBoardBackground()
{
	for (auto it = m_checkersSquares.begin(); it != m_checkersSquares.end(); ++it)
	{
		m_renderTarget->draw(it->m_square);
	}
}

void SceneRenderer::DrawBoardPieces()
{
	const BoardData& boardData = m_game->GetBoardData();
	float yPieceSpacing = 75;
	float xPieceSpacing = 75;
	float xBorderPadding = 25;
	float yBorderPadding = 25;

	for (auto row = boardData.begin(); row != boardData.end(); ++row)
	{
		int rowNumber = std::distance(boardData.begin(), row);
		for (auto col = row->begin(); col != row->end(); ++col)
		{
			// This is a super lightweight object containing only data needed for OpenGL calls.
			// Its fine to construct and destroy in this loop. I'm also okay with creating one for
			// empty spaces, as it simplifies this code quite a bit.
			sf::CircleShape piece(s_pieceSize);

			int columnNumber = std::distance(row->begin(), col);
			piece.setPosition(xBorderPadding + ((s_pieceSize + xPieceSpacing) * columnNumber),
				yBorderPadding + ((s_pieceSize + yPieceSpacing) * rowNumber));

			ApplyPieceColor(*col, piece);

			m_renderTarget->draw(piece);
		}
	}
}

void SceneRenderer::ApplyPieceColor(PieceDisplayType pieceDisplayType, sf::CircleShape& piece)
{
	switch (pieceDisplayType)
	{
	case BLACK:
		piece.setFillColor(sf::Color(117, 69, 57, 255));
		break;
	case WHITE:
		piece.setFillColor(sf::Color(246, 221, 190, 255));
		break;
	case BLACK_KING:
		piece.setFillColor(sf::Color(51, 33, 28, 255));
		break;
	case WHITE_KING:
		piece.setFillColor(sf::Color(230, 230, 230, 255));
		// TODO
		break;
	case EMPTY:
		// Do not draw blank spaces.
		piece.setFillColor(sf::Color(0, 0, 0, 0));
		break;
	default:
		// This should never be the case. If we do have an unknown piece type, I am enforcing that
		// it is added to this switch.
		assert(0);
	}
}

//--------------------------------------------------------------------------------------------------------

CheckersSquare::CheckersSquare(const sf::RectangleShape square, const BoardIndex& boardIndex)
	: m_square(square)
	, m_boardIndex(boardIndex)
{
}
