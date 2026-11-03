// Fill out your copyright notice in the Description page of Project Settings.


#include "Elimination/EliminationComponent.h"


UEliminationComponent::UEliminationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

// 击杀状态回调函数
void UEliminationComponent::OnRoundReported(AActor* Attacker, AActor* Victim, bool bHit, bool bHeadShot, bool bLethal)
{

}
