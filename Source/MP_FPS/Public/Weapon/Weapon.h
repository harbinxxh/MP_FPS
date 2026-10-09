

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Weapon.generated.h"


// 全自动射击和半自动射击
UENUM(BlueprintType)
enum class EFireType : uint8
{
	Auto UMETA(DisplayName = "Automatic"),
	SemiAuto UMETA(DisplayName = "SemiAutomatic")
};


UCLASS()
class MP_FPS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();

	/**
	* 服务器端：
	* 1、AShooterCharacter::PossessedBy() 函数只能在 Server / Standalone 上被调用
	* 2、服务器端调用 AShooterCharacter::PossessedBy() 函数
	* 3、在 PossessedBy() 函数，调用角色身上的战斗组件 UCombatComponent::SpawnInventory() 函数
	* 4、在 SpawnInventory() 函数，服务器端生成玩家角色并为角色绑定武器
	*/
	/**
	* 客户端：
	* 1、当玩家角色网络同步时，服务器端生成的武器也同步到客户端，就可以在客户端为角色绑定武器
	* 2、如果想让这个功能在多人模式下立即生效，可以在 Weapon 武器类里覆盖 OnRep_Instigator() 函数
	* 3、当Instigator被复制时，OnRep_Instigator() 函数在客户端就会被调用
	* 4、在 OnRep_Instigator() 函数里，调用 AttachToOwningPawn() 函数，为玩家角色绑定武器
	* 5、这样在多人模式下，武器就可以正确的绑定到客户端玩家角色身上
	*/

	/**
	* 这是 Replication Notify,只在客户端运行
	* 当Instigator被复制时，会调用客户端事件通知函数
	* 在 OnRep_Instigator() 函数里，调用 AttachToOwningPawn() 函数，为玩家角色绑定武器
	*/
	virtual void OnRep_Instigator() override;


	USkeletalMeshComponent* GetMesh1P() const;
	USkeletalMeshComponent* GetMesh3P() const;

	// 武器射线检测函数
	void WeaponTrace(FHitResult& OutHit, float TraceLength);

	// 把武器绑定到拥有者，也就是控制的Pawn上
	void AttachToOwningPawn() const;

	// 武器类型：根据武器类型决定该播放哪些姿势或蒙太奇
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPS|WeaponType")
	FGameplayTag WeaponType;

	// 武器瞄准视野值
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|Aiming")
	float AimFieldView;

	// 球体半径
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FPS|Trace")
	float TraceRadius;

	// 全自动射击和半自动射击
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPS|FirType")
	EFireType FireType;

	// 武器发射间隔时长
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPS|FirType")
	float FireTime;

	/**
	 * ImpactPoint：击中点
	 * ImpactNormal：击中法线
	 * ImpactSurfaceType：物理表面，这样外观效果就能基于这个物理表面来决定
	 * bIsFirstPerson：布尔值，来判断是不是第一人称网格
	 * 让武器在本地执行操作-不是指本地控制，而是指只在当前这台机器上运行，不会把动作同步给其他玩家
	 */
	void Local_Fire(const FVector& ImpactPoint, const FVector& ImpactNormal,  TEnumAsByte<EPhysicalSurface> ImpactSurfaceType, bool bIsFirstPerson);

protected:
	virtual void BeginPlay() override;

	// 在蓝图中处理外观相关的逻辑
	UFUNCTION(BlueprintImplementableEvent)
	void FireEffects(const FVector& ImpactPoint, const FVector& ImpactNormal, EPhysicalSurface ImpactSurfaceType, bool bIsFirstPerson);

	// Mesh1P 和 Mesh3P 在两种视角下会呈现两套不同的武器
	// WeaponMesh: 1st person view
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS|Weapon")
	TObjectPtr<USkeletalMeshComponent> Mesh1P;

	// WeaponMesh: 3rd person view
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS|Weapon")
	TObjectPtr<USkeletalMeshComponent> Mesh3P;

private:

	// 设置玩家角色网格的可见性
	void SetMeshVisibilities(APawn* OwningPawn) const;

};
