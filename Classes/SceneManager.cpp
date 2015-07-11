//
//  SceneManager.cpp
//  Tetrominos
//
//  Created by Haruki Nakano on 2015/07/01.
//
//

#include "SceneManager.h"
#include "GameScene.h"

using namespace cocos2d;

#pragma mark -
#pragma mark LifeCycle

static SceneManager *sharedSceneManager = nullptr;

SceneManager *SceneManager::getInstance() {
    if (!sharedSceneManager) {
        sharedSceneManager = new (std::nothrow) SceneManager();
    }

    return sharedSceneManager;
}

SceneManager::SceneManager() {
    gameScene = nullptr;
    this->networkingWrapper = std::unique_ptr<NetworkingWrapper>(new NetworkingWrapper());
    this->networkingWrapper->setDelegate(this);
}

SceneManager::~SceneManager() {
}

#pragma mark -
#pragma mark Public Methods

void SceneManager::enterGameScene(bool networked) {
    Scene *scene = Scene::create();
    this->gameScene = GameScene::create();
    this->gameScene->setNetworkedSession(networked);

    scene->addChild(gameScene);

    Director::getInstance()->pushScene(scene);
}

void SceneManager::returnToLobby() {
    if (gameScene) {
        Director::getInstance()->popScene();
        gameScene = nullptr;
        networkingWrapper->disconnect();
    }
}

void SceneManager::showPeerList() {
    networkingWrapper->showPeerList();
}

void SceneManager::receiveMultiplayerInvitations() {
    networkingWrapper->startAdvertisingAvailability();
}

void SceneManager::sendData(const void *data, unsigned long length) {
    networkingWrapper->sendData(data, length);
}

#pragma mark -
#pragma mark NetworkingWrapperDelegate Methods

void SceneManager::receivedData(const void *data, unsigned long length) {
    if (gameScene) {
        gameScene->receivedData(data, length);
    }
}

void SceneManager::stateChanged(ConnectionState state) {
    switch (state) {
        case ConnectionState::CONNECTING:
            CCLOG("Connecting..");
            break;
        case ConnectionState::NOT_CONNECTED:
            CCLOG("Not connected");
            break;
        case ConnectionState::CONNECTED:
            this->networkingWrapper->stopAdvertisingAvailability();
            this->enterGameScene(true);
            break;
    }
}