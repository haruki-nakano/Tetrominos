//
//  JSONPacker.cpp
//  Tetrominos
//
//  Created by Haruki Nakano on 2015/07/01.
//
//

#include "JSONPacker.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

using namespace cocos2d;

namespace JSONPacker {
TetrominoState unpackTetrominoJSON(std::string json, TetrominoType type) {
    rapidjson::Document document;
    document.Parse<0>(json.c_str());

    const char *typeStr;
    switch (type) {
        case TetrominoType::I:
            typeStr = "I";
            break;
        case TetrominoType::O:
            typeStr = "O";
            break;
        case TetrominoType::T:
            typeStr = "T";
            break;
        case TetrominoType::S:
            typeStr = "S";
            break;
        case TetrominoType::Z:
            typeStr = "Z";
            break;
        case TetrominoType::J:
            typeStr = "J";
            break;
        case TetrominoType::L:
            typeStr = "L";
            break;
    }

    rapidjson::Value &tetrominoDoc = document[typeStr];

    rapidjson::Value &colorDoc = tetrominoDoc["color"];
    int r = colorDoc["r"].GetInt();
    int g = colorDoc["g"].GetInt();
    int b = colorDoc["b"].GetInt();
    Color3B color = Color3B(r, g, b);

    std::vector<std::vector<Coordinate>> rotations;
    // rotations.push_back();

    rapidjson::Value &rotationDoc = tetrominoDoc["rotations"];
    for (rapidjson::SizeType rotationIndex = 0; rotationIndex < rotationDoc.Size(); ++rotationIndex) {
        std::vector<Coordinate> coordinates;
        rapidjson::Value &coordinatesDoc = rotationDoc[rotationIndex];
        for (rapidjson::SizeType coordinateIndex = 0; coordinateIndex < coordinatesDoc.Size(); ++coordinateIndex) {
            int x = coordinatesDoc[coordinateIndex]["x"].GetInt();
            int y = coordinatesDoc[coordinateIndex]["y"].GetInt();
            Coordinate coordinate = Coordinate(x, y);
            coordinates.push_back(coordinate);
        }
        rotations.push_back(coordinates);
    }

    TetrominoState tetrominoState;
    tetrominoState.color = color;
    tetrominoState.rotations = rotations;

    return tetrominoState;
}

GameState unpackGameStateJSON(std::string json) {
    rapidjson::Document document;
    document.Parse<0>(json.c_str());

    GameState gameState;

    gameState.score = document["score"].GetInt();
    gameState.name = document["name"].GetString();
    gameState.gameOver = document["gameOver"].GetBool();

    rapidjson::Value &columns = document["board"];

    // Convert Color3B in the grid to JSON obejcts
    for (rapidjson::SizeType column = 0; column < columns.Size(); column++) {
        rapidjson::Value &blocksJson = columns[column];
        std::vector<Color3B> blocks;

        for (rapidjson::SizeType i = 0; i < blocksJson.Size(); i++) {
            rapidjson::Value &block = blocksJson[i];
            int r = block["r"].GetInt();
            int g = block["g"].GetInt();
            int b = block["b"].GetInt();

            Color3B color3B = Color3B(r, g, b);

            blocks.push_back(color3B);
        }
        gameState.board.push_back(blocks);
    }

    return gameState;
}

std::string packGameStateJSON(const GameState gameState) {
    rapidjson::Document document;
    document.SetObject();
    document.AddMember("score", gameState.score, document.GetAllocator());
    document.AddMember("name", gameState.name.c_str(), document.GetAllocator());
    document.AddMember("gameOver", gameState.gameOver, document.GetAllocator());

    rapidjson::Value columns(rapidjson::kArrayType);
    for (int column = 0; column < gameState.board.size(); column++) {
        rapidjson::Value blocks(rapidjson::kArrayType);
        for (int i = 0; i < gameState.board[column].size(); i++) {
            Color3B color = gameState.board[column][i];
            rapidjson::Value colorJson(rapidjson::kObjectType);
            colorJson.AddMember("r", color.r, document.GetAllocator());
            colorJson.AddMember("g", color.g, document.GetAllocator());
            colorJson.AddMember("b", color.b, document.GetAllocator());

            blocks.PushBack(colorJson, document.GetAllocator());
            CCLOG("index");
        }
        CCLOG("column");
        columns.PushBack(blocks, document.GetAllocator());
    }

    document.AddMember("board", columns, document.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    std::string returnString(buffer.GetString(), buffer.Size());

    CCLOG("%s", returnString.c_str());
    return returnString;
}
}
