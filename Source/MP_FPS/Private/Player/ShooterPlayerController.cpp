// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ShooterPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


AShooterPlayerController::AShooterPlayerController()
{
	bReplicates = true;
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (IsValid(Subsystem))
	{
		Subsystem->AddMappingContext(ShooterIMC, 0);
	}
}

void AShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up action bindings
	UEnhancedInputComponent* ShooterInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	ShooterInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Input_Mover);
	ShooterInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
	ShooterInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ThisClass::Input_Crouch);
	ShooterInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Input_Jump);
}

void AShooterPlayerController::Input_Crouch()
{
	// 检测玩家角色是否有效
	if (!IsValid(GetCharacter())) return;
	
	// 玩家角色有效，在从玩家角色中获取移动组件
	if (UCharacterMovementComponent* CMC = GetCharacter()->GetCharacterMovement(); IsValid(CMC))
	{
		// 下蹲切换功能：按一次下蹲，在按一次取消下蹲
		CMC->bWantsToCrouch = !CMC->bWantsToCrouch;
	}
}

void AShooterPlayerController::Input_Jump()
{
	if (!IsValid(GetCharacter())) return;
	UCharacterMovementComponent* CMC = GetCharacter()->GetCharacterMovement();
	if (!IsValid(CMC)) return;

	// 判断是否已经下蹲，如果是就不该跳跃
	if (CMC->bWantsToCrouch)
	{
		CMC->bWantsToCrouch = false;
	}
	else
	{
		GetCharacter()->Jump();
	}
}

void AShooterPlayerController::Input_Mover(const FInputActionValue& InputActionValue)
{
	// 移动的方向，得根据鼠标旋转和控制旋转来定
	// input is a Vector2D
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	
	// 获取控制旋转
	// 只保留偏航旋转值，并设置俯仰和翻滚值归零
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	// 通过旋转矩阵
	// 获取偏航旋转的前向向量
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	// 获取偏航旋转的右向向量
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// 获取被控制的Pawn
	if (APawn* ControlledPawn = GetPawn())
	{
		// W和S映射到了Y轴
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		// A和D映射到了X轴
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AShooterPlayerController::Input_Look(const FInputActionValue& InputActionValue)
{
	// input is a Vector2D
	const FVector2D InputActionVector = InputActionValue.Get<FVector2D>();

	// 用输入轴向量的X分量添加偏航-左右转头既左右旋转
	AddYawInput(InputActionVector.X);

	// 用输入轴向量的丫分量添加俯仰输入-仰头/低头既上下旋转
	AddPitchInput(InputActionVector.Y);
}
