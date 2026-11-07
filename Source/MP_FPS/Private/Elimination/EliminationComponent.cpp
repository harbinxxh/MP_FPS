// Fill out your copyright notice in the Description page of Project Settings.


#include "Elimination/EliminationComponent.h"
#include "Player/ShooterPlayerState.h"


UEliminationComponent::UEliminationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

// 
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