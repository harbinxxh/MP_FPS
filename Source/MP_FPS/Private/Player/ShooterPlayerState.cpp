// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ShooterPlayerState.h"

AShooterPlayerState::AShooterPlayerState()
{
	SetNetUpdateFrequency(100.f);

	ScoredElims	= 0;
	Defeats = 0;
	Hits = 0;
	Misses = 0;
	bOnStreak = false;
	HeadShotElims = 0;
	HighestStreak = 0;
	RevengeElims = 0;
	DethroneElims = 0;
	ShowStopperElims = 0;
	bFirstBlood = false;
	bWinner = false;

}

// 击杀数
void AShooterPlayerState::AddScoredElim()
{
	++ScoredElims;
}

// 击败数
void AShooterPlayerState::AddDefeat()
{
	++Defeats;
}

void AShooterPlayerState::AddHit()
{
	++Hits;
}

void AShooterPlayerState::AddMiss()
{
	++Misses;
}

void AShooterPlayerState::AddHeadShotElim()
{
	++HeadShotElims;
}

void AShooterPlayerState::AddSequentialElim(int32 SequenceCount)
{
	if (SequentialElims.Contains(SequenceCount))
	{
		SequentialElims[SequenceCount]++;
	}
	else
	{
		SequentialElims.Add(SequenceCount, 1);
	}

	/**
	 * Reduce the count for all lower sequence counts
	 * this is because a triple elim means a double was scored first.
	 * But we want to count this as just a triple, i.e.elim 1, elim 2, elim 3 = just a triple, not a double and a triple
	 * 将所有较低击杀数减 1
	 */
	for (auto& Elem : SequentialElims)
	{
		if (Elem.Key < SequenceCount && Elem.Value > 0)
		{
			Elem.Value--;
		}
	}
}

void AShooterPlayerState::UpdateHighestStreak(int32 StreakCount)
{
	if (StreakCount > HighestStreak)
	{
		HighestStreak = StreakCount;
	}
}

void AShooterPlayerState::AddRevengeElim()
{
	++RevengeElims;
}

void AShooterPlayerState::AddDethroneElim()
{
	++DethroneElims;
}

void AShooterPlayerState::AddShowStopperElim()
{
	++ShowStopperElims;
}

void AShooterPlayerState::GotFirstBlood()
{
	bFirstBlood = true;
}

void AShooterPlayerState::IsNowWinner()
{
	bWinner = true;
}

void AShooterPlayerState::SetOnStreak(bool bIsOnStreak)
{
	bOnStreak = bIsOnStreak;
}

void AShooterPlayerState::SetLastAttacker(APlayerState* Attacker)
{
	LastAttacker = Attacker;
}

bool AShooterPlayerState::IsOnStreak()
{
	return bOnStreak;
}

APlayerState* AShooterPlayerState::GetLastAttacker() const
{
	return LastAttacker.IsValid() ? LastAttacker.Get() : nullptr;
}

int32 AShooterPlayerState::GetScoredElims() const
{
	return ScoredElims;
}

void AShooterPlayerState::Client_SpecialElim_Implementation(const ESpecialElimType& SpecialElim, int32 SequentialElimCount, int32 StreakCount, int32 ElimScore)
{
}

void AShooterPlayerState::Client_ScoredElim_Implementation(int32 ElimScore)
{
}

/**
 * 那为什么要为客户端失去领先另设一个RPC?
 * 因为这个 OutElimType 枚举是专属于攻击者的
 * 所有操作都是针对攻击者的，我们正在处理攻击者的击杀组件，上一个领先者，需要告诉那个人，他已经失去领先了
 */
void AShooterPlayerState::Client_LostTheLead_Implementation()
{
	// TOD0: Show the client that they've lost the lead. 向客户显示他们已经失去了领先者
}
