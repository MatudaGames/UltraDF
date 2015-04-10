//
//  WorldMap.cpp
//  DwarfForest
//
//  Created by Kristaps Auzins on 15/12/14.
//
//

#include "WorldMap.h"
#include "AppMacros.h"
#include "GameScene.h"
#include "User.h"
#include "StoreScene.h"
#include "ChallengesScene.h"
#include "UpgradeScene.h"
#include "OptionsScene.h"
#include <SimpleAudioEngine.h>
#include "GameTutorial.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)

#include "ParseX.h"
#include "FacebookX.h"

#endif

USING_NS_CC;

CCScene* WorldMap::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    WorldMap *layer = WorldMap::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

WorldMap::WorldMap(): _levelUnlockAnim(NULL),_levelStars_1(NULL),_levelStars_2(NULL),_levelStars_3(NULL)
{
    
}

WorldMap::~WorldMap()
{
    if(_levelUnlockAnim)_levelUnlockAnim->release();
    if(_levelStars_1)_levelStars_1->release();
    if(_levelStars_2)_levelStars_2->release();
    if(_levelStars_3)_levelStars_3->release();
}


bool WorldMap::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    setTag(SCENE_WORLD_MAP);
    
    //Create the big map?
//    CCSprite* pSprite = CCSprite::create("WorldMap/WorldMap_1.png");
//    addChild(pSprite);
    
    /////////////////////////////
    // 3. add your codes below...
    
    // add a label shows "Hello World"
    // create and initialize a label
    
    // This is wroooong !!!
//    mLastMissionID = User::getInstance()->mCurrentMissionLevel;
    
    mLastMissionID = User::getInstance()->mWorldMapLastMission;
    if(mLastMissionID == -1){
        User::getInstance()->mWorldMapLastMission = mLastMissionID = User::getInstance()->mCurrentMissionLevel;
    }
    
    // Whats the screen size !!!
    mScreenSize = CCDirector::sharedDirector()->getVisibleSize();
    // Init the world map cords
//    std::string positions = "35,296,118,308,203,278,208,203,128,173,76,120,111,52,218,43,310,78,360,147";
    std::string positions =   "70,592,236,616,406,556,416,406,256,346,152,240,222,104,436,86,620,156,720,294,770,580,844,764,1040,886,1316,846,1536,690,1348,500,1476,298,1814,232,1922,504,1826,720,1690,904";
    // Summer map 70,592,236,616,406,556,416,406,256,346,152,240,222,104,436,86,620,156,720,294
    // Winter map 770,580,844,764,1040,886,1316,846,1536,690,1348,500,1476,298,1814,232,1922,504,1826,720,1690,904
    mWorldNodeCords = SplitString(positions,',');
    
    map_base = CCSprite::create( "button_freez.png" );
    map_base->setAnchorPoint( ccp( 0, 0 ) );
    
    pzLayer = PanZoomLayer::create();
    this->addChild( pzLayer );
    
	moveBackground = false;
    
    // Add the map layers !!!
    CCSprite *map_1 = CCSprite::create("WorldMap/WorldMapHD_1.png");
    map_1->setAnchorPoint(ccp(0,0));
    map_1->setPosition(ccp(0,0));
    map_base->addChild(map_1);
    
    float aSizeW = map_1->getContentSize().width;
    float aSizeH = 0;//map->getContentSize().height
    
    // Update the total height of map
    aSizeH+=map_1->getContentSize().height;
    
    CCSprite *map_2 = CCSprite::create("WorldMap/WorldMapHD_2.png");
    map_2->setAnchorPoint(ccp(0,0));
    map_2->setPosition(ccp(0,map_1->getContentSize().height));
    map_base->addChild(map_2);
    
    aSizeH+=map_2->getContentSize().height; // Update the total height of map
    
    // Set it correct in screen
    map_base->setPosition( ccp(aSizeW*-0.5,aSizeH*-0.5) );
    
    pzLayer->SetPanBoundsRect( CCRectMake(
                                          aSizeW * -0.5,
                                          aSizeH * -0.5,
                                          aSizeW * 1.0,
                                          aSizeH * 1.0) );
//    pzLayer->setScale( 1.0 );
    
//    pzLayer->SetMinScale(0.9375f);
    pzLayer->SetMinScale(0.46875f);
    pzLayer->SetMaxScale(1.0f);
    
    pzLayer->addChild( map_base, -1 );
    
    // Set it now at start
    pzLayer->setPosition(ccp(aSizeW,aSizeH));
    /*
    CCSprite *mark = CCSprite::create( "button_freez.png" );
    map_base->addChild( mark );
    mark->setPosition( map_base->getContentSize() * 0.5 );
    */
    
    mPlayingStar_1 = false;
    mPlayingStar_2 = false;
    mPlayingStar_3 = false;
    
    mNeedToShowNewLevel = false;
    
    mWaitForStarToPlay_2 = 0;
    mWaitForStarToPlay_3 = 0;
    
    mShowingNextLevel = false;
    
    // Values
    mCurrentMissionID = 0;//No mission
    
    AddMovingObjects();
    
    CreateLevels();
    
    CreatePlayer();
    
    PrepeareSmallMissionScreen();
    
    CreateHud();
    
    //ResetStats();
    
    return true;
}

// Some particles and stuff like that :)
void WorldMap::AddMovingObjects()
{
    // Some test particles
    CCParticleSystemQuad* p = CCParticleSystemQuad::create("Particles/AutumnFly_01.plist");
    p->setPosition(976, 1920);
    p->setPositionType(kCCPositionTypeGrouped);
    p->setAutoRemoveOnFinish(false);
    map_base->addChild(p,1);
    
    p = CCParticleSystemQuad::create("Particles/AutumnFly_01.plist");
    p->setPosition(406, 2280);
    p->setPositionType(kCCPositionTypeGrouped);
    p->setAutoRemoveOnFinish(false);
    map_base->addChild(p,1);
    
    p = CCParticleSystemQuad::create("Particles/AutumnFly_01.plist");
    p->setPosition(1194, 2272);
    p->setPositionType(kCCPositionTypeGrouped);
    p->setAutoRemoveOnFinish(false);
    map_base->addChild(p,1);
    
    p = CCParticleSystemQuad::create("Particles/AutumnFly_01.plist");
    p->setPosition(1664, 1962);
    p->setPositionType(kCCPositionTypeGrouped);
    p->setAutoRemoveOnFinish(false);
    map_base->addChild(p,1);
    
    p = CCParticleSystemQuad::create("Particles/AutumnFly_01.plist");
    p->setPosition(1812, 2240);
    p->setPositionType(kCCPositionTypeGrouped);
    p->setAutoRemoveOnFinish(false);
    map_base->addChild(p,1);
    
    // The snow
    p = CCParticleSystemQuad::create("Particles/SnowEasy.plist");
    p->setPosition(1000, 900);
    p->cocos2d::CCNode::setScale(0.8f);
    p->setPositionType(kCCPositionTypeGrouped);
    p->setAutoRemoveOnFinish(false);
    map_base->addChild(p,1);
    
    // The autumn
    p = CCParticleSystemQuad::create("effects/wind/Wind_New.plist");
    p->setPosition(ccp(0,1520));
//    p->cocos2d::CCNode::setScale(0.6f);
    p->setPositionType(kCCPositionTypeGrouped);
    p->setAutoRemoveOnFinish(false);
    map_base->addChild(p,1);//The reain tag :)
    

}

