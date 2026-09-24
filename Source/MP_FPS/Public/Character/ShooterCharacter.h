// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCombatComponent;
class UInputAction;

UCLASS()
class MP_FPS_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AShooterCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

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

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCombatComponent> Combat;

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
