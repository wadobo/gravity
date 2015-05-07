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


#include "IntroScene.h"
#include "GameScene.h"

Scene* gravity::IntroScene::createScene()
{
    auto scene = Scene::create();
    auto layer = gravity::IntroScene::create();
    scene->addChild(layer);
    return scene;
}

bool gravity::IntroScene::init()
{
    char str[200];

    if (!Layer::init())
    {
        return false;
    }

    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(gravity::IntroScene::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(gravity::IntroScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    auto keyListener = EventListenerKeyboard::create();
    keyListener->onKeyPressed = CC_CALLBACK_2(gravity::IntroScene::onKeyPressed, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyListener, this);

    Size visibleSize = Director::getInstance()->getVisibleSize();

    _wadobo = Sprite::create("wadobo-games.png");

    sprintf(str, "GRAVITY");
    _gravity = Label::createWithTTF(std::string(str), "fonts/arial.ttf", 80.0f);
    _gravity->setAnchorPoint(Vec2(0.5, 0));
    _gravity->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2 + 10));
    _gravity->setOpacity(0);

    sprintf(str, "click to start");
    _start = Label::createWithTTF(std::string(str), "fonts/arial.ttf", 30.0f);
    _start->setAnchorPoint(Vec2(0.5, 1));
    _start->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2 - 10));
    _start->setOpacity(0);

    _wadobo->setPosition(Vec2(-500, visibleSize.height/2));
    auto center = Vec2(visibleSize.width/2, visibleSize.height/2);

    auto action = Sequence::create(
        EaseElasticOut::create(MoveTo::create(2.0f, center)),
        FadeOut::create(0.5f),
        nullptr);
    _wadobo->runAction(action);

    _gravity->runAction(Sequence::create(DelayTime::create(2.0f),
        FadeIn::create(0.5f),
        nullptr));
    _start->runAction(Sequence::create(DelayTime::create(2.0f),
        FadeIn::create(0.5f), nullptr));
    _start->runAction(RepeatForever::create(Blink::create(2, 1)));

    addChild(_wadobo);
    addChild(_gravity);
    addChild(_start);

    return true;
}

bool gravity::IntroScene::onTouchBegan(Touch* touch, Event  *event)
{
    return true;
}

void gravity::IntroScene::onTouchEnded(Touch* touch, Event  *event)
{
    auto game = GameScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(0.5, game, Color3B(0,0,0)));
}
void gravity::IntroScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event  *event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
    {
        exit(0);
    }
}
