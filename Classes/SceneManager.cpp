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
}

SceneManager::~SceneManager() {
}

#pragma mark -
#pragma mark Public Methods

void SceneManager::enterGameScene(bool networked) {
    Scene *scene = Scene::create();
    GameScene *gameScene = GameScene::create();

    scene->addChild(gameScene);

    Director::getInstance()->pushScene(scene);
}

void SceneManager::returnToLobby() {
    Director::getInstance()->popScene();
}