// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	if(ensureMsgf(BehaviorTree, TEXT("Behavior Tree is nullptr! Please assign BehaviorTree in your AI Controller.")))
	{
		//添加行为树
		RunBehaviorTree(BehaviorTree);
	}


	// //获得玩家的Pawn
	// APawn *MyPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	// if(MyPawn)
	// {
	// 	//为黑板中特定变量设置值，为玩家当前location
	// 	GetBlackboardComponent()->SetValueAsVector("MoveToLocation", MyPawn->GetActorLocation());
	//
	// 	GetBlackboardComponent()->SetValueAsObject("TargetActor", MyPawn);
	// }
}