void WorldMap::CreateLevels()
{
    CCMenuItemImage *dummyItem;
    CCArray*array = CCArray::create();
    
    CCSprite* text_num_1;
    
    int aTotalMapNodes = mWorldNodeCords.size()/2;
    
//    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame("DialogFont_Hint.png", "hintttt.plist");
    
//    CCTextureCache::sharedTextureCache()->addImage("FontsBmp/DialogFont_Hint.plist");
    
    // Preload that batch file
//    worldmap_batch.png
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("WorldMap/worldmap_batch.plist");
    
    _batchWorld = CCSpriteBatchNode::create("WorldMap/worldmap_batch.png");
    _batchWorld->setPosition(CCPointZero);
    map_base->addChild(_batchWorld);
    
    bool _missionCompleted = false;
    int _missionStarsEarned = 0;
    
    // Create all the levels
    for(int i=0;i<aTotalMapNodes;i++)
    {
        //Add button above?
        _missionCompleted = false;
        _missionStarsEarned = -1;
        
        CCLog("Mission Level: %i <= %i",i,(User::getInstance()->mCurrentMissionLevel-1));
        
        if(User::getInstance()->GetUserMissionInfo(i,1)>=0){
            CCLog("Mission is unlocked [%i] !!!",i);
            // we have unlocked it !!!
            _missionCompleted = true;
            
            // 1=points | 2=stars
            _missionStarsEarned = User::getInstance()->GetUserMissionInfo(i,2);
        }
        
        /*
        if((User::getInstance()->mCurrentMissionLevel-1)>=i){
//            CCLog("Mission Level: %i <= %i",i,(User::getInstance()->mCurrentMissionLevel-1));
            
            // If tutorial enabled and step for base tutorial not completed - ignore this !!!
            
            _missionCompleted = true;
            
            // 1=points | 2=stars
            _missionStarsEarned = User::getInstance()->GetUserMissionInfo(i,2);
        }
        */
        
        dummyItem = CCMenuItemImage::create("WorldMap/mission_panel.png",
                                              "WorldMap/mission_panel.png",
                                              this,
                                              menu_selector(WorldMap::OnClickedMission));
        dummyItem->setTag(i+1);
        dummyItem->setOpacity(0);
//        dummyItem->setColor(ccBLUE);
        dummyItem->setPosition(ccp(mWorldNodeCords[i*2],mWorldNodeCords[i*2+1]));
        array->addObject(dummyItem);
        
        // Add the locked nodes etc !!!
        CCSprite* mission_panel = CCSprite::createWithSpriteFrameName("mission_panel.png");
        mission_panel->setTag(1000+i);
        mission_panel->setAnchorPoint(ccp(0.5,0.5));
        mission_panel->setPosition(ccp(mWorldNodeCords[i*2],mWorldNodeCords[i*2+1]));
        _batchWorld->addChild(mission_panel);
        
        CCSprite* flag_locked = CCSprite::createWithSpriteFrameName("mission_flag_locked.png");
        flag_locked->setTag(MISSION_FLAG_LOCKED);
        flag_locked->setVisible(true);
        flag_locked->setAnchorPoint(ccp(0.5,0));
        flag_locked->setPosition(ccp(mission_panel->getContentSize().width/2,23));//28
        mission_panel->addChild(flag_locked);
        
        CCSprite* flag_open = CCSprite::createWithSpriteFrameName("mission_flag.png");
        flag_open->setTag(MISSION_FLAG_OPEN);
        flag_open->setVisible(false);
        flag_open->setAnchorPoint(ccp(0.5,0));
        flag_open->setPosition(ccp(mission_panel->getContentSize().width/2+0.5,22.5));
        mission_panel->addChild(flag_open);
        
        // The 3 stars
        for(int i=0;i<3;i++)
        {
            CCSprite* flag_star = CCSprite::createWithSpriteFrameName("star.png");
            flag_star->setTag(MISSION_FLAG_STAR+i);
            flag_star->setAnchorPoint(ccp(0.5,0.5));
            
            // Star positions
            if(i == 0){
                flag_star->setPosition(ccp(24,80));
            }
            else if(i == 1){
                flag_star->setPosition(ccp(42,103));
            }
            else{
                flag_star->setPosition(ccp(59,80));
            }
            
            // hide not needed stuff
            if(i<=_missionStarsEarned-1){
                CCLog("Mission Stars: %i <= %i",i,(_missionStarsEarned-1));
                flag_star->setVisible(true);
            }
            else{
                flag_star->setVisible(false);
            }
            
            flag_open->addChild(flag_star);
        }
        
        // The numbers???
        
        // Check if this location is locked or not
        if(_missionCompleted)
        {
            // Check if has any stars completed !!!
            flag_open->setVisible(true);
            flag_locked->setVisible(false);
            
            // Show the number
            std::stringstream number_panel;
            number_panel << (i+1);
            
            if(number_panel.str().size()==1){
                // Only one number
                std::stringstream theValue;
                theValue << "world_map_num_" << number_panel.str() << ".png";
                
                text_num_1 = CCSprite::createWithSpriteFrameName(theValue.str().c_str());
                text_num_1->setPosition(ccp(44,156));
                flag_open->addChild(text_num_1);
                
            }
            else{
                // Two numbers
                std::stringstream theValue_1;
                std::stringstream theValue_2;
                
                theValue_1 << "world_map_num_" << number_panel.str().at(0) << ".png";
                theValue_2 << "world_map_num_" << number_panel.str().at(1) << ".png";
                
                text_num_1 = CCSprite::createWithSpriteFrameName(theValue_1.str().c_str());
                text_num_1->setPosition(ccp(36,156));
                flag_open->addChild(text_num_1);
                
                // The 2nd number
                text_num_1 = CCSprite::createWithSpriteFrameName(theValue_2.str().c_str());
                text_num_1->setPosition(ccp(52,156));
                flag_open->addChild(text_num_1);
            }
        }
    }
    
    // Init the animation
    _levelUnlockAnim = SpriteAnimation::create("WorldMap/mission_unlock.plist",false);
    _levelUnlockAnim->retain();
    
    _levelStars_1 = SpriteAnimation::create("WorldMap/star_pop.plist",false);
    _levelStars_1->retain();
    _levelStars_2 = SpriteAnimation::create("WorldMap/star_pop.plist",false);
    _levelStars_2->retain();
    _levelStars_3 = SpriteAnimation::create("WorldMap/star_pop.plist",false);
    _levelStars_3->retain();
    
    // This is debug
    /*
    CCDelayTime* aDelay1 = CCDelayTime::create(1);
    CCCallFuncN* aFuncDone = CCCallFuncN::create(this, callfuncN_selector(WorldMap::delayDebug));
    CCDelayTime* aDelay2 = CCDelayTime::create(2);
    CCCallFuncN* aFuncDone2 = CCCallFuncN::create(this, callfuncN_selector(WorldMap::delayDebug2));
    CCSequence* aSeq = CCSequence::create(aDelay1,aFuncDone,aDelay2,aFuncDone2,NULL);
    runAction(aSeq);
    */
    
    // Add test animation flag
    /*
    SpriteAnimation* aChangeFlagAnim = SpriteAnimation::create("WorldMap/mission_unlock.plist");
    aChangeFlagAnim->setAnchorPoint(ccp(0.5,0));
//    aChangeFlagAnim->setOpacity(128);
    aChangeFlagAnim->setPosition(ccp(mWorldNodeCords[6*2],mWorldNodeCords[6*2+1]));
    map_base->addChild(aChangeFlagAnim);
    */
    
    
    CCMenu*menu = CCMenu::createWithArray(array);
    menu->setPosition(ccp(0,0));
    map_base->addChild(menu,1);
}

void WorldMap::delayDebug()
{
    UnlockLevel(6);
}

void WorldMap::delayDebug2()
{
    ShowMissionStarsEarned(6, 1,0);
}

void WorldMap::update(float delta)
{
    UpdateMap(delta);
}

void WorldMap::UpdateMap(float delta)
{
    //.................................................................................................
    // The star show up animations
    
    if(mPlayingStar_1)
    {
        if(_levelStars_1->_action->isDone())
        {
            // Stop it
            mPlayingStar_1 = false;
            
            // Remove and show the real star
            CCSprite* flag_spot = (CCSprite*)_batchWorld->getChildByTag(1000+mCurrentCompletedMissionID);
            CCSprite* flag_open = (CCSprite*)flag_spot->getChildByTag(MISSION_FLAG_OPEN);
            flag_open->getChildByTag(MISSION_FLAG_STAR+0)->setVisible(true);
            
            map_base->removeChild(_levelStars_1, false);
        }
    }
    
    if(mPlayingStar_2)
    {
        if(_levelStars_2->_action->isDone())
        {
            // Stop it
            mPlayingStar_2 = false;
            
            // Remove and show the real star
            CCSprite* flag_spot = (CCSprite*)_batchWorld->getChildByTag(1000+mCurrentCompletedMissionID);
            CCSprite* flag_open = (CCSprite*)flag_spot->getChildByTag(MISSION_FLAG_OPEN);
            flag_open->getChildByTag(MISSION_FLAG_STAR+1)->setVisible(true);
            
            map_base->removeChild(_levelStars_2, false);
        }
    }
    else if(mWaitForStarToPlay_2>0)
    {
        mWaitForStarToPlay_2-=delta;
        if(mWaitForStarToPlay_2<=0){
            mWaitForStarToPlay_2 = 0;
            map_base->addChild(_levelStars_2);
            mPlayingStar_2 = true;
        }
    }
    
    if(mPlayingStar_3)
    {
        if(_levelStars_3->_action->isDone())
        {
            // Stop it
            mPlayingStar_3 = false;
            
            // Remove and show the real star
            CCSprite* flag_spot = (CCSprite*)_batchWorld->getChildByTag(1000+mCurrentCompletedMissionID);
            CCSprite* flag_open = (CCSprite*)flag_spot->getChildByTag(MISSION_FLAG_OPEN);
            flag_open->getChildByTag(MISSION_FLAG_STAR+2)->setVisible(true);
            
            map_base->removeChild(_levelStars_3, false);
        }
    }
    else if(mWaitForStarToPlay_3>0)
    {
        mWaitForStarToPlay_3-=delta;
        if(mWaitForStarToPlay_3<=0){
            mWaitForStarToPlay_3 = 0;
            map_base->addChild(_levelStars_3);
            mPlayingStar_3 = true;
        }
    }
    
    if(mNeedToShowNewLevel){
        if(mPlayingStar_1==false
           && mWaitForStarToPlay_2<=0 && mPlayingStar_2==false
           && mWaitForStarToPlay_3<=0 && mPlayingStar_3==false){
            mNeedToShowNewLevel = false;
            UnlockLevel(User::getInstance()->mCurrentMissionLevel-1);
        }
    }
    
    //.................................................................................................
    // The flag show up
    
    if(!mShowingNextLevel){
        return;
    }
    
    // Check if completed - then change
    if(_levelUnlockAnim->_action->isDone())
    {
        // Show the unlocked stuff
        mShowingNextLevel = false;
        
        // Get the real unlocked flag and show it
        CCSprite* flag_spot = (CCSprite*)_batchWorld->getChildByTag(1000+mCurrentUnlockID);
        CCSprite* flag_open = (CCSprite*)flag_spot->getChildByTag(MISSION_FLAG_OPEN);//->setVisible(true);
        flag_open->setVisible(true);
        
        // Check how much stars needs
        map_base->removeChild(_levelUnlockAnim,false);
        
        // The startsstss
//        mCurrentUnlockID = 99;
        
        // Show the number
        std::stringstream number_panel;
        number_panel << mCurrentUnlockID+1;
        
        CCSprite* text_num_1;
        
        if(number_panel.str().size()==1){
            // Only one number
            std::stringstream theValue;
            theValue << "world_map_num_" << number_panel.str() << ".png";
            
            text_num_1 = CCSprite::createWithSpriteFrameName(theValue.str().c_str());
            text_num_1->setPosition(ccp(44,156));
            text_num_1->setScale(0);
            flag_open->addChild(text_num_1);
            
            CCScaleTo* aScale = CCScaleTo::create(0.25f,1);
            CCEaseBackOut* aEase = CCEaseBackOut::create(aScale);
            text_num_1->runAction(aEase);
            
        }
        else{
            // Two numbers
            std::stringstream theValue_1;
            std::stringstream theValue_2;
            
            theValue_1 << "world_map_num_" << number_panel.str().at(0) << ".png";
            theValue_2 << "world_map_num_" << number_panel.str().at(1) << ".png";
            
            text_num_1 = CCSprite::createWithSpriteFrameName(theValue_1.str().c_str());
            text_num_1->setPosition(ccp(52,156));
            text_num_1->setScale(0);
            flag_open->addChild(text_num_1);
            
            CCScaleTo* aScale = CCScaleTo::create(0.25f,1);
            CCEaseBackOut* aEase = CCEaseBackOut::create(aScale);
            text_num_1->runAction(aEase);
            
            // The 2nd number
            text_num_1 = CCSprite::createWithSpriteFrameName(theValue_2.str().c_str());
            text_num_1->setPosition(ccp(36,156));
            text_num_1->setScale(0);
            flag_open->addChild(text_num_1);
            
            aScale = CCScaleTo::create(0.25f,1);
            aEase = CCEaseBackOut::create(aScale);
            text_num_1->runAction(aEase);
        }
        
//        User::getInstance()->SaveUserMissionInfo();
        User::getInstance()->SaveUserMissionInfo(User::getInstance()->mCurrentMissionLevel-1, 0, 0);
        User::getInstance()->SaveUserMissionUnlock();
        
        // Start now player move !!!
        CCMoveTo* aMove1 = CCMoveTo::create(1.0f,ccp(mWorldNodeCords[(User::getInstance()->mCurrentMissionLevel-1)*2],
                                                     mWorldNodeCords[(User::getInstance()->mCurrentMissionLevel-1)*2+1]+20));
        CCEaseExponentialInOut* aEase1 = CCEaseExponentialInOut::create(aMove1);
        CCCallFuncN* aFuncDone = CCCallFuncN::create(this, callfuncN_selector(WorldMap::OnPlayerFinishedMove));
        CCSequence* aSeq1 = CCSequence::create(aEase1,aFuncDone,NULL);
        mPlayer->runAction(aSeq1);
    }
}

