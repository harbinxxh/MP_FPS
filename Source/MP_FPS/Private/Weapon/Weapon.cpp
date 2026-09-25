


#include "Weapon/Weapon.h"


// Sets default values
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	// 默认武器可复制
	bReplicates = true;
	
	// 网络相关性会基于持有者状态来判断并与所有者绑定，这样一来就不需要 Always Relevant 始终网络相关性
	// 只有当所有者相关时，才需要复制可见的角色，否则没必要复制你看不到的对象
	// 因此，只要能看到所有者，就能看到真实的武器，它也会自动同步
	bNetUseOwnerRelevancy = true;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh1P");
	// 仅在渲染时更新姿态，当在渲染武器模型时，只对当前操控该角色的玩家生效
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = false;
	// Mesh1P 默认设置隐藏，这样就能在运行时动态调整它，根据武器生成、附加绑定并交付给角色后的当前视角来决定
	Mesh1P->SetHiddenInGame(true);
	SetRootComponent(Mesh1P);

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh3P");
	Mesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh3P->bReceivesDecals = false;
	// Mesh3P 默认投射阴影，在第三人称视角下的完整角色，从别人视角看的时候，他们就能看到阴影了
	// 当第一人称视角时，而我们自己看手臂时，我们手上拿的武器根本没必要投阴影，这能带来不错的性能优化
	Mesh3P->CastShadow = true;
	Mesh3P->SetupAttachment(Mesh1P);
	Mesh3P->SetHiddenInGame(true);
}

USkeletalMeshComponent* AWeapon::GetMesh1P() const
{
	return Mesh1P;
}

USkeletalMeshComponent* AWeapon::GetMesh3P() const
{
	return Mesh3P;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}
