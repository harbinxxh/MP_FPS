// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class UWeaponData;
class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MP_FPS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Cycle to the next weapon in the inventory
	void Initiate_CycleWeapon();		// 切换武器
	void Initiate_FireWeapon_Pressed(); // 连续开火按下
	void Initiate_FireWeapon_Released();// 连续开火释放
	void Initiate_ReloadWeapon();		// 弹药装填
	void Initiate_Aim_Pressed();		// 瞄准按下
	void Initiate_Aim_Released();		// 瞄准释放

	UPROPERTY(EditDefaultsOnly, Category = "FPS|Weapon")
	TObjectPtr<UWeaponData> WeaponData;

	// 生成物品栏函数
	// SpawnInventory() 函数只会在服务器上实例化武器
	void SpawnInventory();
	// 销毁物体栏函数
	void DestroyInventory();
protected:

private:
	// 武器类型
	// 后面会将武器类型变量改为数组-现在先实现武器生成功能
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Weapon")
	TSubclassOf<AWeapon> DefaultWeaponClass;

	// 武器生成函数：
	// 函数将生成一个单独的武器，并返回指向它的指针
	AWeapon* SpawnSeapon(TSubclassOf<AWeapon> WeaponClass) const;
};
