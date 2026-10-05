// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerInterface.h"
#include "ShooterTypes/ShooterTypes.h"
#include "ShooterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCombatComponent;
class UInputAction;

UCLASS()
class MP_FPS_API AShooterCharacter : public ACharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AShooterCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/**
	* PossessedBy() 函数只能在 Server / Standalone 上被调用
	* 当某个 Controller 决定控制这个 Pawn 时，引擎在 AController::OnPossess 里主动调用它
	*/
	virtual void PossessedBy(AController* NewController) override;

	/** PlayerInterface */
	virtual FName GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) const override;
	virtual USkeletalMeshComponent* GetMesh1P_Implementation() const;
	virtual USkeletalMeshComponent* GetMesh3P_Implementation() const;
	/** ~PlayerInterface */

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	// 用来修改瞄准旋转值
	UFUNCTION(BlueprintCallable)
	FRotator GetFixedAimRotation() const;

	// 插槽变换变量，用来存储变换值，并能在蓝图中访问此变量变量
	UPROPERTY(BlueprintReadOnly, Category = "FPS|FABRIK")
	FTransform FABRIK_SocketTransform;

	// 判断当前玩家是否有武器
	UFUNCTION(BlueprintCallable)
	bool HasCurrentWeapon() const;

protected:
	// BlueprintReadOnly 默认不能作用于 private 私有成员内
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS|Combat")
	TObjectPtr<UCombatComponent> Combat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS|Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	// 玩家角色默认视野值
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|Aiming")
	float DefaultFieldOfView;

	// BlueprintImplementableEvent：在蓝图中实现，只在本地使用，完全不用在意在蓝图里实现会有什么性能开销
	// 将在瞄准时被触发，用来判断当前是进入瞄准状态还是退出瞄准状态
	UFUNCTION(BlueprintImplementableEvent)
	void OnAim(bool bIsAiming);

	// 瞄准航向旋转差值的水平分量
	UPROPERTY(BlueprintReadOnly, Category = "FPS|TurnInPlace")
	float AO_Yaw;

	// 移动偏移航向角差值,是用来做横向移动的
	UPROPERTY(BlueprintReadOnly, Category = "FPS|Strafing")
	float MovementOffsetYaw;

	// 旋转状态
	UPROPERTY(BlueprintReadOnly, Category = "FPS|TurnInPlace")
	ETurningInPlace TurningStatus;

private:

	// 切换武器回调函数
	void Input_CycleWeapon();
	// 弹药装填回调函数
	void Input_ReloadWeapon();
	// 连续开火回调函数
	void Input_FireWeapon_Pressed();
	void Input_FireWeapon_Released();
	// 武器瞄准回调函数
	void Input_AimWeapon_Pressed();
	void Input_AimWeapon_Released();

	// 计算将位置/旋转从世界空间转换为骨骼相对空间
	void CalculateFABRIKSocketTransform();

	// 计算原地转向参数函数：用来根据瞄准方向计算一些关键参数
	void CalculateTurnInPlaceParameters(float DeltaTime);

	// 原地旋转函数
	void TurnInPlace(float DeltaTime);

	// 起始瞄准旋转
	FRotator StartingAimRotation;
	// 插值变量：专门负责插值处理
	float InterpA0_Yaw;

	// 1st person View (arms) 用于第一人称视角的手臂
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	// 切换武器Action
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> CycleWeaponAction;

	// 连续开火按下Action
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> FireWeaponAction;
	
	// 弹药装填Action
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> ReloadWeaponAction;
	
	// 武器瞄准Action
	UPROPERTY(EditAnywhere, Category = "FPS|Input")
	TObjectPtr<UInputAction> AimWeaponAction;
};
