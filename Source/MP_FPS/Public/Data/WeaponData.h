

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WeaponData.generated.h"

/**
 * 
 */
UCLASS()
class MP_FPS_API UWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:
	// 这个就是与每种特定武器对应的握把点插槽
	// 要实现多个握把点，并且让这个结构可以动态扩展
	// 当我们新增更多武器类型的时候，可以用数组，也可以用 TAMP

	// 键就是游戏玩法标签
	// 值就是握把点，也就是个FName
	UPROPERTY(EditDefaultsOnly, Category = "FPS|WeaponData|Weapons")
	TMap<FGameplayTag, FName> GripPoints;

};
