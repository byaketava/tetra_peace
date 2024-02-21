#include"Game.h"

Game::Game  (sf::RenderWindow* window, int boardWidth, int boardHeight, 
            bool volume, int* gamePartPtr, int* scorePtr,
            int* levelPtr, int* linesPtr, bool shadowFlag) 
{
    // øèðèíà + 2 (áîêîâûå ãðàíèöû)
    this->boardWidth    = boardWidth + 2;
    // âûñîòà + 1 (íèæíÿÿ ãðàíèöà)
    this->boardHeight   = boardHeight + 1;

    // èíèöèàëèçàöèÿ îáúåêòîâ êëàññîâ ïîëÿ è ñîçäàíèÿ ôèãóð
    this->gameBoard     = new Board(this->boardWidth, this->boardHeight);
    this->pieceCreating = new PieceCreating(this->boardWidth / 2 - 2);

    // îïðåäåëåíèå âñåõ óêàçàòåëåé
    this->window        = window;
    this->volume        = volume;
    this->gamePartPtr   = gamePartPtr;
    this->scorePtr      = scorePtr;
    this->levelPtr      = levelPtr;
    this->linesPtr      = linesPtr;
    this->shadowFlag    = shadowFlag;

    // îòîáðàæåíèå ðåéòèíãà
    scoreText.setFont(textFont);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color(252, 242, 231, 255));
    
    setScore(score);

    // îòîáðàæåíèå êîëè÷åñòâà óáðàííûõ ëèíèé
    linesText.setFont(textFont);
    linesText.setCharacterSize(24);
    linesText.setFillColor(sf::Color(252, 242, 231, 255));

    setLines(totalRows);

    // îïðåäåëåíèå îêíà ïàóçû
    pauseWindowTexture = Resources::getTexture("res/gamePaused.png");
    pauseWindow.setTexture(pauseWindowTexture);	

    // êíîïêè
    pauseButton = new Button(sf::Vector2f(680, 330), Resources::getTexture("res/pauseNormal.png"), Resources::getTexture("res/pausePressed.png"));
    volumeButton = new Button(sf::Vector2f(80, 330), Resources::getTexture("res/volumeon.png"), Resources::getTexture("res/volumeoff.png"));
    shadowButton = new Button(sf::Vector2f(80, 430), Resources::getTexture("res/shadowNormal.png"), Resources::getTexture("res/shadowPressed.png"));
}