void WorldMap::ShowMissionStarsEarned(int theMissionID,int theStars,int fromStars)
{
    mCurrentCompletedMissionID = theMissionID;
    
    // Do the magic
    CCSprite* flag_spot = (CCSprite*)_batchWorld->getChildByTag(1000+theMissionID);
    CCSprite* flag_open = (CCSprite*)flag_spot->getChildByTag(MISSION_FLAG_OPEN);
    CCSprite* aDummyStar;
    
    int aPosX = flag_spot->getPositionX()+flag_open->getPositionX();
    int aPosY = flag_spot->getPositionY()+flag_open->getPositionY();
    
    // Fly the star animation
    if(fromStars == 0 && theStars>0){
        aDummyStar = (CCSprite*)flag_open->getChildByTag(MISSION_FLAG_STAR);
        _levelStars_1->setPosition(ccp(aPosX+aDummyStar->getPositionX()-83,aPosY+aDummyStar->getPositionY()-24));
        map_base->addChild(_levelStars_1);
        mPlayingStar_1 = true;
        fromStars+=1;
        theStars-=1;
    }
    if(fromStars == 1 && theStars>0){
        aDummyStar = (CCSprite*)flag_open->getChildByTag(MISSION_FLAG_STAR+1);
        _levelStars_2->setPosition(ccp(aPosX+aDummyStar->getPositionX()-83,aPosY+aDummyStar->getPositionY()-24));
        mWaitForStarToPlay_2 = 0.5f;
        fromStars+=1;
        theStars-=1;
    }
    if(fromStars == 2 && theStars>0){
        aDummyStar = (CCSprite*)flag_open->getChildByTag(MISSION_FLAG_STAR+2);
        _levelStars_3->setPosition(ccp(aPosX+aDummyStar->getPositionX()-83,aPosY+aDummyStar->getPositionY()-24));
        mWaitForStarToPlay_3 = 1.0f;
    }
}

// Camera magic focus (drunk)
void WorldMap::FocusCameraToCords(int theX,int theY)
{
    // A bit of offset
    theX-=100;
    theY-=100;
    
    int aPercentX = (theX*100)/2048;
    int aRealX = (1024-(1088*aPercentX)/100);//Offest of camera
    if(aRealX+200>=1024){
        aRealX = 1024;
    }
    else{
        aRealX+=200;
    }
    
    int aPrecentY = (theY*100)/2560;
    int aRealY = (1280-(1840*aPrecentY)/100);
    if(aRealY+100>=1280){
        aRealY = 1280;
    }
    else{
        aRealY+=100;
    }
    
    CCMoveTo* aMapMove = CCMoveTo::create(1.0f, ccp(aRealX,aRealY));
    CCEaseSineOut* aEase = CCEaseSineOut::create(aMapMove);
    pzLayer->runAction(aEase);
}

void WorldMap::UnlockLevel(int theID)
{
    // This is our stuff
    mCurrentUnlockID = theID;
    
    _levelUnlockAnim->setPosition(ccp(mWorldNodeCords[theID*2],mWorldNodeCords[theID*2+1]-5));
    _levelUnlockAnim->setAnchorPoint(ccp(0.5,0));
    
    // Start the animation
    map_base->addChild(_levelUnlockAnim);
    
    // Get the current locked flag
    CCSprite* flag_spot = (CCSprite*)_batchWorld->getChildByTag(1000+theID);
    flag_spot->getChildByTag(MISSION_FLAG_LOCKED)->setVisible(false);
    
    mShowingNextLevel = true;
}

void WorldMap::OnClickedPlayer(CCObject* sender)
{
    ShowMissionScreen(User::getInstance()->mCurrentMissionLevel);
}

void WorldMap::OnClickedMission(CCObject* sender)
{
    // Do not open other mission
    if(mSmallMissionScreen->isVisible()){
        return;
    }
    
    //get the button id !!!
    CCMenuItemImage* aButton =(CCMenuItemImage*)sender;
    int aID = aButton->getTag();
    CCLog("Clicked Mission:%i",aID);
    MissionTaskInditificator(aID);
    
    //Check if this mission is clickable !!!
//    if(aID > User::getInstance()->mCurrentMissionLevel){
//        return;
//    }
    
    ccColor3B m_from = aButton->getColor();
    
    if(m_from.r == ccRED.r && m_from.g == ccRED.b && m_from.b == ccRED.b){
        aButton->setColor(ccBLUE);
    }
    else{
        aButton->setColor(ccRED);
    }
    
    ShowMissionScreen(aID);
}

