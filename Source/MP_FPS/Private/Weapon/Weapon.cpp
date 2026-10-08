


#include "Weapon/Weapon.h"
#include "Interfaces/PlayerInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "MP_FPS/MP_FPS.h"
#include "KismetTraceUtils.h"

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

	AimFieldView = 65.0f;
	TraceRadius = 5.f;
}

// 在 OnRep_Instigator() 函数里，调用 AttachToOwningPawn() 函数，为玩家角色绑定武器
void AWeapon::OnRep_Instigator()
{
	// 不需要 Super:: 调用，父类是空实现
	//Super::OnRep_Instigator();

	// 武器绑定到客户端玩家角色上
	AttachToOwningPawn();
}

USkeletalMeshComponent* AWeapon::GetMesh1P() const
{
	return Mesh1P;
}

USkeletalMeshComponent* AWeapon::GetMesh3P() const
{
	return Mesh3P;
}

void AWeapon::WeaponTrace(FHitResult& OutHit, float TraceLength)
{
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.AddIgnoredActor(GetOwner());

	// 设置碰撞的具体行为
	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_WorldStatic, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_WorldDynamic, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_PhysicsBody, ECR_Block);

	// 确保获取到发起者
	ensure(GetInstigator());
	if (APlayerController* PC = Cast<APlayerController>(GetInstigator()->GetController()); IsValid(PC))
	{
		// 获取角色眼睛的位置和朝向
		FVector EyesWorldLocation;
		FRotator EyesWorldRotation;
		PC->GetActorEyesViewPoint(EyesWorldLocation, EyesWorldRotation);
		const FVector EyesWorldDirection = UKismetMathLibrary::GetForwardVector(EyesWorldRotation);

		// 用角色眼睛的位置，作为射线检测的起点
		const FVector Start = EyesWorldLocation;
		const FVector End = Start + EyesWorldDirection * TraceLength;

		// 用指定通道（Channel），在场景中扫掠一个形状（Shape），并返回第一个造成阻挡的命中结果。
		// 用球形状，这样比简单的线性射线检测更容易命中，打中东西就更容易了
		const bool bHit = GetWorld()->SweepSingleByChannel(
			OutHit,
			Start,
			End,
			FQuat::Identity,
			FPSTraceChannels::ECC_Weapon,
			FCollisionShape::MakeSphere(TraceRadius),
			QueryParams,
			ResponseParams);

		if (!bHit)
		{
			OutHit.ImpactPoint = End; // 如果未击中，就将击中点设为 End 点，也就是射线末端
		}

		//DrawDebugSphereTraceSingle(
		//	GetWorld(),
		//	Start,
		//	End,
		//	TraceRadius,
		//	EDrawDebugTrace::ForDuration,
		//	bHit,
		//	OutHit,
		//	FColor::Green,
		//	FColor::Red,
		//	5.f);
	}
}

/**
* 要将武器附加到所属的玩家角色，得知道玩家角色是谁，需要拿到武器的持有者
* 一种方法是把持有者强制转换为ShooterCharacter，然后直接访问两个网格:mesh和mesh1p，避免依赖 Cast<> 类型转换
* 另外一种是使用接口，接口更方面
* 使用接口
*/
void AWeapon::AttachToOwningPawn() const
{
	APawn* OwningPawn = GetInstigator();
	if (!IsValid(OwningPawn) || !OwningPawn->Implements<UPlayerInterface>()) return;

	// 先设好可见性，再开始绑定，要是某个网格不可见，绑定就会失败
	SetMeshVisibilities(OwningPawn);

	// 武器绑定到角色上
	// 1.首先，要获取武器绑定点
	const FName AttachPoint = IPlayerInterface::Execute_GetWeaponAttachPoint(OwningPawn, WeaponType);
	USkeletalMeshComponent* PawnMesh1P = IPlayerInterface::Execute_GetMesh1P(OwningPawn);
	USkeletalMeshComponent* PawnMesh3P = IPlayerInterface::Execute_GetMesh3P(OwningPawn);
	// 2.通过角色的握把点，绑定武器
	// 3.虽说同时绑定到PawnMesh1P、PawnMesh3P上，但是当某个网格不可见时，绑定就会失败
	Mesh1P->AttachToComponent(PawnMesh1P, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
	Mesh3P->AttachToComponent(PawnMesh3P, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
}

void AWeapon::Local_Fire(const FVector& ImpactPoint, const FVector& ImpactNormal, TEnumAsByte<EPhysicalSurface> ImpactSurfaceType, bool bIsFirstPerson)
{
	// local fire stuff...
	FireEffects(ImpactPoint, ImpactNormal, ImpactSurfaceType, bIsFirstPerson);

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

/**
* 判断OwningPawn是不是本地控制，这决定了要隐藏哪个网格，是显示Mesh1P还是Mesh3P
* 如果是OwningPawn是本地控制的，那就是第一人称视角，否则就是第三人称视角
*/
void AWeapon::SetMeshVisibilities(APawn* OwningPawn)  const
{
	if (OwningPawn->IsLocallyControlled())
	{
		Mesh1P->SetHiddenInGame(false);
		Mesh3P->SetHiddenInGame(true);
	}
	else
	{
		Mesh1P->SetHiddenInGame(true);
		Mesh3P->SetHiddenInGame(false);
	}
}
