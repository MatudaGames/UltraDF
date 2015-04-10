//
//  GameTutorial.cpp
//  DwarfForest
//
//  Created by Kristaps Auzins on 08/03/15.
//
//

#include "GameTutorial.h"

#include "User.h"
#include "Utils.h"
#include <cocos2d.h>
#include <stdlib.h>
#include "CCScale9Sprite.h"
#include "Dwarf.h"
#include "StaticSceneManager.h"
#include "IntroScene.h"
#include "AppMacros.h"
#include "CCShake.h"
#include "StoreScene.h"


USING_NS_CC;

static GameTutorial* instance = NULL;

GameTutorial* GameTutorial::getInstance()
{
    if (instance == NULL)
    {
        instance = new GameTutorial();
    }
    return instance;
}

GameTutorial::GameTutorial()
{
    CCLog("GameTutorial INIT");
    
    int aDummyVar = cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey("Tutorial_Completed",0);
    if(aDummyVar == 1) mTutorialCompleted = true;
    else mTutorialCompleted = false;
    
    // For now completed
    mTutorialCompleted = true;
    
    mCurrentTutorialStep = cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey("Tutorial_Current_Step",0);
//    mCurrentTutorialStep = TUTORIAL_S0_WORLD_MAP_INTRO;
    //TUTORIAL_S2_2ND_SHOOT_AT_TOTEM_COMPLETED;//TUTORIAL_S1_WAIT_FOR_ALL;//TUTORIAL_S0_WORLD_MAP_INTRO;
    
    mScreenSize = CCDirector::sharedDirector()->getVisibleSize();
    
    // Set to default values
    mTutorialDrawHand = false;
    mTutorialPointsAdded = false;
    
    mSubTutorialCounter = 0;
    mActivetTimesForNext = 5;
    
    mGameScene = NULL;
    mWorldMap = NULL;
    
    // Some helpers more
    mConnectedTutorialStuff = false;
    
    // Hack
//    User::getInstance()->mCurrentMissionLevel = 2;
}

GameTutorial::~GameTutorial()
{
    cocos2d::CCUserDefault::sharedUserDefault()->flush();
}

bool GameTutorial::StepCompleted(int theStep)
{
    if(mCurrentTutorialStep>theStep || mTutorialCompleted){
        return true;
    }
    
    // We need to do it
    return false;
}

bool GameTutorial::NeedToDoStep(int theStep)
{
    if(mTutorialCompleted || mCurrentTutorialStep>theStep){
        return false;
    }
    
    return true;
}

void GameTutorial::SetGameInstance(GameScene* gameScene)
{
    mWorldMap = NULL;
    
    mGameScene = gameScene;
    
    mDummyAction = CCSprite::create("small_dot_red.png");
    mGameScene->addChild(mDummyAction);
}

void GameTutorial::SetWorldMapInstance(WorldMap* worldScene)
{
    mGameScene = NULL;
    
    mWorldMap = worldScene;
    
    mDummyAction = CCSprite::create("small_dot_red.png");
    mWorldMap->addChild(mDummyAction);
}

void GameTutorial::DoStep(int theStep)
{
    CCLog("TUTORIAL - Do Step [%i]",theStep);
    
//    theStep = TUTORIAL_S0_DIALOG_MORE_DWARFS_DONE;
    
    // Set so that we know where we are in this big world
    mCurrentTutorialStep = theStep;
    
    if(theStep == TUTORIAL_S0_INTRO)
    {
//        CreateHint(3, ccp(300, 100), "Pause");
        
//        mCurrentTutorialStep = TUTORIAL_S0_DIALOG_CRYSTAL_COLLECTED_IN_CAVE;//
        // The very 1st tutorial dialog !!!
//        CreateDialog(0, TUTORIAL_S0_INTRO,2);
        CreateDialog(4, "Hello my friend! I will guide you through some basics so we can kick off.", 2,
                     ccp(mGameScene->_MasterDwarfBase->getPositionX()-60,mScreenSize.height/2+60));
        
//        mCurrentTutorialStep = TUTORIAL_S0_FIRST_DWARF_SPAWN;//We will want to spawn a dawrf
    }
    else if(theStep == TUTORIAL_S0_FIRST_DWARF_SPAWN)
    {
        // Spawn the 1st dwarf and do the magic !!!
        mGameScene->generateDwarfAtSpot(2, DWARF_TYPE_TALL, false);
        
        // Get the dwarf and do the next step
        Dwarf* theDummy = static_cast<Dwarf*>(mGameScene->_dwarves->objectAtIndex(0));
        theDummy->mTutorialWaitTimeToNextStep = 1.0f; // Do next tutorial step after a while
        
    }
    else if(theStep == TUTORIAL_S0_FIRST_DWARF_DRAG)
    {
        // Show how to drag the dwarf !!!
        mGameScene->pauseGame();// Freeze all
        //Create some info how to drag dwarf in cave !!!
        
        // Create all the needed stuff
        _line = CCSpriteBatchNode::create("trajectory_dot_white.png");
        mGameScene->addChild(_line);
        
        _movePoints = CCPointArray::create(0);
        _movePoints->retain();
        
        mGameScene->AddTutorialUpdater();
        
        CreateDrawHand(TUTORIAL_S0_FIRST_DWARF_DRAG);
    }
    else if(theStep == TUTORIAL_S0_FIRST_DWARF_ENTER_CAVE)
    {
        // Just wait when enters the cave
        mGameScene->onPauseFinished();
        
        // Remove the hand stuff
        mGameScene->RemoveTutorialUpdater();
    }
    else if(theStep == TUTORIAL_S0_FIRST_DWARF_ENTERED_CAVE)
    {
        // DK informs that will spanw more stuff
        IncreaseTutorialStep(NULL); // Starts to spawn dwarfs
//        CreateDialog(0, TUTORIAL_S0_DIALOG_MORE_DWARFS,0);
        CreateDialog(4, "Let’s get 3 more dwarfs into the cave!", 0,
                     ccp(mGameScene->_MasterDwarfBase->getPositionX()-60,mScreenSize.height/2+60));
    }
    else if(theStep == TUTORIAL_S0_DIALOG_MORE_DWARFS)
    {
        // Spawwwwn
        mSubTutorialCounter = 0;
        mActivetTimesForNext = 3;
        
        CCCallFunc* aSpawnPart_1 = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::SpawnDwarf));
        CCDelayTime* aDelay_1 = CCDelayTime::create(2.0f);
        CCCallFunc* aSpawnPart_2 = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::SpawnDwarf));
        CCSequence* aSeq_1 = CCSequence::create(aSpawnPart_1,aDelay_1,aSpawnPart_2,NULL);
        mDummyAction->runAction(aSeq_1);
        
        // Update to next that ready to wait !!!
        mCurrentTutorialStep = TUTORIAL_S0_DIALOG_MORE_DWARFS_WAIT;
