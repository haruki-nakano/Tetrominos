//
//  GameScene.cpp
//  Tetrominos
//
//  Created by Haruki Nakano on 2015/07/01.
//
//

#include <Time.h>

#include "GameScene.h"
#include "SceneManager.h"
#include "Grid.h"
#include "Tetromino.h"
#include "UIConstants.h"
#include "JSONPacker.h"
#include "NetworkingWrapper.h"
#include "PreviewGrid.h"

using namespace cocos2d;

#pragma mark -
#pragma mark Lifycycle

bool GameScene::init() {
    if (!Node::init()) {
        return false;
    }

    this->stepInterval = INITIAL_STEP_INTERVAL;

    LayerColor *background = LayerColor::create(Color4B(255, 255, 255, 255));
    this->addChild(background);

    this->tetrominoBag = std::unique_ptr<TetrominoBag>(new TetrominoBag());
    this->active = false;
    this->totalScore = 0;
    this->timeLeft = TIME_PER_GAME;
    this->networkedSession = false;
    this->gameIsOver = false;

    return true;
}
void GameScene::onEnter() {
    Node::onEnter();

    Size visibleSize = Director::getInstance()->getVisibleSize();

    // setup grid
    grid = Grid::create();
    grid->setAnchorPoint(Vec2(0.5f, 0.0f));
    grid->setPosition(Vec2(visibleSize.width * 0.5f, 0.0f));

    this->addChild(grid);

    // setup menus
    ui::Button *backButton = ui::Button::create();
    backButton->setAnchorPoint(Vec2(0.0f, 1.0f));
    backButton->setPosition(Vec2(0.0f, visibleSize.height));
    backButton->loadTextures("backButton.png", "backButtonPressed.png");
    backButton->addTouchEventListener(CC_CALLBACK_2(GameScene::backButtonPressed, this));

    this->addChild(backButton);

    // setup labels
    this->scoreLabel = ui::Text::create("0", FONT_NAME, FONT_SIZE);
    this->scoreLabel->setAnchorPoint(Vec2(0.5f, 1.0f));
    this->scoreLabel->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.95f));
    this->scoreLabel->setColor(LABEL_COLOR);

    this->timeLeftLabel = ui::Text::create("0", FONT_NAME, FONT_SIZE);
    this->timeLeftLabel->setAnchorPoint(Vec2(0.5f, 1.0f));
    this->timeLeftLabel->setPosition(this->scoreLabel->getPosition() - Vec2(0.0f, FONT_SIZE * 1.5f));
    this->timeLeftLabel->setColor(LABEL_COLOR);

    this->addChild(this->scoreLabel);
    this->addChild(this->timeLeftLabel);

    if (networkedSession) {
        this->grid->setAnchorPoint(Vec2(0.0f, 0.0f));
        this->grid->setPosition(Vec2(0.0f, 0.0f));

        this->previewGrid = PreviewGrid::create();
        this->previewGrid->setAnchorPoint(Vec2(1.0f, 1.0f));
        this->previewGrid->setPosition(Vec2(visibleSize.width, visibleSize.height));
        this->previewGrid->setScale(0.5f);

        this->addChild(this->previewGrid);
    }

    setupTouchHandling();

    setGameActive(true);
}

