// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	//检测AI和玩家之间的距离
	//先获得黑板组件
	UBlackboardComponent *BlackboardComp = OwnerComp.GetBlackboardComponent();
	if(ensure(BlackboardComp))
	{
		//再获得AI眼中的目标（在这里就是玩家）
		AActor *TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
		if(TargetActor)
		{
			//获得AI的controller
			AAIController *MyController = OwnerComp.GetAIOwner();
			if(ensure(MyController))
			{
				//获得AI的Pawn
				APawn *AIPawn = MyController->GetPawn();
				if(ensure(AIPawn))
				{
					//计算AI到玩家的距离
					float DistanceTo = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());
					//判断玩家到AI的距离是否小于2000厘米
					bool bWithinRange = DistanceTo < 2000.0f;
					//玩家是否在AI视野内，用于检测墙壁
					bool bHasLOS = false;
					if(bWithinRange)
					{
						bHasLOS = MyController->LineOfSightTo(TargetActor);
					}
					//将判断的bool值重新返回黑板
					BlackboardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, (bWithinRange && bHasLOS));
				}
			}
		}
	}
}
