// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerInterface.h"
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

protected:
	// BlueprintReadOnly 默认不能作用于 private 私有成员内
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS|Combat")
	TObjectPtr<UCombatComponent> Combat;

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

	// 1st person View (arms) 用于第一人称视角的手臂
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FirstPersonCamera;

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
