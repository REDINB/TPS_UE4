// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"


class USSaveGame;
//积分变化的宏
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, ASPlayerState*, PlayerState, int32, NewCredits, int32, Delta);
/**
 * 
 */
UCLASS()
class ACTION_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()
protected:
	//积分
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing = "OnRep_Credits", Category = "Credits")
	int32 Credits;

	// 通过传入原有的积分可以简单计算出delta，只能传递相同类型的一个值，如果无法实现则要使用rpc，消耗会比较大
	UFUNCTION()
	void OnRep_Credits(int32 OldCredits);

public:
	
	UFUNCTION(BlueprintCallable, Category = "Credits")
	int32 GetCredits() const;
	
	UFUNCTION(BlueprintCallable, Category = "PlayerState|Credits") // < Category|SubCategory
	void AddCredits(int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "PlayerState|Credits")
	bool RemoveCredits(int32 Delta);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCreditsChanged OnCreditsChanged;

	//存储和读取玩家状态
	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(USSaveGame *SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(USSaveGame *SaveObject);
};
