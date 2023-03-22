// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SItemChest.generated.h"

UCLASS()
class ACTION_API ASItemChest : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
public:	
	//宝箱打开的角度设置
	UPROPERTY(EditAnywhere)
	float TargetPitch;

	//实现交互接口
	void Interact_Implementation(APawn* InstigatorPawn);
	
	//加载游戏时检测箱子是否打开
	void OnActorLoaded_Implementation() override;
	
	// Sets default values for this actor's properties
	ASItemChest();

protected:

	//网络复制变量，变化时在客户端触发回调函数，因此在服务端需要主动调用该回调函数，设置为存储时关心的变量
	UPROPERTY(ReplicatedUsing = "OnRep_LidOpened", BlueprintReadOnly, SaveGame)
	bool bLidOpened;

	//网络同步时客户端触发函数
	UFUNCTION()
	void OnRep_LidOpened();

	//宝箱底座Mesh
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UStaticMeshComponent *BaseMesh;

	//宝想盖子Mesh
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UStaticMeshComponent *LidMesh;
	
};
