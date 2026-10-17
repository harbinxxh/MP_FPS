// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ReserveAmmo.h"
#include "Weapon/Weapon.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Character/ShooterCharacter.h"
#include "Interfaces/PlayerInterface.h"
#include "Combat/CombatComponent.h"


void UReserveAmmo::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 设置渲染不透明度设为 0
	Image_WeaponIcon->SetRenderOpacity(0.f);
	Text_Ammo->SetRenderOpacity(0.f);


	// 当Controller控制的Pawn发生变更时，会调用该委托
	GetOwningPlayer()->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetPawn());
	if (!IsValid(ShooterCharacter)) return;

	OnPossessedPawnChanged(nullptr, ShooterCharacter);

	if (ShooterCharacter->HasWeaponFirstReplicated())
	{
		// Get Dynamic Material Instances from the Weapon.
		AWeapon* Weapon = IPlayerInterface::Execute_GetCurrentWeapon(ShooterCharacter);
		if (IsValid(Weapon))
		{
			// Reserve Ammo amount for the current weapon, CurrentWeapon->Ammo.
			// param:当前武器的备用弹药量, 当前武器的弹药量
			OnCurrentReserveAmmoChanged(IPlayerInterface::Execute_GetReserveAmmo(ShooterCharacter), Weapon->Ammo);
		}
	}
	else
	{
		ShooterCharacter->OnWeaponFirstReplicated.AddDynamic(this, &ThisClass::OnWeaponFirstReplicated);
	}

	// 服务器端
	if (ShooterCharacter->HasAuthority())
	{
		AWeapon* Weapon = IPlayerInterface::Execute_GetCurrentWeapon(ShooterCharacter);
		if (!IsValid(Weapon)) return;
		OnCurrentReserveAmmoChanged(IPlayerInterface::Execute_GetReserveAmmo(ShooterCharacter), Weapon->Ammo);
	}
}

void UReserveAmmo::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	UCombatComponent* OldPawnCombat = UCombatComponent::FindCombatComponent(OldPawn);
	if (IsValid(OldPawnCombat))
	{
		OldPawnCombat->OnCurrentReserveAmmoChanged.RemoveDynamic(this, &ThisClass::OnCurrentReserveAmmoChanged);
		OldPawnCombat->OnRoundFired.RemoveDynamic(this, &ThisClass::OnRoundFired);
	}

	// Bind to delegates on the New Pawn's Combat Component
	UCombatComponent* NewPawnCombat = UCombatComponent::FindCombatComponent(NewPawn);
	if (IsValid(NewPawnCombat))
	{
		// 控件的渲染不透明度设为 1.0
		Image_WeaponIcon->SetRenderOpacity(1.f);
		Text_Ammo->SetRenderOpacity(1.f);

		NewPawnCombat->OnCurrentReserveAmmoChanged.AddDynamic(this, &ThisClass::OnCurrentReserveAmmoChanged);
		NewPawnCombat->OnRoundFired.AddDynamic(this, &ThisClass::OnRoundFired);
	}
}

void UReserveAmmo::OnCurrentReserveAmmoChanged(int32 RoundsInReserve, int32 RoundsInWeapon)
{
	// TODO: Changed Weapon Icon

	// 如果，角色身上有36发备用弹药，枪里6发【RoundsInWeapon / RoundsInReserve - 显示 6 / 36】
	if (IsValid(Text_Ammo))
	{
		// 宏 NSLOCTEXT() 能让文本实现本地化，这样字符串就会经过本地化系统，自动完成翻译
		FText AmmoText = FText::Format(NSLOCTEXT("AmmoText", "AmmoKey", "{0}/{1}"), RoundsInWeapon, RoundsInReserve);
		Text_Ammo->SetText(AmmoText);
	}
}

void UReserveAmmo::OnRoundFired(int32 RoundsCurrent, int32 RoundsMax, int32 RoundsInReserve)
{
	if (IsValid(Text_Ammo))
	{
		FText AmmoText = FText::Format(NSLOCTEXT("AmmoText", "AmmoKey", "{0}/{1}"), RoundsCurrent, RoundsInReserve);
		Text_Ammo->SetText(AmmoText);
	}
}

void UReserveAmmo::OnWeaponFirstReplicated(AWeapon* Weapon, bool bIsTargetingPlayer)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetPawn());
	if (!IsValid(ShooterCharacter)) return;

	// param:当前武器的备用弹药量和当前武器的弹药量
	OnCurrentReserveAmmoChanged(IPlayerInterface::Execute_GetReserveAmmo(ShooterCharacter), Weapon->Ammo);
}