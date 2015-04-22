#ifndef __INTRO_SCENE_H__
#define __INTRO_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

namespace gravity {

class IntroScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(IntroScene);

private:
    Sprite *_wadobo;
    Label *_gravity;
    Label *_start;

    bool onTouchBegan(Touch* touch, Event  *event);
    void onTouchEnded(Touch* touch, Event  *event);
};

}

#endif // __INTRO_SCENE_H__
