// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "CombatComponent.generated.h"

class UWeaponData;
class AWeapon;

// 在第一次装备武器或切换武器时，广播材质和准星参数，瞄准玩家状态（瞄准玩家时的状态、未瞄准玩家时的状态）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReticleChanged, UMaterialInstanceDynamic*, ReticleDynMatInst, const FReticleParams&, ReticleParams, bool, bCurrentlyTargetingPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAmmoCounterChanged, UMaterialInstanceDynamic*, AmmoCounterDynMatInst, int32, RoundsCurrent, int32, RoundsMax);
// (武器开火时) 广播当前弹药量和最大弹药量、武器备用弹药量
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRoundFired, int32, RoundsCurrent, int32, RoundsMax, int32, RoundsInReserve);
// (武器瞄准时) 广播瞄准状态
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAimingStatusChanged, bool, IsAiming);
// (武器瞄准时) 广播是否瞄准玩家的状态
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetingPlayerStatusChanged, bool, IsAiming);
// (武器备用弹药量变更时) 广播当前武器备用弹药量和武器的弹药量、武器图标材质
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCurrentReserveAmmoChanged, int32, RoundsInReserve, int32, RoundsInWeapon, UMaterialInterface*, WeaponIconMaterial);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MP_FPS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// 返回战斗组件
	// BlueprintPure: C++和蓝图都可以调用
	UFUNCTION(BlueprintPure, Category = "FPS|Comba")
	static UCombatComponent* FindCombatComponent(const AActor* Actor) { return ( IsValid(Actor) ? Actor->FindComponentByClass<UCombatComponent>() : nullptr); }

	// Cycle to the next weapon in the inventory
	void Initiate_CycleWeapon();		// 切换武器
	void Initiate_FireWeapon_Pressed(); // 连续开火按下
	void Initiate_FireWeapon_Released();// 连续开火释放
	void Initiate_ReloadWeapon();		// 装填弹药
	void Initiate_Aim_Pressed();		// 瞄准按下
	void Initiate_Aim_Released();		// 瞄准释放

	// 动画蓝图中调用的动画通知函数：武器切换响应动画通知函数，实际进行武器切换的位置
	void Notify_CycleWeapon();
	// 动画蓝图中调用的动画通知函数：武器装填响应动画通知函数
	void Notify_ReloadWeapon();

	// 广播准星材质
	UPROPERTY(BlueprintAssignable)
	FReticleChanged OnReticleChanged;

	// 广播弹药数量材质
	UPROPERTY(BlueprintAssignable)
	FAmmoCounterChanged OnAmmoCounterChanged;

	// (武器开火时)广播当前弹药数量和最大弹药数量、武器备用弹药量
	UPROPERTY(BlueprintAssignable)
	FRoundFired OnRoundFired;

	// 广播瞄准状态
	UPROPERTY(BlueprintAssignable)
	FAimingStatusChanged OnAimingStatusChanged;

	// 广播是否瞄准玩家的状态
	UPROPERTY(BlueprintAssignable)
	FTargetingPlayerStatusChanged OnTargetingPlayerStatusChanged;

	// (武器备用弹药量变更时) 广播当前武器备用弹药量和武器的弹药量、武器材质图标
	UPROPERTY(BlueprintAssignable)
	FCurrentReserveAmmoChanged OnCurrentReserveAmmoChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|Weapon")
	TObjectPtr<UWeaponData> WeaponData;

	// 装备武器函数，只能在服务器上执行
	void Equip(AWeapon* Weapon);
	void EquipWeapon(AWeapon* Weapon);
	// 装备武器的服务器RPC
	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(AWeapon* Weapon);

	// 生成物品栏函数
	// SpawnInventory() 函数只会在服务器上实例化武器
	void SpawnInventory();
	// 销毁物体栏函数
	void DestroyInventory();
	
	/**
	 * 服务器端： 
	 * 如果在服务器端修改了bAiming值，会复制到所有其他客户端，拥有者客户端除外
	 * 拥有者本地客户端自己会设置并触发瞄准按下动作
	 * 
	 * 客户端：
	 * 如果在本地客户端修改了bAiming值，服务器根本不会知道本地客户端做了什么，
	 * 必须主动通知服务器端，因此，需要服务器RPC
	 */
	// 瞄准变量：将其设为蓝图只读且已复制，动画蓝图可以直接使用它
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bAiming;

	// 因为我们得能在各种类里查当前武器的类型，比如动画蓝图
	// 如果动画蓝图知道武器类型，它就能知道待机时该用哪些姿态，等等
	UPROPERTY(Transient, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentWeapon)
	TObjectPtr<AWeapon> CurrentWeapon;

	// 初始化武器控件
	void InitializeWeaponWidgets() const;

	// 当前备用弹药量
	UPROPERTY(ReplicatedUsing = OnRep_CurrentReserveAmmo)
	int32 CurrentReserveAmmo;

	// 判断当前帧是否瞄准了玩家
	bool bHitPlayer;

