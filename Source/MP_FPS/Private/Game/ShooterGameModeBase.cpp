
#include "Game/ShooterGameModeBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"


void AShooterGameModeBase::RequestRespawn(ACharacter* Character, AController* Controller)
{
	if (!IsValid(Character) || !IsValid(Controller)) return;

	Character->Reset();
	Character->Destroy();

	// 随机生成玩家出生点
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	ensure(PlayerStarts.Num() > 0);
	int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);

	// 在指定的Actor位置生成玩家的Pawn
	RestartPlayerAtPlayerStart(Controller, PlayerStarts[Selection]);
}
