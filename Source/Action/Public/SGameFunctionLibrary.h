// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SGameFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ACTION_API USGameFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	//造成伤害函数
	UFUNCTION(BlueprintCallable, Category = "GamePlay")
	static bool ApplyDamage(AActor *DamageCauser, AActor *TargetActor, float DamageAmount);

	//使用const使该变量变为只读，且在蓝图中引脚也发生变化
	//受击后物理表现
	UFUNCTION(BlueprintCallable, Category = "GamePlay")
	static bool ApplyDirectionalDamage(AActor *DamageCauser, AActor *TargetActor, float DamageAmount, const FHitResult &HitResult);
};
