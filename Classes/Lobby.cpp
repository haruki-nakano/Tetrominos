//
//  Lobby.cpp
//  Tetrominos
//
//  Created by Haruki Nakano on 2015/07/01.
//
//

#include "Lobby.h"

using namespace cocos2d;

bool Lobby::init() {
    if (!Node::init()) {
        return false;
    }

    LayerColor *background = LayerColor::create(Color4B(255, 255, 255, 255));

    this->addChild(background);

    return true;
}

void Lobby::onEnter() {
    Node::onEnter();

    SceneManager::getInstance()->receiveMultiplayerInvitations();

    setupUI();
}

void Lobby::setupUI() {
    Size visibleSize = Director::getInstance()->getVisibleSize();

    Sprite *logo = Sprite::create("logo.png");
    logo->setAnchorPoint(Vec2(0.5f, 0.5f));
    logo->setPosition(Vec2(visibleSize.width * 0.5f, visibleSize.height * 0.75f));
    this->addChild(logo);

    ui::Button *singlePlayerButton = ui::Button::create();
    singlePlayerButton->setAnchorPoint(Vec2(0.5f, 0.5f));
    singlePlayerButton->setPosition(Vec2(visibleSize.width / 2.0f, visibleSize.height * 0.4f));
    singlePlayerButton->loadTextures("singlePlayerButton.png", "singlePlayerButtonPressed.png");
    singlePlayerButton->addTouchEventListener(CC_CALLBACK_2(Lobby::singlePlayerPressed, this));
    this->addChild(singlePlayerButton);

    ui::Button *multiPlayerButton = ui::Button::create();
    multiPlayerButton->setAnchorPoint(Vec2(0.5f, 0.5f));
    multiPlayerButton->setPosition(Vec2(visibleSize.width / 2.0f, visibleSize.height * 0.25f));
    multiPlayerButton->loadTextures("multiplayerButton.png", "multiplayerButtonPressed.png");
    multiPlayerButton->addTouchEventListener(CC_CALLBACK_2(Lobby::multiPlayerPressed, this));
    this->addChild(multiPlayerButton);

    return;
}

void Lobby::singlePlayerPressed(cocos2d::Ref *pSender, ui::Widget::TouchEventType eEventType) {
    if (eEventType == ui::Widget::TouchEventType::ENDED) {
        SceneManager::getInstance()->enterGameScene(false);
    }
}

void Lobby::multiPlayerPressed(cocos2d::Ref *pSender, ui::Widget::TouchEventType eEventType) {
    if (eEventType == ui::Widget::TouchEventType::ENDED) {
        SceneManager::getInstance()->showPeerList();
    }
}