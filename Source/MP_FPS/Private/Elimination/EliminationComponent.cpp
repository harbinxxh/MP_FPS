// Fill out your copyright notice in the Description page of Project Settings.


#include "Elimination/EliminationComponent.h"
#include "Player/ShooterPlayerState.h"
#include "ShooterTypes/ShooterTypes.h"


UEliminationComponent::UEliminationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SequentialElimInterval = 2.f;
	LastElimTime = 0.f;
	SequentialElims = 0;
}

/**
 * 每当有射击发生，击杀组件都能立刻收到通知
 * 在战斗组件中，当在服务器端开枪的时候，就把相关信息广播给击杀组件
 */
void UEliminationComponent::OnRoundReported(AActor* Attacker, AActor* Victim, bool bHit, bool bHeadShot, bool bLethal)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit:%d, HeadShot: %d, Lethal:%d"), bHit, bHeadShot, bLethal);
	
	AShooterPlayerState* AttackerPS = GetPlayerStateFromActor(Attacker);
	if (!IsValid(AttackerPS)) return;

	ProcessHitOrMiss(bHit, AttackerPS);

	if (!bHit) return; // Early return if it was a miss

	AShooterPlayerState* VictimPS = GetPlayerStateFromActor(Victim);
	if (!IsValid(VictimPS)) return;

	if (bLethal)
	{
		PnocessElimination(bHeadShot, AttackerPS, VictimPS);
	}
}

void UEliminationComponent::PnocessElimination(bool bHeadShot, AShooterPlayerState* AttackerPS, AShooterPlayerState* VictimPS)
{
	AttackerPS->AddScoredElim();
	VictimPS->AddDefeat();
	
	ESpecialElimType SpecialElimType{}; // 初始化特殊击杀类型所有位均为 0
	
	// Process Head Shot
	ProcessHeadshot(bHeadShot, SpecialElimType, AttackerPS);

	// Process Sequential Eliminations 处理连续击杀
	ProcessSequentialEliminations(SpecialElimType, AttackerPS);

	// Process Streaks
	// Handle First Blood
	// Update Leader Status

	// if (Has Special Elim Types) 特殊击杀的情况
		// Tell the client which special elims we got
	// else (We just got a regular elim) 非特殊击杀的情况
		// Just tell the client we got a regular elim
}

void UEliminationComponent::ProcessHeadshot(bool bHeadShot, ESpecialElimType& OutElimType, AShooterPlayerState* AttackerPS)
{
	if (bHeadShot)
	{
		//（或等于 |= 运算符）等于右边的写法（Headshot += 1 或  Headshot = Headshot + 1）
		OutElimType |= ESpecialElimType::Headshot;
		AttackerPS->AddHeadShotElim();
	}
}

void UEliminationComponent::ProcessSequentialEliminations(ESpecialElimType& OutElimType, AShooterPlayerState* AttackerPS)
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastElimTime <= SequentialElimInterval)
	{
		++SequentialElims;
	}
	else
	{
		SequentialElims = 1;
	}
	LastElimTime = CurrentTime;

	if (SequentialElims > 1)
	{
		OutElimType |= ESpecialElimType::Sequential;	// 连续击杀位
		AttackerPS->AddSequentialElim(SequentialElims); // 更新攻击者玩家连续击杀状态
	}
}

void UEliminationComponent::ProcessHitOrMiss(bool bHit, AShooterPlayerState* AttackerPS)
{
	if (bHit)
	{
		AttackerPS->AddHit();
	}
	else
	{
		AttackerPS->AddMiss();
	}
}

AShooterPlayerState* UEliminationComponent::GetPlayerStateFromActor(AActor* Actor)
{
	APawn* Pawn = Cast<APawn>(Actor);
	if (IsValid(Pawn))
	{
		return Pawn->GetPlayerState<AShooterPlayerState>();
	}

	return nullptr;
}