void GameScene::setupTouchHandling() {
    auto touchListener = EventListenerTouchOneByOne::create();

    static Vec2 firstTouchPos;
    static Vec2 lastTouchPos;
    static bool allowRotate;
    static std::clock_t touchStartedTime;

    touchListener->onTouchBegan = [&](Touch *touch, Event *event) {
        firstTouchPos = this->convertTouchToNodeSpace(touch);
        lastTouchPos = firstTouchPos;
        allowRotate = true;
        touchStartedTime = clock();
        return true;
    };

    touchListener->onTouchMoved = [&](Touch *touch, Event *event) {
        Vec2 touchPos = this->convertTouchToNodeSpace(touch);
        Vec2 difference = touchPos - lastTouchPos;

        Tetromino *activeTetromino = grid->getActiveTetromino();

        if (activeTetromino) {
            // Coordinate touchCoordinate = this->convertPositionToCoordinate(touchPos);
            Coordinate differenceCoordinate = this->convertPositionToCoordinate(difference);
            Coordinate activeTetrominoCoordinate = grid->getActiveTetrominoCoordinate();

            if (differenceCoordinate.y <= -1) {
                Coordinate newTetrominoCoordinate =
                    Coordinate(activeTetrominoCoordinate.x, activeTetrominoCoordinate.y - 1);
                grid->setActiveTetrominoCoordinate(newTetrominoCoordinate);
                lastTouchPos = touchPos;
            } else if (abs(differenceCoordinate.x) >= 1) {
                Coordinate newTetrominoCoordinate;

                bool movingRight = (differenceCoordinate.x > 0);
                newTetrominoCoordinate =
                    Coordinate(activeTetrominoCoordinate.x + (movingRight ? 1 : -1), activeTetrominoCoordinate.y);
                grid->setActiveTetrominoCoordinate(newTetrominoCoordinate);

                allowRotate = false;
                lastTouchPos = touchPos;
            }
        }
    };

    touchListener->onTouchEnded = [&](Touch *touch, Event *event) {
        if (this->grid->getActiveTetromino()) {
            Vec2 touchEndPos = this->convertTouchToNodeSpace(touch);
            float distance = touchEndPos.distance(firstTouchPos);
            Size blockSize = this->grid->getBlockSize();

            if (distance < blockSize.width && allowRotate) {
                grid->rotateActiveTetromino();
            } else {
                Vec2 difference = touchEndPos - firstTouchPos;
                std::clock_t clockDifference = clock() - touchStartedTime;

                if (clockDifference <= 0) {
                    return;
                }

                float touchDuratioin = (float)(clockDifference) / CLOCKS_PER_SEC;
                float velocity = fabsf(difference.y / touchDuratioin);

                if (velocity > DROP_VELOCITY) {
                    grid->dropActiveTetromino();
                    this->updateStateFromScore();
                }
            }
        }
    };

    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

#pragma mark -
#pragma mark Public Methods

void GameScene::setNetworkedSession(bool networkedSession) {
    this->networkedSession = networkedSession;
}

void GameScene::receivedData(const void *data, unsigned long length) {
    const char *cstr = reinterpret_cast<const char *>(data);
    std::string json = std::string(cstr, length);

    JSONPacker::GameState state = JSONPacker::unpackGameStateJSON(json);

    if (state.gameOver) {
        this->gameOver();
    }

    this->previewGrid->setState(state);
}

#pragma mark -
#pragma mark Protected Methods

void GameScene::setGameActive(bool active) {
    this->active = active;
    if (active) {
        this->schedule(CC_SCHEDULE_SELECTOR(GameScene::step), this->stepInterval);
        this->scheduleUpdate();
    } else {
        this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::step));
        this->unscheduleUpdate();
    }
}

void GameScene::step(float dt) {
    if (this->grid->checkIfTopReached()) {
        gameOver();
    }

    Tetromino *activeTetromino = grid->getActiveTetromino();

    if (!activeTetromino) {
        Tetromino *newTetromino = this->createRandomTetromino();
        this->grid->spawnTetromino(newTetromino);
    } else {
        this->grid->step();
        this->updateStateFromScore();
    }

    if (networkedSession) {
        this->sendGameStateOverNetwork();
    }
}

void GameScene::update(float dt) {
    Node::update(dt);

    this->setTimeLeft(this->timeLeft - dt);

    if (this->timeLeft <= 0.0f) {
        this->gameOver();
    }
}

Tetromino *GameScene::createRandomTetromino() {
    TetrominoType tetrominoType = tetrominoBag->getTetromino();
    Tetromino *newTetromino = Tetromino::createWithType(tetrominoType);
    return newTetromino;
}

