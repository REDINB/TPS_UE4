// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

ASPowerupActor::ASPowerupActor()
{
	//设置新碰撞类型为Powerup
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Powerup");
	RootComponent = SphereComp;

	//为所有物品类添加外形组件
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	// Disable collision, instead we use SphereComp to handle interaction queries
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetupAttachment(RootComponent);
	//重生时间
	RespawnTime = 10.0f;
	bIsActive = true;

	//开启网络同步
	SetReplicates(true);
}


void ASPowerupActor::Interact_Implementation(APawn* InstigatorPawn)
{
	// logic in derived classes...
}


void ASPowerupActor::ShowPowerup()
{
	//可见
	SetPowerupState(true);
}


void ASPowerupActor::HideAndCooldownPowerup()
{
	//隐藏
	SetPowerupState(false);

	FTimerHandle TimerHandle_RespawnTimer;
	GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPowerupActor::ShowPowerup, RespawnTime);
}

void ASPowerupActor::SetPowerupState(bool bNewIsActive)
{
	//设置交互物体的状态
	bIsActive = bNewIsActive;
	OnRep_IsActive();
}
void ASPowerupActor::OnRep_IsActive()
{
	//设置碰撞和是否可见
	SetActorEnableCollision(bIsActive);
	RootComponent->SetVisibility(bIsActive, true);
}

//设置复制体的生命期？
void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASPowerupActor, bIsActive);
}
