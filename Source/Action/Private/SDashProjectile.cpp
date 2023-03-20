// Fill out your copyright notice in the Description page of Project Settings.


#include "SDashProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ASDashProjectile::ASDashProjectile()
{
	TeleportDelay = 0.2f;
	DetonateDelay = 0.2f;
	//重新设置粒子初始速度
	MovementComp->InitialSpeed = 6000.f;
}


void ASDashProjectile::BeginPlay()
{
	Super::BeginPlay();

	//获得计时器方法
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedDetonate, this, &ASDashProjectile::Explode, DetonateDelay);
}

//如果提前爆炸就提前消除粒子各种属性并且调用传送函数
void ASDashProjectile::Explode_Implementation()
{
	//如果爆炸函数已经在另一个源文件中被调用就清除计时器
	GetWorldTimerManager().ClearTimer(TimerHandle_DelayedDetonate);

	//设置粒子位置
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

	//停止特效
	EffectComp->DeactivateSystem();

	//停止移动和碰撞
	MovementComp->StopMovementImmediately();
	SetActorEnableCollision(false);

	//设置计时器调用传送函数
	FTimerHandle TimerHandle_DelayedTeleport;
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedTeleport, this, &ASDashProjectile::TeleportInstigator, TeleportDelay);

	// Skip base implementation as it will destroy actor
	//Super::Explode_Implementation();
}

//传送函数
void ASDashProjectile::TeleportInstigator()
{
	//获得技能发起者
	AActor* ActorToTeleport = GetInstigator();
	if (ensure(ActorToTeleport))
	{
		// Keep instigator rotation or it may end up jarring
		//保持发起者角度，不然可能卡墙？
		ActorToTeleport->TeleportTo(GetActorLocation(), ActorToTeleport->GetActorRotation(), false, false);
	}
}