//        SpawnDwarf();
    }
    else if(theStep == TUTORIAL_S0_DIALOG_MORE_DWARFS_WAIT)
    {
        // All dwarfs entered cave !!! wohoo
        mActivetTimesForNext-=1;
        if(mActivetTimesForNext<=0){
            // Next
            IncreaseTutorialStep(NULL);
        }
    }
    else if(theStep == TUTORIAL_S0_DIALOG_MORE_DWARFS_DONE)
    {
//        CreateDialog(0, TUTORIAL_S0_DIALOG_MORE_DWARFS_DONE,0);
        CreateDialog(4, "Well done my friend! Now collect some crystals for me by drawing a line through them!", 0,
                     ccp(mGameScene->_MasterDwarfBase->getPositionX()-60,mScreenSize.height/2+60));
        
        mGameScene->generateDwarfAtSpot(2, DWARF_TYPE_TALL, false);
        
        Dwarf* theDummy = static_cast<Dwarf*>(mGameScene->_dwarves->objectAtIndex(mGameScene->_dwarves->count()-1));
        theDummy->mTutorialWaitTimeToNextStep = 1.0f; // Do next tutorial step after a while
        
        // Generate a crystal near stuff !!!
        mGameScene->generateCrystalSpecial(400,170,-1);
    }
    else if(theStep == TUTORIAL_S0_DIALOG_SPAWN_CRYSTAL)
    {
        mConnectedTutorialStuff = false;
        
        mGameScene->pauseGame();// Freeze all
        
        // The magic line
        _line = CCSpriteBatchNode::create("trajectory_dot_white.png");
        mGameScene->addChild(_line);
        
        // Clear and give the points
        _movePoints = CCPointArray::create(0);
        _movePoints->retain();
        
        mGameScene->AddTutorialUpdater();
        // Create the crystal part hand
        CreateDrawHand(TUTORIAL_S0_DIALOG_SPAWN_CRYSTAL);
    }
    else if(theStep == TUTORIAL_S0_DIALOG_CRYSTAL_COLLECT)
    {
        // Unpause the game and remove all crap
        mGameScene->onPauseFinished();
        
        mGameScene->RemoveTutorialUpdater();
    }
    else if (theStep == TUTORIAL_S0_DIALOG_CRYSTAL_COLLECTED_IN_CAVE)
    {
        // Well done dwarf !!!
//        CreateDialog(0, TUTORIAL_S0_DIALOG_CRYSTAL_COLLECTED_IN_CAVE,1);
        CreateDialog(4, "Congratulations! You are ready for adventure!", 1,
                     ccp(mGameScene->_MasterDwarfBase->getPositionX()-60,mScreenSize.height/2+60));
    }
    else if(theStep == TUTORIAL_S0_GAME_OVER_START)
    {
//        mGameScene->runAction(CCShake::actionWithDuration( 1.0f, 5.0f ));
        
        // Spawn MT and other stuff
        mGameScene->CreateOnlyMaster(1);
        
        // Create the parade !!!
        mGameScene->CreateMonsterParade();
        
        
        // This will come latter
        CCDelayTime* aToEndBaseTutor = CCDelayTime::create(5.0f);
        CCCallFunc* aToEndBaseTutor_Func = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::IncreaseTutorialStep));
        CCSequence* aToEndBaseTutor_Seq = CCSequence::create(aToEndBaseTutor,aToEndBaseTutor_Func,NULL);
        mDummyAction->runAction(aToEndBaseTutor_Seq);
    }
    else if(theStep == TUTORIAL_S0_GAME_OVER_FINISH)
    {
        // Fade this balc ass in !!!
        CCLayerColor* aColorLayer = CCLayerColor::create(ccc4(0, 0, 0, 0), mScreenSize.width, mScreenSize.height);
        mGameScene->addChild(aColorLayer,1000);
        
        CCFadeIn* aFade = CCFadeIn::create(2);
        aColorLayer->runAction(aFade);
        
        // When that is done - show that image !! and then switch stuff
//        bilde.png
        CCSprite* aPicIntro = CCSprite::create("bilde.png");
        aPicIntro->setAnchorPoint(ccp(0,0));
        aPicIntro->setOpacity(0);
        mGameScene->addChild(aPicIntro,1001);
        
        CCDelayTime* aDelayPic = CCDelayTime::create(3);
        CCFadeIn* aFadeInPic = CCFadeIn::create(0.5);
        CCDelayTime* aWaitForLoad = CCDelayTime::create(1.0);
        CCCallFunc* aWaitToLoadFunc = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::LoadWorldMap));
        CCSequence* aFadePicSeq = CCSequence::create(aDelayPic,aFadeInPic,aWaitForLoad,aWaitToLoadFunc,NULL);
        aPicIntro->runAction(aFadePicSeq);
        
        //Change now the screen
        
        
        
//        addChild(aColorLayer);
        
        //Fade black and go to world map !!!
        /*
        CCScene* options = DF::StaticSceneManager::getInstance()->getScene(DF::StaticSceneManager::MISSIONS);
        CCTransitionScene* transition = CCTransitionFade::create(1.0f,options,ccBLACK);
        CCDirector::sharedDirector()->replaceScene(transition);
        */
        
        // Completed the tutorial
//        mTutorialCompleted = true;
    }
    else if(theStep == TUTORIAL_S0_WORLD_MAP_INTRO)
    {
//        mWorldMap->setVisible(false);
        // Add tap cursor???
        
        mWorldMap->mPlayer->setOpacityModifyRGB(0);
        mWorldMap->mPlayer->setVisible(true);
        
        CCDelayTime* aDelayAbit = CCDelayTime::create(0.75f);
        
        CCMoveTo* aMovePlayerTo = CCMoveTo::create(1.0f,ccp(mWorldMap->mPlayer->getPositionX()+20,mWorldMap->mPlayer->getPositionY()-20));
        CCEaseElasticOut* aEasePlayer = CCEaseElasticOut::create(aMovePlayerTo,0.5f);
        CCFadeIn* aFadeInPlayer = CCFadeIn::create(0.25f);
        
        CCSpawn* aPlayerSpanw_1 = CCSpawn::create(aEasePlayer,aFadeInPlayer,NULL);
        
        // Call function when all completed
        CCBlink* aBlinkPlayer = CCBlink::create(1.0f,5);
        
        CCCallFunc* aFuncPlayer = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::IncreaseTutorialStep));
        
        CCSequence* aSeqPlayer = CCSequence::create(aDelayAbit,aPlayerSpanw_1,aBlinkPlayer,aFuncPlayer,NULL);
        
        // Set a bit up now
        mWorldMap->mPlayer->setPosition(mWorldMap->mPlayer->getPositionX(),mWorldMap->mPlayer->getPositionY()+200);
        
        mWorldMap->mPlayer->runAction(aSeqPlayer);
        
//        cocos2d::CCScene* aScene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
//        LoadingScreen* aLoading = static_cast<LoadingScreen*>(aScene->getChildByTag(888));
        
