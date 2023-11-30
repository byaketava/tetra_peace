#pragma once
#include <SFML/Graphics.hpp>
#include "Enums.h"
#include "Cell.h"
#include "Piece.h"
#define BOARD_OFFSET_X 210
#define BOARD_OFFSET_Y 50

class Board : public sf::Drawable, sf::Transformable 
{
private:
    int boardWidth;     // ширина поля
    int boardHeight;    // высота поля
    int** board;        // массив поля

    // текстура клеток
    sf::Texture tileset = Resources::getTexture("res/tiles.png");

    // каждая клетка доски будет представлена 4 вершинами
    sf::VertexArray vertices;

    // обновляет текстуру клетки доски
    void updateTexture(Cell piecePos, Cell shapeCell, int color, int tileSize);

    // обновляет все клетки
    void updateAllTextures(int tileSize);
public:
    // конструктор класса
    Board(int boardWidth, int boardHeight);
    // деструктор класса
    ~Board();

    // создает массив вершин, представляющий доску.
    // добавляет текстуры спользуя метод UpdateAllTextures()
    void initialization(int tileSize);

    // проверка столкновений с другими фигурами/границами доски
    // возвращает false, если столкновений не обнаружено
    // возвращает true, если любая из клеток фигуры столкнулась с чем-либо
    bool collidesWith(int x, int y, Cell* shape);
    
    // добавляет фигуру на доску и обновляет текстуру доски
    // возвращает false, если игра окончена (фигура была добавлена в запрещенную зону по Y)
    // возвращает true, если фигура была добавлена успешно
    bool add(Piece* piece);

    // удаляет заполненные линии, возвращает число удалённых за раз линий
    int updateBoard();

    // опускает все линии выше удаляемой вниз, обновляет текстуры
    void pushRowDown(int row);

protected:
    // отрисовывает поле
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};