void WorldMap::MissionTaskInditificator(int theID)
{
	WhatMission = theID;
	
	mCall = User::getInstance()->getMissionManager().GetMissionByID(WhatMission-1);
    
    

	TESTdwarfCount = mCall.Mission_SaveDwarfs;
	
	CCSprite* aScreenBooster2 = CCSprite::create("WorldMap/NewPregame/BoosterCardDamage.png");
    aScreenBooster2->setPosition(ccp(mSmallMissionScreen->getContentSize().width/4+80,mSmallMissionScreen->getContentSize().height/3-43));
    aScreenBooster2->setTag(712);
    
    CCSprite* aScreenBooster3 = CCSprite::create("WorldMap/NewPregame/BoosterCardRange.png");
    aScreenBooster3->setPosition(ccp(mSmallMissionScreen->getContentSize().width/4+270,mSmallMissionScreen->getContentSize().height/3-43));
    aScreenBooster3->setTag(713);
    
    CCSprite* Tootem = CCSprite::create("WorldMap/NewPregame/Totem.png");
    Tootem->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0-375,mSmallMissionScreen->getContentSize().height/1.2-110));
    Tootem->setTag(717);
    
    CCSprite* TaskVariables = CCSprite::create("WorldMap/NewPregame/TXTtotem.png");
    TaskVariables->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0-220,mSmallMissionScreen->getContentSize().height/1.2-110));
    TaskVariables->setTag(718);
    
    CCMenuItemImage* plusOneDamage = CCMenuItemImage::create(
                                                        "WorldMap/NewPregame/ButtonPlusOne.png",
                                                        "WorldMap/NewPregame/ButtonPlusOne.png",
                                                        this,
                                                        menu_selector(WorldMap::BuyMoreDamage));
    plusOneDamage->setTag(714);//Play the level
    plusOneDamage->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0-185,mSmallMissionScreen->getContentSize().height/2-200));
    
    CCMenuItemImage* plusOneRange = CCMenuItemImage::create(
                                                        "WorldMap/NewPregame/ButtonPlusOne.png",
                                                        "WorldMap/NewPregame/ButtonPlusOne.png",
                                                        this,
                                                        menu_selector(WorldMap::BuyMoreRange));
    plusOneRange->setTag(715);//Play the level
    plusOneRange->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+5,mSmallMissionScreen->getContentSize().height/2-200));
	
	damagePrice =CCLabelBMFont::create("0", "PowerButtons/PB_Font.fnt",150, kCCTextAlignmentCenter);
   	damagePrice->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2-125,mSmallMissionScreen->getContentSize().height/2-200));//160,700 
    damagePrice->setWidth(10);
    damagePrice->setTag(300017);
    
	
	rangePrice =CCLabelBMFont::create("0", "PowerButtons/PB_Font.fnt",150, kCCTextAlignmentCenter);
   	rangePrice->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2+50,mSmallMissionScreen->getContentSize().height/2-200));//160,700 
    rangePrice->setWidth(10);
    rangePrice->setTag(300018);
	
	if(mCall.Task_type==6 || mCall.Task_type==8)
	{
	_damageCount = CCLabelTTF::create("0","PowerButtons/PB_Font.fnt", TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    _damageCount->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0-2,mSmallMissionScreen->getContentSize().height/2.0+37));//160,700 
    _damageCount->setTag(30012);
    _damageCount->setColor(ccc3(36,102,102));
    mSmallMissionScreen->addChild(_damageCount,1);
	
	_rangeSize = CCLabelTTF::create("0","PowerButtons/PB_Font.fnt", TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    _rangeSize->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+181,mSmallMissionScreen->getContentSize().height/2.0+37));//160,700
    _rangeSize->setTag(30013);
    _rangeSize->setColor(ccc3(36,102,102));
    mSmallMissionScreen->addChild(_rangeSize,1);	
    		
	mSmallMissionScreen->addChild(damagePrice,1);
	mSmallMissionScreen->addChild(rangePrice,1);	
	mSmallMissionScreen->addChild(aScreenBooster2);
	mSmallMissionScreen->addChild(aScreenBooster3);
	CCMenu* aButtonMenuA = CCMenu::create(plusOneDamage,plusOneRange,NULL);
    aButtonMenuA->setAnchorPoint(ccp(0,0));
    aButtonMenuA->setPosition(ccp(0,0));
    aButtonMenuA->setTag(716);
    mSmallMissionScreen->addChild(aButtonMenuA);
    int aTotalSpells = User::getInstance()->getItemDataManager().mSpellDataVector.size();
    for(int i = 0;i<aTotalSpells;i++)
    {
    	if(User::getInstance()->getItemDataManager().isItemActive(User::getInstance()->getItemDataManager().mSpellDataVector[i].id))
    	{
    		bool _upgradable = false;
    		int _itemLevel = 0;
    		spellDamage = 0;
    		if(User::getInstance()->getItemDataManager().mSpellDataVector[i].upgrade_available){
            _upgradable = true;
            _itemLevel = User::getInstance()->getItemDataManager().getSpellItemLevel(User::getInstance()->getItemDataManager().mSpellDataVector[i].id);
        	}
        	else{
            _itemLevel = 0;
            _upgradable = false;
        	}
        	if(_upgradable)
        	{
    		spellDamage = User::getInstance()->getItemDataManager().mSpellDataVector[i].upgrade_damage[_itemLevel];
    		}else{
			spellDamage = User::getInstance()->getItemDataManager().mSpellDataVector[i].damage;
    		}
    		//User::getInstance()->getItemDataManager().addExtraRange(User::getInstance()->getItemDataManager().mSpellDataVector[i].id);
    		spellRange = User::getInstance()->getItemDataManager().mSpellDataVector[i].range;
    		std::stringstream damageCountReal;
    		damageCountReal<<int(spellDamage);
    		_damageCount->setString(damageCountReal.str().c_str());
    		
    		std::stringstream rangeCountReal;
    		rangeCountReal<<int(spellRange);
    		_rangeSize->setString(rangeCountReal.str().c_str());
			
			std::stringstream damagePriceReal;
    		damagePriceReal<<int(mCall.STORE_Booster_DamagePrice);
    		damagePrice->setString(damagePriceReal.str().c_str());
    	
    		std::stringstream rangePriceReal;
    		rangePriceReal<<int(mCall.STORE_Booster_RangePrice);
    		rangePrice->setString(rangePriceReal.str().c_str());
		}
	}
	}
	if (mCall.Task_type==1)
	{
		taskInfo = CCLabelTTF::create("RESCUE DWARFS","fonts/Marker Felt.ttf", TITLE_FONT_SIZE*0.8, CCSize(350,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+110,mSmallMissionScreen->getContentSize().height/2.0+345));//160,700
    	taskInfo->setTag(30004);
    	taskInfo->setColor(ccc3(36,102,102));
    	mSmallMissionScreen->addChild(taskInfo);
		mSmallMissionScreen->removeChildByTag(712);	
		mSmallMissionScreen->removeChildByTag(713);	
	}else if(mCall.Task_type==11){
		taskInfo = CCLabelTTF::create("SURVIVE","fonts/Marker Felt.ttf", TITLE_FONT_SIZE*0.8, CCSize(350,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+110,mSmallMissionScreen->getContentSize().height/2.0+345));//160,700
    	taskInfo->setTag(30004);
    	taskInfo->setColor(ccc3(36,102,102));
    	mSmallMissionScreen->addChild(taskInfo);
	}else if(mCall.Task_type==6){
		taskInfo = CCLabelTTF::create("DEFEAT MASTER TROLL","fonts/Marker Felt.ttf", TITLE_FONT_SIZE*0.8, CCSize(550,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+215,mSmallMissionScreen->getContentSize().height/2.0+345));//160,700
    	taskInfo->setTag(30004);
    	taskInfo->setColor(ccc3(36,102,102));
    	mSmallMissionScreen->addChild(taskInfo);		
	}
	else if (mCall.Task_type==8){
		taskInfo = CCLabelTTF::create("DESTROY TOTEM","fonts/Marker Felt.ttf", TITLE_FONT_SIZE*0.8, CCSize(350,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+110,mSmallMissionScreen->getContentSize().height/2.0+345));//160,700
    	taskInfo->setTag(30004);
    	taskInfo->setColor(ccc3(36,102,102));
    	mSmallMissionScreen->addChild(taskInfo);
    	
    	_totemHP = CCLabelTTF::create("","fonts/Marker Felt.ttf", TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	_totemHP->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0-125,mSmallMissionScreen->getContentSize().height/1.2+15));//160,700
		_totemHP->setTag(30009);
    	_totemHP->setColor(ccc3(36,102,102));
    	mSmallMissionScreen->addChild(_totemHP);
		
    	std::stringstream _totemHPReal;
    	_totemHPReal<<int(mCall.TOTEM_HP);
    	_totemHP->setString(_totemHPReal.str().c_str());
    	
    	if(mCall.TOTEM_Event_Type1==1)
    	{
    	_totemShield = CCLabelTTF::create("Bubble Shield","fonts/Marker Felt.ttf", TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	_totemShield->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0-80,mSmallMissionScreen->getContentSize().height/1.2-20));//160,700
		_totemShield->setTag(30010);
    	_totemShield->setColor(ccc3(36,102,102));
    	mSmallMissionScreen->addChild(_totemShield);
    	}else if (mCall.TOTEM_Event_Type1==2){
    	_totemShield = CCLabelTTF::create("Spike Shield","fonts/Marker Felt.ttf", TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	_totemShield->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0-80,mSmallMissionScreen->getContentSize().height/1.2-20));//160,700
		_totemShield->setTag(30010);
    	_totemShield->setColor(ccc3(36,102,102));
    	mSmallMissionScreen->addChild(_totemShield);	
    	}
    	
    	if(mCall.TOTEM_Event_Type3==1)
    	{
    		_totemAttack = CCLabelTTF::create("Bullets","fonts/Marker Felt.ttf", TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    		_totemAttack->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0-75,mSmallMissionScreen->getContentSize().height/1.2-55));//160,700
			_totemAttack->setTag(30011);
    		_totemAttack->setColor(ccc3(36,102,102));
    		mSmallMissionScreen->addChild(_totemAttack);
    	}else if (mCall.TOTEM_Event_Type3==2){
    		_totemAttack = CCLabelTTF::create("Flamethrower","fonts/Marker Felt.ttf", TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    		_totemAttack->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0-75,mSmallMissionScreen->getContentSize().height/1.2-55));//160,700
			_totemAttack->setTag(30011);
    		_totemAttack->setColor(ccc3(36,102,102));
    		mSmallMissionScreen->addChild(_totemAttack);
    	}
    	
    	mSmallMissionScreen->addChild(Tootem);
    	mSmallMissionScreen->addChild(TaskVariables);		
	}else if (mCall.Task_type==10){
		taskInfo = CCLabelTTF::create("Gather Crystals","fonts/Marker Felt.ttf", TITLE_FONT_SIZE*0.8, CCSize(550,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+215,mSmallMissionScreen->getContentSize().height/2.0+355));//160,700
    	taskInfo->setTag(30004);
    	taskInfo->setColor(ccc3(36,102,102));
    	mSmallMissionScreen->addChild(taskInfo);	
		mSmallMissionScreen->removeChildByTag(712);		
		mSmallMissionScreen->removeChildByTag(713);	
	}
	
	if(mCall.Mission_SaveDwarfs)
	{
		dwarfCount = CCLabelTTF::create("","PowerButtons/PB_Font.fnt", TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	dwarfCount->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0-190,mSmallMissionScreen->getContentSize().height/2.0+37));//160,700
    	dwarfCount->setTag(30005);
    	dwarfCount->setColor(ccc3(36,102,102));
    	mSmallMissionScreen->addChild(dwarfCount);	
    	
    	std::stringstream dwarfCountReal;
    	dwarfCountReal<<int(mCall.Mission_SaveDwarfs);
    	dwarfCount->setString(dwarfCountReal.str().c_str());
    	
    	CCMenuItemImage* plusOne = CCMenuItemImage::create(
                                                        "WorldMap/NewPregame/ButtonPlusOne.png",
                                                        "WorldMap/NewPregame/ButtonPlusOne.png",
                                                        this,
                                                        menu_selector(WorldMap::BuyMoreDwarfs));
    	plusOne->setTag(1);//Play the level
    	plusOne->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0-377,mSmallMissionScreen->getContentSize().height/2-200));
    
    	CCMenu* aButtonMenu2 = CCMenu::create(plusOne,NULL);
    	aButtonMenu2->setAnchorPoint(ccp(0,0));
    	aButtonMenu2->setPosition(ccp(0,0));
    	mSmallMissionScreen->addChild(aButtonMenu2);
    
     	dwarfPrice =CCLabelBMFont::create("50", "PowerButtons/PB_Font.fnt",150, kCCTextAlignmentCenter);
   	 	dwarfPrice->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0-325,mSmallMissionScreen->getContentSize().height/2-200));
    	//dwarfPrice->setColor(ccc3(36,102,102));
    	dwarfPrice->setWidth(10);
    	dwarfPrice->setTag(30006);
    	mSmallMissionScreen->addChild(dwarfPrice);	
    	
    	std::stringstream dwarfPriceReal;
    	dwarfPriceReal<<int(mCall.STORE_Booster_DwarfPrice);
    	dwarfPrice->setString(dwarfPriceReal.str().c_str());
    
	}
	
		_diamondsLabel = CCLabelTTF::create("Game", "fonts/Marker Felt.ttf", TITLE_FONT_SIZE*0.5, CCSize(90, 55), kCCTextAlignmentRight, kCCVerticalTextAlignmentBottom);
		_diamondsLabel->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+234,mSmallMissionScreen->getContentSize().height/2+95));
		_diamondsLabel->setString(toString(User::getInstance()->getDiamonds()).c_str());
    	_diamondsLabel->setColor(ccc3(255,246,200));
    	_diamondsLabel->setTag(30007);
    	mSmallMissionScreen->addChild(_diamondsLabel);	
    	
    	_crystalsLabel = CCLabelTTF::create("Game", "fonts/Marker Felt.ttf", TITLE_FONT_SIZE*0.5, CCSize(90, 55), kCCTextAlignmentRight, kCCVerticalTextAlignmentBottom);
		_crystalsLabel->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+234,mSmallMissionScreen->getContentSize().height/2+175));
		_crystalsLabel->setString(toString(User::getInstance()->getCrystals()).c_str());
    	_crystalsLabel->setColor(ccc3(255,246,200));
    	_crystalsLabel->setTag(30008);
    	mSmallMissionScreen->addChild(_crystalsLabel);	
    	
	
	if(WhatMission == 1)//Identificate what mission number we need
	{
		//mCall = User::getInstance()->getMissionManager().GetMissionByID(0);
		//mCall.Task_type
		/*
		CCSprite* missionNumber = CCSprite::create("WorldMap/mission_number/M1.png");
    	missionNumber->setPosition(ccp(595,570));//550,480
    	missionNumber->setTag(30002);
    	missionNumber->setScale(0.5);
    	mSmallMissionScreen->addChild(missionNumber);
    	
    	mCall = User::getInstance()->getMissionManager().GetMissionByID(0);
 		int minCrystalCount = mCall.Star_1;
		
		taskInfo = CCLabelTTF::create("",FONT_SKRANJI, TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+140,mSmallMissionScreen->getContentSize().height/2.0+220));//160,700
    	taskInfo->setTag(30004);
    	mSmallMissionScreen->addChild(taskInfo);	
    
    	std::stringstream taskInfoCount;
    	taskInfoCount<<"Collect "<<int(minCrystalCount)<<" green Crystals";
    	taskInfo->setString(taskInfoCount.str().c_str());
	
		CCSprite* missionTask = CCSprite::create("WorldMap/mission_number/defeat_master_troll.png");
    	missionTask->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+120,mSmallMissionScreen->getContentSize().height/2.0+120));//550,480
    	missionTask->setTag(30004);
    	mSmallMissionScreen->addChild(missionTask);
			*/		
	}else if(WhatMission == 2)
	{
		/*
		CCSprite* missionNumber = CCSprite::create("WorldMap/mission_number/M2.png");
    	missionNumber->setPosition(ccp(595,570));//550,480
    	missionNumber->setTag(30002);
    	missionNumber->setScale(0.5);
    	mSmallMissionScreen->addChild(missionNumber);	
    	
    	mCall = User::getInstance()->getMissionManager().GetMissionByID(1);
 		int minCrystalCount = mCall.Star_1;
		
		taskInfo = CCLabelTTF::create("",FONT_SKRANJI, TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+140,mSmallMissionScreen->getContentSize().height/2.0+220));//160,700
    	taskInfo->setTag(30004);
    	mSmallMissionScreen->addChild(taskInfo);	
    
    	std::stringstream taskInfoCount;
    	taskInfoCount<<"Collect "<<int(minCrystalCount)<<" green Crystals";
    	taskInfo->setString(taskInfoCount.str().c_str());
		//removeChildByTag(30004);
		
		CCSprite* missionTask = CCSprite::create("WorldMap/mission_number/defeat_master_troll.png");
    	missionTask->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+120,mSmallMissionScreen->getContentSize().height/2.0+120));//550,480
    	missionTask->setTag(30004);
    	mSmallMissionScreen->addChild(missionTask);	
		*/			
	}else if(WhatMission == 3)
	{
		/*
		CCSprite* missionNumber = CCSprite::create("WorldMap/mission_number/M3.png");
    	missionNumber->setPosition(ccp(595,570));//550,480
    	missionNumber->setTag(30002);
    	missionNumber->setScale(0.5);
    	mSmallMissionScreen->addChild(missionNumber);	
    	
    	mCall = User::getInstance()->getMissionManager().GetMissionByID(2);
 		int minCrystalCount = mCall.Star_1;
		
		taskInfo = CCLabelTTF::create("",FONT_SKRANJI, TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+140,mSmallMissionScreen->getContentSize().height/2.0+220));//160,700
    	taskInfo->setTag(30004);
    	mSmallMissionScreen->addChild(taskInfo);	
    
    	std::stringstream taskInfoCount;
    	taskInfoCount<<"Collect "<<int(minCrystalCount)<<" green Crystals";
    	taskInfo->setString(taskInfoCount.str().c_str());
		
		CCSprite* missionTask = CCSprite::create("WorldMap/mission_number/defeat_master_troll.png");
    	missionTask->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+120,mSmallMissionScreen->getContentSize().height/2.0+120));//550,480
    	missionTask->setTag(30004);
    	mSmallMissionScreen->addChild(missionTask);	
		*/			
	}else if(WhatMission == 4)
	{
		/*
		CCSprite* missionNumber = CCSprite::create("WorldMap/mission_number/M4.png");
    	missionNumber->setPosition(ccp(595,570));//550,480
    	missionNumber->setTag(30002);
    	missionNumber->setScale(0.5);
    	mSmallMissionScreen->addChild(missionNumber);
    	
    	mCall = User::getInstance()->getMissionManager().GetMissionByID(3);
 		int minCrystalCount = mCall.Star_1;
		
		taskInfo = CCLabelTTF::create("",FONT_SKRANJI, TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+140,mSmallMissionScreen->getContentSize().height/2.0+220));//160,700
    	taskInfo->setTag(30004);
    	mSmallMissionScreen->addChild(taskInfo);	
    
    	std::stringstream taskInfoCount;
    	taskInfoCount<<"Collect "<<int(minCrystalCount)<<" green Crystals";
    	taskInfo->setString(taskInfoCount.str().c_str());
		*/
		    	/*
    	mCall = User::getInstance()->getMissionManager().GetMissionByID(2);
 		int minCrystalCount = mCall.Star_1;
		
		taskInfo = CCLabelTTF::create("",FONT_SKRANJI, TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+140,mSmallMissionScreen->getContentSize().height/2.0+220));//160,700
    	taskInfo->setTag(30004);
    	mSmallMissionScreen->addChild(taskInfo);	
    
    	std::stringstream taskInfoCount;
    	taskInfoCount<<"Collect "<<int(minCrystalCount)<<" green Crystals";
    	taskInfo->setString(taskInfoCount.str().c_str());
		
		CCSprite* missionTask = CCSprite::create("WorldMap/mission_number/defeat_master_troll.png");
    	missionTask->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+120,mSmallMissionScreen->getContentSize().height/2.0+120));//550,480
    	missionTask->setTag(30004);
    	mSmallMissionScreen->addChild(missionTask);			
		*/	
	}else if(WhatMission == 5)
	{
		/*
		CCSprite* missionNumber = CCSprite::create("WorldMap/mission_number/M5.png");
    	missionNumber->setPosition(ccp(595,570));//550,480
    	missionNumber->setTag(30002);
    	missionNumber->setScale(0.5);
    	mSmallMissionScreen->addChild(missionNumber);	
    	
    	mCall = User::getInstance()->getMissionManager().GetMissionByID(4);
 		int minCrystalCount = mCall.Star_1;
		
		taskInfo = CCLabelTTF::create("",FONT_SKRANJI, TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+140,mSmallMissionScreen->getContentSize().height/2.0+220));//160,700
    	taskInfo->setTag(30004);
    	mSmallMissionScreen->addChild(taskInfo);	
    
    	std::stringstream taskInfoCount;
    	taskInfoCount<<"Collect "<<int(minCrystalCount)<<" green Crystals";
    	taskInfo->setString(taskInfoCount.str().c_str());
		*/	
		    	/*
    	mCall = User::getInstance()->getMissionManager().GetMissionByID(2);
 		int minCrystalCount = mCall.Star_1;
		
		taskInfo = CCLabelTTF::create("",FONT_SKRANJI, TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+140,mSmallMissionScreen->getContentSize().height/2.0+220));//160,700
    	taskInfo->setTag(30004);
    	mSmallMissionScreen->addChild(taskInfo);	
    
    	std::stringstream taskInfoCount;
    	taskInfoCount<<"Collect "<<int(minCrystalCount)<<" green Crystals";
    	taskInfo->setString(taskInfoCount.str().c_str());
		
		CCSprite* missionTask = CCSprite::create("WorldMap/mission_number/defeat_master_troll.png");
    	missionTask->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+120,mSmallMissionScreen->getContentSize().height/2.0+120));//550,480
    	missionTask->setTag(30004);
    	mSmallMissionScreen->addChild(missionTask);
		*/			
	}else if(WhatMission == 6)
	{
			/*
		CCSprite* missionNumber = CCSprite::create("WorldMap/mission_number/M6.png");
    	missionNumber->setPosition(ccp(595,570));//550,480
    	missionNumber->setTag(30002);
    	missionNumber->setScale(0.5);
    	mSmallMissionScreen->addChild(missionNumber);
	
		mCall = User::getInstance()->getMissionManager().GetMissionByID(5);
 		int minCrystalCount = mCall.Star_1;
		
		taskInfo = CCLabelTTF::create("",FONT_SKRANJI, TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+140,mSmallMissionScreen->getContentSize().height/2.0+220));//160,700
    	taskInfo->setTag(30004);
    	mSmallMissionScreen->addChild(taskInfo);	
    
    	std::stringstream taskInfoCount;
    	taskInfoCount<<"Collect "<<int(minCrystalCount)<<" green Crystals";
    	taskInfo->setString(taskInfoCount.str().c_str());
		*/
		    	/*
    	mCall = User::getInstance()->getMissionManager().GetMissionByID(2);
 		int minCrystalCount = mCall.Star_1;
		
		taskInfo = CCLabelTTF::create("",FONT_SKRANJI, TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+140,mSmallMissionScreen->getContentSize().height/2.0+220));//160,700
    	taskInfo->setTag(30004);
    	mSmallMissionScreen->addChild(taskInfo);	
    
    	std::stringstream taskInfoCount;
    	taskInfoCount<<"Collect "<<int(minCrystalCount)<<" green Crystals";
    	taskInfo->setString(taskInfoCount.str().c_str());
		
		CCSprite* missionTask = CCSprite::create("WorldMap/mission_number/defeat_master_troll.png");
    	missionTask->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+120,mSmallMissionScreen->getContentSize().height/2.0+120));//550,480
    	missionTask->setTag(30004);
    	mSmallMissionScreen->addChild(missionTask);		
		*/			
    }else if(WhatMission == 7)
	{
		/*
		CCSprite* missionNumber = CCSprite::create("WorldMap/mission_number/M7.png");
    	missionNumber->setPosition(ccp(595,570));//550,480
    	missionNumber->setTag(30002);
    	missionNumber->setScale(0.5);
    	mSmallMissionScreen->addChild(missionNumber);
		
		mCall = User::getInstance()->getMissionManager().GetMissionByID(5);
 		int minCrystalCount = mCall.Star_1;
		
		taskInfo = CCLabelTTF::create("",FONT_SKRANJI, TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+140,mSmallMissionScreen->getContentSize().height/2.0+220));//160,700
    	taskInfo->setTag(30004);
    	mSmallMissionScreen->addChild(taskInfo);	
    
    	std::stringstream taskInfoCount;
    	taskInfoCount<<"Collect "<<int(minCrystalCount)<<" green Crystals";
    	taskInfo->setString(taskInfoCount.str().c_str());	
		*/
		    	/*
    	mCall = User::getInstance()->getMissionManager().GetMissionByID(2);
 		int minCrystalCount = mCall.Star_1;
		
		taskInfo = CCLabelTTF::create("",FONT_SKRANJI, TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+140,mSmallMissionScreen->getContentSize().height/2.0+220));//160,700
    	taskInfo->setTag(30004);
    	mSmallMissionScreen->addChild(taskInfo);	
    
    	std::stringstream taskInfoCount;
    	taskInfoCount<<"Collect "<<int(minCrystalCount)<<" green Crystals";
    	taskInfo->setString(taskInfoCount.str().c_str());
		
		CCSprite* missionTask = CCSprite::create("WorldMap/mission_number/defeat_master_troll.png");
    	missionTask->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+120,mSmallMissionScreen->getContentSize().height/2.0+120));//550,480
    	missionTask->setTag(30004);
    	mSmallMissionScreen->addChild(missionTask);	
		*/			
    }else if(WhatMission == 8)
	{
			/*
		CCSprite* missionNumber = CCSprite::create("WorldMap/mission_number/M8.png");
    	missionNumber->setPosition(ccp(595,570));//550,480
    	missionNumber->setTag(30002);
    	missionNumber->setScale(0.5);
    	mSmallMissionScreen->addChild(missionNumber);
	
		mCall = User::getInstance()->getMissionManager().GetMissionByID(5);
 		int minCrystalCount = mCall.Star_1;
		
		taskInfo = CCLabelTTF::create("",FONT_SKRANJI, TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+140,mSmallMissionScreen->getContentSize().height/2.0+220));//160,700
    	taskInfo->setTag(30004);
    	mSmallMissionScreen->addChild(taskInfo);	
    
    	std::stringstream taskInfoCount;
    	taskInfoCount<<"Collect "<<int(minCrystalCount)<<" green Crystals";
    	taskInfo->setString(taskInfoCount.str().c_str());
		*/
		    	/*
    	mCall = User::getInstance()->getMissionManager().GetMissionByID(2);
 		int minCrystalCount = mCall.Star_1;
		
		taskInfo = CCLabelTTF::create("",FONT_SKRANJI, TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+140,mSmallMissionScreen->getContentSize().height/2.0+220));//160,700
    	taskInfo->setTag(30004);
    	mSmallMissionScreen->addChild(taskInfo);	
    
    	std::stringstream taskInfoCount;
    	taskInfoCount<<"Collect "<<int(minCrystalCount)<<" green Crystals";
    	taskInfo->setString(taskInfoCount.str().c_str());
		
		CCSprite* missionTask = CCSprite::create("WorldMap/mission_number/defeat_master_troll.png");
    	missionTask->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+120,mSmallMissionScreen->getContentSize().height/2.0+120));//550,480
    	missionTask->setTag(30004);
    	mSmallMissionScreen->addChild(missionTask);	
		*/				
    }else if(WhatMission == 9)
	{
		/*
		CCSprite* missionNumber = CCSprite::create("WorldMap/mission_number/M9.png");
    	missionNumber->setPosition(ccp(595,570));//550,480
    	missionNumber->setTag(30002);
    	missionNumber->setScale(0.5);
    	mSmallMissionScreen->addChild(missionNumber);
		
		mCall = User::getInstance()->getMissionManager().GetMissionByID(5);
 		int minCrystalCount = mCall.Star_1;
		
		taskInfo = CCLabelTTF::create("",FONT_SKRANJI, TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+140,mSmallMissionScreen->getContentSize().height/2.0+220));//160,700
    	taskInfo->setTag(30004);
    	mSmallMissionScreen->addChild(taskInfo);	
    
    	std::stringstream taskInfoCount;
    	taskInfoCount<<"Collect "<<int(minCrystalCount)<<" green Crystals";
    	taskInfo->setString(taskInfoCount.str().c_str());
		*/
		    	/*
    	mCall = User::getInstance()->getMissionManager().GetMissionByID(2);
 		int minCrystalCount = mCall.Star_1;
		
		taskInfo = CCLabelTTF::create("",FONT_SKRANJI, TITLE_FONT_SIZE*0.5, CCSize(250,250), kCCTextAlignmentLeft, kCCVerticalTextAlignmentBottom);
    	taskInfo->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+140,mSmallMissionScreen->getContentSize().height/2.0+220));//160,700
    	taskInfo->setTag(30004);
    	mSmallMissionScreen->addChild(taskInfo);	
    
    	std::stringstream taskInfoCount;
    	taskInfoCount<<"Collect "<<int(minCrystalCount)<<" green Crystals";
    	taskInfo->setString(taskInfoCount.str().c_str());
		
		CCSprite* missionTask = CCSprite::create("WorldMap/mission_number/defeat_master_troll.png");
    	missionTask->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2.0+120,mSmallMissionScreen->getContentSize().height/2.0+120));//550,480
    	missionTask->setTag(30004);
    	mSmallMissionScreen->addChild(missionTask);	
		*/				
    }
}

