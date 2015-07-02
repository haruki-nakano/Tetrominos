//
//  GameScene.h
//  Tetrominos
//
//  Created by Haruki Nakano on 2015/07/01.
//
//

#ifndef __Tetrominos__GameScene__
#define __Tetrominos__GameScene__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "TetrominoBag.h"

class Grid;
class Tetromino;

class GameScene : public cocos2d::Node {
public:
    CREATE_FUNC(GameScene);

protected:
    Grid *grid;
    std::unique_ptr<TetrominoBag> tetrominoBag;

    bool init() override;
    void onEnter() override;
    void setupTouchHandling();
    Tetromino *createRandomTetromino();

    void backButtonPressed(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);
};

#endif /* defined(__Tetrominos__GameScene__) */
