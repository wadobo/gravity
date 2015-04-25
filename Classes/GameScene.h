/**
 * Copyright (C) 2015 Daniel Garcia <danigm@wadobo.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/


#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

namespace gravity {

class GameScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameScene);

private:
    Sprite *_me;
    Label *_label;
    Label *_label2;
    Label *_gameOverLabel;
    Vector<Sprite*> _others;
    Vector<Sprite*> _fireBalls;
    int _collisions;
    int _max;
    bool _gameOver;

    void createOther(float dt);
    void fire(float dt);

    bool onTouchBegan(Touch* touch, Event  *event);
    void onTouchEnded(Touch* touch, Event  *event);
    void update(float dt);
    void removeOther(Sprite *sender);
    void explosion(Sprite *sender);

    void restart();
};

}

#endif // __GAME_SCENE_H__