protected:
	// 射线长度
	UPROPERTY(EditDefaultsOnly, Category = "FPS|Weapon")
	float TraceLength;

	// 武器切换：动画蒙太奇淡出时的通知函数
	UFUNCTION()
	void BlendOut_CycleWeapon(UAnimMontage* Montage, bool bInterrupted);

private:
	/**
	 * 所有武器被备用弹药量： 
	 * 必须为所有武器类型维护备用弹药，只在服务器上更新它，这是由服务器掌控的权威数据
	 * 一旦它发生变化，根据当前装备的武器类型，更新当前备用弹药数量
	 * 不需要同步数据，只是把这部分数据保留在服务器上
	 */
	TMap<FGameplayTag, int32> ReserveAmmo;
	
	// 判断上一帧是否瞄准了玩家
	bool bHitPlayerLastFrame;
	// 是否一直在触发枪的扳机
	bool bTriggerPressed;
	// 设置多长的持续时间，就是全自动射击时武器的发射间隔是多长
	FTimerHandle FireTimer;

	void FireTimerFinished();

	/**
	* 复制通知函数：当武器变量被修改时，就触发 RepNotify 复制通知函数
	* 在C++中，如果你在服务器上修改复制变量，那么只会在客户端触发 RepNotify 复制通知函数
	* 能接收变量类型参数，这样就能得到该变量的前值，也就是复制前的值，有时候需要这个信息时特别实用
	* 必须是 UFUNCTION() 标识，因为必须通过反射系统暴露，才能与对应的复制变量绑定
	*/
	UFUNCTION()
	void OnRep_CurrentWeapon(AWeapon* LastWeapon);
	// 装备新武器，解绑旧武器
	void SetCurrentWeapon(AWeapon* NewWeapon, AWeapon* LastWeapon);

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

	/**
	 * 服务器RPC : 在客户端上调用函数，并在服务器上执行
	 * 客户端修改了bAiming值，需要通知服务器端，就需要服务器RPC
	 * 用同一个函数开启瞄准状态，也同时能取消瞄准
	 */
	UFUNCTION(Server, Reliable)
	void Server_Aim(bool bPressed);

	// 通知服务器，已执行开枪操作
	UFUNCTION(Server, Reliable)
	void Server_FireWeapon(const FHitResult& Hit);

	// 服务器调用多播代理时，客户端每台机器都会执行多播 RPC
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_FileWeapon(const FHitResult& Hit, int32 AuthAmmo);

	// 当前备用弹药量回调函数
	UFUNCTION()
	void OnRep_CurrentReserveAmmo();

	// 用来处理瞄准 bAiming 变量
	void Local_Aim(bool bPressed);

	// 处理本地武器射击效果
	void Local_FireWeapon();

	// 递增武器索引并返回新值
	int32 AdvanceWeaponIndex();
	// 武器数组中的索引值,每次切换武器时，都会更新这个索引
	int32 Local_WeaponIndex;

	// 本地触发武器切换函数，用来播放动画片段
	void Local_CycleWeapon(int32 WeaponIndex);

	// 服务器端切换武器RPC
	UFUNCTION(Server, Reliable)
	void Server_CycleWeapon(int32 WeaponIndex);

	// 网络多播，服务器调用客户端执行
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_CycleWeapon(int32 WeaponIndex);

	// 本地装填武器的函数，用来播放动画片段
	void Local_ReloadWeapon();

	// 服务器端装填/换弹RPC
	UFUNCTION(Server, Reliable)
	void Server_ReloadWeapon();

	// 客户端装填/换弹RPC
	UFUNCTION(Client, Reliable)
	void Client_ReloadWeapon(int32 NewWeaponAmmo, int32 NewCarriedAmmo);

	// 网络多播，服务器调用客户端执行
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ReloadWeapo();
};
