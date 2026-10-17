// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReserveAmmo.generated.h"

class UImage;
class UTextBlock;
class AWeapon;

/**
 * 显示武器的弹药数量控件
 */
UCLASS()
class MP_FPS_API UReserveAmmo : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeOnInitialized() override;

	// 武器图标
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_WeaponIcon;

	// 弹药数量
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Ammo;

private:

	// 玩家角色变更回调函数
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	// 当前备用弹药量和武器里的弹药量
	UFUNCTION()
	void OnCurrentReserveAmmoChanged(int32 RoundsInReserve, int32 RoundsInWeapon);

	// (武器开火时)当前弹药数量和最大弹药数量、武器备用弹药量
	UFUNCTION()
	void OnRoundFired(int32 RoundsCurrent, int32 RoundsMax, int32 RoundsInReserve);

	// 当客户端首次接收到武器副本时的回调函数
	UFUNCTION()
	void OnWeaponFirstReplicated(AWeapon* Weapon, bool bIsTargetingPlayer);
};
