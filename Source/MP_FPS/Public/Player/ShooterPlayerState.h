// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ShooterPlayerState.generated.h"

/**
 * 玩家状态：负责保存玩家得分相关的数据，比如当前得分，是否领先、特殊击杀等等
 * 保存在游戏过程中持续追踪的信息
 */
UCLASS()
class MP_FPS_API AShooterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	AShooterPlayerState();

	// 击杀数
	void AddScoredElim();
	// 击败数
	void AddDefeat();
	void AddHit();
	void AddMiss();
	void AddHeadShotElim();
	void AddSequentialElim(int32 SequenceCount);
	void UpdateHighestStreak(int32 StreakCount);
	void AddRevengeElim();
	void AddDethroneElim();
	void AddShowStopperElim();
	void GotFirstBlood();
	void IsNowWinner();
	void SetOnStreak(bool bIsOnStreak); // 设置连胜状态
	// 设置击杀者：每次被击杀时都设置它
	void SetLastAttacker(APlayerState* Attacker);

	bool IsOnStreak();
	APlayerState* GetLastAttacker() const;
	int32 GetScoredElims() const;

private:
	
	int32 ScoredElims;		// 击杀数
	int32 Defeats;			// 击败数
	int32 Hits;				// 开枪命中次数
	int32 Misses;			// 开枪未命中次数
	bool bOnStreak;			// 是否连续击杀/连胜（自出生以来，有多少个敌人被击杀 How many elims do we have since we've spawned?）
	int32 HeadShotElims;	// 爆头击中数
	TMap<int32, int32> SequentialElims; // 连续击杀 - 相邻击杀必须落在时间窗口内，时间窗口一过连段清空。短时间内多次击杀，如三杀、四杀、五杀等（Sequential elims - multiple elims within a short period of time）
	int32 HighestStreak;	// 最高连击数/最高连胜次数
	int32 RevengeElims;		// 复仇击杀数 - 复仇就是击杀刚击杀你的人
	int32 DethroneElims;	// 击败榜首的击杀次数
	int32 ShowStopperElims;	// 终结连杀的统计 - 就是击杀正在连杀的对手
	bool bFirstBlood;		// 是否首杀
	bool bWinner;			// 是否赢得比赛
	
	// 注意，我们的射手玩家状态应该保持一个指向最后攻击者的指针，每当我们被击败时，我想保存指向消灭我们的对象的指针
	TWeakObjectPtr<APlayerState> LastAttacker;
};
