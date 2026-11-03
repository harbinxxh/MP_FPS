// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EliminationComponent.generated.h"

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
};
