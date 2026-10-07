// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


// 使用命名空间里就不会用 ECC_Weapon 把公共命名空间搞乱了
namespace FPSTraceChannels
{
	// constexpr 实现编译时常量定义
	constexpr ECollisionChannel ECC_Weapon = ECC_GameTraceChannel1;
}