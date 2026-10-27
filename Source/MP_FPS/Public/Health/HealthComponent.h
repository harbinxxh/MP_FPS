// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

// 广播健康值
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FHealthChanged, UHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);


// 死亡状态
UENUM(BlueprintType)
enum class EDeathState : uint8
{
	NotDead,		// 非死亡
	DeathStarted,	// 死亡开始
	beathFinished	// 死亡结束
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MP_FPS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// 获取 UHealthComponent 组件 BlueprintPure: C++和蓝图都可以调用
	UFUNCTION(BlueprintPure, Category = "FPS|Health")
	static UHealthComponent* FindHealthComponent(const AActor* Actor) { return (IsValid(Actor) ? Actor->FindComponentByClass<UHealthComponent>() : nullptr); }

	// 最大生命值比例
	UFUNCTION(BlueprintCallable)
	float GetHealthNormalized() const;
	
	// 修改健康值 (return true if lethal 如果致命则返回 true)
	bool ChangeHealthByAmount(float Amount, AActor* Instigator);
	// 修改最大健康值
	void ChangeMaxHealthByAmount(float Amount, AActor* Instigator);

	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	EDeathState DeathState;

	UPROPERTY(ReplicatedUsing = OnRep_Health, EditDefaultsOnly, Category = "FPS|Health")
	float Health;

	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth, EditDefaultsOnly, Category = "FPS|Health")
	float MaxHealth;

	// 广播健康值
	UPROPERTY(BlueprintAssignable)
	FHealthChanged OnHealthChanged;
	// 广播最大健康值
	UPROPERTY(BlueprintAssignable)
	FHealthChanged OnMaxHealthChanged;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_DeathState(EDeathState OldDeathState);

	UFUNCTION()
	void OnRep_Health(float OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(float OldValue);

};
