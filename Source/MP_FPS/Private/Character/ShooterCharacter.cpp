// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Combat/CombatComponent.h"
#include "EnhancedInputComponent.h"

AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 开启Pawn下蹲功能:
	// 直接改组件的类默认值，蓝图子类若未显式覆盖就会继承
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		// 组件构造函数里改 NavAgentProps.bCanCrouch（这才是 CharacterMovement 自己的权威字段，蓝图上的 Can Crouch 勾选框绑定的就是它）
		MoveComp->NavAgentProps.bCanCrouch = true; // 权威字段
		// 只改 MovementState.bCanCrouch 属于"改了缓存副本"，一旦 NavAgentProps 重新同步过来就没了
		MoveComp->MovementState.bCanCrouch = true; // 同步缓存
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 0.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 15.f;
	SpringArm->bUsePawnControlRotation = true; // 摄像机跟随控制器移动

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>("FirstPersonCamera");
	FirstPersonCamera->SetupAttachment(SpringArm);
	// 让摄像机随弹簧臂移动，但不希望它随控制器的旋转而联动，也不希望它随控制器一同旋转，这正是弹簧臂的任务
	FirstPersonCamera->bUsePawnControlRotation = false;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh1P");
	Mesh1P->SetupAttachment(FirstPersonCamera);
	// 只有控制者才能看到这个网格，要确保只有当你操控角色时，它才会显示
	Mesh1P->bOnlyOwnerSee = true;
	// 这样一来其他玩家就看不到手臂部分了，只让我们自己看见
	Mesh1P->bOwnerNoSee = false;
	// 取消阴影投射
	Mesh1P->bCastDynamicShadow = false;
	// 不希望贴花显示在手臂模型上
	Mesh1P->bReceivesDecals = false;
	// 仅在渲染时更新姿态，当在渲染手臂模型时，只对当前操控该角色的玩家生效
	// 接着我们勾选手臂网格中的动画骨骼，所以如果玩家处于空闲、重新装填或开火状态，只有真正能看见的人，才会看到这些手臂动画，其他人则不会触发动画，这是一种优化。
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	// 将Mesh1P的更新组设为 TG_Prefysics，这样就能确保该网格的所有更新都在物理阶段之前完成
	// 这样能让网格更流畅地跟随摄像机，在物理计算前就更新位置、变换和全部动作
	// 这样当物理系统施加任何作用力时，比如角色被抛起、或者角色正在下落，网格的tick组就会在物理阶段之前执行，动作也会更同步于摄像机，从而避免第一人称手臂产生抖动和卡顿。
	// 所以为了流畅体验，我们把它tick组设为pre_physics，这样就搞定我们的手臂网格了。
	// 我们确保只有操控角色的玩家能看到它
	Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;

	// 角色自带的网格，通过 GetMesh() 获取
	// 自己看不见它，但别人能看见它,现在我们还要关掉这个网格的贴花，作为性能优化
	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;

	Combat = CreateDefaultSubobject<UCombatComponent>("Combat");
	Combat->SetIsReplicated(true);// 开启组件复制功能
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* ShooterInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	ShooterInputComponent->BindAction(CycleWeaponAction, ETriggerEvent::Started, this, &ThisClass::Input_CycleWeapon);
	ShooterInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Started, this, &ThisClass::Input_FireWeapon_Pressed);
	ShooterInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Completed, this, &ThisClass::Input_FireWeapon_Released);
	ShooterInputComponent->BindAction(AimWeaponAction, ETriggerEvent::Started, this, &ThisClass::Input_AimWeapon_Pressed);
	ShooterInputComponent->BindAction(AimWeaponAction, ETriggerEvent::Completed, this, &ThisClass::Input_AimWeapon_Released);
	ShooterInputComponent->BindAction(ReloadWeaponAction, ETriggerEvent::Started, this, &ThisClass::Input_ReloadWeapon);
}

void AShooterCharacter::Input_CycleWeapon()
{
	Combat->Initiate_CycleWeapon();
}

void AShooterCharacter::Input_ReloadWeapon()
{
	Combat->Initiate_ReloadWeapon();
}

void AShooterCharacter::Input_FireWeapon_Pressed()
{
	Combat->Initiate_FireWeapon_Pressed();
}

void AShooterCharacter::Input_FireWeapon_Released()
{
	Combat->Initiate_FireWeapon_Released();
}

void AShooterCharacter::Input_AimWeapon_Pressed()
{
	Combat->Initiate_Aim_Pressed();
}

void AShooterCharacter::Input_AimWeapon_Released()
{
	Combat->Initiate_Aim_Released();
}
