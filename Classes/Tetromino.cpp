//
//  Tetromino.cpp
//  Tetrominos
//
//  Created by Haruki Nakano on 2015/07/01.
//
//

#include "Tetromino.h"
#include "JSONPacker.h"

#define GRID_SIZE 4

using namespace cocos2d;

#pragma mark -
#pragma mark Life Cycle Methods

Tetromino *Tetromino::createWithType(TetrominoType type) {
    Tetromino *tetromino = new (std::nothrow) Tetromino();

    if (tetromino && tetromino->initWithType(type)) {
        tetromino->autorelease();
        return tetromino;
    }

    CC_SAFE_DELETE(tetromino);
    return nullptr;
}

bool Tetromino::initWithType(TetrominoType type) {
    if (!Node::init()) {
        return false;
    }

    this->type = type;
    rotations = std::vector<std::vector<Coordinate>>();

    std::string jsonString = FileUtils::getInstance()->getStringFromFile("tetrominos.json");

    JSONPacker::TetrominoState tetrominoState = JSONPacker::unpackTetrominoJSON(jsonString, type);

    this->color = tetrominoState.color;
    this->rotations = tetrominoState.rotations;
    this->blocks = std::vector<Sprite *>();
    this->blocks.reserve(4);
    this->rotationIndex = 0;

    Sprite *dummyBlock = Sprite::create("block.png");
    Size dummySize = dummyBlock->getContentSize();

    float gridSizeF = float(GRID_SIZE);
    this->setContentSize(Size(dummySize.width * gridSizeF, dummySize.height * gridSizeF));

    auto coordinates = rotations[this->rotationIndex];
    for (Coordinate coordinate : coordinates) {
        Sprite *block = Sprite::create("block.png");
        block->setColor(this->color);
        block->setAnchorPoint(Vec2(0.0f, 0.0f));
        block->setPosition(Vec2(coordinate.x * dummySize.width, coordinate.y * dummySize.height));

        this->addChild(block);
        blocks.push_back(block);
    }

    return true;
}

#pragma mark -
#pragma mark Public Methods

void Tetromino::rotate(bool right) {
    if (right) {
        rotationIndex++;
    } else {
        rotationIndex--;
    }

    if (rotationIndex >= (int)rotations.size()) {
        rotationIndex = 0;
    } else if (rotationIndex < 0) {
        rotationIndex = (int)rotations.size() - 1;
    }

    std::vector<Coordinate> coordinates = rotations[this->rotationIndex];
    for (int i = 0; i < GRID_SIZE; i++) {
        Sprite *block = blocks[i];
        Coordinate coordinate = coordinates[i];

        Size blockSize = block->getContentSize();
        block->setPosition(Vec2(coordinate.x * blockSize.width, coordinate.y * blockSize.height));
    }
}

int Tetromino::getHighestYCoordinate() {
    int highest = 0;

    auto coordinates = rotations[rotationIndex];
    for (Coordinate coordinate : coordinates) {
        highest = MAX(highest, coordinate.y);
    }

    return highest;
}

int Tetromino::getMininumXCoordinate() {
    int min = GRID_SIZE;
    auto coordinates = rotations[rotationIndex];
    for (Coordinate coordinate : coordinates) {
        min = MIN(min, coordinate.x);
    }

    return min;
}

int Tetromino::getWidthInBlocks() {
    int start = GRID_SIZE - 1;
    int end = 0;

    auto coordinates = rotations[rotationIndex];
    for (Coordinate coordinate : coordinates) {
        start = MIN(start, coordinate.x);
        end = MAX(end, coordinate.x);
    }

    return end - start + 1;
}

std::vector<int> Tetromino::getSkirt() {
    int width = this->getWidthInBlocks();
    int skirtStart = this->getMininumXCoordinate();
    std::vector<int> skirt = std::vector<int>(width, GRID_SIZE);

    auto coordinates = rotations[rotationIndex];
    for (Coordinate coordinate : coordinates) {
        int x = coordinate.x - skirtStart;
        skirt[x] = MIN(skirt[x], coordinate.y);
    }

    return skirt;
}