//        if()
    }
    else if(theStep == TUTORIAL_S0_WORLD_MAP_MISSION_TAP)
    {
        //Show that need to tap???
        
        CCScaleTo* aScale_1 = CCScaleTo::create(0.25f, 0.8f);
        CCCallFunc* aRipleCall_S1 = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::OnTutorialHandClick));
        CCSequence* aSeqHand_1 = CCSequence::create(aScale_1,aRipleCall_S1,NULL);
        CCDelayTime* aTimeDelay1 = CCDelayTime::create(0.5f);
        CCScaleTo* aScale_2 = CCScaleTo::create(0.1f, 1.0f);
        CCDelayTime* aTimeDelay2 = CCDelayTime::create(0.1f);
        
        CCSequence* aSeq1 = CCSequence::create(aSeqHand_1,aTimeDelay1,aScale_2,aTimeDelay2,NULL);
        CCRepeatForever* aRep1 = CCRepeatForever::create(aSeq1);
        
        //Fat ass to cave
        CCSprite* aHand = CCSprite::create("Interfeiss/tutorial/hand_cursor.png");
        aHand->setPosition(ccp(mWorldMap->mPlayer->getPositionX()+10,mWorldMap->mPlayer->getPositionY()-50));
        aHand->setTag(301);
        
        mWorldMap->map_base->addChild(aHand,kHUD_Z_Order+1);
        
        aHand->runAction(aRep1);
        //Wait when player will click
    }
    else if(theStep == TUTORIAL_S0_WORLD_MAP_LOAD)
    {
        // Show the intro old? before load game?
        GameTutorial::getInstance()->RemoveDrawHand();
        
        // Show old tutorial
        ShowComic();
    }
    else if(theStep == TUTORIAL_S0_WORLD_MAP_COMIC_END)
    {
        if(mWorldMap->getChildByTag(SCENE_COMIC) != NULL){
            mWorldMap->removeChildByTag(SCENE_COMIC);
        }
        
        // Load level
        //Fade all black and load the needed level
        
        // Set the value what level will be played now
        User::getInstance()->getMissionManager().mCurrentActiveMission = 0;
        
        // If need to show some promo - do it
        CCScene* scene = GameScene::scene();
        CCTransitionScene* transition = CCTransitionFade::create(0.5f, scene);
        CCDirector::sharedDirector()->replaceScene(transition);
    }
    else if(theStep == TUTORIAL_S1_INTRO)
    {
        // The mission #1 start
//        CreateDialog(0, TUTORIAL_S1_INTRO,0);
        CreateDialog(4, "Bring home 20 dwarfs!", 0,
                     ccp(mGameScene->_MasterDwarfBase->getPositionX()-60,mScreenSize.height/2+60));
    }
    else if(theStep == TUTORIAL_S1_FIRST_5_IN)
    {
        //Freeze for safe
//        mGameScene->pauseGame();
        
        mGameScene->CreateOnlyMaster(1);
//        CreateDialog(0, TUTORIAL_S1_FIRST_5_IN,1);
    }
    else if(theStep == TUTORIAL_S1_PANIC)
    {
        mGameScene->pauseGame();
//        CreateDialog(0, TUTORIAL_S1_FIRST_5_IN,1);
        CreateDialog(4, "Oh no! That is the Master Troll!", 1,
                     ccp(mGameScene->_MasterDwarfBase->getPositionX()-60,mScreenSize.height/2+60));
    }
    else if(theStep == TUTORIAL_S1_TROLL_HAHA)
    {
        // The mission #1 start
//        CreateDialog(1, TUTORIAL_S1_TROLL_HAHA,2);
        CreateDialog(7, "HA HA HA! Let’s catch all the dwarfs.", 2,
                     ccp(mGameScene->_MasterTrollBase->getPositionX()+60,mScreenSize.height/2));
    }
    else if(theStep == TUTORIAL_S1_QUICK_BRING_15)
    {
//        mGameScene->onPauseFinished();
        // The mission #1 start
//        CreateDialog(0, TUTORIAL_S1_QUICK_BRING_15,0);
        CreateDialog(2, "Quickly, bring home all dwarfs! And beware of Master Troll’s magic!", 3,
                     ccp(mGameScene->_MasterDwarfBase->getPositionX()-60,mScreenSize.height/2-60));
    }
    else if(theStep == TUTORIAL_S1_QUICK_PANIC_END)
    {
        mGameScene->onPauseFinished();
    }
    else if(theStep == TUTORIAL_S1_QUICK_BRING_10)
    {
        // The mission #1 start
//        CreateDialog(0, TUTORIAL_S1_QUICK_BRING_10,1);
        CreateDialog(4, "10 more to go!", 1,
                     ccp(mGameScene->_MasterDwarfBase->getPositionX()-60,mScreenSize.height/2+60));
    }
    else if(theStep == TUTORIAL_S1_WAIT_FOR_ALL)
    {
        // Nothing - go to map :)
//        mTutorialCompleted = true; // For now
        
    }
    else if(theStep == TUTORIAL_S1_SHOW_WORLD_MAP)
    {
        // Dwarfs move on map from 1->2
        IncreaseTutorialStep(NULL);
        
    }
    else if(theStep == TUTORIAL_S1_SHOW_WORLD_MAP_CLICK)
    {
        // Wait for map click !!!

    }
    else if(theStep == TUTORIAL_S1_SHOW_WORLD_CLICKED)
    {
        // Set the value what level will be played now
        User::getInstance()->getMissionManager().mCurrentActiveMission = 1;
        
        // If need to show some promo - do it
        CCScene* scene = GameScene::scene();
        CCTransitionScene* transition = CCTransitionFade::create(0.5f, scene);
        CCDirector::sharedDirector()->replaceScene(transition);
    }
    else if(theStep == TUTORIAL_S2_INTRO)
    {
        // Master troll says something bad !!!
        mGameScene->SetMasterTrollAnimation("Magic");
        // Wait for totem fall down
        mGameScene->pauseGame();
        
        mGameScene->mCurrentSpellCharge = 250;
        
        CCDelayTime* aDelayAnim = CCDelayTime::create(0.8f);
        CCCallFuncN* aDelayAnimFunc = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::SetMasterTrollIdle));
        CCSequence* aDelayAnimSeq = CCSequence::create(aDelayAnim,aDelayAnimFunc,NULL);
        mGameScene->_MasterTrollBase->runAction(aDelayAnimSeq);
        
    }
    else if(theStep == TUTORIAL_S2_TOTEM_FALLED_DOWN)
    {
        CreateDialog(6, "Let’s drill for some Treasure.", 2,
                     ccp(mGameScene->_MasterTrollBase->getPositionX()+60,mScreenSize.height/2+60));
    }
    else if(theStep == TUTORIAL_S2_DK_PANIC_1)
    {
        mGameScene->onPauseFinished();
        
        CreateDialog(3, "Master Troll is stealing our precious crystals! We must stop that thing!", 3,
                     ccp(mGameScene->_MasterDwarfBase->getPositionX()-60,mScreenSize.height/2));
        
        // SPAWN ONE DWARF FOR ACTION O:
        mGameScene->generateDwarfAtSpot(2, DWARF_TYPE_TALL, true);
        
        // Make screen dark and try to highlight some stuff ???
        HighlightScreenPart(1,0);
    }
    else if(theStep == TUTORIAL_S2_MEGENE_WAIT_1ST_DWARF)
    {
        // Lets just wait for dwarf to enter cave !!! and remove that black screen
        StartHighlightScreenRemove();
    }
    else if(theStep == TUTORIAL_S2_MEGENE_SHOW_1ST_DWARF)
    {
        // Dwarf entered cave - show again megene
        HighlightScreenPart(1,2);
        
        // Dwarf King says something
        CreateDialog(2, "Collecting additional crystals will help you fill up energy quicker.", 1.5,
                     ccp(mGameScene->_MasterDwarfBase->getPositionX()-60,mScreenSize.height/2-60));
    }
    else if(theStep == TUTORIAL_S2_MEGENE_SPAWN_CRYSTAL_SHOW)
    {
        // Spawn dwarf too???
        mGameScene->generateDwarfAtSpot(1, DWARF_TYPE_TALL, false);
        
        Dwarf* theDummy = static_cast<Dwarf*>(mGameScene->_dwarves->objectAtIndex(mGameScene->_dwarves->count()-1));
        theDummy->mTutorialWaitTimeToNextStep = 1.0f; // Do next tutorial step after a while
        
        // Generate a crystal near stuff !!!
        mGameScene->generateCrystalSpecial(500,440,3);
    }
    else if(theStep == TUTORIAL_S2_MEGENE_SPAWN_CRYSTAL_WAIT)
    {
        // Show how to connect again crysta??? wtf??
        mConnectedTutorialStuff = false;
        
        mGameScene->pauseGame();// Freeze all
        
        // The magic line
        _line = CCSpriteBatchNode::create("trajectory_dot_white.png");
        mGameScene->addChild(_line);
        
        // Clear and give the points
        _movePoints = CCPointArray::create(0);
        _movePoints->retain();
        
        mGameScene->AddTutorialUpdater();
        // Create the crystal part hand
        CreateDrawHand(TUTORIAL_S2_MEGENE_SPAWN_CRYSTAL_WAIT);
    }
    else if(theStep == TUTORIAL_S2_MEGENE_SPAWN_CRYSTAL_DONE)
    {
        // Connected to cave with crystal in hands !!!
        mGameScene->onPauseFinished();
        
        mGameScene->RemoveTutorialUpdater();
    }
    else if(theStep == TUTORIAL_S2_MEGENE_WAIT_DWARF_ENTER)
    {
        //Spawn next dwarf with spell !!!
        mGameScene->generateDwarfAtSpot(2, DWARF_TYPE_TALL, true);
        
        Dwarf* theDummy = static_cast<Dwarf*>(mGameScene->_dwarves->objectAtIndex(mGameScene->_dwarves->count()-1));
        theDummy->mTutorialWaitTimeToNextStep = 1.0f; // Do next tutorial step after a while
        
        // Add spell to it???
//        Dwarf* dwarf = static_cast<Dwarf*>(mGameScene->_dwarves->objectAtIndex(0));
//        dwarf->setPowerButton(100);
        
    }
    else if(theStep == TUTORIAL_S2_MEGENE_WAIT_DWARF_ATTACK_INFO)
    {
        mGameScene->pauseGame();// Freeze all
        // Dwarf moved in at the cave !!!
        CreateDialog(3, "Use this spell to attack that thing!", 0.5,
                     ccp(mGameScene->_MasterDwarfBase->getPositionX()-60,mScreenSize.height/2));
    }
    else if(theStep == TUTORIAL_S2_MEGENE_WAIT_DWARF_ATTACK)
    {
        HighlightScreenPart(2,0);
        
        // The magic line
        _line = CCSpriteBatchNode::create("trajectory_dot_white.png");
        mGameScene->addChild(_line);
        
        // Clear and give the points
        _movePoints = CCPointArray::create(0);
        _movePoints->retain();
        
        mGameScene->AddTutorialUpdater();
        // Create the crystal part hand
        CreateDrawHand(TUTORIAL_S2_MEGENE_WAIT_DWARF_ATTACK);
    }
    else if(theStep == TUTORIAL_S2_MEGENE_WAIT_DWARF_ATTACKING)
    {
        StartHighlightScreenRemove();
        // Connected to the totem - good job
        mGameScene->onPauseFinished();
        mGameScene->RemoveTutorialUpdater();
    }
    else if(theStep == TUTORIAL_S2_1ST_SHOOT_AT_TOTEM_COMPLETED)
    {
        // Just wait for normal gameplay
        CreateDialog(4, "Well done!", 0,
                     ccp(mGameScene->_MasterDwarfBase->getPositionX()-60,mScreenSize.height/2+60));
        
    }
    else if(theStep == TUTORIAL_S2_2ND_SHOOT_AT_TOTEM_COMPLETED)
    {
        // Just wait for normal gameplay
        CreateDialog(4, "One more hit", 0,
                     ccp(mGameScene->_MasterDwarfBase->getPositionX()-60,mScreenSize.height/2+60));
    }
    else if (theStep == TUTORIAL_S2_WORLD_MAP_MOVE_TO_3_FINISHED)
    {
        // Show up the world map buttons
        mWorldMap->MoveInWorldMapButtons();
        
        // Special stuff
        User::getInstance()->mBoghtSpells = "";
        
        // Show that need to tap
        CraeteWorldMapHand(140,20,1,HINT_ADD_TO_WORLDMAP);
        
        HighlightScreenPart(3,0);
    }
    else if(theStep == TUTORIAL_S2_WORLD_MAP_STORE_OPEN)
    {
        StartHighlightScreenRemove();
        
        // Remove old stuff
        RemoveDrawHand();
        
        // Black the shit out???
        
        // Scroll the shop to needed item !!!
//        mWorldMap->getChildByTag(2223);
//        ScrollToItem
        
        StoreScene* aStore = (StoreScene*)mWorldMap->getChildByTag(2223);
        aStore->mBox->ScrollToItem();
        
        CraeteWorldMapHand(800,450,1,HINT_ADD_TO_WORLDMAP);
        
        HighlightScreenPart(4,0);
    }
    else if(theStep == TUTORIAL_S2_WORLD_MAP_STORE_SPELL_BOUGHT)
    {
        StartHighlightScreenRemove();
        RemoveDrawHand();
        mTutorialCompleted = true; // Finish for now !!!
    }
    
}