void Game::run()
{
    // èíèöèàëèçàöèÿ ïîëÿ
    gameBoard->initialization(40);

    // ôîí èãðû
    sf::Sprite background;
    sf::Texture backgroundTexture = Resources::getTexture("res/background.png");
    background.setTexture(backgroundTexture);

    // ïîëó÷åíèå òåêóùåé ôèãóðû
    Piece* currentPiece = pieceCreating->getPiece();
    // ïîëó÷åíèå ñëåäóþùåé ôèãóðû
    Piece* nextPiece = pieceCreating->getPiece();
    // ïîëó÷åíèå òåíè ôèãóðû
    Piece* shadowPiece = pieceCreating->getShadowPiece(currentPiece);
    setShadowPosition(currentPiece, shadowPiece);

    // îêíî ñëåäóþùåé ôèãóðû
    NextPieceBoard nextBoard(nextPiece, 40);

    if (volume)
        GameSound::play(0);
    else
        GameSound::stop(0);

    // ôëàã áûñòðîãî ïàäåíèÿ
    bool fastFallFlag = false;
    // ôëàã ìãíîâåííîãî ïàäåíèÿ
    bool instantFall = false;
    // ôëàã ïàóçû
    bool pausedFlag = false;

    // óñòàíîâêà ÷àñîâ äëÿ ïîäñ÷¸òà âðåìåíè
    sf::Clock frameClock;
    sf::Clock keyClock;
    sf::Time frameTime = frameClock.getElapsedTime();
    sf::Time keyTime = keyClock.getElapsedTime();

    // îñíîâíîé èãðîâîé öèêë
    while (window->isOpen())
    {
        // ïîçèöèÿ ìûøè
        sf::Vector2f mousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));

        // âõîä â îñíîâíîå ìåíþ
        if (*gamePartPtr == MENU)
        {
            GameSound::stop(0);
            break;
        }

        // åñëè èñòèíåí ôëàã êîíöà èãðû
        if (gameOver)
        {
            GameSound::stop(0);
            *gamePartPtr = GAMEOVER;
            *levelPtr = level;
            *scorePtr = score;
            *linesPtr = totalRows;

            delete currentPiece;
            delete nextPiece;
            delete shadowPiece;

            break;
        }

        sf::Event event;

        // îáðàáîòêà ñîáûòèé â î÷åðåäè (çàêðûòèå îêíà è êëàâèøè ïîâîðîòà ôèãóðû/âûõîäà/ïàóçû)
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window->close();

            // åñëè èãðà íå íà ïàóçå
            if (!pausedFlag)
            {
                // íàæàòà êíîïêà ïàóçû, òî ïîñòàâèòü èãðó íà ïàóçó
                if (pauseButton->updateButton(mousePos))
                {
                    GameSound::play(1);
                    GameSound::stop(0);
                    pausedFlag = !pausedFlag;
                }
                // íàæàòà êíîïêà çâóêà, òî âêëþ÷èòü/âûêëþ÷èòü çâóê
                else if (volumeButton->updateButton(mousePos))
                {
                    GameSound::play(1);
                    volume = !volume;
                    if (volume)
                        GameSound::play(0);
                    else
                        GameSound::stop(0);
                }
                // íàæàòà êíîïêà òåíè, òî âêëþ÷èòü/âûêëþ÷èòü òåíü
                else if (shadowButton->updateButton(mousePos))
                {
                    GameSound::play(1);
                    shadowFlag = !shadowFlag;
                }
            }
            // åñëè èãðà íà ïàóçå, òî ïðîäîëæèòü èãðàòü ìîæíî íàæàâ ââîä/ïðîáåë (â óñëîâèè äàëüøå åù¸ P)
            if (pausedFlag && (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)))
            {
                pausedFlag = !pausedFlag;
                if (volume)
                    GameSound::play(0);
                else
                    GameSound::stop(0);
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                {
                    if (!pausedFlag)
                        rotate(currentPiece, shadowPiece);
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    *gamePartPtr = MENU;
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
                {
                    pausedFlag = !pausedFlag;
                    if (pausedFlag)
                        GameSound::stop(0);
                    else if (volume)
                        GameSound::play(0);
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
                {
                    shadowFlag = !shadowFlag;
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
                {
                    volume = !volume;
                    if (volume)
                        GameSound::play(0);
                    else
                        GameSound::stop(0);
                }
            }
        }

        // îáðàáîòêà ñîáûòèé íåïðåðûâíîãî ââîäà êëàâèø (äëÿ áîëåå ïëàâíîé èãðû íåçàâèñèìî îò î÷åðåäè ñîáûòèé)
        keyTime = keyClock.getElapsedTime();

        if (!pausedFlag)
        {
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                && keyTime.asSeconds() > 0.04)
            {
                fastFallFlag = true;
                keyClock.restart();
            }
            else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::F))
                && keyTime.asSeconds() > 0.2)
            {
                instantFall = true;
                keyClock.restart();
            }
            else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                && keyTime.asSeconds() > 0.1)
            {
                moveLeft(currentPiece, shadowPiece);
                keyClock.restart();
            }
            else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                && keyTime.asSeconds() > 0.1)
            {
                moveRight(currentPiece, shadowPiece);
                keyClock.restart();
            }
        }

        // îòðèñîâêà
        window->clear();
        window->draw(background);
        window->draw(nextBoard);
        window->draw(*gameBoard);

        window->draw(scoreText);
        window->draw(linesText);

        window->draw(*pauseButton);
        window->draw(*volumeButton);
        window->draw(*shadowButton);
        if (shadowFlag)
            window->draw(*shadowPiece);
        window->draw(*currentPiece);
        if (pausedFlag)
            window->draw(pauseWindow);

        window->display();

        // ïàäåíèå ôèãóð
        frameTime = frameClock.getElapsedTime();
        if (!pausedFlag && (fastFallFlag || (frameTime.asSeconds() > 1 - (level - 1) * 0.025)))
        {
            frameClock.restart();

            // åñëè ïàäåíèå ôèãóðû íà 1 âíèç íåâîçìîæíî
            if (!fallDown(currentPiece))
            {
                // add âîçâðàùàåò true åñëè ôèãóðà äîáàâëåíà óñïåøíî
                gameOver = !gameBoard->add(currentPiece);

                delete currentPiece;
                delete shadowPiece;

                // ñëåäóþùàÿ ôèãóðà ñòàíîâèòñÿ òåêóùåé
                currentPiece = nextPiece;
                currentPiece->setPiecePosition(this->boardWidth / 2 - 1, 0);

                // íîâàÿ ñëåäóþùàÿ ôèãóðà
                nextPiece = pieceCreating->getPiece();
                nextBoard.setPiece(nextPiece);

                // íîâàÿ ôèãóðà òåíè
                shadowPiece = pieceCreating->getShadowPiece(currentPiece);

                // óäàëåíèå çàïîëíåííûõ ëèíèé
                // ïîëó÷àåì ÷èñëî î÷èùåííûõ ñòðîê ñ ïîìîùüþ ìåòîäà updateBoard()
                int clearedRows = gameBoard->updateBoard();

                if (clearedRows)
                    GameSound::play(2);

                // îáíîâëåíèå ïîçèöèè òåíè ôèãóðû
                setShadowPosition(currentPiece, shadowPiece);

                // îáíîâëåíèå ðåéòèíãà è óðîâíÿ
                this->totalRows += clearedRows;

                updateScore(clearedRows);
                updateLevel();
            }
            fastFallFlag = false;

            // îáíîâëåíèå îòîáðàæåíèÿ ðåéòèíãà è êîëè÷åñòâà óáðàííûõ ëèíèé íà ýêðàíå
            setScore(score);
            setLines(totalRows);
        }
        if (!pausedFlag && instantFall)
        {
            // óâåëè÷åíèå ðåéòèíãà ïðè ìãíîâåííîì ïàäåíèè
            this->score += 10;

            currentPiece->setPiecePosition(shadowPiece->getPiecePosition().getX(), shadowPiece->getPiecePosition().getY());

            // add âîçâðàùàåò true åñëè ôèãóðà äîáàâëåíà óñïåøíî
            gameOver = !gameBoard->add(currentPiece);

            delete currentPiece;
            delete shadowPiece;

            // ñëåäóþùàÿ ôèãóðà ñòàíîâèòñÿ òåêóùåé
            currentPiece = nextPiece;
            currentPiece->setPiecePosition(this->boardWidth / 2 - 1, 0);

            // íîâàÿ ñëåäóþùàÿ ôèãóðà
            nextPiece = pieceCreating->getPiece();
            nextBoard.setPiece(nextPiece);

            // íîâàÿ ôèãóðà òåíè
            shadowPiece = pieceCreating->getShadowPiece(currentPiece);

            // óäàëåíèå çàïîëíåííûõ ëèíèé
            // ïîëó÷àåì ÷èñëî î÷èùåííûõ ñòðîê ñ ïîìîùüþ ìåòîäà updateBoard()
            int clearedRows = gameBoard->updateBoard();

             if (clearedRows)
                 GameSound::play(2);

            // îáíîâëåíèå ïîçèöèè òåíè ôèãóðû
            setShadowPosition(currentPiece, shadowPiece);

            // îáíîâëåíèå ðåéòèíãà è óðîâíÿ
            this->totalRows += clearedRows;

            updateScore(clearedRows);
            updateLevel();

            instantFall = false;

            // îáíîâëåíèå îòîáðàæåíèÿ ðåéòèíãà è êîëè÷åñòâà óáðàííûõ ëèíèé íà ýêðàíå
            setScore(score);
            setLines(totalRows);
        }
    }
}

