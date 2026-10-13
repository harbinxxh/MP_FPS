// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShooterReticle.generated.h"

class UImage;

/**
 * 
 */
UCLASS()
class MP_FPS_API  UShooterReticle : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// 瞄准镜准星
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Reticle;

	// 弹药计数器
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_AmmoCounter;

private:

	// 这些动态材质实例将由武器类负责创建并返回，准星控件需要引用这些实例，因此将采用弱对象指针来引用它们
	// TWeakObjectPtr 指针不需要 UPROPERTY 宏
	TWeakObjectPtr<UMaterialInstanceDynamic> CurrentReticle_DynMatInst;
	TWeakObjectPtr<UMaterialInstanceDynamic> CurrentAmmoCounter_DynMatInst;

	// 当Controller操控的Pawn变更时触发的委托
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UFUNCTION()
	void OnWeaponFirstReplicated(AWeapon* Weapon);

	// 准星和弹药数量代理回调函数
	UFUNCTION()
	void OnReticleChanged(UMaterialInstanceDynamic* ReticleDynMatInst);

	UFUNCTION()
	void OnAmmoCounterChanged(UMaterialInstanceDynamic* AmmoCounterDynMatInst, int32 RoundsCurrent, int32 RoundsMax);
};