void WorldMap::ResetStats()
{
	//mCall = User::getInstance()->getMissionManager().Reset();
}

void WorldMap::BuyMoreDwarfs()
{
	//CCLog("Works so far!!!");
    
    int aPrice = mCall.STORE_Booster_DwarfPrice;
	
	int aDidUseDiamonds = User::getInstance()->canUseDiamonds(aPrice);//BOOSTER_1_PRICE
        if(aDidUseDiamonds<0)
        {
            //Show popup that no money
            return;
        }else{
        	if(aPrice <=0)
        	{
        	}else{
        	//mCall = User::getInstance()->getMissionManager().GetMissionByID(WhatMission+1);
			if(WhatMission == 1)//Identificate what mission number we need
			{
			mCall = User::getInstance()->getMissionManager().AddDwarfs(0);
			mCall.Mission_SaveDwarfs;
			}else if(WhatMission == 2)
			{
			mCall = User::getInstance()->getMissionManager().AddDwarfs(1);
			mCall.Mission_SaveDwarfs;	
			}else if(WhatMission == 3)
			{
			mCall = User::getInstance()->getMissionManager().AddDwarfs(2);
			mCall.Mission_SaveDwarfs;	
			}else if(WhatMission == 4)
			{
			mCall = User::getInstance()->getMissionManager().AddDwarfs(3);
			mCall.Mission_SaveDwarfs;	
			}else if(WhatMission == 5)
			{
			mCall = User::getInstance()->getMissionManager().AddDwarfs(4);
			mCall.Mission_SaveDwarfs;	
			}else if(WhatMission == 6)
			{
			mCall = User::getInstance()->getMissionManager().AddDwarfs(5);
			mCall.Mission_SaveDwarfs;	
			}else if(WhatMission == 7)
			{
			mCall = User::getInstance()->getMissionManager().AddDwarfs(6);
			mCall.Mission_SaveDwarfs;	
			}else if(WhatMission == 8)
			{
			mCall = User::getInstance()->getMissionManager().AddDwarfs(7);
			mCall.Mission_SaveDwarfs;	
			}else if(WhatMission == 9)
			{
			mCall = User::getInstance()->getMissionManager().AddDwarfs(8);
			mCall.Mission_SaveDwarfs;	
			}else if(WhatMission == 10)
			{
			mCall = User::getInstance()->getMissionManager().AddDwarfs(9);
			mCall.Mission_SaveDwarfs;	
			}
			
        	}
		}

	UpdateStats();
	
}