void GameScene::updateStateFromScore() {
    int newScore = this->grid->getScore();

    if (newScore > this->totalScore) {
        this->totalScore = newScore;
        this->updateScoreLabel(newScore);
        this->updateGameSpeed(totalScore);
    }
}

void GameScene::updateGameSpeed(int score) {
    int stepAcceleration = score / SCORE_TO_ACCELERATE;

    float intervalDeduction = powf(ACCELERATION_FACTOR, stepAcceleration);

    float newInterval = MAX(intervalDeduction, SPEED_MAX);

    this->stepInterval = newInterval;
    // CCLOG("%f", stepInterval);

    this->unschedule(CC_SCHEDULE_SELECTOR(GameScene::step));
    this->schedule(CC_SCHEDULE_SELECTOR(GameScene::step), this->stepInterval);
}

void GameScene::gameOver() {
    this->gameIsOver = true;
    this->setGameActive(false);

    if (networkedSession) {
        this->sendGameStateOverNetwork();
    }

    std::string scoreString = StringUtils::toString(this->totalScore);
    std::string messageContent = "Your score is " + scoreString + "!";
    MessageBox(messageContent.c_str(), "GAMEOVER");

    SceneManager::getInstance()->returnToLobby();
}

void GameScene::setTimeLeft(float time) {
    this->timeLeft = time;
    std::string timeLeftString = StringUtils::format("%2.1f", time);

    this->timeLeftLabel->setString(timeLeftString);
}

void GameScene::sendGameStateOverNetwork() {
    JSONPacker::GameState state;

    state.name = NetworkingWrapper::getDeviceName();
    state.score = this->totalScore;
    state.gameOver = this->gameIsOver;

    std::vector<std::vector<Sprite *>> blocksLanded = this->grid->getBlocksLanded();

    for (int y = 0; y < blocksLanded.size(); y++) {
        std::vector<Color3B> blocks(blocksLanded.size(), Color3B::WHITE);
        state.board.push_back(blocks);

        std::vector<Sprite *> column = blocksLanded[y];
        for (int x = 0; x < column.size(); x++) {
            Sprite *block = column[x];

            if (block) {
                state.board[y][x] = block->getColor();
            }
        }
    }

    Tetromino *activeTetromino = this->grid->getActiveTetromino();

    if (activeTetromino) {
        std::vector<Coordinate> coordinates = activeTetromino->getCurrentRotation();
        Coordinate tetrominoCoordinate = this->grid->getActiveTetrominoCoordinate();

        for (Coordinate blockCoordinate : coordinates) {
            Coordinate gridCoordinate = Coordinate::add(tetrominoCoordinate, blockCoordinate);
            if (gridCoordinate.x < GRID_WIDTH && gridCoordinate.y < GRID_HEIGHT) {
                state.board[gridCoordinate.y][gridCoordinate.x] = activeTetromino->getTetrominoColor();
            }
        }
    }

    std::string json = JSONPacker::packGameStateJSON(state);
    SceneManager::getInstance()->sendData(json.c_str(), json.length());
}

#pragma mark -
#pragma mark UI Methods

void GameScene::backButtonPressed(cocos2d::Ref *pSender, ui::Widget::TouchEventType eEventType) {
    if (eEventType == ui::Widget::TouchEventType::ENDED) {
        SceneManager::getInstance()->returnToLobby();
    }
}

void GameScene::updateScoreLabel(int score) {
    std::string scoreString = StringUtils::toString(score);
    this->scoreLabel->setString(scoreString);
}

#pragma mark -
#pragma mark UtilityMethods

Coordinate GameScene::convertPositionToCoordinate(cocos2d::Vec2 position) {
    Size blockSize = this->grid->getBlockSize();
    return Coordinate(position.x / blockSize.width, position.y / blockSize.height);
}
