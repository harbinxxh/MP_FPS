// Fill out your copyright notice in the Description page of Project Settings.


#include "Elimination/EliminationComponent.h"
#include "Player/ShooterPlayerState.h"
#include "ShooterTypes/ShooterTypes.h"
#include "Game/ShooterGameStateBase.h"
#include "Kismet/GameplayStatics.h"


UEliminationComponent::UEliminationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SequentialElimInterval = 2.f;
	LastElimTime = 0.f;
	SequentialElims = 0;
	Streak = 0;
	ElimsNeededForStreak = 5; // 如果你连续击杀五个敌人而不死亡，那你就是在连胜状态
}

/**
 * 每当有射击发生，击杀组件都能立刻收到通知
 * 在战斗组件中，当在服务器端开枪的时候，就把相关信息广播给击杀组件
 */
void UEliminationComponent::OnRoundReported(AActor* Attacker, AActor* Victim, bool bHit, bool bHeadShot, bool bLethal)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit:%d, HeadShot: %d, Lethal:%d"), bHit, bHeadShot, bLethal);
	
	AShooterPlayerState* AttackerPS = GetPlayerStateFromActor(Attacker);
	if (!IsValid(AttackerPS)) return;

	ProcessHitOrMiss(bHit, AttackerPS);

	if (!bHit) return; // Early return if it was a miss

	AShooterPlayerState* VictimPS = GetPlayerStateFromActor(Victim);
	if (!IsValid(VictimPS)) return;

	if (bLethal)
	{
		PnocessElimination(bHeadShot, AttackerPS, VictimPS);
	}
}

void UEliminationComponent::PnocessElimination(bool bHeadShot, AShooterPlayerState* AttackerPS, AShooterPlayerState* VictimPS)
{
	AttackerPS->AddScoredElim();
	VictimPS->AddDefeat();
	
	ESpecialElimType SpecialElimType{}; // 初始化特殊击杀类型所有位均为 0
	
	// Process Head Shot
	ProcessHeadshot(bHeadShot, SpecialElimType, AttackerPS);

	// Process Sequential Eliminations 处理连续击杀-相邻击杀必须落在时间窗口内，时间窗口一过连段清空。短时间内多次击杀，如三杀、四杀、五杀等
	ProcessSequentialEliminations(SpecialElimType, AttackerPS);

	// Process Streaks 连击 - 就是指在被击倒前的击杀数，只看是否阵亡，击杀间隔多久都不影响累加
	PnocessStreaks(SpecialElimType, AttackerPS, VictimPS);

	// 获取游戏状态
	AShooterGameStateBase* GameState = Cast<AShooterGameStateBase>(UGameplayStatics::GetGameState(AttackerPS));
	if (IsValid(GameState))
	{
		// Handle First Blood 首次击杀
		HandleFinstBLood(GameState, SpecialElimType, AttackerPS);
	}

	// Update Leader Status 可以是多个领先者，不止一个领先者玩家





	// if (Has Special Elim Types) 特殊击杀的情况
		// Tell the client which special elims we got
	// else (We just got a regular elim) 非特殊击杀的情况
		// Just tell the client we got a regular elim
}

void UEliminationComponent::ProcessHeadshot(bool bHeadShot, ESpecialElimType& OutElimType, AShooterPlayerState* AttackerPS)
{
	if (bHeadShot)
	{
		//（或等于 |= 运算符）等于右边的写法（Headshot += 1 或  Headshot = Headshot + 1）
		OutElimType |= ESpecialElimType::Headshot;
		AttackerPS->AddHeadShotElim();
	}
}

void UEliminationComponent::ProcessSequentialEliminations(ESpecialElimType& OutElimType, AShooterPlayerState* AttackerPS)
{
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastElimTime <= SequentialElimInterval)
	{
		++SequentialElims;
	}
	else
	{
		SequentialElims = 1;
	}
	LastElimTime = CurrentTime;

	if (SequentialElims > 1)
	{
		OutElimType |= ESpecialElimType::Sequential;	// 连续击杀位置位
		AttackerPS->AddSequentialElim(SequentialElims); // 更新攻击者玩家连续击杀状态
	}
}

void UEliminationComponent::PnocessStreaks(ESpecialElimType& OutElimType, AShooterPlayerState* AttackerPS, AShooterPlayerState* VictimPS)
{
	++Streak;

	// 判断连击是否达到了所需的击杀数，所以要检查连击数是否达到触发所需的击杀数
	if (Streak >= ElimsNeededForStreak)
	{
		// 击杀类型对连击位执行按位或赋值
		OutElimType |= ESpecialElimType::Streak;	// 连击位置位
		AttackerPS->SetOnStreak(true);
		AttackerPS->UpdateHighestStreak(Streak);
	}

	// 检查受害者是否正在连胜中
	if (VictimPS->IsOnStreak())
	{
		OutElimType |= ESpecialElimType::Showstopper;	// 终结连杀位置位
		AttackerPS->AddShowStopperElim();
		VictimPS->SetOnStreak(false);
	}

	// 检查上一个击杀者是否是现在的受害者
	if (AttackerPS->GetLastAttacker() == VictimPS)
	{
		// 如果上一个攻击者（LastAttacker）是受害者，我们就正好击杀了自己的上一个攻击者，这就意味着我们触发了复仇击杀
		OutElimType |= ESpecialElimType::Revenge;	// 复仇击杀位置位
		AttackerPS->AddRevengeElim();
		AttackerPS->SetLastAttacker(nullptr);
	}
	VictimPS->SetLastAttacker(AttackerPS);
}

void UEliminationComponent::HandleFinstBLood(AShooterGameStateBase* GameState, ESpecialElimType& OutElimType, AShooterPlayerState* AttackerPS)
{
	if (!GameState->HasFirstBloodBeenHad())
	{
		OutElimType |= ESpecialElimType::FirstBlood;
		AttackerPS->GotFirstBlood();
	}
}

void UEliminationComponent::ProcessHitOrMiss(bool bHit, AShooterPlayerState* AttackerPS)
{
	if (bHit)
	{
		AttackerPS->AddHit();
	}
	else
	{
		AttackerPS->AddMiss();
	}
}

AShooterPlayerState* UEliminationComponent::GetPlayerStateFromActor(AActor* Actor)
{
	APawn* Pawn = Cast<APawn>(Actor);
	if (IsValid(Pawn))
	{
		return Pawn->GetPlayerState<AShooterPlayerState>();
	}

	return nullptr;
}