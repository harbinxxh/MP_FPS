

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Weapon.generated.h"

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

	// 把武器绑定到拥有者，也就是控制的Pawn上
	void AttachToOwningPawn() const;

	// 武器类型：根据武器类型决定该播放哪些姿势或蒙太奇
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FPS|WeaponType")
	FGameplayTag WeaponType;

protected:
	virtual void BeginPlay() override;

private:

	// Mesh1P 和 Mesh3P 在两种视角下会呈现两套不同的武器
	
	// WeaponMesh: 1st person view
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;

	// WeaponMesh: 3rd person view
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh3P;

	// 设置玩家角色网格的可见性
	void SetMeshVisibilities(APawn* OwningPawn) const;

};
