// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SWorldUserWidget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
ASAICharacter::ASAICharacter()
{
 	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");
	//使得后来在世界中生成的AI人物也会自动被AI控制
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	//修改AI模型的碰撞通道
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	//受击特效
	TimeToHitParamName = "TimeToHit";
	//追击玩家
	TargetActorKey = "TargetActor";
}

void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if(Delta < 0.0f)
	{
		//当AI被除了自己的actor攻击时会自动获得攻击者的位置
		if(GetInstigator() != this)
		{
			SetTargetActor(InstigatorActor);
		}

		//受击时显示生命值UI
		if(ActiveHealthBar == nullptr)
		{
			ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if(ActiveHealthBar)
			{
				ActiveHealthBar->AttachedActor = this;
				ActiveHealthBar->AddToViewport();
			}
		}
		
		//判断受到攻击时添加受击特效
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		if(NewHealth <= 0.0f)
		{
			//停止行为树
			AAIController *AIC = Cast<AAIController>(GetController());
			if(AIC)
			{
				AIC->GetBrainComponent()->StopLogic("Killed");
			}
			//使AI网格体骨架开启物理模拟，并调整碰撞模式为布娃娃系统防止陷入地表
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");

			//死亡后禁用碰撞和移动
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCharacterMovement()->DisableMovement();

			//设置该AI人物死亡后10s消失
			SetLifeSpan(10.0f);
		}
	}
}

void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
	//获得AIcontroller
	AAIController *AIC = Cast<AAIController>(GetController());
	if(AIC)
	{
		//为黑板组件中的目标Actor值设值
		AIC->GetBlackboardComponent()->SetValueAsObject(TargetActorKey, NewTarget);

	}
}

//获取AI当前追击的玩家
AActor* ASAICharacter::GetTargetActor() const
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if (AIC)
	{
		return Cast<AActor>(AIC->GetBlackboardComponent()->GetValueAsObject(TargetActorKey));
	}

	return nullptr;
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//触发AI感知事件
	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	// 仅在第一次发现玩家时生成UI
	if (GetTargetActor() != Pawn)
	{
		//设置看见的人物为目标对象
		SetTargetActor(Pawn);
		//广播AI感知的效果
		MulticastPawnSeen();
	}


		

}
void ASAICharacter::MulticastPawnSeen_Implementation()
{
		USWorldUserWidget* NewWidget = CreateWidget<USWorldUserWidget>(GetWorld(), SpottedWidgetClass);
		if (NewWidget)
		{
			NewWidget->AttachedActor = this;
			// Index of 10 (or anything higher than default of 0) places this on top of any other widget.
			// May end up behind the minion health bar otherwise.
			NewWidget->AddToViewport(10);
		}
}
