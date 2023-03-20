// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SAction_ProjectileAttack.generated.h"

class UAnimMontage;
class UParticleSystem;
/**
 * 
 */
UCLASS()
class ACTION_API USAction_ProjectileAttack : public USAction
{
	GENERATED_BODY()
	protected:
		//选择粒子类别
    	UPROPERTY(EditAnywhere, Category = "Attack")
    	TSubclassOf<AActor> ProjectileClass;
	
		//发出技能手部名称
    	UPROPERTY(VisibleAnywhere, Category = "Effects")
    	FName HandSocketName;
	
		//攻击动作和粒子产生的延迟
    	UPROPERTY(EditDefaultsOnly, Category = "Attack")
    	float AttackAnimDelay;
	
		//攻击的动画
    	UPROPERTY(EditAnywhere, Category = "Attack")
    	UAnimMontage* AttackAnim;
    
    	//攻击动作产生时的特效
    	UPROPERTY(EditAnywhere, Category = "Attack")
    	UParticleSystem* CastingEffect;

		//延迟攻击的定时器
    	UFUNCTION()
    	void AttackDelay_Elapsed(ACharacter* InstigatorCharacter);
    
    public:
		//开始action的实现
    	virtual void StartAction_Implementation(AActor* Instigator) override;
    	USAction_ProjectileAttack();
};
