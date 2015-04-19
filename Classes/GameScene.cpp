#include "GameScene.h"
#include <iostream>
#include <cmath>

#define PI 3.14159265

Scene* gravity::GameScene::createScene()
{
    auto scene = Scene::create();
    auto layer = gravity::GameScene::create();
    scene->addChild(layer);
    return scene;
}

bool gravity::GameScene::init()
{
    if (!Layer::init())
    {
        return false;
    }

    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(gravity::GameScene::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(gravity::GameScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    _me = Sprite::create("me.png");
    _me->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));

    schedule(CC_CALLBACK_1(gravity::GameScene::createOther, this), 1.0f, "create_other_key");

    schedule(schedule_selector(gravity::GameScene::update));

    _collisions = 0;
    _label = Label::createWithTTF(std::string("Collisions: ") + std::to_string(_collisions), "fonts/arial.ttf", 16.0f);
    _label->setPosition(10, visibleSize.height - 10);
    _label->setAnchorPoint(Vec2(0, 1));

    addChild(_me, 1);
    addChild(_label);

    return true;
}

void gravity::GameScene::update(float dt)
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Rect r(0, 0, visibleSize.width, visibleSize.height);
    Vec2 pos = _me->getPosition();

    if (!r.containsPoint(pos)) {
        if (pos.x > visibleSize.width) {
            pos.x = 0;
        } else if (pos.x < 0) {
            pos.x = visibleSize.width;
        }
        if (pos.y > visibleSize.height) {
            pos.y = 0;
        } else if (pos.y < 0) {
            pos.y = visibleSize.height;
        }
        _me->setPosition(pos);
    }

    // collisions
    for ( auto &other: _others ) {
        if (other->getTag() == 2) {
            continue;
        }

        auto bbox = other->getBoundingBox();
        if (bbox.intersectsRect(_me->getBoundingBox())) {
            _collisions++;
            _label->setString(std::string("Collisions: ") + std::to_string(_collisions));
            other->setTag(2);
            other->setColor(Color3B::BLUE);
            explosion(other);
        }
    }
}

void gravity::GameScene::createOther(float dt)
{
    Size visibleSize = Director::getInstance()->getVisibleSize();

    auto other = Sprite::create("other.png");
    other->setTag(1);
    _others.pushBack(other);

    int startx, starty, endx, endy;
    float rand;

    startx = (int)(CCRANDOM_0_1() * visibleSize.width);
    starty = (int)(CCRANDOM_0_1() * visibleSize.height);

    endx = visibleSize.width - startx + (int)(CCRANDOM_0_1()*400);
    endy = visibleSize.height - starty + (int)(CCRANDOM_0_1()*400);

    rand = CCRANDOM_0_1();
    if (rand < 0.25f) {
        startx = -100;
        endx = visibleSize.width + 100;
    } else if (rand < 0.5f) {
        starty = visibleSize.height + 100;
        endy = - 100;
    } else if (rand < 0.75f) {
        startx = visibleSize.width + 100;
        endx = -100;
    } else {
        starty = - 100;
        endy = visibleSize.height + 100;
    }

    other->setPosition(Vec2(startx, starty));

    int speed = (int)(CCRANDOM_0_1() * 4) + 3;

    auto action = Spawn::create(
        MoveTo::create(speed, Vec2(endx, endy)),
        RotateBy::create(speed,  720),
        nullptr);

    other->runAction(
        Sequence::create(
            DelayTime::create(int(CCRANDOM_0_1() * 5)),
            action,
            CallFunc::create(CC_CALLBACK_0(gravity::GameScene::removeOther,this, other)),
            nullptr));

    addChild(other, 1);
}

void gravity::GameScene::removeOther(Sprite *sender)
{
    removeChild(sender, true);
    _others.eraseObject(sender);
}

bool gravity::GameScene::onTouchBegan(Touch* touch, Event  *event)
{
    return true;
}

void gravity::GameScene::onTouchEnded(Touch* touch, Event  *event)
{
    auto location = touch->getLocation();
    Vec2 dest = Vec2(0, 0);
    Vec2 pos = _me->getPosition();

    int diffx = location.x - pos.x;
    int diffy = location.y - pos.y;
    float angle = - atan2(diffy, diffx) * 180 / PI;

    dest.x = diffx * 100 / 300;
    dest.y = diffy * 100 / 300;

    auto action = MoveBy::create(0.5f, dest);
    _me->runAction(RepeatForever::create(action));
    _me->runAction(RotateTo::create(1.5f, angle));
}

void gravity::GameScene::explosion(Sprite *other)
{
    ParticleSystemQuad *emitter = ParticleSystemQuad::create("ExplodingRing.plist");
    emitter->setPosition(other->getPosition());
    addChild(emitter, 100);
    removeOther(other);
}