void WorldMap::BuyMoreDamage()
{
	CCLog("Damage++");
	int aPrice = mCall.STORE_Booster_DamagePrice;
	
	int aDidUseDiamonds = User::getInstance()->canUseDiamonds(aPrice);//BOOSTER_2_PRICE
        if(aDidUseDiamonds<0)
        {
            //Show popup that no money
            return;
        }else{
        	if(aPrice <=0)
        	{
        	}else{
        		int aTotalSpells = User::getInstance()->getItemDataManager().mSpellDataVector.size();
        		 for(int i = 0;i<aTotalSpells;i++)
    			{
    				if(User::getInstance()->getItemDataManager().isItemActive(User::getInstance()->getItemDataManager().mSpellDataVector[i].id))//Check if spell is active.
    				{
						User::getInstance()->getItemDataManager().addExtraDamage(User::getInstance()->getItemDataManager().mSpellDataVector[i].id);//AddExtraRange
						if(User::getInstance()->getItemDataManager().mSpellDataVector[i].upgrade_available){
							int _itemLevel = User::getInstance()->getItemDataManager().getSpellItemLevel(User::getInstance()->getItemDataManager().mSpellDataVector[i].id);
            				spellDamage = User::getInstance()->getItemDataManager().mSpellDataVector[i].upgrade_damage[_itemLevel];
						}
        				else{
            				spellDamage = User::getInstance()->getItemDataManager().mSpellDataVector[i].damage;
        				}
        			}
				}
        	}
		}

	UpdateStats();
}

void WorldMap::BuyMoreRange()
{
	CCLog("Range++");
	
		int aPrice = mCall.STORE_Booster_RangePrice;
	
	int aDidUseDiamonds = User::getInstance()->canUseDiamonds(aPrice);//BOOSTER_2_PRICE
        if(aDidUseDiamonds<0)
        {
            //Show popup that no money
            return;
        }else{
        	if(aPrice <=0)
        	{
        	}else{
        		int aTotalSpells = User::getInstance()->getItemDataManager().mSpellDataVector.size();
        		 for(int i = 0;i<aTotalSpells;i++)
    			{
    				if(User::getInstance()->getItemDataManager().isItemActive(User::getInstance()->getItemDataManager().mSpellDataVector[i].id))//Check if spell is active.
    				{
						User::getInstance()->getItemDataManager().addExtraRange(User::getInstance()->getItemDataManager().mSpellDataVector[i].id);//AddExtraRange
						spellRange = User::getInstance()->getItemDataManager().mSpellDataVector[i].range;//Update Range counter
        			}
				}
			}
			}
	UpdateStats();
}

void WorldMap::UpdateStats()
{
	std::stringstream _diamondsLabelReal;
    _diamondsLabelReal<<int(User::getInstance()->getDiamonds());
    _diamondsLabel->setString(_diamondsLabelReal.str().c_str());
    
    std::stringstream dwarfCountReal;
    dwarfCountReal<<int(mCall.Mission_SaveDwarfs);
    dwarfCount->setString(dwarfCountReal.str().c_str());
    
    std::stringstream damageCountReal;
    damageCountReal<<int(spellDamage);
    _damageCount->setString(damageCountReal.str().c_str());
    		
    std::stringstream rangeCountReal;
    rangeCountReal<<int(spellRange);
    _rangeSize->setString(rangeCountReal.str().c_str());
			
	std::stringstream damagePriceReal;
    damagePriceReal<<int(mCall.STORE_Booster_DamagePrice);
    damagePrice->setString(damagePriceReal.str().c_str());
    	
    std::stringstream rangePriceReal;
    rangePriceReal<<int(mCall.STORE_Booster_RangePrice);
    rangePrice->setString(rangePriceReal.str().c_str());
}

void WorldMap::removeNode(CCNode* sender)
{
    this->removeChild(sender, true);
}

void WorldMap::CreatePlayer()
{
    // Try to load player with the image what he has on fb !!!
//    int _currentLevel = User::getInstance()->mCurrentMissionLevel-1;// Load from save
    int _currentLevel = User::getInstance()->mWorldMapLastMission-1;
    
    
    // Check if has FB image !!
    bool aHasFB_Face = false;
    CCSprite* base_1;
    CCSprite* base_2;
    
    if(aHasFB_Face)
    {
        base_1 = CCSprite::create("WorldMap/player_spot.png");
        base_2 = CCSprite::create("WorldMap/dwarfs_map.png");
        base_2->setPosition(ccp(base_1->getContentSize().width/2,base_1->getContentSize().height/2));
        base_1->addChild(base_2);
    }
    else
    {
        base_1 = CCSprite::create("WorldMap/dwarfs_map.png");
    }
    
    CCMenuItemSprite *dummyItem = CCMenuItemSprite::create(base_1, base_1, this, menu_selector(WorldMap::OnClickedPlayer));
    mPlayer = CCMenu::createWithItem(dummyItem);
    mPlayer->setPosition(ccp(mWorldNodeCords[_currentLevel*2],mWorldNodeCords[_currentLevel*2+1]+30));
    
    // Check if tutorial - then fall from sky
    if(GameTutorial::getInstance()->mTutorialCompleted==false)
    {
        if(GameTutorial::getInstance()->mCurrentTutorialStep<=TUTORIAL_S0_WORLD_MAP_MISSION_TAP){
            mPlayer->setVisible(false);
        }
        else{
             mPlayer->setVisible(true);
        }
    }
    else{
         mPlayer->setVisible(true);
    }
    
    map_base->addChild(mPlayer);
    
    //MissionTaskInditificator(_currentLevel);
    
    // Maybe now create other players with their progress ? TODO
    
}

void WorldMap::onExit()
{
//    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
    
    // Stop updating
    this->unscheduleUpdate();
}

void WorldMap::onEnter()
{
    CCLayer::onEnter();
    
    //Start it
    this->scheduleUpdate();
    
    if(GameTutorial::getInstance()->mTutorialCompleted == false)
    {
        // Set the scene
        GameTutorial::getInstance()->SetWorldMapInstance(this);
        
        if(GameTutorial::getInstance()->mCurrentTutorialStep < TUTORIAL_S0_WORLD_MAP_MISSION_TAP)
        {
            // Move map a bit up?
            CCMoveBy* aMoveMap = CCMoveBy::create(1.0f,ccp(0,-70));
            CCEaseSineOut* aEase = CCEaseSineOut::create(aMoveMap);
            pzLayer->runAction(aEase);
        }
        else if(GameTutorial::getInstance()->mCurrentTutorialStep == TUTORIAL_S1_WAIT_FOR_ALL)
        {
            // Do the next magic !!!
            GameTutorial::getInstance()->IncreaseTutorialStep(NULL);
        }
    }
    
    /*
    if(mLastMissionID == 1)
    {
        if(GameTutorial::getInstance()->mTutorialCompleted == false)
        {
            // Set the scene
            GameTutorial::getInstance()->SetWorldMapInstance(this);
            
            if(GameTutorial::getInstance()->mCurrentTutorialStep < TUTORIAL_S0_WORLD_MAP_MISSION_TAP)
            {
                // Move map a bit up?
                CCMoveBy* aMoveMap = CCMoveBy::create(1.0f,ccp(0,-70));
                CCEaseSineOut* aEase = CCEaseSineOut::create(aMoveMap);
                pzLayer->runAction(aEase);
                
//                pzLayer->setPosition(ccp(pzLayer->getPositionX(),pzLayer->getPositionY()-70));
            }
        }
    }
    else
    {
        if(GameTutorial::getInstance()->mTutorialCompleted == false)
        {
            if(GameTutorial::getInstance()->mCurrentTutorialStep == TUTORIAL_S1_WAIT_FOR_ALL)
            {
                // Do the next magic !!!
                GameTutorial::getInstance()->IncreaseTutorialStep(NULL);
            }
        }
    }
    */
}

