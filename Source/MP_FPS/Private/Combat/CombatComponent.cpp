// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/CombatComponent.h"
#include "Engine/Engine.h"
#include "Weapon/Weapon.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::Initiate_CycleWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Initiate_CycleWeapon"), false);
}

void UCombatComponent::Initiate_FireWeapon_Pressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Initiate_FireWeapon_Pressed"), false);
}

void UCombatComponent::Initiate_FireWeapon_Released()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Initiate_FireWeapon_Released"), false);
}

void UCombatComponent::Initiate_ReloadWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Initiate_ReloadWeapon"), false);
}

void UCombatComponent::Initiate_Aim_Pressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Initiate_Aim_Pressed"), false);
}

void UCombatComponent::Initiate_Aim_Released()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Initiate_Aim_Released"), false);
}

void UCombatComponent::SpawnInventory()
{
	// 会在世界坐标 0，0，0 的位置生成武器
	AWeapon* NewWeapon = SpawnSeapon(DefaultWeaponClass);
}

void UCombatComponent::DestroyInventory()
{
	// TODO: Destroy the inventory once we have one.
}

AWeapon* UCombatComponent::SpawnSeapon(TSubclassOf<AWeapon> WeaponClass) const
{
	// 首先，让武器的 owner 和 instigator 都设为 combat component 的 owner
	AActor* OwningActor = GetOwner();
	if (!IsValid(OwningActor)) return nullptr;
	// 判断只在服务器上生成武器
	if (OwningActor->GetLocalRole() < ROLE_Authority) return nullptr;

	FActorSpawnParameters SpawnInfo;
	// 生成这个武器的"拥有者"
	SpawnInfo.Instigator = Cast<APawn>(OwningActor);
	/**
	* 承担伤害责任的 Pawn。
	* 经典例子:
	*		玩家在坦克里开炮 —— 炮弹的 Owner 是坦克,Instigator 是玩家。
	*		伤害结算时通过它找出"谁干的",用于加分、仇恨、击杀归属。
	*/
	SpawnInfo.Owner = OwningActor;
	/**
	* ESpawnActorCollisionHandlingMethod,决定生成点被占住时怎么办。默认 Undefined = 不覆盖,沿用 Actor 类自己的设置。
	*		AlwaysSpawn —— 不管有没有碰撞都生成(生成特效、子弹首选)
	*		AdjustIfPossibleButAlwaysSpawn —— 先尝试挪开位置, 挪不了也生成
	*		AdjustIfPossibleButDontSpawnIfColliding —— 能挪就挪, 挪不开就不生成
	*		DontSpawnIfColliding —— 有碰撞就不生成
	*/
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	return GetWorld()->SpawnActor<AWeapon>(WeaponClass, SpawnInfo);
}

