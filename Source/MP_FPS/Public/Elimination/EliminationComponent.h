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

private:

	float LastElimTime;		// 上一次击杀时间
	int32 SequentialElims;	// 连续击杀

	AShooterPlayerState* GetPlayerStateFromActor(AActor* Actor);
	// 处理命中或未命中结果的函数
	void ProcessHitOrMiss(bool bHit, AShooterPlayerState* AttackerPS);
	// 处理击杀函数
	void PnocessElimination(bool bHeadShot, AShooterPlayerState* AttackerPS, AShooterPlayerState* VictimPS);
	// 处理头部击杀函数
	void ProcessHeadshot(bool bHeadShot, ESpecialElimType& OutElimType, AShooterPlayerState* AttackerPS);
	// 处理连续击杀函数
	void ProcessSequentialEliminations(ESpecialElimType& OutElimType, AShooterPlayerState* AttackerPS);
};