void GameTutorial::CraeteWorldMapHand(int theX,int theY, int theDelayToShow, int theSetToTargetID)
{
    CCSprite* aHand = CCSprite::create("Interfeiss/tutorial/hand_cursor.png");
    aHand->setPosition(ccp(theX,theY));
    aHand->setTag(301);
    
    // Lets add it to the world map
    if(theSetToTargetID == HINT_ADD_TO_WORLDMAP){
        mWorldMap->addChild(aHand,kHUD_Z_Order+1);
    }
    
    // The animation
    CCScaleTo* aScale_1 = CCScaleTo::create(0.25f, 0.8f);
    CCCallFunc* aRipleCall_S1 = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::OnTutorialHandClick));
    CCSequence* aSeqHand_1 = CCSequence::create(aScale_1,aRipleCall_S1,NULL);
    CCDelayTime* aTimeDelay1 = CCDelayTime::create(0.5f);
    
    CCScaleTo* aScale_2 = CCScaleTo::create(0.25f, 1.0f);
    CCDelayTime* aTimeDelay2 = CCDelayTime::create(0.5f);
    
    CCSequence* aSeq1 = CCSequence::create(aSeqHand_1,aTimeDelay1,aScale_2,aTimeDelay2,NULL);
    CCRepeatForever* aRep1 = CCRepeatForever::create(aSeq1);
    aHand->runAction(aRep1);
}

// helper & fixer
void GameTutorial::SetMasterTrollIdle(CCNode* sender)
{
    mGameScene->_MasterTrollBase->setAnimationByName("Idle");
}

// The screen black out feature
void GameTutorial::HighlightScreenPart(int theHighlightType,float theRemoveTime)
{
    CCLog("Create Highlight screen part");
    
    CCLayerColor* aCreateLayer = CCLayerColor::create(ccc4(0, 0, 0, 255), mScreenSize.width, mScreenSize.height);
    CCSprite* aHighlight;
    
//    aCreateLayer->setTag(TUTORIAL_BLACK_LAYER);
    
    // The megene stuff
    if(theHighlightType == 1)
    {
        aHighlight = CCSprite::create("megene_highlight.png");
        aHighlight->setPosition(ccp(876,646));
    }
    else if(theHighlightType == 2)
    {
        aHighlight = CCSprite::create("dwarf_tunel_to_totem.png");
        aHighlight->setPosition(ccp(532,200));
    }
    else if(theHighlightType == 3)
    {
        aHighlight = CCSprite::create("world_map_open_shop.png");
        aHighlight->setPosition(ccp(100,100));
    }
    else if(theHighlightType == 4)
    {
        aHighlight = CCSprite::create("shop_buy_item.png");
        aHighlight->setPosition(ccp(500,500));
    }
    
    //.................................................
    // The baking part
    
    CCRenderTexture* rt = CCRenderTexture::create(aCreateLayer->getContentSize().width,aCreateLayer->getContentSize().height);
    
    ccBlendFunc someBlend2 = { GL_ONE, GL_ZERO };
    ccBlendFunc someBlend = { GL_DST_ALPHA, GL_ZERO };
    
//    ccBlendFunc* someBlend = new ccBlendFunc();
//    someBlend->src = GL_ONE;
//    someBlend->dst = GL_ZERO;
    aHighlight->setBlendFunc(someBlend);
    
//    ccBlendFunc* someBlend2 = new ccBlendFunc();
//    someBlend2->src = GL_DST_ALPHA;
//    someBlend2->dst = GL_ZERO;
    aCreateLayer->setBlendFunc(someBlend2);
    
    rt->begin();
    
    aCreateLayer->visit();
    aHighlight->visit();
    
    rt->end();
    
    CCSprite* retval = CCSprite::createWithTexture(rt->getSprite()->getTexture());
    retval->setAnchorPoint(ccp(0,0));
    retval->setFlipY(true);
    retval->setOpacity(0);
    retval->setTag(TUTORIAL_BLACK_LAYER);
    //.................................................
    
    // The finish
    if(theHighlightType == 3 || theHighlightType == 4){
        mWorldMap->addChild(retval,kHUD_Z_Order+1);
    }
    else{
        mGameScene->addChild(retval,kHUD_Z_Order+1);
    }

    
    //Fade it in
    CCFadeTo* aFadeIn = CCFadeTo::create(0.5f,128);
    if(theRemoveTime>0){
        CCDelayTime* aDelayToRemove = CCDelayTime::create(theRemoveTime);
        CCCallFuncN* aFuncForRemove = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::StartHighlightScreenRemove));
        CCSequence* aSeq = CCSequence::create(aFadeIn,aDelayToRemove,aFuncForRemove,NULL);
        
        retval->runAction(aSeq);
    }
    else{
        retval->runAction(aFadeIn);
    }
    
    
    
//    CCFadeTo* aFadeTo = CCFadeTo::create(1.0f, 128);
//    aCreateLayer->runAction(aFadeTo);
}

void GameTutorial::RemoveHighlightScreen(CCNode* sender)
{
    CCLog("Remove Highlight screen part");
    
    if(mGameScene!=NULL)
    {
        if(mGameScene->getChildByTag(TUTORIAL_BLACK_LAYER)!=NULL){
            mGameScene->removeChildByTag(TUTORIAL_BLACK_LAYER);
        }
    }
    else if(mWorldMap!=NULL)
    {
        if(mWorldMap->getChildByTag(TUTORIAL_BLACK_LAYER)!=NULL){
            mWorldMap->removeChildByTag(TUTORIAL_BLACK_LAYER);
        }
    }
}

