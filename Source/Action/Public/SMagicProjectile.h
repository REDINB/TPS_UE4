// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SMagicProjectile.generated.h"

class USActionEffect;
UCLASS()
class ACTION_API ASMagicProjectile : public ASProjectileBase
{
	GENERATED_BODY()

protected:
	//用于触发命中受伤事件
	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	//反弹tag
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag ParryTag;
	
	//伤害属性
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageAmount;

	//燃烧的action类，在引擎中选择蓝图
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<USActionEffect> BurningActionClass;

public:

	ASMagicProjectile();

};
