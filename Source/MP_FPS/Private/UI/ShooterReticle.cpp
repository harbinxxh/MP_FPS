// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShooterReticle.h"
#include "Character/ShooterCharacter.h"


void UShooterReticle::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 当被控制的棋子发生变更时，会调用该委托
	GetOwningPlayer()->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetPawn());
	if (!IsValid(ShooterCharacter)) return;

	OnPossessedPawnChanged(nullptr, ShooterCharacter);

	if (ShooterCharacter->HasWeaponFirstReplicated())
	{
		// Get Dynamic Material Instances from the Weapon.
	}
	else
	{
		ShooterCharacter->OnWeaponFirstReplicated.AddDynamic(this, &ThisClass::OnWeaponFirstReplicated);
	}
}

void UShooterReticle::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
}

void UShooterReticle::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// 这些事件应该影响瞄准器的外观效果
	// Unbind from delegates on the oldPawn's Combat Component
	// Bind to delegates on the New Pawn's Combat Component
}

void UShooterReticle::OnWeaponFirstReplicated(AWeapon* Weapon)
{
	// Get Dynamic Material Instances from the Weapon.
}
