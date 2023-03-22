// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"
#include "DrawDebugHelpers.h"
#include "SGameplayInterface.h"
#include "SWorldUserWidget.h"
#include "Blueprint/UserWidget.h"

//开启调试线的控制台变量
static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("su.InteractionDebugDraw"), false, TEXT("Enable Debug Lines for Interact Component."), ECVF_Cheat);

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//为变量赋初值
	TraceRadius = 30.0f;
	TraceDistance = 500.0f;
	CollisionChannel = ECC_WorldDynamic;
}



// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//寻找交互物品没有必要复制
	APawn *MyPawn = Cast<APawn>(GetOwner());
	if(MyPawn->IsLocallyControlled())
	{
		FindBestInteractable();
	}

}



void USInteractionComponent::FindBestInteractable()
{
	//控制台变量
	bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();

	//设置对象碰撞属性为世界动态
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

	//获得当前组件的所有者
	AActor *MyOwner = GetOwner();

	//获得当前组件所有者的视角位置和角度
	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	//设置视线终点
	FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance);

	//获得视线接触的对象
	//FHitResult Hit;
	//bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);
	
	
	//获得视线接触的对象组，并设置碰撞球（当需要debug搜索对象较小难以对准时可以使用碰撞体）
	TArray <FHitResult> Hits;
	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);
	
	//用球体进行扫射，并将扫射到的对象加入Hits对象组
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);

	//射线颜色根据视线是否命中物体决定
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	//清除聚焦对象的引用
	FocusedActor = nullptr;
	
	//遍历搜索命中结果
	for(FHitResult Hit : Hits)
	{
		if(bDebugDraw)
		{
			//生成debug球
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, TraceRadius, 32, LineColor, false, 0.0f);
		}
		//获得Hit中的actor
		AActor *HitActor = Hit.GetActor();
		if(HitActor)
		{
			if(HitActor->Implements<USGameplayInterface>())
			{
				FocusedActor = HitActor;
				break;
			}
		}
	}

	//如果聚焦到对象
	if(FocusedActor)
	{
		//如果UI实例为空并且分配了UI蓝图类，则创建新的UI实例
		if(DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass))
		{
			DefaultWidgetInstance = CreateWidget<USWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}

		//如果UI实例分配成功，则用选取聚焦对象
		if(DefaultWidgetInstance)
		{
			DefaultWidgetInstance->AttachedActor = FocusedActor;

			//只有当前屏幕没有UI实例，才会将新的UI实例投放到屏幕上
			if(!DefaultWidgetInstance->IsInViewport())
			{
				DefaultWidgetInstance->AddToViewport();
			}
		}
	}
	//如果没有聚焦到对象，则清除之前屏幕上的UI
	else
	{
		if(DefaultWidgetInstance)
		{
			DefaultWidgetInstance->RemoveFromParent();
		}
	}
	
	if(bDebugDraw)
	{
		//生成debug用的射线
		DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0, 0.0f);
	}
}

void USInteractionComponent::PrimaryInteract()
{
	//Server RPC
	ServerInteract(FocusedActor);
}

void USInteractionComponent::ServerInteract_Implementation(AActor *InFocus)
{
	if(InFocus == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "No Focus Actor to interact.");
		return ;
	}
	//将AActor类型安全地转换为Pawn，并且实现人物与物体的交互
	APawn *MyPawn = Cast<APawn>(GetOwner());
	ISGameplayInterface::Execute_Interact(InFocus, MyPawn);
}