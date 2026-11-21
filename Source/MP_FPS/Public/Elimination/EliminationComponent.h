// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EliminationComponent.generated.h"

class AShooterPlayerState;
enum class ESpecialElimType : uint16;

/**
 * 击杀组件
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MP_FPS_API UEliminationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEliminationComponent();
	
	// 击杀状态回调函数（攻击者、受害者、击中、头部命中、致命）
	UFUNCTION()
	void OnRoundReported(AActor* Attacker, AActor* Victim, bool bHit, bool bHeadShot, bool bLethal);

	// 连续击杀时间间隔（短时间内多次击杀）
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Elimination")
	float SequentialElimInterval;

	// 连击所需击杀次数
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Elimination")
	int32 ElimsNeededForStreak;

private:

	float LastElimTime;		// 上一次击杀时间
	int32 SequentialElims;	// 连续击杀
	int32 Streak;			// 连击数

	AShooterPlayerState* GetPlayerStateFromActor(AActor* Actor);
	// 处理命中或未命中结果的函数
	void ProcessHitOrMiss(bool bHit, AShooterPlayerState* AttackerPS);
	// 处理击杀函数
	void PnocessElimination(bool bHeadShot, AShooterPlayerState* AttackerPS, AShooterPlayerState* VictimPS);
	// 处理头部击杀函数
	void ProcessHeadshot(bool bHeadShot, ESpecialElimType& OutElimType, AShooterPlayerState* AttackerPS);
	// 处理连续击杀函数 - 相邻击杀必须落在时间窗口内，时间窗口一过连段清空。短时间内多次击杀，如三杀、四杀、五杀等
	void ProcessSequentialEliminations(ESpecialElimType& OutElimType, AShooterPlayerState* AttackerPS);
	// 处理连击函数 - 连击就是指在被击杀前的击杀次数，只看是否阵亡，击杀间隔多久都不影响累加
	void PnocessStreaks(ESpecialElimType& OutElimType, AShooterPlayerState* AttackerPS, AShooterPlayerState* VictimPS);
};
