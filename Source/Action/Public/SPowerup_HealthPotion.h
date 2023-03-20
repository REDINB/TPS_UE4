// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerupActor.h"
#include "SPowerup_HealthPotion.generated.h"

UCLASS()
class ACTION_API ASPowerup_HealthPotion : public ASPowerupActor
{
	GENERATED_BODY()
	
protected:

	//使用药水需要消耗分数
	UPROPERTY(EditAnywhere, Category = "HealthPotion")
	int32 CreditCost;

	public:

	//交互事件
	void Interact_Implementation(APawn* InstigatorPawn) override;

	ASPowerup_HealthPotion();
};
