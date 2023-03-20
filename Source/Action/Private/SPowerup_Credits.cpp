// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_Credits.h"

#include "SPlayerState.h"

ASPowerup_Credits::ASPowerup_Credits()
{
	//初始分数设置为80
	CreditsAmount = 80;
}


void ASPowerup_Credits::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn))
	{
		return;
	}

	//获得当前玩家状态
	if (ASPlayerState* PS = InstigatorPawn->GetPlayerState<ASPlayerState>())
	{
		//为他增加分数并且硬币消失进入冷却
		PS->AddCredits(CreditsAmount);
		HideAndCooldownPowerup();
	}
}