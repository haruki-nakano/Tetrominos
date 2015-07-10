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
    int getTotalLinesCleared();
    int getScore();
    void dropActiveTetromino();

private:
    Tetromino *activeTetromino;
    Tetromino *ghostTetromino;
    Coordinate activeTetrominoCoordinate;
    int score;
    int totallLinesCleared;

    std::vector<std::vector<cocos2d::Sprite *>> blocksLanded;

    bool init() override;
    void onEnter() override;
    cocos2d::Vec2 convertCoordinateToPosition(Coordinate Coordinate);
    bool checkIfTetrominoCollides(Tetromino *tetromino, Coordinate tetrominoCoordinate);
    void deactivateTetromino(Tetromino *tetromino, Coordinate tetrominoCoordinate);
    void placeTetrominoOnBoard(Tetromino *tetromino, Coordinate tetrominoCoordinate);
    Coordinate getTetrominoLandingCoordinate();
    void clearLines();
    void updateGhostTetrominoPositino();
    void updateScore(int linesCleared);
};

#endif /* defined(__Tetrominos__Grid__) */
