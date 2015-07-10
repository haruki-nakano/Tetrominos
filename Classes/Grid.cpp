//
//  Grid.cpp
//  Tetrominos
//
//  Created by Haruki Nakano on 2015/07/01.
//
//

#include "Grid.h"

#include "Tetromino.h"
#include "UIConstants.h"

using namespace cocos2d;

#pragma mark -
#pragma mark Lifecycle

bool Grid::init() {
    if (!Sprite::initWithFile("grid.png")) {
        return false;
    }

    this->activeTetromino = nullptr;
    this->activeTetrominoCoordinate = Coordinate();
    this->score = 0;
    this->totallLinesCleared = 0;

    for (int i = 0; i < GRID_HEIGHT; i++) {
        std::vector<Sprite *> row(GRID_WIDTH, nullptr);
        blocksLanded.push_back(row);
    }

    return true;
}

void Grid::onEnter() {
    Sprite::onEnter();
}

#pragma mark -
#pragma mark Public Methods

void Grid::rotateActiveTetromino() {
    if (this->activeTetromino) {
        this->activeTetromino->rotate(true);
        if (this->checkIfTetrominoCollides(activeTetromino, activeTetrominoCoordinate)) {
            this->activeTetromino->rotate(false);
        } else {
            ghostTetromino->rotate(true);
            this->updateGhostTetrominoPositino();
        }
    }
}

void Grid::dropActiveTetromino() {
    Coordinate landingCoordinate = this->getTetrominoLandingCoordinate();

    this->setActiveTetrominoCoordinate(landingCoordinate);
    this->deactivateTetromino(activeTetromino, activeTetrominoCoordinate);
}

void Grid::spawnTetromino(Tetromino *tetromino) {
    this->activeTetromino = tetromino;
    this->activeTetromino->setAnchorPoint(Vec2(0.0f, 0.0f));

    int highestY = activeTetromino->getHighestYCoordinate();
    int width = activeTetromino->getWidthInBlocks();

    Coordinate spawnCoordinate = Coordinate((GRID_WIDTH / 2) - (width / 2) - 1, GRID_HEIGHT - highestY - 1);

    this->setActiveTetrominoCoordinate(spawnCoordinate);

    this->addChild(this->activeTetromino);

    // Add ghost
    this->ghostTetromino = activeTetromino->createWithType(activeTetromino->getTetrominoType());

    this->ghostTetromino->setCascadeOpacityEnabled(true);
    this->ghostTetromino->setOpacity(GHOST_TETROMINO_OPACITY);
    this->updateGhostTetrominoPositino();

    this->addChild(ghostTetromino);
}

void Grid::step() {
    if (activeTetromino) {
        Coordinate activeCoordinate = this->getActiveTetrominoCoordinate();
        Coordinate nextCoordinate = Coordinate(activeCoordinate.x, activeCoordinate.y - 1);

        if (this->checkIfTetrominoCollides(activeTetromino, nextCoordinate)) {
            this->deactivateTetromino(activeTetromino, activeTetrominoCoordinate);
        } else {
            this->setActiveTetrominoCoordinate(nextCoordinate);
        }
    }
}

#pragma mark -
#pragma mark Setters / Getters

void Grid::setActiveTetrominoCoordinate(Coordinate coordinate) {
    if (activeTetromino) {
        if (this->checkIfTetrominoCollides(activeTetromino, coordinate)) {
            return;
        }

        activeTetrominoCoordinate = coordinate;
        activeTetromino->setPosition(this->convertCoordinateToPosition(activeTetrominoCoordinate));
        this->updateGhostTetrominoPositino();
    }
}

Coordinate Grid::getActiveTetrominoCoordinate() {
    return activeTetrominoCoordinate;
}

Size Grid::getBlockSize() {
    Size contentSize = this->getContentSize();
    Size blockSize(contentSize.width / float(GRID_WIDTH), contentSize.height / float(GRID_HEIGHT));
    return blockSize;
}

Tetromino *Grid::getActiveTetromino() {
    return this->activeTetromino;
}

int Grid::getTotalLinesCleared() {
    return this->totallLinesCleared;
}

int Grid::getScore() {
    return this->score;
}

#pragma mark -
#pragma mark Private Methods

