#ifndef __DwarfForest__WorldMap__
#define __DwarfForest__WorldMap__

#include "cocos2d.h"
#include "MissionManager.h"
#include "PanZoomLayer.h"
#include "ItemDataManager.h"

#define MISSION_FLAG_LOCKED 10
#define MISSION_FLAG_OPEN 20
#define MISSION_FLAG_STAR 30

class WorldMap : public cocos2d::CCLayer
{
public:
    
    CREATE_FUNC(WorldMap);
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    WorldMap();
    virtual ~WorldMap();
    
    virtual void update(float delta);
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    void onEnterTransitionDidFinish();
    void onEnter();
    void onExit();
    
    // implement the "static node()" method manually
    bool mNeedToShowNewLevel;
    
    // The map of the map base
    cocos2d::CCSprite *map_base;
    
    CCMenu *mMainMenu;
    void MoveInWorldMapButtons();
    
    // Adds the cool map stuff
    void AddMovingObjects();
    
    void UpdateStats();
    
    void CreateLevels();
    
    void removeNode(CCNode* sender);
    
    void OnClickedMission(CCObject* sender);
    void OnClickedPlayer(CCObject* sender);
    
    void MissionTaskInditificator(int theID);
    
    void CreatePlayer();
    
    // The all node cords
//    int worldNodeCords;//[] = {35,296,118,308,203,278,208,203,128,173,76,120,111,52,218,43,310,78,360,147};
    std::vector<int> SplitString(const std::string s,char delim);
    std::vector<int> mWorldNodeCords;
    cocos2d::CCSize mScreenSize;
    
    void PrepeareSmallMissionScreen();
    void ShowMissionScreen(int theID);
    void HideMissionScreen(CCObject * pSender);
    
    void OnPlayerFinishedMove();
    
    void ResetStats();
    
    void BuyMoreDwarfs();
    void BuyMoreDamage();
    void BuyMoreRange();
    
    PanZoomLayer *pzLayer;
    
    cocos2d::CCMenu* mPlayer;
    cocos2d::CCLabelTTF * taskInfo;
    cocos2d::CCLabelTTF * dwarfCount;
    cocos2d::CCLabelTTF * _damageCount;
    cocos2d::CCLabelTTF * _rangeSize;
    cocos2d::CCLabelTTF * _diamondsLabel;
    cocos2d::CCLabelTTF * _crystalsLabel;
    cocos2d::CCLabelBMFont * dwarfPrice;
    cocos2d::CCLabelBMFont * damagePrice;
    cocos2d::CCLabelBMFont * rangePrice;
    cocos2d::CCLabelTTF * _totemHP;
    cocos2d::CCLabelTTF * _totemShield;
    cocos2d::CCLabelTTF * _totemAttack;
    
    bool moveBackground;
    
    int mCurrentMissionID;
    int mLastMissionID;
    int WhatMission;
    
    int spellDamage;
    int spellRange;
    
    int TESTdwarfCount;
    
    MissionSet mCall;
    SpellInfo mSpellInfo;
    
    cocos2d::CCLayerColor* mSmallMissionScreen;
    
    // Hud stuff
    void CreateHud();
    void OnReloadStuff();
    void Hud_ShowOptions(CCObject* sender);
    void Hud_ShowStore(CCObject* sender);
    void OnRemoveStore();
    void Hud_ShowChallenges(CCObject* sender);
    void Hud_ShowUpgades(CCObject* sender);
    
    cocos2d::CCSpriteBatchNode * _batchWorld;
    SpriteAnimation* _levelUnlockAnim;
    
    int mCurrentCompletedMissionID;
    SpriteAnimation* _levelStars_1;
    bool mPlayingStar_1;
    SpriteAnimation* _levelStars_2;
    bool mPlayingStar_2;
    SpriteAnimation* _levelStars_3;
    bool mPlayingStar_3;
    
    bool mShowingNextLevel;
    int mCurrentUnlockID;
    
    void UpdateMap(float delta);
    void UnlockLevel(int theID);
    void FocusCameraToCords(int theX,int theY);
    
    void ShowMissionStarsEarned(int theMissionID,int theStars,int fromStars);
    void OnActivateStarShow(CCNode* sender);
    bool DidWeImproveMissionStars(int theMissionID);
    
    float mWaitForStarToPlay_2;
    float mWaitForStarToPlay_3;
    
    // Debug
    void delayDebug();
    void delayDebug2();

};

#endif // __DwarfForest__WorldMap__
