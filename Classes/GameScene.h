#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

namespace gravity {

class GameScene : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(GameScene);

private:
    Sprite *_me;
    Label *_label;
    Vector<Sprite*> _others;
    int _collisions;

    void createOther(float dt);

    bool onTouchBegan(Touch* touch, Event  *event);
    void onTouchEnded(Touch* touch, Event  *event);
    void update(float dt);
    void removeOther(Sprite *sender);
    void explosion(Sprite *sender);
};

}

#endif // __GAME_SCENE_H__