bool WorldMap::DidWeImproveMissionStars(int theMissionID)
{
    // Check save etc
    int theStarsNow = User::getInstance()->GetUserMissionInfo(theMissionID,2);
    // Check what we have now on map
    CCSprite* aPlaceSprite = (CCSprite*)_batchWorld->getChildByTag(1000+theMissionID);
    CCSprite* theFlagOpen = (CCSprite*)aPlaceSprite->getChildByTag(MISSION_FLAG_OPEN);
    CCSprite* theStar;
    
    // Now check the stars
    int aCurrentVisibleStars = 0;
    for(int i=0;i<3;i++)
    {
        theStar = (CCSprite*)theFlagOpen->getChildByTag(MISSION_FLAG_STAR+i);
        if(theStar->isVisible()){
            aCurrentVisibleStars+=1;
        }
    }
    CCLog("Current map stars [%i] vs Saved map stars [%i]",aCurrentVisibleStars,theStarsNow);
    // We need to show new stars?
    if(aCurrentVisibleStars!=theStarsNow){
        // Pop the stars
        ShowMissionStarsEarned(theMissionID, theStarsNow-aCurrentVisibleStars, aCurrentVisibleStars);
        return true;
    }
    
    return false;
}

void WorldMap::onEnterTransitionDidFinish()
{
    CCLOG("mLastMissionID: %i | mCurrentMissionLevel: %i",mLastMissionID,User::getInstance()->mCurrentMissionLevel);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
    CocosDenshion::SimpleAudioEngine::sharedEngine()->stopAllEffects();
    
    // Check tutorial
    if(mLastMissionID == 1)
    {
        if(GameTutorial::getInstance()->mTutorialCompleted == false)
        {
            // Set the scene
            GameTutorial::getInstance()->SetWorldMapInstance(this);
            
            if(GameTutorial::getInstance()->mCurrentTutorialStep < TUTORIAL_S0_WORLD_MAP_MISSION_TAP)
            {
                // Do the intro for 1st map
                GameTutorial::getInstance()->DoStep(TUTORIAL_S0_WORLD_MAP_INTRO);
                
                // Move map a bit up?
//                pzLayer->setPosition(ccp(pzLayer->getPositionX(),pzLayer->getPositionY()-70));
//                pzLayer->setPosition(ccp(map_base->getPositionX(),map_base->getPositionY()-100));
            }
        }
    }
    
    // Check if we did imporve mission progress !!!
    
    // Check if at this mission we did do something - any improvements in stars etc
    
    bool showMissionImprove = DidWeImproveMissionStars(User::getInstance()->mCurrentStartedMission);
    CCLog("Did we improve mission[%i]: %d",User::getInstance()->mCurrentStartedMission,showMissionImprove);
    
    
    if(mLastMissionID == User::getInstance()->mCurrentMissionLevel){
        return;
    }
    
    // For now
//    GameTutorial::getInstance()->mTutorialCompleted = true;
    
    mLastMissionID = User::getInstance()->mCurrentMissionLevel;
    int _currentLevel = mLastMissionID-1;// Load from save
    
    // Save it
    User::getInstance()->mWorldMapLastMission = mLastMissionID = User::getInstance()->mCurrentMissionLevel;
    CCLOG("_currentLevel: %i",_currentLevel);

    
    // Move that player avatar to new space
    
    // First unlock anim - then move
    if(showMissionImprove){
        // Call after it
        mNeedToShowNewLevel = true;
    }
    else{
        mNeedToShowNewLevel = false;
        UnlockLevel(User::getInstance()->mCurrentMissionLevel-1);
    }
    
    
    /*
    CCMoveTo* aMove1 = CCMoveTo::create(1.0f,ccp(mWorldNodeCords[_currentLevel*2],mWorldNodeCords[_currentLevel*2+1]+20));
    CCEaseExponentialInOut* aEase1 = CCEaseExponentialInOut::create(aMove1);
    CCCallFuncN* aFuncDone = CCCallFuncN::create(this, callfuncN_selector(WorldMap::OnPlayerFinishedMove));
    CCSequence* aSeq1 = CCSequence::create(aEase1,aFuncDone,NULL);
    mPlayer->runAction(aSeq1);
    */
}

void WorldMap::OnPlayerFinishedMove()
{
    // Create some particles and stuff like that !!!
    CCParticleSystemQuad* p = CCParticleSystemQuad::create("Particles/bullet_explode.plist");
    p->setPosition(mPlayer->getPosition());
    p->setAutoRemoveOnFinish(true);
    map_base->addChild(p,1000);
    
    if(GameTutorial::getInstance()->mTutorialCompleted == false)
    {
        if(GameTutorial::getInstance()->mCurrentTutorialStep < TUTORIAL_S0_WORLD_MAP_MISSION_TAP)
        {
            // Do the intro for 1st map
            GameTutorial::getInstance()->DoStep(TUTORIAL_S0_WORLD_MAP_INTRO);
            
        }
        else if(GameTutorial::getInstance()->mCurrentTutorialStep == TUTORIAL_S2_2ND_SHOOT_AT_TOTEM_COMPLETED)
        {
            // Dooo - SHOW THE STORE BUTTON
            GameTutorial::getInstance()->DoStep(TUTORIAL_S2_WORLD_MAP_MOVE_TO_3_FINISHED);
        }
    }
}

/*
void WorldMap::onEnter()
{
    CheckPlayerMoveFuther();
}

void WorldMap::onExit()
{
    mLastMissionID = User::getInstance()->mCurrentMissionLevel;
}
*/

/*
void WorldMap::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    
}

bool WorldMap::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    return true;
}

void WorldMap::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    
}

void WorldMap::onEnter()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority, true);
    CCLayer::onEnter();
}

void WorldMap::onExit()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}
*/

std::vector<int> static &split(const std::string &s, char delim, std::vector<int> &elems){
    std::stringstream ss(s);
    std::string item;
    int resultInt;
    
    while (std::getline(ss, item, delim)) {
        resultInt = atoi(item.c_str());
        elems.push_back(resultInt);
    }
    return elems;
}

std::vector<int> WorldMap::SplitString(const std::string s,char delim){
    std::vector<int> elems;
    split(s, delim, elems);
    return elems;
}

//-------------------------------------------------------
// The small mission screen !!!

void WorldMap::PrepeareSmallMissionScreen()
{
    mSmallMissionScreen = CCLayerColor::create(ccc4(0,0,0,128),mScreenSize.width,mScreenSize.height);
    mSmallMissionScreen->setVisible(false);
    
    //Add the screen
    CCSprite* aScreenPlay = CCSprite::create("WorldMap/NewPregame/Background.png");
    aScreenPlay->setPosition(ccp(mSmallMissionScreen->getContentSize().width/2,mSmallMissionScreen->getContentSize().height/2));
    mSmallMissionScreen->addChild(aScreenPlay);
    
    // Add 2 buttons clouse and play for now !!!
    
    // The play button
    CCMenuItemImage* playItem = CCMenuItemImage::create(
                                                        "WorldMap/NewPregame/ButtonPlay.png",
                                                        "WorldMap/NewPregame/ButtonPlay.png",
                                                        this,
                                                        menu_selector(WorldMap::HideMissionScreen));
    playItem->setTag(1);//Play the level
    playItem->setPosition(ccp(aScreenPlay->getContentSize().width/2.0+300,aScreenPlay->getContentSize().height/6));
    
    // The clouse button
    CCMenuItemImage* clouseItem = CCMenuItemImage::create(
                                                        "Interfeiss/before_quit/btn_cross.png",
                                                        "Interfeiss/before_quit/btn_cross.png",
                                                        this,
                                                        menu_selector(WorldMap::HideMissionScreen));
    clouseItem->setTag(2);//Clouse screen
    clouseItem->setPosition(ccp(aScreenPlay->getContentSize().width-30,aScreenPlay->getContentSize().height-30));
    
    CCMenuItemImage* upgradeItem = CCMenuItemImage::create(
                                                        "WorldMap/NewPregame/ButtonUpgrades.png",
                                                        "WorldMap/NewPregame/ButtonUpgrades.png",
                                                        this,
                                                        menu_selector(WorldMap::Hud_ShowStore));
    upgradeItem->setTag(1);//Play the level
    upgradeItem->setPosition(ccp(aScreenPlay->getContentSize().width/2.0+300,aScreenPlay->getContentSize().height/3));
    
    CCSprite* MissionTask = CCSprite::create("WorldMap/NewPregame/TXTMissionTask.png");
    MissionTask->setPosition(ccp(aScreenPlay->getContentSize().width/4,aScreenPlay->getContentSize().height+18));
    mSmallMissionScreen->addChild(MissionTask);
    
    CCSprite* aScreenPlay2 = CCSprite::create("WorldMap/NewPregame/Background2.png");
    aScreenPlay2->setPosition(ccp(aScreenPlay->getContentSize().width/2.0-140,aScreenPlay->getContentSize().height/1.2));
    mSmallMissionScreen->addChild(aScreenPlay2);
    
    CCSprite* aScreenBooster = CCSprite::create("WorldMap/NewPregame/BoosterCardDwarfs.png");
    aScreenBooster->setPosition(ccp(aScreenPlay->getContentSize().width/6.0-24,aScreenPlay->getContentSize().height/3));
    mSmallMissionScreen->addChild(aScreenBooster);
    
    CCSprite* aNeedle = CCSprite::create("WorldMap/NewPregame/boostersneedle.png");
    aNeedle->setFlipX(true);
    aNeedle->setPosition(ccp(aScreenPlay->getContentSize().width/6.0-10,aScreenPlay->getContentSize().height/2+60));
    mSmallMissionScreen->addChild(aNeedle);
    
    CCSprite* aNeedle2 = CCSprite::create("WorldMap/NewPregame/boostersneedle.png");
    aNeedle2->setFlipX(false);
    aNeedle2->setPosition(ccp(aScreenPlay->getContentSize().width/6.0+350,aScreenPlay->getContentSize().height/2+60));
    mSmallMissionScreen->addChild(aNeedle2);
    
    CCSprite* Boosters = CCSprite::create("WorldMap/NewPregame/TXTBoosters.png");
    Boosters->setPosition(ccp(aScreenPlay->getContentSize().width/6.0+170,aScreenPlay->getContentSize().height/2+60));
    mSmallMissionScreen->addChild(Boosters);
    
    CCSprite* CurrencyCrystals = CCSprite::create("WorldMap/NewPregame/CurrencyCrystals.png");
    CurrencyCrystals->setPosition(ccp(aScreenPlay->getContentSize().width/2+240,aScreenPlay->getContentSize().height/1.1));
    mSmallMissionScreen->addChild(CurrencyCrystals);
    
    CCSprite* CurrencyDiamonds = CCSprite::create("WorldMap/NewPregame/CurrencyDiamonds.png");
    CurrencyDiamonds->setPosition(ccp(aScreenPlay->getContentSize().width/2+231,aScreenPlay->getContentSize().height/1.3));
    mSmallMissionScreen->addChild(CurrencyDiamonds);
    
    CCMenu* aButtonMenu = CCMenu::create(playItem,clouseItem,upgradeItem,NULL);
    aButtonMenu->setAnchorPoint(ccp(0,0));
    aButtonMenu->setPosition(ccp(0,0));
    aScreenPlay->addChild(aButtonMenu, 1);
    
    addChild(mSmallMissionScreen,1);//Above all
}

