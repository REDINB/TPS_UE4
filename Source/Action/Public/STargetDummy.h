// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STargetDummy.generated.h"


class UStaticMeshComponent;
class USAttributeComponent;

UCLASS()
class ACTION_API ASTargetDummy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTargetDummy();

protected:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *MeshComp;

	UPROPERTY(VisibleAnywhere)
	USAttributeComponent *AttributeComp;

	//属性插件中自定义的宏事件，用于发起生命值变化后的事件
	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);


};
