


#include "Tags/ShooterGameplayTags.h"

/**
 * 通过设置武器类型变量，可以看到，在射击类游戏标签中定义的新武器类型
 * 但不会在编辑器里显示为，在声明处写的那些名称，如 TAG_WeaponType_None、TAG_WeaponType_Rifle、TAG_WeaponType_Pistol
 * 而是会显示为宏定义中指定的那些名称，如 "Weapon.Type.None"、"Weapon.Type.Rifle"、"Weapon.Type.Pistol" 等名称
 */
namespace ShooterTags
{
	// 定义一个原生游戏玩法标签，并附带注释，该注释在头文件中外部声明，以便其他模块或代码可以使用创建的标签变量。
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_WeaponType_None, "Weapon.Type.None", "No Weapon Type");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_WeaponType_Rifle, "Weapon.Type.Rifle", "Rifle Weapon Type");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_WeaponType_Pistol, "Weapon.Type.Pistol", "Pistol Weapon Type");
}