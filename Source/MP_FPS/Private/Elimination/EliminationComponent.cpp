// Fill out your copyright notice in the Description page of Project Settings.


#include "Elimination/EliminationComponent.h"


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
		
}
