//
//  Lobby.h
//  Tetrominos
//
//  Created by Haruki Nakano on 2015/07/01.
//
//

#ifndef __Tetrominos__Lobby__
#define __Tetrominos__Lobby__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "SceneManager.h"

class Lobby : public cocos2d::Node {
public:
    CREATE_FUNC(Lobby);

private:
    bool init() override;
    void onEnter() override;

    void setupUI();

    void singlePlayerPressed(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);
    void multiPlayerPressed(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType eEventType);
};

#endif /* defined(__Tetrominos__Lobby__) */
