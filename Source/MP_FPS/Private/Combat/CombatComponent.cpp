// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/CombatComponent.h"
#include "Engine/Engine.h"
#include "Weapon/Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimMontage.h"
#include "Data/WeaponData.h"
#include "Interfaces/PlayerInterface.h"


// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	TraceLength = 20000;
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, Inventory);
	DOREPLIFETIME(UCombatComponent, CurrentWeapon);
	/**
	 * COND_SkipOwner:此属性会发送给除拥有者之外的所有连接
	 * 跳过拥有者，因为在本地通过右键鼠标或手柄左扳机来设置瞄准，所以不需要把更新发回自己
	 * 所以本地设置瞄准，就不用让服务器再把数据推回来，它是复制变量，让其他玩家都能看见我们瞄准，但不需要同步回我们自己
	 */
	DOREPLIFETIME_CONDITION(UCombatComponent, bAiming, COND_SkipOwner);
}

void UCombatComponent::Initiate_CycleWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Initiate_CycleWeapon"), false);
}

void UCombatComponent::Initiate_FireWeapon_Pressed()
{
	 Local_FireWeapon();
}

void UCombatComponent::Local_FireWeapon()
{
	// 先判断:如果当前武器无效，直接返回
	if (!IsValid(CurrentWeapon)) return;
	ensure(IsValid(WeaponData));

	// 获取第一人称开火蒙太奇 - play the fire weapon montage for the first-person meshs
	UAnimMontage* Montage1P = WeaponData->FirstPersonMontages.FindChecked(CurrentWeapon->WeaponType).FireMontage;

	// 需要获取拥有者的第一人称网格
	USkeletalMeshComponent* Mesh1P = IPlayerInterface::Execute_GetMesh1P(GetOwner());
	
	if (IsValid(Montage1P) && IsValid(Mesh1P))
	{
		// 播放蒙太奇
		Mesh1P->GetAnimInstance()->Montage_Play(Montage1P);
	}

	// 获取命中结果
	FHitResult Hit;
	CurrentWeapon->WeaponTrace(Hit, TraceLength);

	// 获取表面类型
	EPhysicalSurface ImpactSurfaceType = Hit.PhysMaterial.IsValid(false) ? Hit.PhysMaterial->SurfaceType.GetValue() : SurfaceType1;
	CurrentWeapon->Local_Fire(Hit.ImpactPoint, Hit.ImpactNormal, ImpactSurfaceType, true);

	// 通知服务器，已执行开枪操作
	 Server_FireWeapon(Hit);
}

void UCombatComponent::Server_FireWeapon_Implementation(const FHitResult& Hit)
{
	Multicast_FileWeapon(Hit);
}

void UCombatComponent::Multicast_FileWeapon_Implementation(const FHitResult& Hit)
{
	APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (OwningPawn->IsLocallyControlled())
	{
		// do locally-controlled stuff.
		// 最终我们发射时要在本地处理多播相关的逻辑，这涉及让武器执行具体操作
	}
	else
	{
		ensure(IsValid(WeaponData));

		// 获取表面类型
		EPhysicalSurface ImpactSurfaceType = Hit.PhysMaterial.IsValid(false) ? Hit.PhysMaterial->SurfaceType.GetValue() : SurfaceType1;
		CurrentWeapon->Local_Fire(Hit.ImpactPoint, Hit.ImpactNormal, ImpactSurfaceType, true);

		UAnimMontage* Montage3P = WeaponData->ThirdPersonMontages.FindChecked(CurrentWeapon->WeaponType).FireMontage;
		USkeletalMeshComponent* Mesh3P = IPlayerInterface::Execute_GetMesh3P(GetOwner());
		if (IsValid(Montage3P) && IsValid(Mesh3P))
		{
			// 播放第三人称开火蒙太奇
			Mesh3P->GetAnimInstance()->Montage_Play(Montage3P);
		}
	}
}

void UCombatComponent::Initiate_FireWeapon_Released()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Initiate_FireWeapon_Released"), false);
}

void UCombatComponent::Initiate_ReloadWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Initiate_ReloadWeapon"), false);
}

// 瞄准按下
void UCombatComponent::Initiate_Aim_Pressed()
{
	Local_Aim(true);
	Server_Aim(true);// 向服务器发送瞄准值
}

// 瞄准释放
void UCombatComponent::Initiate_Aim_Released()
{
	Local_Aim(false);
	Server_Aim(false);// 向服务器发送瞄准值
}

// 服务器RPC : 在客户端上调用函数，并在服务器上执行
void UCombatComponent::Server_Aim_Implementation(bool bPressed)
{
	Local_Aim(bPressed);
}

void UCombatComponent::Local_Aim(bool bPressed)
{
	bAiming = bPressed;
}

// 装备武器函数，只能在服务器上执行
void UCombatComponent::Equip(AWeapon* Weapon)
{
	// 这会改变CurrentWeapon复制变量，让它同步复制，并让它的onRep函数在客户端自动触发
	CurrentWeapon = Weapon;
	// 挂载武器
	CurrentWeapon->AttachToOwningPawn();
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
		Equip(Inventory[0]);
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
	// Destroy the inventory once we have one.
	for (AWeapon* Weapon : Inventory)
	{
		if (IsValid(Weapon))
		{
			Weapon->Destroy();
		}
	}
}

void UCombatComponent::OnRep_CurrentWeapon(AWeapon* LastWeapon)
{
	if (!IsValid(CurrentWeapon)) return;
	CurrentWeapon->AttachToOwningPawn();

	/**
	* 还需要在Weapon类中的OnRep_Instigator()函数里完成这个操作
	* 因为，当我们生成武器并设定当前武器时,有时候多个对象的复制顺序是不可预测的
	* 所以，我们得确保在有有效实例者时执行这个操作
	*/
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