// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ShooterGameStateBase.h"
#include "Player/ShooterPlayerState.h"


AShooterGameStateBase::AShooterGameStateBase()
{
	bHasFirstBloodBeenHad = false;
}

bool AShooterGameStateBase::HasFirstBloodBeenHad() const
{
	return bHasFirstBloodBeenHad;
}

// 更新领先者列表
void AShooterGameStateBase::UpdateLeader()
{
	// 玩家排名排序:通过玩家击杀数排序
	TArray<APlayerState*> LocalSortedPlayers = PlayerArray;
	LocalSortedPlayers.Sort([](const APlayerState& A, const APlayerState& B)
	{
		const AShooterPlayerState* PlayerA = Cast<AShooterPlayerState>(&A);
		const AShooterPlayerState* PlayerB = Cast<AShooterPlayerState>(&B);
		return PlayerA->GetScoredElims() > PlayerB->GetScoredElims();
	});

	Leaders.Empty();
	
	if (LocalSortedPlayers.Num() > 0)
	{
		int32 HighestScore = 0;
		for (APlayerState* Player : LocalSortedPlayers)
		{
			AShooterPlayerState* ShooterPS = Cast<AShooterPlayerState>(Player);
			if (IsValid(ShooterPS))
			{
				int32 PlayerScore = ShooterPS->GetScoredElims();

				// 把那些分数与最高分持平的玩家都加入到领导者数组里
				// 在第一次迭代时，设置最高分 (on the first iteration, set the highest score)
				if (Leaders.Num() == 0)
				{
					HighestScore = PlayerScore;
					Leaders.Add(ShooterPS);
				}
				else if (PlayerScore == HighestScore)
				{
					Leaders.Add(ShooterPS); // 如果得分相同，加入领先者 Add to Leaders if scores are tied
				}
				else
				{
					break; // 已按顺序排列，无需再检查 (As it's sorted,no need to check further.)
				}
			}
		}
	}

	bHasFirstBloodBeenHad = true;
}

AShooterPlayerState* AShooterGameStateBase::GetSoleLeader() const
{
	if (Leaders.Num() == 1)
	{
		return Leaders[0];
	}

	return nullptr;
}

bool AShooterGameStateBase::IsTiedForTheLead(AShooterPlayerState* PlayerState)
{
	return Leaders.Contains(PlayerState);
}
