// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"

#include "DrawDebugHelpers.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	//创建对象
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	//启动物理效果
	MeshComp->SetSimulatePhysics(true);
	//设置碰撞方式
	MeshComp->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	RootComponent = MeshComp;

	ForceComp = CreateDefaultSubobject<URadialForceComponent>("ForceComp");
	//绑定对象
	ForceComp->SetupAttachment(MeshComp);
	//自动激活组件
	ForceComp->SetAutoActivate(true);
	//设置爆炸半径和爆炸力度
	ForceComp->Radius = 750.0f;
	ForceComp->ImpulseStrength = 700.0f;
	//冲量速度变更，将无视物体当前运动状态和质量大小
	ForceComp->bImpulseVelChange = true;
	//增加碰撞对象类型
	ForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASExplosiveBarrel::PostInitializeComponents()
{
	//执行函数原本的功能
	Super::PostInitializeComponents();
	//绑定到OnComponentHit事件上
	MeshComp->OnComponentHit.AddDynamic(this, &ASExplosiveBarrel::OnActionHit);
}

void ASExplosiveBarrel::OnActionHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ForceComp->FireImpulse();

	//UE引擎的输出日志信息
	UE_LOG(LogTemp, Log, TEXT("OnActionHit in Explosive Barrel"));
	//安全地输出actor和世界时间
	UE_LOG(LogTemp, Warning, TEXT("OtherActor:%s, at game timer:%f"), *GetNameSafe(OtherActor), GetWorld()->TimeSeconds);

	//将信息输出在游戏界面上
	FString CombinedString = FString::Printf(TEXT("Hit at location:%s"), *Hit.ImpactPoint.ToString());
	DrawDebugString(GetWorld(), Hit.ImpactPoint, CombinedString, nullptr, FColor::Green, 2.0f, true);
}


// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

