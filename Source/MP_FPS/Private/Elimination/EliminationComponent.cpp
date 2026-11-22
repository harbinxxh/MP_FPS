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
		// Update Leader Status 更新领先者状态，可以是多个领先者，不止一个领先者
		UpdateLeaderStatus(GameState, SpecialElimType, AttackerPS, VictimPS);
	}


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

void UEliminationComponent::UpdateLeaderStatus(AShooterGameStateBase* GameState, ESpecialElimType& OutElimType, AShooterPlayerState* AttackerPS, AShooterPlayerState* VictimPS)
{
	// 首先，如果存在唯一的领先者，那就从游戏状态中获取这个领先者
	AShooterPlayerState* LastLeader = GameState->GetSoleLeader();
	// 判断攻击者是否并列领先
	const bool bAttackerWasTiedForTheLead = GameState->IsTiedForTheLead(AttackerPS);
	// 更新领先者状态
	GameState->UpdateLeader();

	// 如果攻击者之前并不在并列领先中，同时也要确认攻击者现在是否并列领先
	if (!bAttackerWasTiedForTheLead && GameState->IsTiedForTheLead(AttackerPS))
	{
		// Attacker was not tied for the lead before and is now tied for the lead
		// 换句话说，攻击者之前不是并列领先，现在成了并列领先，这就是特殊的击杀类型
		OutElimType |= ESpecialElimType::TiedTheLeader;

		// 检查上一位领先者是不是已经不再是当前的领先者
		if (IsValid(LastLeader) && LastLeader != GameState->GetSoleLeader() )
		{
			/**
			 * 那为什么要为客户端失去领先另设一个RPC?
			 * 因为这个 OutElimType 枚举是专属于攻击者的
			 * 所有操作都是针对攻击者的，我们正在处理攻击者的击杀组件，上一个领先者，需要告诉那个人，他已经失去领先了
			 * 
			 * LastLeader has lost the lead
			 * 如果，上一位领先者失去领先，那需要让当前玩家状态里的上一位领先者向客户端发送通知，告知客户端它已不再拥有领先。
			 */
			LastLeader->Client_LostTheLead();

			// 是否是受害者
			if (VictimPS == LastLeader)
			{
				// 如果是，这意味着我们刚刚击杀了上一位领先者
				OutElimType |= ESpecialElimType::Dethrone;
				AttackerPS->AddDethroneElim();
			}
		}
	}

	// 检查一下，攻击者是否不等于上一位领先者，且现在他成了唯一领先者
	if (AttackerPS != LastLeader && AttackerPS == GameState->GetSoleLeader())
	{
		OutElimType |= ESpecialElimType::GainedTheLead;
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