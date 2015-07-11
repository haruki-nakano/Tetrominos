//
//  PreviewGrid.cpp
//  Tetrominos
//
//  Created by Haruki Nakano on 2015/07/10.
//
//

#include "PreviewGrid.h"

using namespace cocos2d;

#pragma mark -
#pragma mark Lifecycle

bool PreviewGrid::init() {
    if (!Sprite::initWithFile("grid.png")) {
        return false;
    }
    return true;
}

#pragma mark -
#pragma mark Public Methods

void PreviewGrid::setState(JSONPacker::GameState state) {
    this->removeAllChildren();

    std::vector<std::vector<Color3B>> board = state.board;

    for (int column = 0; column < board.size(); column++) {
        for (int i = 0; i < board[column].size(); i++) {
            Color3B color = board[column][i];

            if (!color.equals(Color3B::WHITE)) {
                Sprite *block = Sprite::create("block.png");
                Size blockSize = block->getContentSize();

                block->setColor(color);
                block->setPosition(Vec2(i * blockSize.width, column * blockSize.height));

                this->addChild(block);
            }
        }
    }
}