bool Game::moveLeft(Piece* piece, Piece* shadowPiece) 
{
    // åñëè íå áóäåò ñòîëêíîâåíèÿ ïðè äâèæåíèè âëåâî
    if (!gameBoard->collidesWith(piece->getPiecePosition().getX() - 1,
                                 piece->getPiecePosition().getY(),
                                 piece->getCurrentShape())) 
    {
        // óñòàíîâèòü íîâóþ ïîçèöèþ ôèãóðû íà ïîëå
        piece->setPiecePosition(piece->getPiecePosition().getX() - 1, 
                                piece->getPiecePosition().getY());
        // óñòàíîâèòü íîâóþ ïîçèöèþ òåíè íà ïîëå
        setShadowPosition(piece, shadowPiece);
        return true;
    }
    return false;
}

bool Game::moveRight(Piece* piece, Piece* shadowPiece)
{
    // åñëè íå áóäåò ñòîëêíîâåíèÿ ïðè äâèæåíèè âïðàâî
    if (!gameBoard->collidesWith(piece->getPiecePosition().getX() + 1,
                                 piece->getPiecePosition().getY(),
                                 piece->getCurrentShape())) 
    {
        // óñòàíîâèòü íîâóþ ïîçèöèþ ôèãóðû íà ïîëå
        piece->setPiecePosition(piece->getPiecePosition().getX() + 1, 
                                piece->getPiecePosition().getY());
        // óñòàíîâèòü íîâóþ ïîçèöèþ òåíè íà ïîëå
        setShadowPosition(piece, shadowPiece);
        return true;
    }
    return false;
}

