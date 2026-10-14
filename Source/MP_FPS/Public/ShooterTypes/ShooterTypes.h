// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ShooterTypes.generated.h"

/**
 * 旋转状态枚举
 */
UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	Left UMETA(DisplayName = "TuringLeft"),
	Right UMETA(DisplayName = "TuringRight"),
	NotTurning UMETA(DisplayName = "NotTurning")
};

/**
 * 通过动态材质实例参数，实时调整准星参数
 * 每个武器都有自己对应的动态材质实例，参数各不相同
 */
USTRUCT(BlueprintType)
struct FReticleParams
{
	GENERATED_BODY()

	// 仅在开火时变化，开火后立刻恢复
	// 形状切割厚度，会改变准星的粗细
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ShapeCutFactor_RoundFired = 0.f; // 形状切割厚度，会改变准星的粗细

	// 形状缩放系数，既圆角缩放
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ScaleFactor_RoundFired = 0.f;

	// 开火时的插值速度，通过设置插值速度来控制这个过程
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RoundFiredInterpSpeed = 20.f;
};