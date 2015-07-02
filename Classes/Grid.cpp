//
//  Grid.cpp
//  Tetrominos
//
//  Created by Haruki Nakano on 2015/07/01.
//
//

#include "Grid.h"
#include "Tetromino.h"

#pragma mark -
#pragma mark Lifecycle

bool Grid::init() {
    if (!Sprite::initWithFile("grid.png")) {
        return false;
    }

    this->activeTetromino = nullptr;

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
    }

    // TODO: Check if collision, under rotation
}

void Grid::spawnTetromino(Tetromino *tetromino) {
    this->activeTetromino = tetromino;
    this->addChild(this->activeTetromino);

    // TODO place tetromino in corret position in grid.
}