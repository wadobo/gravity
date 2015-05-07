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


#include "GameScene.h"
#include "IntroScene.h"
#include <iostream>
#include <cmath>

#define PI 3.14159265
#define DIED 132
#define ASTEROID 1
#define FIREBALL 2

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

    _gameOver = false;
    _gameOverLabel = 0;

    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(gravity::GameScene::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(gravity::GameScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    auto keyListener = EventListenerKeyboard::create();
    keyListener->onKeyPressed = CC_CALLBACK_2(gravity::GameScene::onKeyPressed, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyListener, this);

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    _me = Sprite::create("me.png");
    _me->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));

    schedule(CC_CALLBACK_1(gravity::GameScene::createOther, this), 1.0f, "create_other_key");
    schedule(CC_CALLBACK_1(gravity::GameScene::fire, this), 0.5f, "fire_key");

    schedule(schedule_selector(gravity::GameScene::update));

    char str[200];
    sprintf(str, "GAME OVER!");
    _gameOverLabel = Label::createWithTTF(std::string(str), "fonts/arial.ttf", 80.0f);
    _gameOverLabel->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    _gameOverLabel->setVisible(false);
    addChild(_gameOverLabel, 500);

    _collisions = 0;
    _label = Label::createWithTTF(std::string("Points: 0"), "fonts/arial.ttf", 30.0f);
    _label->setPosition(10, visibleSize.height - 10);
    _label->setAnchorPoint(Vec2(0, 1));

    _max = 0;
    _label2 = Label::createWithTTF(std::string("Max: 0"), "fonts/arial.ttf", 30.0f);
    _label2->setPosition(visibleSize.width - 10, visibleSize.height - 10);
    _label2->setAnchorPoint(Vec2(1, 1));

    addChild(_me, 1);
    addChild(_label);
    addChild(_label2);

    return true;
}

void gravity::GameScene::update(float dt)
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Rect r(0, 0, visibleSize.width, visibleSize.height);
    Vec2 pos = _me->getPosition();

    // The space is infinite
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

    if (_gameOver) {
        return;
    }

    // collisions
    for ( auto &other: _others ) {
        if (other->getTag() == DIED) {
            continue;
        }

        // Asteroids collision
        auto bbox = other->getBoundingBox();
        for ( auto &fireball: _fireBalls ) {
            if (fireball->getTag() == DIED) {
                continue;
            }

            if (bbox.intersectsRect(fireball->getBoundingBox())) {
                char str[200];

                _collisions++;
                if (_collisions > _max) {
                    _max = _collisions;
                    sprintf(str, "Max: %d", _max);
                    _label2->setString(std::string(str));
                }
                sprintf(str, "Points: %d", _collisions);
                _label->setString(std::string(str));
                fireball->setTag(DIED);
                other->setTag(DIED);
                explosion(other);
                removeOther(fireball);
            }

        }

        // Me and asteroid collision
        if (bbox.intersectsRect(_me->getBoundingBox())) {
            _gameOver = true;
            _gameOverLabel->setVisible(true);
            _gameOverLabel->runAction(RepeatForever::create(Blink::create(2, 1)));

            explosion(_me);
            _me->setColor(Color3B(180,100,100));
            _me->runAction(RepeatForever::create(Blink::create(1, 1)));
        }
    }

    // Removing out fireballs
    int i=0;
    while(i<_fireBalls.size()) {
        auto fb = _fireBalls.at(i);
        if (!r.intersectsRect(fb->getBoundingBox())) {
            removeOther(fb);
        }
        i++;
    }
}

void gravity::GameScene::createOther(float dt)
{
    Size visibleSize = Director::getInstance()->getVisibleSize();

    auto other = Sprite::create("other.png");
    other->setTag(ASTEROID);
    _others.pushBack(other);

    float scale = 0.8f;
    int startx, starty, endx, endy;
    float rand;

    scale = (CCRANDOM_0_1() + 0.5f) * scale;
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
    other->setScale(scale, scale);

    int speed = (int)(CCRANDOM_0_1() * 4) + 3;

    auto action = Spawn::create(
        MoveTo::create(speed, Vec2(endx, endy)),
        RotateBy::create(speed,  720),
        nullptr);

    other->runAction(
        Sequence::create(
            DelayTime::create(int(CCRANDOM_0_1() * 5)),
            action,
            CallFunc::create(CC_CALLBACK_0(gravity::GameScene::removeOther, this, other)),
            nullptr));

    addChild(other, 1);
}

void gravity::GameScene::removeOther(Sprite *sender)
{
    switch (sender->getTag()) {
        case ASTEROID:
            _others.eraseObject(sender);
            break;
        case FIREBALL:
            _fireBalls.eraseObject(sender);
            break;
        default:
            break;
    }

    removeChild(sender, true);
}

bool gravity::GameScene::onTouchBegan(Touch* touch, Event  *event)
{
    return true;
}

void gravity::GameScene::onTouchEnded(Touch* touch, Event  *event)
{
    if (_gameOver) {
        restart();
        return;
    }

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
    _me->runAction(RotateTo::create(0.5f, angle));
}

void gravity::GameScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event  *event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
    {
        auto intro = IntroScene::createScene();
        Director::getInstance()->replaceScene(TransitionFade::create(0.5, intro, Color3B(0,0,0)));
    }
}

void gravity::GameScene::explosion(Sprite *other)
{
    ParticleSystemQuad *emitter = ParticleSystemQuad::create("ExplodingRing.plist");
    emitter->setPosition(other->getPosition());
    addChild(emitter, 100);
    if (other->getTag() == DIED) {
        removeOther(other);
    }
}

void gravity::GameScene::fire(float dt)
{
    if (_gameOver) {
        return;
    }

    auto fireball = Sprite::create("fire.png");
    auto fireball2 = Sprite::create("fire.png");
    fireball->setTag(FIREBALL);
    fireball2->setTag(FIREBALL);
    _fireBalls.pushBack(fireball);
    _fireBalls.pushBack(fireball2);

    fireball->setAnchorPoint(Vec2(0.5, 3));
    fireball->setPosition(_me->getPosition());
    fireball->setRotation(_me->getRotation());

    fireball2->setAnchorPoint(Vec2(0.5, -2));
    fireball2->setPosition(_me->getPosition());
    fireball2->setRotation(_me->getRotation());

    float rad = PI * _me->getRotation() / 180.0f;
    int speed = 180;
    int incx = (int) (cos(rad) * speed);
    int incy = (int) -(sin(rad) * speed);

    fireball->runAction(
            RepeatForever::create(MoveBy::create(0.5f, Vec2(incx, incy)))
            );

    fireball2->runAction(
            RepeatForever::create(MoveBy::create(0.5f, Vec2(incx, incy)))
            );

    addChild(fireball, -1);
    addChild(fireball2, -1);
}

void gravity::GameScene::restart()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Rect r(0, 0, visibleSize.width, visibleSize.height);
    char str[200];
    _me->setPosition(r.getMidX(), r.getMidY());
    _collisions = 0;
    sprintf(str, "Points: %d", _collisions);
    _label->setString(std::string(str));
    _me->setRotation(0);
    _me->setColor(Color3B(255,255,255));
    _me->stopAllActions();
    _me->setVisible(true);
    _me->runAction(Blink::create(1, 2));
    _gameOverLabel->stopAllActions();
    _gameOverLabel->setVisible(false);

    _gameOver = false;
}
