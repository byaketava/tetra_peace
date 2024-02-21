#include "Piece.h"
#include "Board.h"
#include <random>
#include <chrono>

// ãåíåðèðóåò ñëó÷àéíîå ÷èñëî îò 1 äî 7
int getRandomNumber()
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine gen(seed);
    std::uniform_int_distribution<int> dis(1, 7);
    return dis(gen);
}

Piece::Piece(int rotation, int currentPiece, Cell* shapes, int spawnX) : rotation(rotation), currentPiece(currentPiece)
{
    // êîïèðóåì âîçìîæíûå âàðèàíòû ðàñïîëîæåíèÿ ôèãóðû â ìàññèâ
    setShapes(shapes);

    // óñòàíàâëèâàåì ñòàðòîâóþ ïîçèöèþ
    this->piecePosition.setPos(spawnX, 0);
    int random = getRandomNumber();

    // ïðåäñòàâëåíèå ôèãóðû ÷åòûðüìÿ ñïðàéòàìè
    for (int i = 0; i < 4; i++) 
    {
        // åñëè ôèãóðà ýòî òåíü
        if (currentPiece == 8)
        {
            // óñòàíîâèòü òåêñòóðó òåíè
            tileSprite[i].setTextureRect(sf::IntRect(40 * currentPiece, 0, 40, 40));
            this->color = 8;
        }
        else
        {
            // óñòàíîâèòü ñëó÷àéíóþ òåêñòóðó
            tileSprite[i].setTextureRect(sf::IntRect(40 * random, 0, 40, 40));
            this->color = random;
        }
        tileSprite[i].setTexture(tiles);
        // óñòàíîâèòü ïîçèöèþ ñïðàéòà íà ýêðàíå
        tileSprite[i].setPosition((piecePosition.getX() + shape[i].getX()) * 40 + BOARD_OFFSET_X,
                                  (piecePosition.getY() + shape[i].getY()) * 40 + BOARD_OFFSET_Y);
    }
}

void Piece::setShapes(Cell* newShapes) 
{
    // êîïèðóåì â ïîëå êëàññà âàðèàíòû ðàñïîëîæåíèÿ ôèãóðû
    for (int i = 0; i < 4; i++) 
    {
        for (int j = 0; j < 4; j++) 
        {
            this->shapes[i][j] = newShapes[i * 4 + j];
        }
    }
    setCurrentShape();
}

void Piece::setCurrentShape() 
{
    // óñòàíàâëèâàåì 4 ñïðàéòà â ïîðÿäêå, óñòàíîâëåííîì òåêóùèì ïîâîðîòîì ôèãóðû
    for (int i = 0; i < 4; i++) 
    {
        this->shape[i] = shapes[rotation][i];
    }
}

void Piece::draw(sf::RenderTarget& target, sf::RenderStates states) const 
{
    // äëÿ êàæäîãî èç ÷åòûð¸õ ñïðàéòîâ
    for (int i = 0; i < 4; i++)
    {
        target.draw(tileSprite[i], states);
    }
}

Cell Piece::getPiecePosition() const 
{
    return piecePosition;
}
// ïåðåãðóçêà äëÿ îêíà ñëåäóþùåé ôèãóðû
void Piece::setPiecePosition(int x, int y, bool fixedToBoard) 
{
    piecePosition.setX(x);
    piecePosition.setY(y);

    for (int i = 0; i < 4; i++) 
    {
        // îòíîñèòåëüíî ïîëÿ (äîñêà íà÷èíàåòñÿ ñ áëîêà ãðàíèöû + îòñòóï â îïðåäåë¸ííîå êîëè÷åñòâî ïèêñåëåé)
        // + îòñòóï äî ïîëÿ èãðîâîãî ïî Y
        if (fixedToBoard)
            tileSprite[i].setPosition((piecePosition.getX() + shape[i].getX()) * 40 + BOARD_OFFSET_X,
                                      (piecePosition.getY() + shape[i].getY()) * 40 + BOARD_OFFSET_Y);
        // ê ïîëþ íå ïðèâÿçàíû ôèãóðû â îêíå next, äëÿ ðàçíûõ ôèãóð ðàçíûå îòñòóïû
        else
        {
            if (currentPiece == 4)
                tileSprite[i].setPosition((piecePosition.getX() + shape[i].getX()) * 40 + 30,
                                          (piecePosition.getY() + shape[i].getY()) * 40 + 30);
            else if (currentPiece == 1)
                tileSprite[i].setPosition((piecePosition.getX() + shape[i].getX()) * 40 + 10,
                                          (piecePosition.getY() + shape[i].getY()) * 40 + 30);
            else 
                tileSprite[i].setPosition((piecePosition.getX() + shape[i].getX()) * 40 + 30,
                                          (piecePosition.getY() + shape[i].getY()) * 40 + 10);
        }
    }
}
// ïåðåãðóçêà äëÿ îñíîâíîãî ïîëÿ
void Piece::setPiecePosition(int x, int y) 
{
    setPiecePosition(x, y, true);
}
// ïåðåãðóçêà äëÿ òåíè
void Piece::setPiecePosition(Cell x) 
{
    setPiecePosition(x.getX(), x.getY(), true);
    setCurrentShape();
}

void Piece::rotate() 
{
    int nextRotation = (this->rotation + 1) % 4;
    setRotation(nextRotation);
    setCurrentShape();
    setPiecePosition(this->getPiecePosition());
}

int Piece::getRotation() const
{
    return rotation;
}

void Piece::setRotation(int rotation) 
{
    if (rotation >= 0 && rotation < 4)
        Piece::rotation = rotation;
    else
        Piece::rotation = 0;
}

Cell* Piece::getCurrentShape() 
{
    return this->shape;
}

Cell* Piece::getRotationShape() 
{
    int nextRotation = this->rotation - 1;
    if (nextRotation < 0) 
    {
        nextRotation = 3;
    }
    return this->shapes[nextRotation];
}

int Piece::getCurrentColor() const
{
    return color;
}

int Piece::getCurrentShapeInt() const 
{
    return currentPiece;
}

Cell* Piece::getShapes() const 
{
    return (Cell*)shapes;
}
