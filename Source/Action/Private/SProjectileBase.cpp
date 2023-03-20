// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileBase.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"


// Sets default values
ASProjectileBase::ASProjectileBase()
{
 	//创建物理碰撞组件。将碰撞组件设置为根组件
    SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
    //设置自定义碰撞规则，名称为Projectile
    SphereComp->SetCollisionProfileName("Projectile");
    
    //为球组件添加动态触发事件
    SphereComp->OnComponentHit.AddDynamic(this, &ASProjectileBase::OnActorHit);
    RootComponent = SphereComp;
    	
    	
	//创建特效组件，将特效绑定于碰撞组件
	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);

	//创建音效组件
	AudioComp = CreateDefaultSubobject<UAudioComponent>("AudioComp");
	AudioComp->SetupAttachment(RootComponent);

    	
    //创建运动组件，设置初速度
    MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComp");
    MovementComp->InitialSpeed = 8000.0f;
    //旋转跟踪速度，每帧将更新粒子旋转以匹配当前的速度
    MovementComp->bRotationFollowsVelocity = true;
    //本地空间中的初始速度，如果不设置粒子将会始终往一个方向发射
    MovementComp->bInitialVelocityInLocalSpace = true;
    MovementComp->ProjectileGravityScale = 0.0f;
	
	//设置镜头摇晃的范围
	ImpactShakeInnerRadius = 0.0f;
	ImpactShakeOuterRadius = 1500.0f;

	SetReplicates(true);
}

void ASProjectileBase::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//命中时调用爆炸函数
	Explode();
}

void ASProjectileBase::Explode_Implementation()
{
	//防止内存泄露？
	if(ensure(!IsPendingKill()))
	{
		//发射粒子的特效
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());
		//粒子的声效
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		//镜头晃动效果
		UGameplayStatics::PlayWorldCameraShake(this, ImpactShake, GetActorLocation(), ImpactShakeInnerRadius, ImpactShakeOuterRadius);

		Destroy();
	}
}

void ASProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}


