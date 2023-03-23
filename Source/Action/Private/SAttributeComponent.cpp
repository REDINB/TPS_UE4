// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"

//改变伤害系数的控制台变量
static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for Attribute Component."), ECVF_Cheat);

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	//初始默认生命值为100，上限为100
	HealthMax = 100;
	Health = HealthMax;

	//初始怒气默认为0，上限为100
	Rage = 0;
	RageMax = 100;

	//设置为默认进行网络复制
	SetIsReplicatedByDefault(true);
}


bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetHealthMax());
}

bool USAttributeComponent::IsAlive() const
{
	//生命大于0则活
	return Health > 0;
}

bool USAttributeComponent::IsFullHealth() const
{
	//生命值满
	return Health == HealthMax;
}

float USAttributeComponent::GetHealth() const
{
	return Health;
}


float USAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}


bool USAttributeComponent::ApplyHealthChange(AActor *InstigatorActor, float Delta)
{
	if(!GetOwner()->CanBeDamaged() && Delta < 0.0f)
	{
		return false;
	}

	//当判定为伤害时
	if (Delta < 0.0f)
	{
		//攻击乘上伤害系数
		float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();
		Delta *= DamageMultiplier;
	}
	
	float OldHealth = Health;
	float NewHealth = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

	float ActualDelta = NewHealth - OldHealth;
	//限制客户端权限
	if(GetOwner()->HasAuthority())
	{
		//使得Health的区间大于0小于最大生命值
		Health = NewHealth;
		
		//调用生命值变化的广播事件
		//OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);
		//该广播事件如果是客户端则会在本地运行，如果是服务端则会广播到客户端
		if(ActualDelta != 0.0f)
		{
			MulticastHealthChanged(InstigatorActor, Health, ActualDelta);
		}
		//生命值为零时死亡，gamemode只在服务端上运行，在客户端没有必要花费时间验证
		if(ActualDelta < 0.0f && Health == 0.0f)
		{
			ASGameModeBase *GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
			if(GM)
			{
				//调用击杀玩家重生功能
				GM->OnActionKilled(GetOwner(), InstigatorActor);
			}
		}
	}
	return ActualDelta != 0;


}

float USAttributeComponent::GetRage() const
{
	return Rage;
}

//更改怒气
bool USAttributeComponent::ApplyRage(AActor* InstigatorActor, float Delta)
{
	float OldRage = Rage;

	Rage = FMath::Clamp(Rage + Delta, 0.0f, RageMax);

	float ActualDelta = Rage - OldRage;
	if (ActualDelta != 0.0f)
	{
		OnRageChanged.Broadcast(InstigatorActor, this, Rage, ActualDelta);
	}

	return ActualDelta != 0;
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if(FromActor)
	{
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(StaticClass()));
	}
	return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* Actor)
{
	USAttributeComponent *AttributeComp = GetAttributes(Actor);
	if(AttributeComp)
	{
		return AttributeComp->IsAlive();
	}
	return false;
}

//该函数包含在.generated.h头文件中
void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAttributeComponent, Health);
	DOREPLIFETIME(USAttributeComponent, HealthMax);

	DOREPLIFETIME(USAttributeComponent, Rage);
	DOREPLIFETIME(USAttributeComponent, RageMax);
	//可以根据情况使用条件节省资源
	//DOREPLIFETIME_CONDITION(USAttributeComponent, HealthMax, COND_InitialOnly);
}

void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor,this, NewHealth, Delta);
}

void USAttributeComponent::MulticastRageChanged_Implementation(AActor* InstigatorActor, float NewRage, float Delta)
{
	OnRageChanged.Broadcast(InstigatorActor, this, NewRage, Delta);
}