void GameTutorial::StartHighlightScreenRemove()
{
    CCLog("Start Remove/FadeOut Highlight screen part");
    if(mGameScene!=NULL)
    {
        if(mGameScene->getChildByTag(TUTORIAL_BLACK_LAYER)!=NULL){
            
            CCFadeTo* aFadeOut = CCFadeTo::create(0.5f,0);
            CCCallFunc* aFunction = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::RemoveHighlightScreen));
            CCSequence* aSeq = CCSequence::create(aFadeOut,aFunction,NULL);
            
            CCLayerColor* theLayer = (CCLayerColor*)mGameScene->getChildByTag(TUTORIAL_BLACK_LAYER);
            theLayer->runAction(aSeq);
        }
    }
    else if(mWorldMap!=NULL)
    {
        if(mWorldMap->getChildByTag(TUTORIAL_BLACK_LAYER)!=NULL){
            
            CCFadeTo* aFadeOut = CCFadeTo::create(0.5f,0);
            CCCallFunc* aFunction = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::RemoveHighlightScreen));
            CCSequence* aSeq = CCSequence::create(aFadeOut,aFunction,NULL);
            
            CCLayerColor* theLayer = (CCLayerColor*)mWorldMap->getChildByTag(TUTORIAL_BLACK_LAYER);
            theLayer->runAction(aSeq);
        }
    }
}

void GameTutorial::LoadWorldMap(CCNode* sender)
{
    CCScene* options = DF::StaticSceneManager::getInstance()->getScene(DF::StaticSceneManager::MISSIONS);
//    CCTransitionScene* transition = CCTransitionFade::create(1.0f,options,ccBLACK);
    CCTransitionScene* transition = CCTransitionPageTurn::create(1, options, false);
    CCDirector::sharedDirector()->replaceScene(transition);
    
    // Clear it
    mGameScene = NULL;
}

void GameTutorial::ShowComic()
{
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    
    IntroScene* tutorial = IntroScene::create(false);
    tutorial->setScale(0.87f);
    
    tutorial->setPosition(ccp(-42,-34));
    
    CCSprite* aFrame = CCSprite::create("Interfeiss/stats/bigpop.png");
    aFrame->setPosition(ccp(visibleSize.width/2,visibleSize.height/2-30));
    //Add to this popup the intor + scale
    aFrame->addChild(tutorial);
    aFrame->setTag(SCENE_COMIC);
    
    //---------------------------------
    //Add the header !!!
    
    //Add the clouse button
    CCMenuItemImage* clouseButton = CCMenuItemImage::create(
                                                            "Interfeiss/buttons/x_btn0001.png",
                                                            "Interfeiss/buttons/x_btn0002.png",
                                                            this,
                                                            menu_selector(GameTutorial::IncreaseTutorialStep));
    
    CCMenu* aClouse = CCMenu::create(clouseButton,NULL);
    aClouse->setPosition(ccp(tutorial->getContentSize().width+10,tutorial->getContentSize().height));
    tutorial->addChild(aClouse,1);
    
    //the header
    CCSprite* aHeaderBase = CCSprite::create("Interfeiss/challenges/header_title.png");
    aHeaderBase->setPosition(ccp(tutorial->getContentSize().width/2,tutorial->getContentSize().height+42));
    tutorial->addChild(aHeaderBase);
    
    //Add the decor icons
    CCSprite* aDecor = CCSprite::create("Interfeiss/store/FRESH/text_deco.png");
    aDecor->setPosition(ccp(88,aHeaderBase->getContentSize().height/2));
    aDecor->setColor(ccc3(255,0,0));
    aHeaderBase->addChild(aDecor);
    
    aDecor = CCSprite::create("Interfeiss/store/FRESH/text_deco.png");
    aDecor->setPosition(ccp(268,aHeaderBase->getContentSize().height/2));
    aDecor->setFlipX(true);
    aDecor->setColor(ccc3(255,0,0));
    aHeaderBase->addChild(aDecor);
    
    CCLabelTTF* aStoreHeader = CCLabelTTF::create("Intro", "fonts/Marker Felt.ttf", TITLE_FONT_SIZE*0.8, CCSize(200, aHeaderBase->getContentSize().height), kCCTextAlignmentCenter);
    aStoreHeader->setPosition(ccp(178,aHeaderBase->getContentSize().height/2-4));
    aStoreHeader->setColor(ccc3(89, 19, 12));
    aHeaderBase->addChild(aStoreHeader);
    
    
    //---------------------------------
    
    mWorldMap->addChild(aFrame,100);
//    addChild(aFrame,100);
    
//    CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(getSoundFx("Toggle").c_str());
}

void GameTutorial::SpawnDwarf()
{
    if(mCurrentTutorialStep == TUTORIAL_S0_DIALOG_MORE_DWARFS || mCurrentTutorialStep == TUTORIAL_S0_DIALOG_MORE_DWARFS_WAIT)
    {
        // 1st time 2 dwarfs - then 3
        if(mSubTutorialCounter == 0)
        {
            mGameScene->generateDwarfAtSpot(2, DWARF_TYPE_TALL, false);
            mGameScene->generateDwarfAtSpot(1, DWARF_TYPE_TALL, false);
        }
        else if(mSubTutorialCounter == 1)
        {
            mGameScene->generateDwarfAtSpot(0, DWARF_TYPE_TALL, false);
//            mGameScene->generateDwarfAtSpot(1, DWARF_TYPE_TALL, false);
//            mGameScene->generateDwarfAtSpot(2, DWARF_TYPE_TALL, false);
        }
        
        mSubTutorialCounter += 1;
    }
    
}

//....................................................................................................................................
// The hand cursor functional

void GameTutorial::CreateDrawHand(int theDrawHandType)
{
    RemoveDrawHand();
    
    if(theDrawHandType == TUTORIAL_S0_FIRST_DWARF_DRAG)
    {
        //The anim !!!
        Dwarf* dwarf = static_cast<Dwarf*>(mGameScene->_dwarves->objectAtIndex(0));
        
        //Fat ass to cave
        CCSprite* aHand = CCSprite::create("Interfeiss/tutorial/hand_cursor.png");
        aHand->setPosition(ccp(dwarf->getPositionX()+20,dwarf->getPositionY()-50));
        aHand->setTag(301);
        
        mGameScene->addChild(aHand,kHUD_Z_Order+1);
        
        //Tap effect
        CCScaleTo* aScale_1 = CCScaleTo::create(0.25f, 0.8f);
        CCCallFunc* aRipleCall_S1 = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::OnTutorialHandClick));
        CCSequence* aSeqHand_1 = CCSequence::create(aScale_1,aRipleCall_S1,NULL);
        CCDelayTime* aTimeDelay1 = CCDelayTime::create(0.5f);
        
        CCMoveTo* aToPos_1 = CCMoveTo::create(1.5f,ccp(mScreenSize.width/2-10,mScreenSize.height/2-100));
        CCDelayTime* aTime1 = CCDelayTime::create(0.5f);
        CCMoveTo* aToPos_2 = CCMoveTo::create(0.0f, ccp(aHand->getPositionX(),aHand->getPositionY()));
        
        //On finish clear all points drawed !!!
        CCCallFunc* aRemoveCall_S1 = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::OnTutorialHandStart));
        CCCallFunc* aRemoveCall_F1 = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::OnTutorialHandFinish));
        
        CCScaleTo* aScale_2 = CCScaleTo::create(0.0f, 1.0f);
        
        CCSequence* aSeq1 = CCSequence::create(aSeqHand_1,aTimeDelay1,aRemoveCall_S1,aToPos_1,aTime1,aToPos_2,aRemoveCall_F1,aScale_2,NULL);
        CCRepeatForever* aRep1 = CCRepeatForever::create(aSeq1);
        
        aHand->runAction(aRep1);
    }
    else if(theDrawHandType == TUTORIAL_S0_DIALOG_SPAWN_CRYSTAL)
    {
        //The anim !!!
        Dwarf* dwarf = static_cast<Dwarf*>(mGameScene->_dwarves->objectAtIndex(0));
        
        //Fat ass to cave
        CCSprite* aHand = CCSprite::create("Interfeiss/tutorial/hand_cursor.png");
        aHand->setPosition(ccp(dwarf->getPositionX()+20,dwarf->getPositionY()-50));
        aHand->setTag(301);
        
        mGameScene->addChild(aHand,kHUD_Z_Order+1);
        
        //Tap effect
        CCScaleTo* aScale_1 = CCScaleTo::create(0.25f, 0.8f);
        CCCallFunc* aRipleCall_S1 = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::OnTutorialHandClick));
        CCSequence* aSeqHand_1 = CCSequence::create(aScale_1,aRipleCall_S1,NULL);
        CCDelayTime* aTimeDelay1 = CCDelayTime::create(0.5f);
        
        // The braizer trough crystal
