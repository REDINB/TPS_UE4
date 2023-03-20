// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SPlayerState.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"

//停止机器人生成的控制台变量
static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);


ASGameModeBase::ASGameModeBase()
{
	//机器人生成间隔
	SpawnTimerInterval = 2.0f;
	//击杀奖励
	CreditsPerKill = 20;
	//生成物品的需求量
	DesiredPowerupCount = 10;
	//物品间要求的最小距离
	RequiredPowerupDistance = 2000;

	PlayerStateClass = ASPlayerState::StaticClass();
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();
	//设置定时器，每过一段时间调用生成机器人函数
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);

	// Make sure we have assigned at least one power-up class
	//确保我们在引擎里为这个类分配了
	if (ensure(PowerupClasses.Num() > 0))
	{
		// Run EQS to find potential power-up spawn locations
		//通过EQS寻找该类物体生成的位置
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, PowerupSpawnQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
		if (ensure(QueryInstance))
		{
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnPowerupSpawnQueryCompleted);
		}
	}
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot spawning disabled via cvar 'CVarSpawnBots'."));
		return;
	}
	//检查当前还存活的机器人数量
	int32 NrOfAliveBots = 0;
	for(TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter *Bot = *It;
		USAttributeComponent *AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if(AttributeComp && USAttributeComponent::IsActorAlive(Bot))
		{
			NrOfAliveBots++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %i alive bots"), NrOfAliveBots);

	//默认最大机器人数量为10
	float MaxBotCount = 10.0f;
	//如果设置了难度曲线，则根据世界时间重新为最大机器人数赋值
	if(DifficultyCurve)
	{
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}
	//如果机器人数量过多，则跳出
	if(NrOfAliveBots >= MaxBotCount)
	{
		UE_LOG(LogTemp, Log, TEXT("At maximum bot capacity. Skipping bot spawn."));
		return ;
	}
	
	//获得EQS中搜索得到的实例
	UEnvQueryInstanceBlueprintWrapper *QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	if(ensure(QueryInstance))
	{
		//通过该实例调用查询完成后的事件
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnBotSpawnQueryCompleted);
	}
}


void ASGameModeBase::OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if(QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"))
		return ;
	}
	
	//获得查询实例中的位置
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if(Locations.Num() > 0)
	{
		//在指定位置生成AI人物
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);


		DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);
	}
	
}

void ASGameModeBase::OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	//通过EQS获得位置
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	// Keep used locations to easily check distance between points
	TArray<FVector> UsedLocations;

	int32 SpawnCounter = 0;
	// Break out if we reached the desired count or if we have no more potential positions remaining
	//一直生成物品类直到数量满足要求或者没有可以生成的位置
	while (SpawnCounter < DesiredPowerupCount && Locations.Num() > 0)
	{
		// 从EQS查询的位置中随机选择一个位置
		int32 RandomLocationIndex = FMath::RandRange(0, Locations.Num() - 1);

		FVector PickedLocation = Locations[RandomLocationIndex];
		// 从位置列表中移除该位置，以免重复生成
		Locations.RemoveAt(RandomLocationIndex);

		// 确认物品间生成的最小距离
		bool bValidLocation = true;
		for (FVector OtherLocation : UsedLocations)
		{
			float DistanceTo = (PickedLocation - OtherLocation).Size();

			//如果当前选择的位置与任意一个物品距离过近则退出重新选择位置
			if (DistanceTo < RequiredPowerupDistance)
			{
				// Show skipped locations due to distance
				//DrawDebugSphere(GetWorld(), PickedLocation, 50.0f, 20, FColor::Red, false, 10.0f);

				// too close, skip to next attempt
				bValidLocation = false;
				break;
			}
		}

		// Failed the distance test
		if (!bValidLocation)
		{
			continue;
		}

		// 选择一个随即物品类来生成
		int32 RandomClassIndex = FMath::RandRange(0, PowerupClasses.Num() - 1);
		TSubclassOf<AActor> RandomPowerupClass = PowerupClasses[RandomClassIndex];

		GetWorld()->SpawnActor<AActor>(RandomPowerupClass, PickedLocation, FRotator::ZeroRotator);

		// 生成物品后在已有位置加入生成物品的位置，同时生成物品数加一
		UsedLocations.Add(PickedLocation);
		SpawnCounter++;
	}
}


void ASGameModeBase::KillAll()
{
	//遍历机器人并击杀
	for(TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter *Bot = *It;
		USAttributeComponent *AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if(AttributeComp && USAttributeComponent::IsActorAlive(Bot))
		{
			AttributeComp->Kill(this);
		}
	}
}

void ASGameModeBase::OnActionKilled(AActor* VictimActor, AActor* Killer)
{
	//获得被杀者
	ASCharacter *Player = Cast<ASCharacter>(VictimActor);
	if(Player)
	{
		//设置定时器
		FTimerHandle TimerHandle_RespawnDelay;
		//设置定时委托，并绑定重生函数
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());
		//设置重生延迟
		float RespawnDelay = 2.0f;
		//触发定时器
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}
	// 成功击杀电脑将获得击杀奖励
	APawn* KillerPawn = Cast<APawn>(Killer);
	if (KillerPawn)
	{
		if (ASPlayerState* PS = KillerPawn->GetPlayerState<ASPlayerState>()) // < can cast and check for nullptr within if-statement.
			{
				PS->AddCredits(CreditsPerKill);
			}
	}
}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if(ensure(Controller))
	{
		//解除控制器与原来人物的绑定
		Controller->UnPossess();
		//生成一个新的人物
		RestartPlayer(Controller);
	}
}
