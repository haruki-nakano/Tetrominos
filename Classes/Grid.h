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

class Grid : public cocos2d::Sprite {
public:
    CREATE_FUNC(Grid);

private:
    bool init() override;
};

#endif /* defined(__Tetrominos__Grid__) */
