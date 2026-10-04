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
