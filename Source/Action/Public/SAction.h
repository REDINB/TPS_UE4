// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "SAction.generated.h"


//action复制数据的结构体，用于替换isRunning的同时附带一个执行的actor，可以确认变量同时到达
USTRUCT()
struct FActionRepData
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	bool bIsRunning;

	UPROPERTY()
	AActor *Instigator;
};

class USActionComponent;
class UWorld;
/**
 * 
 */
//使得该类可以作为一种蓝图初始类在引擎中创建
UCLASS(Blueprintable)
class ACTION_API USAction : public UObject
{
	GENERATED_BODY()
protected:

	UPROPERTY(Replicated)
	USActionComponent *ActionComp;
	
	//获取action组件
	UFUNCTION(BlueprintCallable, Category = "Action")
	USActionComponent *GetOwningComponent() const;

	//激活时加入，停止时移出
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer GrantsTags;

	//当该列表中tags生效时行动无法开始
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
    FGameplayTagContainer BlockTags;

	//互斥变量，只有在与原有值不同时才会触发回调函数
	UPROPERTY(ReplicatedUsing = "OnRep_RepData")
	FActionRepData RepData;
	//bool bIsRunning;

	//回调函数，主要用于修改IsRunning变量的状态
	UFUNCTION()
	void OnRep_RepData();
public:

	//初始化action组件
	void Initialize(USActionComponent *NewActionComp);
	
	/* Start immediately when added to an action component */
	//用于buff自动启动，免于各种判定
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	bool bAutoStart;

	//获得互斥锁状态
	UFUNCTION(BlueprintCallable, Category = "Action")
	bool IsRunning() const;
	
	//判断该tag有没有被block
	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	bool CanStart(AActor *Instigator);
	
	//开始action
	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StartAction(AActor* Instigator);
	
	//停止action
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void StopAction(AActor* Instigator);

	//action的名字，使用FName相当于哈希，速度很快
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FName ActionName;

	//重载getworld，用来获得outer
	UWorld* GetWorld() const override;

	//支持网络功能
	bool IsSupportedForNetworking() const override
	{
		return true;
	}
};
