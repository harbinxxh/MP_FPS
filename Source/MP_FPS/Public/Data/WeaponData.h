

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WeaponData.generated.h"

class UAnimSequence;
class UBlendSpace;

/**
 * 动画数据结构体
 * 游戏里的武器可能有不同的骨骼网格，每种武器类型需要不同的动画,这样就可以为每种网格配置相应的武器数据
 *
 * 设计一个存储动画的结构体
 * 根据武器类型的游戏标签，就能加载对应的动画，这样就能为第一人称、第三人称网格提供各种动画、如空闲、瞄准、蹲伏等
 *
 * PlayerAnims结构体：
 * 专门用来管理动画，可以把它看作是针对某一特定网格的动画集合
 * 如果我们要处理两种武器，比如手枪和步枪，我们会为手枪动画准备一个完整的结构体，再为步枪动画准备一个完整的结构体
 */
USTRUCT(BlueprintType)
struct FPlayerAnims
{
	GENERATED_BODY()

	/**
	 * 动画序列
	 */
	// 空闲动画
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimSequence> IdleAnim = nullptr;

	// 瞄准空闲动画
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimSequence> AimIdleAnim = nullptr;

	// 蹲伏空闲动画
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimSequence> CrouchIdleAnim = nullptr;

	// 冲刺动画
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimSequence> SprintAnim = nullptr;

	/**
	 * 混合空间：
	 * 让我们移动鼠标时手臂能自动跟随，无论是瞄准上方还是下方
	 * 用来做瞄准偏移的
	 */
	// 表示未瞄准时的状态
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBlendSpace> AimOffset_Hip;

	// 瞄准偏移
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBlendSpace> AimOffset_Aim;

	/**
	 * 侧移混合空间：
	 * 第一人称时，我们只看到自己的手臂，我们用不到这些
	 * 第三人称模型就能用上，在场景里看到的其他玩家，他们可能在侧移，这些玩家我们会为他们配置一个混合空间
	 */

	// 站立时的侧移混合空间：站立侧移
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBlendSpace> Strafe_Standing;

	// 蹲伏时的侧移混合空间：蹲伏侧移
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBlendSpace> Strafe_Crouching;

	// 蒙太奇：
	// 等到了要播放换弹、切枪、射击这类蒙太奇时在添加
};


// 蒙太奇数据结构体: 包括装备、换弹和射击三类动画蒙太奇
USTRUCT(BlueprintType)
struct FMontageData
{
	GENERATED_BODY()

	// 装备蒙太奇
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> EquipMontage = nullptr;

	// 换弹蒙太奇
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ReloadMontage = nullptr;

	// 射击蒙太奇
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> FireMontage = nullptr;
};


/**
 * 
 */
UCLASS()
class MP_FPS_API UWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:

   /**
	* 握把点插槽:
	* 这个就是与每种特定武器对应的握把点插槽
	* 要实现多个握把点，并且让这个结构可以动态扩展
	* 当我们新增更多武器类型的时候，可以用数组，也可以用 TAMP
	* 
	* 键就是游戏玩法标签
	* 值就是握把点，也就是个FName
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|WeaponData|Weapons")
	TMap<FGameplayTag, FName> GripPoints;

	// 武器蒙太奇
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|WeaponData|Weapons")
	TMap<FGameplayTag, FMontageData> WeaponMontages;

	// 第一人称动画
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|WeaponData|FirstPerson")
	TMap<FGameplayTag, FPlayerAnims> FirstPersonAnims;

	// 第三人称动画
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|WeaponData|ThirdPerson")
	TMap<FGameplayTag, FPlayerAnims> ThirdPersonAnims;

	// 第一人称蒙太奇
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|WeaponData|FirstPerson")
	TMap<FGameplayTag, FMontageData> FirstPersonMontages;

	// 第三人称蒙太奇
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|WeaponData|ThirdPerson")
	TMap<FGameplayTag, FMontageData> ThirdPersonMontages;
};
