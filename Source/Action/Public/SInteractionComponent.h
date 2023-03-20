// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SInteractionComponent.generated.h"

class USWorldUserWidget;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTION_API USInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	//reliable类似TCP，如果没收到回复将会一直发送，确保执行
	//unreliable类似UDP，不保证能够到达，也不会重试
	//Server RPC
	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor *InFocus);

	//扫描获得视线最先碰到的物体，并将其赋为交互物体
	void FindBestInteractable();

	//目前聚焦的交互对象
	UPROPERTY()
	AActor* FocusedActor;

	//视线跟踪的距离
	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceDistance;

	//视线跟踪的半径
	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceRadius;

	//碰撞通道，由于该类是引擎提供的枚举类，因此需要用这种设置方法
	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	//UI的蓝图类
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USWorldUserWidget> DefaultWidgetClass;

	//UI的实例类
	UPROPERTY()
	USWorldUserWidget *DefaultWidgetInstance;

	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	USInteractionComponent();
	
	//主要互动函数
	void PrimaryInteract();
};
