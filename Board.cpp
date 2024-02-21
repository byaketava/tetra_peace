#include "Board.h"

void Board::initialization(int tileSize)
{
    // ïîçèöèÿ äîñêè
    this->setPosition(BOARD_OFFSET_X, BOARD_OFFSET_Y);

    // èçìåíåíèå ìàññèâà âåðøèí
    vertices.setPrimitiveType(sf::Quads);
    vertices.resize(this->boardWidth * this->boardHeight * 4);

    // èíèöèàëèçàöèÿ òåêñòóð
    updateAllTextures(tileSize);
}

void Board::draw(sf::RenderTarget& target, sf::RenderStates states) const           
{
    states.transform *= getTransform();
    states.texture = &tileset;
    target.draw(vertices, states);
}

Board::Board(int boardWidth, int boardHeight) : boardWidth(boardWidth), boardHeight(boardHeight)
{
    this->board = new int* [this->boardWidth];
    for (int i = 0; i < this->boardWidth; i++)
        this->board[i] = new int[boardHeight];

    for (int i = 0; i < this->boardWidth; i++) 
    {
        for (int j = 0; j < this->boardHeight; j++) 
        {
            if (i == 0 || i == this->boardWidth - 1 || j == this->boardHeight - 1) 
            {
                board[i][j] = WALL;
            }
            else 
            {
                board[i][j] = NONE;
            }
        }
    }
}

bool Board::collidesWith(int x, int y, Cell* shape)
{
    // êàæäûå 4 êëåòêè ôèãóðû
    for (int i = 0; i < 4; i++)
    {
        // åñëè êàêàÿ-ëèáî êëåòêà íå ïóñòà âåðíóòü èñòèíó (ñòîëêíîâåíèå îáíàðóæåíî)
        if (this->board[x + shape[i].getX()][y + shape[i].getY()] != NONE)
            return true;
    }
    // åñëè ñòîëêíîâåíèÿ íå îáíàðóæåíî âåðíóòü ëîæü
    return false;
}

bool Board::add(Piece* piece)
{
    // êàæäàÿ êëåòêà ôèãóðû çàïîëíÿåòñÿ ÷èñëîì öâåòà ôèãóðû
    for (int i = 0; i < 4; i++) 
    {
        // åñëè êëåòêà â çîíå ïîÿâëåíèÿ ôèãóð (~4 áëîêà â âûñîòó, öåíòðàëüíûå ïî èêñó), âåðíóòü false (gameover)
        if ((piece->getPiecePosition().getY() + piece->getCurrentShape()[i].getY() < 4) && 
            (piece->getPiecePosition().getX() + piece->getCurrentShape()[i].getX() >4 && piece->getPiecePosition().getX() + piece->getCurrentShape()[i].getX() < 7))
            return false;
        // èíà÷å çàïîëíèòü êëåòêó öâåòîì ôèãóðû (÷èñëî)
        board[piece->getPiecePosition().getX() + piece->getCurrentShape()[i].getX()]
             [piece->getPiecePosition().getY() + piece->getCurrentShape()[i].getY()] = piece->getCurrentColor();
        // îáíîâèòü òåêñòóðó êëåòêè
        updateTexture(piece->getPiecePosition(), piece->getCurrentShape()[i], piece->getCurrentColor(), 40);
    }
    return true;
}

void Board::updateTexture(Cell piecePos, Cell shapeCell, int color, int tileSize)
{
    // îáíîâèòü òåêñòóðó îäèíî÷íîé êëåòêè
    sf::Vertex* quad = &vertices[(piecePos.getX() + shapeCell.getX() + (piecePos.getY() + shapeCell.getY()) * this->boardWidth) * 4];
    quad[0].texCoords = sf::Vector2f(color * tileSize, 0);
    quad[1].texCoords = sf::Vector2f((color + 1) * tileSize, 0);
    quad[2].texCoords = sf::Vector2f((color + 1) * tileSize, tileSize);
    quad[3].texCoords = sf::Vector2f(color * tileSize, tileSize);
}

int Board::updateBoard() 
{
    bool FullRow = true;
    int lineCounter = 0;
    // ïîèñê ñ íèæíåé ñòðîêè (íå ó÷èòûâàÿ ãðàíèöó) äâèãàÿñü ââåðõ
    for (int y = this->boardHeight-2; y > 0; y--) 
    {
        FullRow = true;
        for (int x = 1; x < this->boardWidth - 1; x++) 
        {
            // åñëè õîòÿ áû îäíà êëåòêà ïóñòà - ëèíèÿ íå çàïîëíåíà
            if (board[x][y] == NONE)
                FullRow = false;
        }
        if (FullRow)
        {
            pushRowDown(y);
            lineCounter++;
            // âîçâðàùàåì ïîèñê â òî æå ìåñòî, íà ñëó÷àé åñëè ñëåäóþùàÿ ñòðîêà òîæå çàïîëíåíà
            y++;
        }
    }
    return lineCounter;
}

void Board::pushRowDown(int row) 
{
    for (int y = row; y > 0; y--) 
    {
        for (int x = 1; x < this->boardWidth - 1; x++) 
        {
            board[x][y] = board[x][y - 1];
            updateAllTextures(40);
        }
    }
}

void Board::updateAllTextures(int tileSize) 
{
    // íå îòðèñîâûâàþòñÿ ñòåíû
    for (int i = 1; i < this->boardWidth - 1; i++) 
    {
        for (int j = 0; j < this->boardHeight-1; j++) 
        {
            // ÷èñëî â êëåòêå - å¸ öâåò
            int tileNumber = board[i][j];
            
            // óêàçàòåëü íà òåêóùèé êâàäðàò
            sf::Vertex* quad = &vertices[(i + j * this->boardWidth) * 4];

            // îïðåäåëåíèå åãî 4 âåðøèí
            quad[0].position = sf::Vector2f(i * tileSize, j * tileSize);
            quad[1].position = sf::Vector2f((i + 1) * tileSize, j * tileSize);
            quad[2].position = sf::Vector2f((i + 1) * tileSize, (j + 1) * tileSize);
            quad[3].position = sf::Vector2f(i * tileSize, (j + 1) * tileSize);

            // îïðåäåëåíèå åãî 4õ êîîðäèíàò òåêñòóðû
            quad[0].texCoords = sf::Vector2f(tileNumber * tileSize, 0);             
            quad[1].texCoords = sf::Vector2f((tileNumber + 1) * tileSize, 0);       
            quad[2].texCoords = sf::Vector2f((tileNumber + 1) * tileSize, tileSize);
            quad[3].texCoords = sf::Vector2f(tileNumber * tileSize, tileSize);      
        }
    }
}

Board::~Board() 
{
    for (int i = 0; i < this->boardWidth; i++)
        delete[] this->board[i];
    delete[] this->board;
}
