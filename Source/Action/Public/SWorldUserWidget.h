// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SWorldUserWidget.generated.h"


class USizeBox;
/**
 * 
 */
UCLASS()
class ACTION_API USWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()
protected:

	//一个SizeBox，能够比较好地控制size
	UPROPERTY(meta=(BindWidget))
	USizeBox *ParentSizeBox;

	//tick
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:

	//设置UI的世界偏移
	UPROPERTY(EditAnywhere, Category = "UI")
	FVector WorldOffset;

	//在蓝图中选择的蓝图类，关联机器人
	UPROPERTY(BlueprintReadWrite, Category = "UI", meta = (ExposeOnSpawn = true))
	AActor *AttachedActor;
};
