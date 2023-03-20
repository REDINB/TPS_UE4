// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"


class UEnvQueryInstanceBlueprintWrapper;
class UEnvQuery;
class UCurveFloat;
UCLASS()
class ACTION_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
protected:

	//选择AI人物的蓝图类
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AActor> MinionClass;
	//机器人生成位置EQS查询结果
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery *SpawnBotQuery;
	//难度曲线，用于调整机器人产生速度
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat *DifficultyCurve;

	//定时器变量
	FTimerHandle TimerHandle_SpawnBots;

	//定时器时间间隔
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnTimerInterval;

	// 击杀机器人获得的分数
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	int32 CreditsPerKill;

	//物品生成的EQS查询
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	UEnvQuery* PowerupSpawnQuery;

	//自己选择要生成的物品列表
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	TArray<TSubclassOf<AActor>> PowerupClasses;

	//物品生成的距离
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float RequiredPowerupDistance;

	//物品生成的数量
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 DesiredPowerupCount;


	//定时器触发函数
	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION()
	void OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
	//重生函数
	UFUNCTION()
	void RespawnPlayerElapsed(AController *Controller);
public:

	ASGameModeBase();
	virtual void StartPlay() override;

	//控制台命令，杀死所有机器人
	UFUNCTION(Exec)
	void KillAll();

	//被击杀函数
	virtual void OnActionKilled(AActor *VictimActor, AActor *Killer);
	
};
