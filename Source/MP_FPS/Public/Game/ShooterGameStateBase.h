// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ShooterGameStateBase.generated.h"

/**
 * 游戏状态类：
 *     游戏状态能保存整个游戏的运行状态，比如首次击杀这类信息，它都能存下来，还能记录谁暂时领先
 *     游戏状态还能保存，所有玩家及其在比赛中的排名，特别适合用来显示排行榜或比赛实时状态
 * 
 * 需要注意：
 *     如果你的游戏模式用的是游戏模式基类而不是游戏模式，那么游戏状态就要用游戏状态基类，而不是游戏状态
 */
UCLASS()
class MP_FPS_API AShooterGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AShooterGameStateBase();

	bool HasFirstBloodBeenHad() const;
	// 更新领先者的方法
	void UpdateLeader();
private:
	// 是否已发生首次击杀
	bool bHasFirstBloodBeenHad;

};
