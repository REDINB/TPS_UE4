// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_Action.h"

#include "SAction.h"
#include "SActionComponent.h"

void ASPowerup_Action::Interact_Implementation(APawn* InstigatorPawn)
{
	// 确保发起者并且为获得能力设置了蓝图类
	if (!ensure(InstigatorPawn && ActionToGrant))
	{
		return;
	}

	USActionComponent* ActionComp = Cast<USActionComponent>(InstigatorPawn->GetComponentByClass(USActionComponent::StaticClass()));
	// 确认是否有该组件
	if (ActionComp)
	{
		if (ActionComp->GetAction(ActionToGrant))
		{
			//如果已经拥有该能力则直接返回
			FString DebugMsg = FString::Printf(TEXT("Action '%s' already known."), *GetNameSafe(ActionToGrant));
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, DebugMsg);
			return;
		}

		// 赋予新能力
		ActionComp->AddAction(InstigatorPawn, ActionToGrant);	
		HideAndCooldownPowerup();
	}
}