bool Game::rotate(Piece* piece, Piece* shadowPiece)
{
    // åñëè íå áóäåò ñòîëêíîâåíèÿ ïîñëå ïîâîðîòà
    if (!gameBoard->collidesWith(piece->getPiecePosition().getX(),
                                 piece->getPiecePosition().getY(),
                                 piece->getRotationShape())) 
    {
        // ïîâåðíóòü ôèãóðó
        piece->rotate();
        // ïîâåðíóòü òåíü
        setShadowPosition(piece, shadowPiece);
        return true;
    }
    return false;
}
//
bool Game::fallDown(Piece* piece) 
{
    // åñëè ïîñëå ïàäåíèÿ íå áóäåò ñòîëêíîâåíèÿ
    if (!gameBoard->collidesWith(piece->getPiecePosition().getX(),
                                 piece->getPiecePosition().getY() + 1,
                                 piece->getCurrentShape())) 
    {
        // îïóñòèòü ôèãóðó
        piece->setPiecePosition(piece->getPiecePosition().getX(), piece->getPiecePosition().getY() + 1);
        return true;
    }
    return false;
}

void Game::setShadowPosition(Piece* currentPiece, Piece* shadowPiece)
{
    // óñòàíîâèòü ïîâîðîò òåíè òàêèì æå êàê ó ôèãóðû
    shadowPiece->setRotation(currentPiece->getRotation());
    // óñòàíîâèòü ïîçèöèþ òåíè òàêîé æå êàê ó ôèãóðû
    shadowPiece->setPiecePosition(currentPiece->getPiecePosition());

    // íàéòè íèæàéøåå âîçìîæíîå ïîëîæåíèå òåíè
    // (äâèæåíèå âíèç äî îáíàðóæåíèÿ ñòîëêíîâåíèÿ)
    while (!gameBoard->collidesWith(shadowPiece->getPiecePosition().getX(),
                                    shadowPiece->getPiecePosition().getY() + 1,
                                    shadowPiece->getCurrentShape())) 
    {
        shadowPiece->setPiecePosition(shadowPiece->getPiecePosition().getX(),
                                      shadowPiece->getPiecePosition().getY() + 1);
    }
}
//
void Game::updateScore(int clearedRows) 
{
    // óâåëè÷åíèå ðåéòèíãà íà îñíîâàíèè êîëè÷åñòâà óäàë¸ííûõ ëèíèé
    switch (clearedRows) 
    {
    case 1:
        this->score += 50 * this->level;
        break;
    case 2:
        this->score += 150 * this->level;
        break;
    case 3:
        this->score += 350 * this->level;
        break;
    case 4:
        this->score += 750 * this->level;
        break;
    default:
        break;
    }
}

void Game::updateLevel() 
{
    this->level = this->totalRows / 5 + 1;
}

void Game::setScore(int score)
{
    scoreText.setString(std::to_string(score));
    sf::FloatRect rectangle(90, 100, 120, 50);
    sf::FloatRect textBounds = scoreText.getGlobalBounds();
    float X = rectangle.left + (rectangle.width - textBounds.width) / 2.f;
    float Y = rectangle.top + (rectangle.height - textBounds.height) / 2.f;
    scoreText.setPosition(X, Y);
}

void Game::setLines(int lines)
{
    linesText.setString(std::to_string(lines));
    sf::FloatRect rectangle(90, 220, 120, 50);
    sf::FloatRect textBounds = linesText.getGlobalBounds();
    float X = rectangle.left + (rectangle.width - textBounds.width) / 2.f;
    float Y = rectangle.top + (rectangle.height - textBounds.height) / 2.f;
    linesText.setPosition(X, Y);
}

Game::~Game() 
{
    delete this->gameBoard;
    delete this->pieceCreating;

    delete this->pauseButton;
    delete this->volumeButton;
    delete this->shadowButton;
}
