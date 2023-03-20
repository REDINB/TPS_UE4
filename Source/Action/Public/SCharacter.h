// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class USActionComponent;
class USpringArmComponent;
class UCameraComponent;
class USInteractionComponent;
class UAnimMontage;
class USAttributeComponent;

UCLASS()
class ACTION_API ASCharacter : public ACharacter
{
	GENERATED_BODY()
protected:

	/* VisibleAnywhere = read-only, still useful to view in-editor and enforce a convention. */
	//为材质设置命中时间？
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParamName;
	//TSubclassof是安全的模板类，一般设置后在引擎中选择对应（Actor）属性的蓝图类
	
	
	//使得该对象属性在编辑器中可见
	//弹簧臂组件，用于连接相机，实现相机跟随。此外，弹簧臂是一个碰撞体，当角色靠近墙壁时相机不会穿模到墙的另一边，保证玩家视野不会因为墙壁遮挡自己的角色。
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent *SpringArmComp;
	
	//相机组件
	UPROPERTY(VisibleAnywhere)
	UCameraComponent *CameraComp;

	//交互组件
	UPROPERTY(VisibleAnywhere)
	USInteractionComponent *InteractionComp;

	//生命值组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent *AttributeComp;

	//ACTION组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USActionComponent* ActionComp;
	
	//人物交互功能
	void PrimaryInteract();
	
	//前进后退功能
	void MoveForward(float Value);
	void MoveRight(float Value);
	
	//开始冲刺和冲刺结束
	void SprintStart();
	void SprintStop();

	//基础攻击
	void PrimaryAttack();

	//黑洞技能
	void BlackHoleAttack();

	//传送技能
	void Dash();
	
	//属性插件中自定义的宏事件，用于发起生命值变化后的事件
	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	//重载actor的视线函数为摄像机组件
	virtual FVector GetPawnViewLocation() const override;

	virtual void PostInitializeComponents() override;

public:
	
	ASCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//控制台命令，恢复生命值
	UFUNCTION(Exec)
	void HealSelf(float Amount = 100);

};
