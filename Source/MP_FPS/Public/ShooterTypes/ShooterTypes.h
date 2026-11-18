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
 * 仅在开火时变化，开火后立刻恢复
 */
USTRUCT(BlueprintType)
struct FReticleParams
{
	GENERATED_BODY()

	// 形状切割厚度因子，会改变准星的粗细
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ShapeCutFactor_RoundFired = 0.f; // 形状切割厚度，会改变准星的粗细

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ShapeCutFactor_Aiming = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ShapeCutFactor_NotAiming = 0.f;


	// 形状缩放系数，既圆角缩放
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ScaleFactor_RoundFired = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ScaleFactor_Aiming = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ScaleFactor_NotAiming = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere) // 瞄准时玩家缩放系数
	float ScaleFactor_Targeting = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere) // 未瞄准时玩家缩放系数
	float ScaleFactor_NotTargeting = 0.f;


	// 插值速度：开火时通过设置插值速度来控制这个过程
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RoundFiredInterpSpeed = 20.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AimingInterpSpeed = 15.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TargetingPlayerInterpSpeed = 10.f; // 瞄准玩家时的插值速度
};

/**
 * 特殊枚举:用来存储多种可能的特殊击杀类型，如果枚举类型不是 uint8，那就不能当蓝图类型用
 * 
 * BitFlags 是什么意思? 可以用枚举的 16 位，能随时翻转这些位的状态，随时都能检查这些位是否被置位
 * 
 * 使用 ENUM_CLASS_FLAGS(ESpecialElimType) 宏才能生效
 */
// 特殊击杀类型枚举
UENUM(meta = (BitFlags))
enum class ESpecialElimType : uint16
{
	None			= 0,
	Headshot		= 1 << 0,		// 00000000 00000001 - 爆头击杀
	Sequential		= 1 << 1,		// 00000000 00000010 - 连续击杀 ：短时间内多次击杀，如三杀、四杀、五杀等
	Streak			= 1 << 2,		// 00000000 00000100 - 连击击杀
	Revenge			= 1 << 3,		// 00000000 00001000 - 复仇击杀
	Dethrone		= 1 << 4,		// 00000000 00010000 - 击败榜首击杀
	Showstopper		= 1 << 5,		// 00000000 00100000 - 终结连杀
	FirstBlood		= 1 << 6,		// 00000000 01000000 - 首杀
	GainedTheLead	= 1 << 7,		// 00000000 10000000 - 赢得领先
	TiedTheLeader	= 1 << 8,		// 00000001 00000000 - 追平领先
	LostTheLead		= 1 << 9		// 00000010 00000000 - 失去领先者
};

ENUM_CLASS_FLAGS(ESpecialElimType)





