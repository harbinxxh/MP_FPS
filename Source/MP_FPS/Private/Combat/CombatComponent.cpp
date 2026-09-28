// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/CombatComponent.h"
#include "Engine/Engine.h"
#include "Weapon/Weapon.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, Inventory);
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

// SpawnInventory() 函数只会在服务器上实例化武器
void UCombatComponent::SpawnInventory()
{
	// 需要检测权威性，然后遍历库存中的武器类型
	if (GetOwner()->GetLocalRole() < ROLE_Authority) return;

	for (TSubclassOf<AWeapon>& WeaponClass : DefaultWeaponClasses)
	{
		AWeapon* Weapon = SpawnSeapon(WeaponClass);
		Inventory.AddUnique(Weapon);
	}

	// 现在把武器附加上去，算是临时装备了，之后会做更多事，比如设置当前武器变量。
	// 目前重点是展示并附着其中一把武器
	if (Inventory.Num() > 0)
	{
		Inventory[0]->AttachToOwningPawn();
	}

	/**
	* 服务器端：
	* 1、AShooterCharacter::PossessedBy() 函数只能在 Server / Standalone 上被调用
	* 2、服务器端调用 AShooterCharacter::PossessedBy() 函数
	* 3、在 PossessedBy() 函数，调用角色身上的战斗组件 UCombatComponent::SpawnInventory() 函数
	* 4、在 SpawnInventory() 函数，服务器端生成玩家角色并为角色绑定武器
	*/
	/**
	* 客户端：
	* 1、当玩家角色网络同步时，服务器端生成的武器也同步到客户端，就可以在客户端为角色绑定武器
	* 2、如果想让这个功能在多人模式下立即生效，可以在 Weapon 武器类里覆盖 OnRep_Instigator() 函数
	* 3、当Instigator被复制时，OnRep_Instigator() 函数在客户端就会被调用
	* 4、在 OnRep_Instigator() 函数里，调用 AttachToOwningPawn() 函数，为玩家角色绑定武器
	* 5、这样在多人模式下，武器就可以正确的绑定到客户端玩家角色身上
	*/
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

