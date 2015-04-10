//
//  GameTutorial.h
//  DwarfForest
//
//  Created by Kristaps Auzins on 08/03/15.
//
//

#pragma once

#include <stdint.h>
#include "cocos2d.h"
#include "GameScene.h"
#include "WorldMap.h"

// Why index of 100 - because if we need extra steps - we can shift easy
// Tutorial steps
#define TUTORIAL_S0_INTRO 101

// For intro
#define TUTORIAL_MAP_PATCH_1 1101
#define TUTORIAL_MAP_PATCH_2 1102

#define TUTORIAL_S0_FIRST_DWARF_SPAWN 102
#define TUTORIAL_S0_FIRST_DWARF_DRAG 103
#define TUTORIAL_S0_FIRST_DWARF_ENTER_CAVE 104
#define TUTORIAL_S0_FIRST_DWARF_ENTERED_CAVE 105

#define TUTORIAL_S0_DIALOG_MORE_DWARFS 106
#define TUTORIAL_S0_DIALOG_MORE_DWARFS_WAIT 107
#define TUTORIAL_S0_DIALOG_MORE_DWARFS_DONE 108

#define TUTORIAL_S0_DIALOG_SPAWN_CRYSTAL 109
#define TUTORIAL_S0_DIALOG_CRYSTAL_COLLECT 110
#define TUTORIAL_S0_DIALOG_CRYSTAL_COLLECTED_IN_CAVE 111

#define TUTORIAL_S0_GAME_OVER_START 112
#define TUTORIAL_S0_GAME_OVER_FINISH 113

#define TUTORIAL_S0_WORLD_MAP_INTRO 114
#define TUTORIAL_S0_WORLD_MAP_MISSION_TAP 115
#define TUTORIAL_S0_WORLD_MAP_LOAD 116
#define TUTORIAL_S0_WORLD_MAP_COMIC_END 117

// The tutorial - Mission #1
#define TUTORIAL_S1_INTRO 200
#define TUTORIAL_S1_FIRST_5_IN 201
#define TUTORIAL_S1_PANIC 202
#define TUTORIAL_S1_TROLL_HAHA 203
#define TUTORIAL_S1_QUICK_BRING_15 204
#define TUTORIAL_S1_QUICK_PANIC_END 205
#define TUTORIAL_S1_QUICK_BRING_10 206
#define TUTORIAL_S1_WAIT_FOR_ALL 207
#define TUTORIAL_S1_SHOW_WORLD_MAP 208
#define TUTORIAL_S1_SHOW_WORLD_MAP_CLICK 209
#define TUTORIAL_S1_SHOW_WORLD_CLICKED 210

// The tutorial - Mission #2
#define TUTORIAL_S2_INTRO 300
#define TUTORIAL_S2_TOTEM_FALLED_DOWN 301
#define TUTORIAL_S2_DK_PANIC_1 302
#define TUTORIAL_S2_MEGENE_WAIT_1ST_DWARF 303
#define TUTORIAL_S2_MEGENE_SHOW_1ST_DWARF 304
#define TUTORIAL_S2_MEGENE_SPAWN_CRYSTAL_SHOW 305
#define TUTORIAL_S2_MEGENE_SPAWN_CRYSTAL_WAIT 306
#define TUTORIAL_S2_MEGENE_SPAWN_CRYSTAL_DONE 307
#define TUTORIAL_S2_MEGENE_WAIT_DWARF_ENTER 308
#define TUTORIAL_S2_MEGENE_WAIT_DWARF_ATTACK_INFO 309
#define TUTORIAL_S2_MEGENE_WAIT_DWARF_ATTACK 310
#define TUTORIAL_S2_MEGENE_WAIT_DWARF_ATTACKING 311
#define TUTORIAL_S2_1ST_SHOOT_AT_TOTEM_COMPLETED 312
#define TUTORIAL_S2_2ND_SHOOT_AT_TOTEM_COMPLETED 313
#define TUTORIAL_S2_WORLD_MAP_MOVE_TO_3_FINISHED 314
#define TUTORIAL_S2_WORLD_MAP_STORE_OPEN 315
#define TUTORIAL_S2_WORLD_MAP_STORE_SPELL_BOUGHT 316



#define HINT_ADD_TO_GAMESCREEN 0
#define HINT_ADD_TO_WORLDMAP 1


#define TUTORIAL_S0_DIALOG_CRYSTALS 1001

#define TUTORIAL_BLACK_LAYER 4422

struct HintToShow
{
    std::string text;
    int x;
    int y;
    int position;
    
    // on what screen to show - gameclass etc
    int screen;
};

class GameTutorial: public cocos2d::CCNode
{
public:
    
    ~GameTutorial();
    GameTutorial();
    
    static GameTutorial* getInstance();
    
    // Vars for tutorial
    CCSize mScreenSize;
    int mSubTutorialCounter; // For some extra cases
    int mActivetTimesForNext; // How many times need to come in this step for next to activate
    
    bool mTutorialCompleted;
    int mCurrentTutorialStep;
    
    // If user needs to cennect with crystal
    bool mConnectedTutorialStuff;
    
    void CraeteWorldMapHand(int theX,int theY, int theDelayToShow, int theSetToTargetID);
    
    // Helpers
    void SetGameInstance(GameScene* gameScene);
    GameScene* mGameScene;
    void SetWorldMapInstance(WorldMap* worldScene);
    WorldMap* mWorldMap;
    CCSprite* mDummyAction;
    
    bool StepCompleted(int theStep);
    bool NeedToDoStep(int theStep);
    void DoStep(int theStep);
    
    // Creative part
    void AddHintToShow(int thePositionArrow,CCPoint thePosition, const char* theText,int theScreen);
    void RemovedHint(CCNode* sender);
    void CreateNextHint(CCNode* sender);
    void CreateHint(int theArrowPosition,CCPoint thePosition, const char* theHintText, int theScreen);
    
    std::vector<HintToShow> mHintsToShowVector;
    
//    void CreateDialog(int theShowType, int theDialog, int callExtra);
    void CreateDialog(int theArrowPos, const char* theDialog, float callExtra, CCPoint thePosition);
    void RemoveDialog(CCNode* sender);
    void IncreaseTutorialStep(CCNode* sender);
    
    // Move hand stuff
    void UpdateTutorial();
    
    bool mTutorialDrawHand;
    bool mTutorialPointsAdded;
    cocos2d::CCPoint mTutorialHandStartCords;
    cocos2d::CCPointArray* _movePoints;
    cocos2d::CCSpriteBatchNode* _line;
    
    void OnTutorialHandClick(CCNode* sender);
    void OnTutorialHandFinish();
    void OnTutorialHandStart();
    void RemoveDrawHand();
    bool addTutorialMovePoint(const CCPoint& point, const CCPoint& previousPoint);
    void removeTutorialMovePoints();
    void CreateDrawHand(int theDrawHandType);
    void SpawnDwarf();
    void ShowComic();
    void LoadWorldMap(CCNode* sender);
    void HighlightScreenPart(int theHighlightType,float theRemoveTime);
    void SetMasterTrollIdle(CCNode* sender);
    void RemoveHighlightScreen(CCNode* sender);
    void StartHighlightScreenRemove();
};

/*
class GameTutorial:
public cocos2d::CCLayer
{
    public:
    
    GameTutorial();
    bool init();
    
    // implement the "static node()" method manually
    CREATE_FUNC(InGamePowers);
    
    // The master troll as talker !!!
    
    CCLabelTTF __pScoreLabel;
    
    private:
    
};
*/

//#endif
