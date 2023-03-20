// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerupActor.h"
#include "SPowerup_Action.generated.h"

class USAction;
/**
 * 
 */
UCLASS()
class ACTION_API ASPowerup_Action : public ASPowerupActor
{
	GENERATED_BODY()
	
protected:

	//在蓝图中选择要获取的能力
	UPROPERTY(EditAnywhere, Category = "Powerup")
	TSubclassOf<USAction> ActionToGrant;

public:

	void Interact_Implementation(APawn* InstigatorPawn) override;

};