//        CCMoveTo* aToPos_1 = CCMoveTo::create(1.5f,ccp(mScreenSize.width/2-10,mScreenSize.height/2-100));
        //Create another braizer combo
        ccBezierConfig bezier;
        bezier.controlPoint_1 = ccp(dwarf->getPositionX(), dwarf->getPositionY());//1096,168
        bezier.controlPoint_2 = ccp(mScreenSize.width/2+100, dwarf->getPositionY());//635,105
        bezier.endPosition = ccp(mScreenSize.width/2+50, mScreenSize.height/2-50);
        
        CCBezierTo* aToPos_1a = CCBezierTo::create(1.5f, bezier);
        
        CCDelayTime* aTime1 = CCDelayTime::create(0.5f);
        CCMoveTo* aToPos_2 = CCMoveTo::create(0.0f, ccp(aHand->getPositionX(),aHand->getPositionY()));
        
        //On finish clear all points drawed !!!
        CCCallFunc* aRemoveCall_S1 = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::OnTutorialHandStart));
        CCCallFunc* aRemoveCall_F1 = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::OnTutorialHandFinish));
        
        CCScaleTo* aScale_2 = CCScaleTo::create(0.0f, 1.0f);
        
        CCSequence* aSeq1 = CCSequence::create(aSeqHand_1,aTimeDelay1,aRemoveCall_S1,aToPos_1a,aTime1,aToPos_2,aRemoveCall_F1,aScale_2,NULL);
//        CCSequence* aSeq1 = CCSequence::create(aSeqHand_1,aTimeDelay1,aRemoveCall_S1,aToPos_1,aTime1,aToPos_2,aRemoveCall_F1,aScale_2,NULL);
        CCRepeatForever* aRep1 = CCRepeatForever::create(aSeq1);
        
        aHand->runAction(aRep1);
    }
    else if(theDrawHandType == TUTORIAL_S2_MEGENE_SPAWN_CRYSTAL_WAIT)
    {
        //The anim !!!
        Dwarf* dwarf = static_cast<Dwarf*>(mGameScene->_dwarves->objectAtIndex(0));
        
        //Fat ass to cave
        CCSprite* aHand = CCSprite::create("Interfeiss/tutorial/hand_cursor.png");
        aHand->setPosition(ccp(dwarf->getPositionX()+20,dwarf->getPositionY()-50));
        aHand->setTag(301);
        
        mGameScene->addChild(aHand,kHUD_Z_Order+1);
        
        //Tap effect
        CCScaleTo* aScale_1 = CCScaleTo::create(0.25f, 0.8f);
        CCCallFunc* aRipleCall_S1 = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::OnTutorialHandClick));
        CCSequence* aSeqHand_1 = CCSequence::create(aScale_1,aRipleCall_S1,NULL);
        CCDelayTime* aTimeDelay1 = CCDelayTime::create(0.5f);
        
        // The braizer trough crystal
        //Create another braizer combo
        ccBezierConfig bezier;
        bezier.controlPoint_1 = ccp(dwarf->getPositionX(), dwarf->getPositionY());//1096,168
        bezier.controlPoint_2 = ccp(dwarf->getPositionX(), 200);//635,105
        bezier.endPosition = ccp(mScreenSize.width-250, mScreenSize.height-300);
        
        CCBezierTo* aToPos_1a = CCBezierTo::create(1.5f, bezier);
        
        CCDelayTime* aTime1 = CCDelayTime::create(0.5f);
        CCMoveTo* aToPos_2 = CCMoveTo::create(0.0f, ccp(aHand->getPositionX(),aHand->getPositionY()));
        
        //On finish clear all points drawed !!!
        CCCallFunc* aRemoveCall_S1 = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::OnTutorialHandStart));
        CCCallFunc* aRemoveCall_F1 = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::OnTutorialHandFinish));
        
        CCScaleTo* aScale_2 = CCScaleTo::create(0.0f, 1.0f);
        
        CCSequence* aSeq1 = CCSequence::create(aSeqHand_1,aTimeDelay1,aRemoveCall_S1,aToPos_1a,aTime1,aToPos_2,aRemoveCall_F1,aScale_2,NULL);
        CCRepeatForever* aRep1 = CCRepeatForever::create(aSeq1);
        
        aHand->runAction(aRep1);
    }
    else if(theDrawHandType == TUTORIAL_S2_MEGENE_WAIT_DWARF_ATTACK)
    {
        // the totem attack
        Dwarf* dwarf = static_cast<Dwarf*>(mGameScene->_dwarves->objectAtIndex(mGameScene->_dwarves->count()-1)); // Get the last dwarf
        
        //Fat ass to cave
        CCSprite* aHand = CCSprite::create("Interfeiss/tutorial/hand_cursor.png");
        aHand->setPosition(ccp(dwarf->getPositionX()+20,dwarf->getPositionY()-50));
        aHand->setTag(301);
        
        mGameScene->addChild(aHand,kHUD_Z_Order+1);
        
        //Tap effect
        CCScaleTo* aScale_1 = CCScaleTo::create(0.25f, 0.8f);
        CCCallFunc* aRipleCall_S1 = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::OnTutorialHandClick));
        CCSequence* aSeqHand_1 = CCSequence::create(aScale_1,aRipleCall_S1,NULL);
        CCDelayTime* aTimeDelay1 = CCDelayTime::create(0.5f);
        
        CCMoveTo* aToPos_1 = CCMoveTo::create(1.5f,ccp(mGameScene->mTotem->getPositionX(),mGameScene->mTotem->getPositionY()));
        CCDelayTime* aTime1 = CCDelayTime::create(0.5f);
        CCMoveTo* aToPos_2 = CCMoveTo::create(0.0f, ccp(aHand->getPositionX(),aHand->getPositionY()));
        
        //On finish clear all points drawed !!!
        CCCallFunc* aRemoveCall_S1 = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::OnTutorialHandStart));
        CCCallFunc* aRemoveCall_F1 = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::OnTutorialHandFinish));
        
        CCScaleTo* aScale_2 = CCScaleTo::create(0.0f, 1.0f);
        
        CCSequence* aSeq1 = CCSequence::create(aSeqHand_1,aTimeDelay1,aRemoveCall_S1,aToPos_1,aTime1,aToPos_2,aRemoveCall_F1,aScale_2,NULL);
        CCRepeatForever* aRep1 = CCRepeatForever::create(aSeq1);
        
        aHand->runAction(aRep1);
    }
}