cocos2d::Vec2 Grid::convertCoordinateToPosition(Coordinate coordinate) {
    Size contentSize = this->getContentSize();
    float blockWidth = contentSize.width / float(GRID_WIDTH);
    float blockHeight = contentSize.height / float(GRID_HEIGHT);

    return Vec2(coordinate.x * blockWidth, coordinate.y * blockHeight);
}

bool Grid::checkIfTetrominoCollides(Tetromino *tetromino, Coordinate tetrominoCoordinate) {
    int skirtStart = tetromino->getMininumXCoordinate();
    std::vector<int> skirt = tetromino->getSkirt();

    for (int i = 0; i < skirt.size(); i++) {
        Coordinate localCoordinate = Coordinate(i + skirtStart, skirt[i]);
        Coordinate gridCoordinate = Coordinate::add(tetrominoCoordinate, localCoordinate);

        if (gridCoordinate.x < 0 || gridCoordinate.y < 0 || gridCoordinate.x >= GRID_WIDTH ||
            gridCoordinate.y >= GRID_HEIGHT) {
            return true;
        }

        if (blocksLanded[gridCoordinate.y][gridCoordinate.x]) {
            return true;
        }
    }

    return false;
}

void Grid::deactivateTetromino(Tetromino *tetromino, Coordinate tetrominoCoordinate) {
    this->placeTetrominoOnBoard(tetromino, tetrominoCoordinate);

    this->activeTetromino->removeFromParent();
    this->activeTetromino = nullptr;

    this->ghostTetromino->removeFromParent();
    this->ghostTetromino = nullptr;

    this->clearLines();
}

void Grid::placeTetrominoOnBoard(Tetromino *tetromino, Coordinate tetrominoCoordinate) {
    std::vector<Sprite *> blocks = tetromino->getBlocks();
    std::vector<Coordinate> coordinates = tetromino->getCurrentRotation();

    for (int i = 0; i < BLOCKS_PER_TETROMONO; i++) {
        Coordinate localCoordinate = coordinates[i];
        Coordinate globalCoordinate = Coordinate::add(tetrominoCoordinate, localCoordinate);

        Sprite *block = blocks[i];
        Vec2 gridPosition = block->getPosition() + tetromino->getPosition();

        block->retain();
        block->removeFromParent();

        this->addChild(block);
        block->release();

        block->setPosition(gridPosition);

        blocksLanded[globalCoordinate.y][globalCoordinate.x] = block;
    }
}

Coordinate Grid::getTetrominoLandingCoordinate() {
    Coordinate landingCoordinate = this->getActiveTetrominoCoordinate();
    while (true) {
        landingCoordinate.y--;
        if (this->checkIfTetrominoCollides(activeTetromino, landingCoordinate)) {
            landingCoordinate.y++;
            break;
        }
    }
    return landingCoordinate;
}

void Grid::clearLines() {
    int linesCleared = 0;

    for (int y = 0; y < GRID_HEIGHT; y++) {
        bool fullLine = true;
        std::vector<Sprite *> row = blocksLanded[y];
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (!row[x]) {
                fullLine = false;
                break;
            }
        }
        if (fullLine) {
            linesCleared++;

            for (Sprite *block : row) {
                block->removeFromParent();
            }

            blocksLanded.erase(blocksLanded.begin() + y);

            std::vector<std::vector<Sprite *>> rowsToMoveDown(blocksLanded.begin() + y, blocksLanded.end());

            for (std::vector<Sprite *> rowAbove : rowsToMoveDown) {
                for (Sprite *block : rowAbove) {
                    if (block) {
                        block->setPositionY(block->getPositionY() - block->getContentSize().height);
                    }
                }
            }

            std::vector<Sprite *> newRow(GRID_WIDTH, nullptr);
            blocksLanded.push_back(newRow);

            y--;
        }
    }

    this->totallLinesCleared = linesCleared;
    this->updateScore(linesCleared);
}

void Grid::updateGhostTetrominoPositino() {
    if (ghostTetromino) {
        Coordinate landingCoordinate = this->getTetrominoLandingCoordinate();
        ghostTetromino->setPosition(this->convertCoordinateToPosition(landingCoordinate));
    }
}

void Grid::updateScore(int linesCleared) {
    this->score += linesCleared == 4 ? linesCleared + 1 : linesCleared;
}