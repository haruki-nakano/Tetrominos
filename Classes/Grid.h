//
//  Grid.h
//  Tetrominos
//
//  Created by Haruki Nakano on 2015/07/01.
//
//

#ifndef __Tetrominos__Grid__
#define __Tetrominos__Grid__

#include "cocos2d.h"
#include "Coordinate.h"

class Tetromino;

class Grid : public cocos2d::Sprite {
public:
    CREATE_FUNC(Grid);

    void rotateActiveTetromino();
    void spawnTetromino(Tetromino *tetromino);
    void step();

    void setActiveTetrominoCoordinate(Coordinate coordinate);
    Coordinate getActiveTetrominoCoordinate();
    cocos2d::Size getBlockSize();
    Tetromino *getActiveTetromino();

private:
    Tetromino *activeTetromino;
    Coordinate activeTetrominoCoordinate;

    bool init() override;
    void onEnter() override;
    cocos2d::Vec2 convertCoordinateToPosition(Coordinate Coordinate);
};

#endif /* defined(__Tetrominos__Grid__) */