void GameTutorial::OnTutorialHandClick(CCNode* sender)
{
    if(mWorldMap != NULL && mWorldMap->isVisible())
    {
        // The world map time
        if(mWorldMap->map_base->getChildByTag(334)){
            mWorldMap->map_base->removeChildByTag(334);
        }
        if(mWorldMap->getChildByTag(334)){
            mWorldMap->removeChildByTag(334);
        }
        
        CCSprite* aHand = CCSprite::create("Interfeiss/tutorial/ripple_effect.png");
        aHand->setPosition(ccp(sender->getPositionX()-30,sender->getPositionY()+40));
//        aHand->setPosition(ccp(0,0));
        aHand->setTag(334);
        aHand->setScale(0);
//        mWorldMap->map_base->addChild(aHand,kHUD_Z_Order-1);
        sender->getParent()->addChild(aHand,kHUD_Z_Order-1);
        
        CCScaleTo* aScale_1 = CCScaleTo::create(0.5f, 1.0f);
        
        CCDelayTime* aDelay1 = CCDelayTime::create(0.75f);
        CCFadeTo* aFade_1 = CCFadeTo::create(0.5f,0);
        CCSpawn* aSpawn1 = CCSpawn::create(aDelay1,aFade_1,NULL);
        
        CCSpawn* aSpawn2 = CCSpawn::create(aScale_1,aSpawn1,NULL);
        
        CCCallFunc* aRemoveCall_S1 = CCCallFuncN::create(this, callfuncN_selector(WorldMap::removeNode));
        CCSequence* aSeq1 = CCSequence::create(aSpawn2,aRemoveCall_S1,NULL);
        aHand->runAction(aSeq1);

    }
    else if(mGameScene != NULL && mGameScene->isVisible())
    {
        // The game screen stuff
        if(mGameScene->getChildByTag(334))
            mGameScene->removeChildByTag(334);
        
        CCSprite* aHand = CCSprite::create("Interfeiss/tutorial/ripple_effect.png");
        aHand->setPosition(ccp(sender->getPositionX()-30,sender->getPositionY()+40));
        aHand->setTag(334);
        aHand->setScale(0);
        mGameScene->addChild(aHand,kHUD_Z_Order-1);
        
        CCScaleTo* aScale_1 = CCScaleTo::create(0.5f, 1.0f);
        
        CCDelayTime* aDelay1 = CCDelayTime::create(0.75f);
        CCFadeTo* aFade_1 = CCFadeTo::create(0.5f,0);
        CCSpawn* aSpawn1 = CCSpawn::create(aDelay1,aFade_1,NULL);
        
        CCSpawn* aSpawn2 = CCSpawn::create(aScale_1,aSpawn1,NULL);
        
        CCCallFunc* aRemoveCall_S1 = CCCallFuncN::create(this, callfuncN_selector(GameScene::removeNode));
        CCSequence* aSeq1 = CCSequence::create(aSpawn2,aRemoveCall_S1,NULL);
        aHand->runAction(aSeq1);
    }
}

void GameTutorial::OnTutorialHandFinish()
{
    removeTutorialMovePoints();
}

void GameTutorial::OnTutorialHandStart()
{
    mTutorialDrawHand = true;
}

void GameTutorial::RemoveDrawHand()
{
    removeTutorialMovePoints();
    
    if(mGameScene != NULL && mGameScene->isVisible())
    {
        if(mGameScene->getChildByTag(301) != NULL)
            mGameScene->removeChildByTag(301);
    }
    else if(mWorldMap != NULL && mWorldMap->isVisible())
    {
        if(mWorldMap->map_base->getChildByTag(301) != NULL)
            mWorldMap->map_base->removeChildByTag(301);
        if(mWorldMap->getChildByTag(301) != NULL)
            mWorldMap->removeChildByTag(301);
    }
}

bool GameTutorial::addTutorialMovePoint(const CCPoint& point, const CCPoint& previousPoint)
{
    CCPoint diff = point - previousPoint;
    
    float angle = atan2f(diff.y, diff.x);
    
    float dx = 20.0f * cosf(angle);
    float dy = 20.0f * sinf(angle);
    
    float x = previousPoint.x + dx;
    float y = previousPoint.y + dy;
    
    while (true)
    {
        if (dx >= 0.0f && x >= point.x) return true;
        if (dy >= 0.0f && y >= point.y) return true;
        if (dx < 0.0f && x <= point.x) return true;
        if (dy < 0.0f && y <= point.y) return true;
        
        _movePoints->addControlPoint(ccp(x, y));
        mTutorialPointsAdded = true;
        
        CCSprite* dot = CCSprite::create("trajectory_dot_white.png");
        dot->setPosition(ccp(x, y));
        _line->addChild(dot);
        
        x += dx;
        y += dy;
    }
    
    return true;
}

void GameTutorial::removeTutorialMovePoints()
{
    mTutorialHandStartCords = ccp(0,0);
    // remove all control points
    if(_movePoints && _movePoints!=NULL && mTutorialPointsAdded)
    {
        while (_movePoints->count() != 0)
        {
            _movePoints->removeControlPointAtIndex(0);
        }
        _line->removeAllChildrenWithCleanup(false);
        
        mTutorialPointsAdded = false;
    }
}

void GameTutorial::UpdateTutorial()
{
//    CCLog("wbaba");
    if(mGameScene->getChildByTag(301))
    {
        //Check if can do this
        if(mTutorialDrawHand)
        {
            if(mTutorialHandStartCords.x == 0 && mTutorialHandStartCords.y == 0)
            {
                mTutorialHandStartCords = mGameScene->getChildByTag(301)->getPosition();
                mTutorialHandStartCords.x-=10;
                mTutorialHandStartCords.y+=20;
            }
            CCPoint previousPoint = mTutorialHandStartCords;
            
            if (_movePoints && mTutorialPointsAdded &&  _movePoints->count() > 0)
                previousPoint = _movePoints->getControlPointAtIndex(_movePoints->count() - 1);
            
            CCPoint position = mGameScene->getChildByTag(301)->getPosition();
            position.x-=25;
            position.y+=52;
            
            addTutorialMovePoint(position, previousPoint);
        }
    }
}

//....
// The universal hint for all cases

void GameTutorial::CreateHint(int theArrowPosition,CCPoint thePosition, const char* theHintText, int theScreen)
{
    // Lets create it
    cocos2d::extension::CCScale9Sprite*sp = cocos2d::extension::CCScale9Sprite::create(CCRect(10,10,22,22),"Dialog9Slice_Hint.png");
    sp->setTag(kNoPause);
    
    // Write the text
    std::stringstream theTextToShow;
    theTextToShow << theHintText;
    
    CCLabelBMFont* theText = CCLabelBMFont::create(theTextToShow.str().c_str(), "FontsBmp/DialogFont_Hint.fnt");
    // Check if text is not too wide - wordwrap time !!!
    if(theText->getContentSize().width>200){
        // Yep - it's too wide
        theText->setWidth(200);
    }
    
    // Update the dialog to correct size
    sp->setContentSize( CCSizeMake(theText->getContentSize().width+30, theText->getContentSize().height+30) );
    
    // Add the speach stuff
    CCSprite* aDialogTalk = CCSprite::create("Dialog_Hint_Arrow.png");
//    aDialogTalk->setPosition(ccp(sp->getContentSize().width/2,sp->getContentSize().height/2)); // Set pos by
    
    // The positions of all
    if(theArrowPosition == 0){// Right side
        aDialogTalk->setPosition(ccp(-4,sp->getContentSize().height/2));
        
        sp->setAnchorPoint(ccp(0,0.5));
    }
    else if(theArrowPosition == 1){// Left side
        aDialogTalk->setFlipX(true);
        aDialogTalk->setAnchorPoint(ccp(0,0.5));
        aDialogTalk->setPosition(ccp(sp->getContentSize().width-4,sp->getContentSize().height/2));
        
        sp->setAnchorPoint(ccp(1,0.5));
    }
    else if(theArrowPosition == 2){// Top side
        aDialogTalk->setRotation(90);
        aDialogTalk->setAnchorPoint(ccp(1,0.5));
        aDialogTalk->setPosition(ccp(sp->getContentSize().width/2,sp->getContentSize().height-4));
        
        sp->setAnchorPoint(ccp(0.5,1));
    }
    else if(theArrowPosition == 3){// Bottom side
        aDialogTalk->setRotation(-90);
        aDialogTalk->setAnchorPoint(ccp(1,0.5));
        aDialogTalk->setPosition(ccp(sp->getContentSize().width/2,4));
        
        sp->setAnchorPoint(ccp(0.5,0));
    }
    
    sp->addChild(aDialogTalk);
    
    theText->setAlignment(kCCTextAlignmentCenter);
    // Now set the correct position
    theText->setPosition(ccp(sp->getContentSize().width/2,sp->getContentSize().height/2));
    
    sp->addChild(theText);
    
    sp->setPosition(thePosition);
    
    if(theScreen == 0){
        mGameScene->addChild(sp,100000);
    }
    
    sp->setScale(0.0f);
    
    // The show up action
    // Show it up
    CCDelayTime* aDelay = CCDelayTime::create(0.1f);
    CCScaleTo* aScale = CCScaleTo::create(0.5f,1);
    CCEaseElasticOut* aEase = CCEaseElasticOut::create(aScale, 0.5f);
    
    // Add automatic clouse stuff???
    CCDelayTime* aDelayToClouse = CCDelayTime::create(4.0f);
    CCScaleTo* aScaleOut = CCScaleTo::create(0.5f, 0);
    CCEaseElasticIn* aEaseClouse = CCEaseElasticIn::create(aScaleOut, 0.5f);
    CCCallFuncN* aDoNextStepFunc = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::RemovedHint)); // Checks if need to show next hint etc
    CCCallFuncN* aHideDialog = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::RemoveDialog));
    
    // Do the seq
    CCSequence* aSeq = CCSequence::create(aDelay,aEase,aDelayToClouse,aEaseClouse,aDoNextStepFunc,aHideDialog,NULL);;
    sp->runAction(aSeq);
}

