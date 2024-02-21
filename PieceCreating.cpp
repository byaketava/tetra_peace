#include "PieceCreating.h"

PieceCreating::PieceCreating(int spawnX): spawnX(spawnX)
{
    fillVector();
}

void PieceCreating::fillVector() 
{
    // çàïîëíåíèå âåêòîðà âñåìè âîçìîæíûìè ôèãóðàìè (èõ ïðåäñòàâëåíèÿìè â ïåðå÷èñëåíèè)
    this->pieces.push_back(IP);
    this->pieces.push_back(JP);
    this->pieces.push_back(LP);
    this->pieces.push_back(OP);
    this->pieces.push_back(SP);
    this->pieces.push_back(TP);
    this->pieces.push_back(ZP);

    // ïåðåìåøèâàíèå ôèãóð ñ ïîìîùüþ ãåíåðàòîðà ñëó÷àéíûõ ÷èñåë
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(this->pieces.begin(), this->pieces.end(), std::default_random_engine(seed));
}

Piece* PieceCreating::getPiece() 
{
    // åñëè âåêòîð ïóñò, òî çàïîëíèòü åãî çàíîâî
    if (pieces.empty())
        fillVector();

    // ïîëó÷åíèå ñëó÷àéíîé ôèãóðû
    int random = pieces[0];

    // óäàëåíèå å¸ èç âåêòîðà
    pieces.erase(pieces.begin(), pieces.begin() + 1);

    Piece* figure = nullptr;
    switch (random) 
    {
    case 1:
        figure = new IPiece(0, spawnX);
        break;
    case 2:
        figure = new JPiece(0, spawnX);
        break;
    case 3:
        figure = new LPiece(0, spawnX);
        break;
    case 4:
        figure = new OPiece(0, spawnX);
        break;
    case 5:
        figure = new SPiece(0, spawnX);
        break;
    case 6:
        figure = new TPiece(0, spawnX);
        break;
    case 7:
        figure = new ZPiece(0, spawnX);
        break;
    }
    return figure;
}

Piece* PieceCreating::getShadowPiece(Piece* currentPiece) 
{
    return new ShadowPiece(currentPiece->getRotation(), currentPiece->getShapes());
}
