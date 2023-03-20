// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckHealth.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

USBTService_CheckHealth::USBTService_CheckHealth()
{
	//低生命阈值
	LowHealthFraction = 0.6f;
}


void USBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	//获得当前AI人物
	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ensure(AIPawn))
	{
		//获得其人物属性组件
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(AIPawn);
		if (ensure(AttributeComp))
		{
			//获得其生命值比例并且和阈值作比较
			bool bLowHealth = (AttributeComp->GetHealth() / AttributeComp->GetHealthMax()) < LowHealthFraction;

			//获得黑板组件并修改黑板上的值
			UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
			BlackBoardComp->SetValueAsBool(LowHealthKey.SelectedKeyName, bLowHealth);
		}
	}
}