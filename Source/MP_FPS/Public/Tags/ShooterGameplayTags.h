

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

/**
 * 需要在 Build.cs 中添加 "GameplayTags" 模块
 */
namespace ShooterTags
{
	// 声明一个在C++中使用UE_DEFINE_GAMEPLAY_TAG定义的原生游戏玩法标签，以便其他模块或代码可以使用该创建的标签变量。
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WeaponType_None);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WeaponType_Rifle);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_WeaponType_Pistol);
}