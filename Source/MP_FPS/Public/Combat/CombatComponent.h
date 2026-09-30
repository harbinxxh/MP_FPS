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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// Cycle to the next weapon in the inventory
	void Initiate_CycleWeapon();		// 切换武器
	void Initiate_FireWeapon_Pressed(); // 连续开火按下
	void Initiate_FireWeapon_Released();// 连续开火释放
	void Initiate_ReloadWeapon();		// 弹药装填
	void Initiate_Aim_Pressed();		// 瞄准按下
	void Initiate_Aim_Released();		// 瞄准释放

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|Weapon")
	TObjectPtr<UWeaponData> WeaponData;

	// 装备武器函数，只能在服务器上执行
	void Equip(AWeapon* Weapon);

	// 生成物品栏函数
	// SpawnInventory() 函数只会在服务器上实例化武器
	void SpawnInventory();
	// 销毁物体栏函数
	void DestroyInventory();
protected:
	// 因为我们得能在各种类里查当前武器的类型，比如动画蓝图
	// 如果动画蓝图知道武器类型，它就能知道待机时该用哪些姿态，等等
	UPROPERTY(Transient, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentWeapon)
	TObjectPtr<AWeapon> CurrentWeapon;
private:
	/**
	* 复制通知函数：在C++中，如果你在服务器上修改复制变量，那么只会在客户端触发 RepNotify 复制通知函数
	* 复制通知函数：能接收变量类型参数，这样就能得到该变量的前值，也就是复制前的值，有时候需要这个信息时特别实用
	* 复制通知函数：必须是 UFUNCTION() 标识，因为必须通过反射系统暴露，才能与对应的复制变量绑定
	*/
	UFUNCTION()
	void OnRep_CurrentWeapon(AWeapon* LastWeapon);

	// 保存生成的武器指针数组
	// 网络复制数组：Transient 标记为瞬态，不能保存到硬盘上
	UPROPERTY(Transient, Replicated)
	TArray<AWeapon*> Inventory;

	// 武器类型数组：比如 手枪类、步枪类等...
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Weapon")
	TArray<TSubclassOf<AWeapon>> DefaultWeaponClasses;

	// 武器生成函数：
	// 函数将生成一个单独的武器，并返回指向它的指针
	AWeapon* SpawnSeapon(TSubclassOf<AWeapon> WeaponClass) const;
};
