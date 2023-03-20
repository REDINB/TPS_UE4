// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameFunctionLibrary.h"

#include "SAttributeComponent.h"

bool USGameFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount)
{
	//获得受击者的属性组件
	USAttributeComponent *AttributeComp = USAttributeComponent::GetAttributes(TargetActor);
	//如果有属性组件就造成伤害
	if(AttributeComp)
	{
		return AttributeComp->ApplyHealthChange(DamageCauser, -DamageAmount);
	}
	return false;
}

bool USGameFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult)
{
	//如果成功造成伤害
	if (ApplyDamage(DamageCauser, TargetActor, DamageAmount))
	{
		//获得受击组件
		UPrimitiveComponent *HitComp = HitResult.GetComponent();
		//如果该受击者拥有组件并且命中点开启了物理模拟，则为其添加冲击力
		if(HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			HitComp->AddImpulseAtLocation(-HitResult.ImpactNormal * 300000.0f, HitResult.ImpactPoint, HitResult.BoneName);
		}
		return true;
	}
	return false;
}
