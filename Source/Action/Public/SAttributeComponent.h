// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"

//自定义广播宏，当角色生命值发生变化后主动广播，包含四个对象，分别是事件发出者，组件拥有者，新的生命值和生命变化值，使用事件属性为FOnHealthChanged。
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewHealth, float, Delta);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnRageChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewRage, float, Delta);

// 通用宏
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAttributeChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewValue, float, Delta);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTION_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	//返回属性的静态类的统一静态方法
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static USAttributeComponent *GetAttributes(AActor *FromActor);
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static bool IsActorAlive(AActor *Actor);
	
	// Sets default values for this component's properties
	USAttributeComponent();

protected:

	//广播健康值变化，如果这里使用unreliable，可能在丢包时产生bug
	//因为属性值变量关系到人物死亡重生的重要逻辑，但是在这里使用reliable会严重影响性能。
	UFUNCTION(NetMulticast, Reliable)
	void MulticastHealthChanged(AActor *InstigatorActor, float NewHealth, float Delta);

	//其他可能属性：HealthMax, Stamina, Strength
	//生命值，只有在蓝图中才能编辑，进入关卡中以后就不能再编辑了，同时分类属性为Attributes
	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category = "Attributes")
	float Health;
	//生命最大值
	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category = "Attributes")
	float HealthMax;

	/* Resource used to power certain Actions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float Rage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float RageMax;

public:

	//杀死机器人，用于杀死所有机器人控制台指令
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool Kill(AActor *InstigatorActor);

	//用于动画中快速判定角色是否还活着
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsAlive() const;

	//判断生命是否是满的
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsFullHealth() const;

	//获取当前生命值
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealth() const;
	
	//获取当前人物的生命最大值
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealthMax() const;
	
	//自己定义的事件，用于生命值产生变化时主动引起生命值ui变化
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnAttributeChanged OnRageChanged;

	
	//用于改变健康值的函数，如造成伤害或是回复生命
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(AActor *InstigatorActor, float Delta);
	
	UFUNCTION(BlueprintCallable)
	float GetRage() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyRage(AActor* InstigatorActor, float Delta);
	
};
