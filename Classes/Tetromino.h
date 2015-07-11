//
//  Tetromino.h
//  Tetrominos
//
//  Created by Haruki Nakano on 2015/07/01.
//
//

#ifndef __Tetrominos__Tetromino__
#define __Tetrominos__Tetromino__

#include "cocos2d.h"
#include "Constants.h"
#include "Coordinate.h"

class Tetromino : public cocos2d::Node {
public:
    static Tetromino *createWithType(TetrominoType type);

    void rotate(bool right);

    TetrominoType getTetrominoType();
    int getHighestYCoordinate();
    int getMininumXCoordinate();
    int getWidthInBlocks();

    std::vector<int> getSkirt();
    std::vector<cocos2d::Sprite *> getBlocks();
    std::vector<Coordinate> getCurrentRotation();
    cocos2d::Color3B getTetrominoColor();

private:
    TetrominoType type;
    std::vector<std::vector<Coordinate>> rotations;
    cocos2d::Color3B color;
    std::vector<cocos2d::Sprite *> blocks;
    int rotationIndex;

    bool initWithType(TetrominoType type);
};

#endif /* defined(__Tetrominos__Tetromino__) */
