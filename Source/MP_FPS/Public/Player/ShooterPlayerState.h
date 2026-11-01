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

	void AddScoredElim();
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

private:
	
	int32 ScoredElims;		// 击杀数
	int32 Defeats;			// 击败数
	int32 Hits;				// 开枪命中次数
	int32 Misses;			// 开枪未命中次数
	bool bOnStreak;			// 是否连续击杀数（自出生以来，有多少个敌人被击杀 How many elims do we have since we've spawned?）
	int32 HeadShotElims;	// 爆头击中数
	TMap<int32, int32> SequentialElims; // 连续击杀 - 短时间内多次击杀，如三杀、四杀、五杀等（Sequential elims - multiple elims within a short period of time）
	int32 HighestStreak;	// 最高连杀数
	int32 RevengeElims;		// 复仇击杀数 - 复仇就是击杀刚击杀你的人
	int32 DethroneElims;	// 击败榜首的击杀次数
	int32 ShowStopperElims;	// 终结连杀的统计 - 也就是击倒正在连杀的对手
	bool bFirstBlood;		// 是否首杀
	bool bWinner;			// 是否赢得比赛
};
