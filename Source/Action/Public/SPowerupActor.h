// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

class USphereComponent;

UCLASS(Abstract)
class ACTION_API ASPowerupActor : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
	
protected:

	//重生时间
	UPROPERTY(EditAnywhere, Category = "Powerup")
	float RespawnTime;

	//显示，冷却时期隐藏，设置状态
	UFUNCTION()
	void ShowPowerup();

	void HideAndCooldownPowerup();

	void SetPowerupState(bool bNewIsActive);

	//碰撞和外观
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;
public:

	//继承的Gameplay接口的交互实现
	void Interact_Implementation(APawn* InstigatorPawn) override;
	
	ASPowerupActor();

};
