// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"

#include "SActionComponent.h"
#include "Action/Action.h"
#include "Net/UnrealNetwork.h"

void USAction::Initialize(USActionComponent* NewActionComp)
{
	ActionComp = NewActionComp;
}

void USAction::StartAction_Implementation(AActor* Instigator)
{
	//UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));
	LogOnScreen(this, FString::Printf(TEXT("Started: %s"), *ActionName.ToString()), FColor::Green);
	//开始行动时在列表中添加tags
	USActionComponent *Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.AppendTags(GrantsTags);
	//获取互斥资源
	RepData.bIsRunning = true;
	RepData.Instigator = Instigator;

	//只有服务端设置时长，然后进行属性复制给客户端
	if (GetOwningComponent()->GetOwnerRole() == ROLE_Authority)
	{
		//获取获得buff时的游戏时间用于计算buff剩余时长
		TimeStarted = GetWorld()->TimeSeconds;
	}
	//开始action时触发广播
	GetOwningComponent()->OnActionStarted.Broadcast(GetOwningComponent(), this);
}


void USAction::StopAction_Implementation(AActor* Instigator)
{
	//UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));
	LogOnScreen(this, FString::Printf(TEXT("Stopped: %s"), *ActionName.ToString()), FColor::White);

	//这个检验只在服务端生效
	//ensureAlways(bIsRunning);
	
	//停止行动时在列表中移除tags
	USActionComponent *Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.RemoveTags(GrantsTags);
	//释放互斥资源
	RepData.bIsRunning = false;
	RepData.Instigator = Instigator;

	//停止action时触发广播
	GetOwningComponent()->OnActionStopped.Broadcast(GetOwningComponent(), this);
}

//重载actor中的getworld
 UWorld* USAction::GetWorld() const
 {
 	// Outer is set when creating action via NewObject<T>
 	//GetWorld只有在AActor中才会自动实现，在UObject类中需要重载
 	AActor *Actor = Cast<AActor>(GetOuter());
 	if (Actor)
 	{
 		return Actor->GetWorld();
 	}
 	return nullptr;
 }

USActionComponent* USAction::GetOwningComponent() const
{
	return ActionComp;
}

void USAction::OnRep_RepData()
{
	if(RepData.bIsRunning)
	{
		StartAction(RepData.Instigator);
		//StartAction(nullptr);
	}
	else
	{
		StopAction(RepData.Instigator);
	}
}

bool USAction::IsRunning() const
{
	return RepData.bIsRunning;
}


bool USAction::CanStart_Implementation(AActor* Instigator)
{
	//互斥资源被占用了就无法开始新的action
	if(IsRunning())
	{
		return false;
	}
	//查看当前action有没有被其他action所block
	USActionComponent *Comp = GetOwningComponent();
	if(Comp->ActiveGameplayTags.HasAny(BlockTags))
	{
		return false;
	}
	return true;
}

//获得网络复制属性的生命期？
void USAction::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAction, RepData);
	DOREPLIFETIME(USAction, TimeStarted);
	DOREPLIFETIME(USAction, ActionComp);
}