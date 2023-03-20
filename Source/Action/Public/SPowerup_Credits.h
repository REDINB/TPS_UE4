// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerupActor.h"
#include "SPowerup_Credits.generated.h"

/**
 * 
 */
UCLASS()
class ACTION_API ASPowerup_Credits : public ASPowerupActor
{
	GENERATED_BODY()
protected:
	//每个硬币的分数
	UPROPERTY(EditAnywhere, Category = "Credits")
	int32 CreditsAmount;

public:
	//交互
	void Interact_Implementation(APawn* InstigatorPawn) override;

	ASPowerup_Credits();
};