// Check if need to show next hint
void GameTutorial::RemovedHint(CCNode* sender)
{
    // We can show next or do next stuff
    
}

void GameTutorial::AddHintToShow(int thePositionArrow,CCPoint thePosition, const char* theText,int theScreen)
{
    HintToShow* aHintToShow = new HintToShow();
    
    aHintToShow->text = theText;
    aHintToShow->x = thePosition.x;
    aHintToShow->y = thePosition.y;
    aHintToShow->position = thePositionArrow;
    aHintToShow->screen = theScreen;
    
    mHintsToShowVector.push_back(*aHintToShow);
}

void GameTutorial::CreateNextHint(CCNode* sender)
{
    if(mHintsToShowVector.size()>0)
    {
        // We have something to show
        HintToShow *aHint = NULL;
        aHint = &mHintsToShowVector[0];
        
        CreateHint(aHint->position, ccp(aHint->x,aHint->y), aHint->text.c_str(), aHint->screen);
        
        // Erase it now
        mHintsToShowVector.erase(mHintsToShowVector.begin());
    }
}

//....................................................................................................................................
// The Universal dialog for all cases

void GameTutorial::CreateDialog(int theArrowPos, const char* theDialog, float callExtra, CCPoint thePosition)
{
    // Create the dilaog so wide as it's only needed
    cocos2d::extension::CCScale9Sprite*sp = cocos2d::extension::CCScale9Sprite::create(CCRect(40,40,48,48),"Dialog9Slice.png");
    sp->setTag(kNoPause);
    
    // Write the text
    std::stringstream theTextToShow;
    theTextToShow << theDialog;
    
    CCLabelBMFont* theText = CCLabelBMFont::create(theTextToShow.str().c_str(), "FontsBmp/DialogFont.fnt");
    // Check if text is not too wide - wordwrap time !!!
    if(theText->getContentSize().width>500){
        // Yep - it's too wide
        theText->setWidth(500);
    }
    
    // Update the dialog to correct size
    sp->setContentSize( CCSizeMake(theText->getContentSize().width+30, theText->getContentSize().height+30) );
    
    // Add the speach stuff
    CCSprite* aDialogTalk;
    
    if(theArrowPos == 0 || theArrowPos == 2 || theArrowPos == 4 || theArrowPos == 6)
    {
        // The side diognal
        aDialogTalk = CCSprite::create("Dialog_bottom_right.png");
        if(theArrowPos == 0){
            aDialogTalk->setFlipX(true);
            aDialogTalk->setFlipY(true);
            aDialogTalk->setPosition(ccp(-1.5,sp->getContentSize().height-0.5));
            
            sp->setAnchorPoint(ccp(0,1));
        }
        else if(theArrowPos == 2){
            aDialogTalk->setFlipY(true);
            aDialogTalk->setPosition(ccp(sp->getContentSize().width+1.5,sp->getContentSize().height-0.5));
            
            sp->setAnchorPoint(ccp(1,1));
        }
        else if(theArrowPos == 4){
            aDialogTalk->setPosition(ccp(sp->getContentSize().width+1.5,0.5));
            
            sp->setAnchorPoint(ccp(1,0));
        }
        else if(theArrowPos == 6){
            aDialogTalk->setFlipX(true);
            aDialogTalk->setPosition(ccp(-1.5,0.5));
            
            sp->setAnchorPoint(ccp(0,0));
        }
    }
    else
    {
        // the sides
        aDialogTalk = CCSprite::create("Dialog_mid_right.png");
        
        if(theArrowPos == 1){
            aDialogTalk->setRotation(-90);
            aDialogTalk->setAnchorPoint(ccp(0,0.5));
            aDialogTalk->setPosition(ccp(sp->getContentSize().width/2,sp->getContentSize().height-10));
            
            sp->setAnchorPoint(ccp(0.5,1));
        }
        else if(theArrowPos == 3){
            aDialogTalk->setPosition(ccp(sp->getContentSize().width+14,sp->getContentSize().height/2));
            
            sp->setAnchorPoint(ccp(1,0.5));
        }
        else if(theArrowPos == 5){
            aDialogTalk->setRotation(90);
            aDialogTalk->setAnchorPoint(ccp(0,0.5));
            aDialogTalk->setPosition(ccp(sp->getContentSize().width/2,10));
            
            sp->setAnchorPoint(ccp(0.5,0));
        }
        else if(theArrowPos == 7){
            aDialogTalk->setFlipX(true);
            aDialogTalk->setPosition(ccp(-14,sp->getContentSize().height/2));
            
            sp->setAnchorPoint(ccp(0,0.5));
        }
    }
    
    sp->addChild(aDialogTalk);
    
    theText->setAlignment(kCCTextAlignmentCenter);
    // Now set the correct position
    theText->setPosition(ccp(sp->getContentSize().width/2,sp->getContentSize().height/2));
    
    sp->addChild(theText);
    
    // Check from what side need to show up
    sp->setScale(0.0f);
//    sp->setAnchorPoint(ccp(1,0));
    
    // Where should we set it ???
    
    /*
    if(theShowType == 0){
        // Dwarf King
//        mGameScene->_MasterDwarfBase->getPositionX()
        sp->setPosition(ccp(mGameScene->_MasterDwarfBase->getPositionX()-60,mScreenSize.height/2+60));
    }
    else{
        aDialogTalk->setFlipX(true);
        aDialogTalk->setFlipY(true);
        
        aDialogTalk->setPosition(ccp(-1.5,sp->getContentSize().height-0.5));
        sp->setAnchorPoint(ccp(0,0.5));
        sp->setPosition(ccp(mGameScene->_MasterTrollBase->getPositionX()+60,mScreenSize.height/2-60));
        
    }
    */
    
    sp->setPosition(thePosition);
    
    mGameScene->addChild(sp,100000);
    
    // Show it up
    CCDelayTime* aDelay = CCDelayTime::create(0.1f);
    CCScaleTo* aScale = CCScaleTo::create(0.5f,1);
    CCEaseElasticOut* aEase = CCEaseElasticOut::create(aScale, 0.5f);
    
    // Check if need to do next step when showing it
    CCSequence* aDoNextStepSeq;
    if(callExtra>0)
    {
        CCDelayTime* aDoNextStepDelay = CCDelayTime::create(callExtra);
        CCCallFuncN* aDoNextStepFunc = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::IncreaseTutorialStep));
        aDoNextStepSeq = CCSequence::create(aDoNextStepDelay,aDoNextStepFunc,NULL);
    }
    
    // Add automatic clouse stuff???
    CCDelayTime* aDelayToClouse = CCDelayTime::create(4.0f);
    CCScaleTo* aScaleOut = CCScaleTo::create(0.5f, 0);
    CCEaseElasticIn* aEaseClouse = CCEaseElasticIn::create(aScaleOut, 0.5f);
    CCCallFuncN* aHideDialog = CCCallFuncN::create(this, callfuncN_selector(GameTutorial::RemoveDialog));
    
    // Do the seq
    CCSequence* aSeq = CCSequence::create(aDelay,aEase,aDelayToClouse,aEaseClouse,aHideDialog,NULL);;
    
    if(callExtra>0)
    {
        CCSpawn* aDoMultiStuff = CCSpawn::create(aDoNextStepSeq,aSeq,NULL);
        sp->runAction(aDoMultiStuff);
    }
    else
    {
        sp->runAction(aSeq);
    }
    
    

    
    // Run it now
//    sp->runAction(aSeq);
}

//....................................................................................................................................
// The global stuff

void GameTutorial::IncreaseTutorialStep(CCNode* sender)
{
    mCurrentTutorialStep+=1;
    DoStep(mCurrentTutorialStep);
}

void GameTutorial::RemoveDialog(CCNode* sender)
{
    // Clear from the screen etc
    mGameScene->removeChild(sender);
}