void WorldMap::ShowMissionScreen(int theID)
{
    if(GameTutorial::getInstance()->mTutorialCompleted == false)
    {
        // Check if can click
        if(GameTutorial::getInstance()->mCurrentTutorialStep == TUTORIAL_S0_WORLD_MAP_MISSION_TAP && theID == 1)
        {
            // All ok continue to tutorial
            GameTutorial::getInstance()->IncreaseTutorialStep(NULL);
            return;
        }
        else if(GameTutorial::getInstance()->mCurrentTutorialStep == TUTORIAL_S1_SHOW_WORLD_MAP_CLICK && theID == 2)
        {
            // Can continue
            GameTutorial::getInstance()->IncreaseTutorialStep(NULL);
            return;
        }
        else
        {
            
            // No luck
            return;
        }
    }
    
	moveBackground = true;
	mSmallMissionScreen->removeChildByTag(300018);
	mSmallMissionScreen->removeChildByTag(300017);
	mSmallMissionScreen->removeChildByTag(30013);
	mSmallMissionScreen->removeChildByTag(30012);
	mSmallMissionScreen->removeChildByTag(30011);
	mSmallMissionScreen->removeChildByTag(30010);
	mSmallMissionScreen->removeChildByTag(30009);
	mSmallMissionScreen->removeChildByTag(30008);
	mSmallMissionScreen->removeChildByTag(30007);
	mSmallMissionScreen->removeChildByTag(30006);
	mSmallMissionScreen->removeChildByTag(30005);
	mSmallMissionScreen->removeChildByTag(30004);
    mSmallMissionScreen->removeChildByTag(30002);
    mSmallMissionScreen->removeChildByTag(712);
    mSmallMissionScreen->removeChildByTag(713);
    mSmallMissionScreen->removeChildByTag(714);
    mSmallMissionScreen->removeChildByTag(715);
    mSmallMissionScreen->removeChildByTag(716);
    mSmallMissionScreen->removeChildByTag(717);
    mSmallMissionScreen->removeChildByTag(718);
    //removeChildByTag(67881);
    mCurrentMissionID = theID;
    MissionTaskInditificator(mCurrentMissionID);
    mSmallMissionScreen->setVisible(true);
}

void WorldMap::HideMissionScreen(CCObject * pSender)
{
	moveBackground = false;
	//moveBackground = false;
    CCMenuItem* pMenuItem = (CCMenuItem *)(pSender);
    int tag = (int)pMenuItem->getTag();
    
    mSmallMissionScreen->removeChildByTag(300018);
    mSmallMissionScreen->removeChildByTag(300017);
    mSmallMissionScreen->removeChildByTag(30013);
    mSmallMissionScreen->removeChildByTag(30012);
    mSmallMissionScreen->removeChildByTag(30011);
    mSmallMissionScreen->removeChildByTag(30010);
    mSmallMissionScreen->removeChildByTag(30009);
    mSmallMissionScreen->removeChildByTag(30008);
    mSmallMissionScreen->removeChildByTag(30007);
    mSmallMissionScreen->removeChildByTag(30006);
    mSmallMissionScreen->removeChildByTag(30005);
    mSmallMissionScreen->removeChildByTag(30004);
    mSmallMissionScreen->removeChildByTag(30002);
    mSmallMissionScreen->removeChildByTag(712);
    mSmallMissionScreen->removeChildByTag(713);
    mSmallMissionScreen->removeChildByTag(714);
    mSmallMissionScreen->removeChildByTag(715);
    mSmallMissionScreen->removeChildByTag(716);
    mSmallMissionScreen->removeChildByTag(717);
    mSmallMissionScreen->removeChildByTag(718);
    mSmallMissionScreen->setVisible(false);
    
    if(tag == 1){
        //Fade all black and load the needed level
        CCLOG("mCurrentMissionID:%i",mCurrentMissionID);
        
        // Set the value what level will be played now
        User::getInstance()->getMissionManager().mCurrentActiveMission = mCurrentMissionID-1;
        
        CCScene* scene;
        
        // If need to show some promo - do it
        scene = GameScene::scene();
        CCTransitionScene* transition = CCTransitionFade::create(0.5f, scene);
        CCDirector::sharedDirector()->replaceScene(transition);
    }
    else{
        CCLOG("Do nothing cloused screen");
    }
}

// The other stuff
void WorldMap::CreateHud()
{
    // If tutorial - skip this - dont show player any crap
    if(GameTutorial::getInstance()->mTutorialCompleted == false)
    {
        // Only when needed shot iw !!!
        if(GameTutorial::getInstance()->mCurrentTutorialStep<TUTORIAL_S2_INTRO)
        {
            return;
        }
    }
    
    // All like normal
    
    CCSprite * storeNormalSprite= CCSprite::create("Interfeiss/main_menu/new/store_btn_new0001.png");
    CCSprite * storeSelectedlSprite= CCSprite::create("Interfeiss/main_menu/new/store_btn_new0002.png");
    
    CCMenuItemSprite *storeItem = CCMenuItemSprite::create(storeNormalSprite, storeSelectedlSprite,
                                                           this,
                                                           menu_selector(WorldMap::Hud_ShowStore));
    storeItem->setAnchorPoint(ccp(0,0));
    storeItem->setPosition(ccp(0,4));
    
    CCSprite * challengesNormalSprite= CCSprite::create("Interfeiss/main_menu/new/challenges_btn_new0001.png");
    CCSprite * challengesSelectedlSprite= CCSprite::create("Interfeiss/main_menu/new/challenges_btn_new0002.png");
    
    CCMenuItemSprite *challengesItem = CCMenuItemSprite::create(challengesNormalSprite, challengesSelectedlSprite,
                                                                this,
                                                                menu_selector(WorldMap::Hud_ShowChallenges));
    challengesItem->setAnchorPoint(ccp(1,0));
    challengesItem->setPosition(ccp(mScreenSize.width,4));
    
    CCMenuItemImage* optionsItem = CCMenuItemImage::create(
                                                           "Interfeiss/main_menu/new/options_btn0001.png",
                                                           "Interfeiss/main_menu/new/options_btn0002.png",
                                                           this,
                                                           menu_selector(WorldMap::Hud_ShowOptions));
    optionsItem->setAnchorPoint(ccp(0,1));
    optionsItem->setPosition(ccp(0,mScreenSize.height-8));
    
    
    CCMenuItemImage* backButtonReload = CCMenuItemImage::create(
                                                                "DebugStuff/load_mission_1.png",
                                                                "DebugStuff/load_mission_1.png",
                                                                this,
                                                                menu_selector(WorldMap::OnReloadStuff));
    backButtonReload->setAnchorPoint(ccp(1,1));
    backButtonReload->setPosition(ccp(mScreenSize.width-8,mScreenSize.height-8));
    
    mMainMenu = CCMenu::create(storeItem,
                               challengesItem,backButtonReload,optionsItem,
                               NULL);
    
    mMainMenu->setPosition(ccp(0,0));
    
    mMainMenu->setOpacity(0);
    
    mMainMenu->setTag(67881);
    addChild(mMainMenu);
    
    // if tutorial - do not show instant !! - wait a bit
    if(GameTutorial::getInstance()->mTutorialCompleted == false && GameTutorial::getInstance()->mCurrentTutorialStep <= TUTORIAL_S2_WORLD_MAP_MOVE_TO_3_FINISHED){
        // Wait
        
        // HACK
//        mLastMissionID = 2;
    }
    else{
        // Just move up the buttons
        MoveInWorldMapButtons();
    }
}

void WorldMap::MoveInWorldMapButtons()
{
    CCFadeIn* aFadeIn = CCFadeIn::create(1.0f);
    mMainMenu->runAction(aFadeIn);
    
//    TUTORIAL_S2_WORLD_MAP_MOVE_TO_3_FINISHED
}

void WorldMap::Hud_ShowOptions(CCObject* sender)
{
//    ParseX::createUser();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    FacebookX::logInUser();
#endif
    
    return;
    CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(getSoundFx("button_click").c_str());
    
    OptionsScene* optionsLayer = OptionsScene::create();
    optionsLayer->setAnchorPoint(ccp(0,0));
    addChild(optionsLayer,100);
}

void WorldMap::OnReloadStuff()
{
    User::getInstance()->getMissionManager().OnDownloadSpecialMissions();
}

void WorldMap::Hud_ShowStore(CCObject* sender)
{
//    CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(getSoundFx("button_click").c_str());
    
//    if(mOptionsOpen)
//        return;
//    
//    mOptionsOpen = true;
    
    
    StoreScene* storeLayer = StoreScene::create();
    storeLayer->setTag(2223);
    this->addChild(storeLayer,100);
    
    if(GameTutorial::getInstance()->mTutorialCompleted == false)
    {
        // CHeck if does not need to process to next step
        if(GameTutorial::getInstance()->mCurrentTutorialStep == TUTORIAL_S2_WORLD_MAP_MOVE_TO_3_FINISHED)
        {
            // Show shop hand where to buy stuff
            GameTutorial::getInstance()->DoStep(TUTORIAL_S2_WORLD_MAP_STORE_OPEN);
        }
    }
}

void WorldMap::OnRemoveStore()
{
//    AnalyticX::flurryLogEvent("Menu Hide: Store");
    
    removeChildByTag(2223,true);
//    mOptionsOpen = false;
}

void WorldMap::Hud_ShowChallenges(CCObject* sender)
{
    //Old stuff
//    mOptionsOpen = true;
    
//    mMainMenu->setEnabled(false);
    
//    AnalyticX::flurryLogEvent("Menu Show: Challenges");
    
    ChallengesScene* challengeLayer = ChallengesScene::create();
    this->addChild(challengeLayer,100);
    moveBackground = true;
//    SimpleAudioEngine::sharedEngine()->playEffect(getSoundFx("button_click").c_str());
}


