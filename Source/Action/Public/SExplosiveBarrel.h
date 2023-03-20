// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"

class UStaticMeshComponent;
class URadialForceComponent;

UCLASS()
class ACTION_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

protected:
	//静态网格体组件，用于计算碰撞
	UPROPERTY(VisibleAnywhere)
	 UStaticMeshComponent *MeshComp;

	//力场组件，用于生成爆炸时的作用力
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	URadialForceComponent *ForceComp;

	//后处理？
	virtual void PostInitializeComponents() override;
	
	//设置触发函数
	UFUNCTION(BlueprintCallable)
	void OnActionHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
