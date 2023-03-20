// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "AITypes.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SActionEffect.h"
#include "SGameFunctionLibrary.h"
#include "Components/SphereComponent.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
	//重新设置碰撞组件半径，并为其设置重载后的函数
	SphereComp->SetSphereRadius(20.0f);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);
	//设置伤害为20
	DamageAmount = 20.0f;


}

void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//获得粒子碰撞者，并且设置无法对自己造成伤害
	if (OtherActor && OtherActor != GetInstigator())
	{
		// //获得受击者身上的属性组件，用于更改受击者属性
		// USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));
		// if (AttributeComp)
		// {
		// 	//命中时造成伤害并产生爆炸效果
		// 	AttributeComp->ApplyHealthChange(GetInstigator(), -DamageAmount);
		// 	
		// 	// Only explode when we hit something valid
		// 	Explode();
		// }
		
		//当玩家被攻击时获得其身上的action组件，如果该玩家身上拥有反弹的tag，则将子弹反弹会反方向
		USActionComponent *ActionComp = Cast<USActionComponent>(OtherActor->GetComponentByClass(USActionComponent::StaticClass())); 
		if(ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			//反转粒子速度
			MovementComp->Velocity = -MovementComp->Velocity;
			//由于粒子无法命中发射者，因此将发射者转换为玩家
			SetInstigator(Cast<APawn>(OtherActor));
			return ;
		}
		//调用函数库的造成伤害和物理事件函数
		if(USGameFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
		{
			//产生爆炸效果
			Explode();

			//给粒子附加燃烧tag
			if(ActionComp)
			{
				ActionComp->AddAction(GetInstigator(), BurningActionClass);
			}
		}
	}
}

