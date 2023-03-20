// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBase.h"
#include "SDashProjectile.generated.h"

UCLASS()
class ACTION_API ASDashProjectile : public ASProjectileBase
{
	GENERATED_BODY()
	
protected:

	//传送延迟
	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float TeleportDelay;

	//爆炸延迟
	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float DetonateDelay;

	//如果粒子命中物体，提前取消计时器
	FTimerHandle TimerHandle_DelayedDetonate;

	// Base class using BlueprintNativeEvent, we must override the _Implementation not the Explode()
	virtual void Explode_Implementation() override;

	//传送函数
	void TeleportInstigator();

	virtual void BeginPlay() override;

public:

	ASDashProjectile();
};
