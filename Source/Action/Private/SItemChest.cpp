// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemChest.h"

#include "Net/UnrealNetwork.h"

// Sets default values
ASItemChest::ASItemChest()
{
	//宝箱底座
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BashMesh"));
	RootComponent = BaseMesh;
	//宝箱盖子
	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(BaseMesh);
	//设置宝箱旋转角度
	TargetPitch = 110;

	//设置该变量可网络复制
	SetReplicates(true);

}


void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	//取相反结果
	bLidOpened = !bLidOpened;
	//在服务端手动调用，否则只在客户端运行
	OnRep_LidOpened();
}

void ASItemChest::OnActorLoaded_Implementation()
{
	OnRep_LidOpened();
}

void ASItemChest::OnRep_LidOpened()
{
	float CurrPitch = bLidOpened ? TargetPitch : 0.0f;
	//使盖子Mesh沿当前位置旋转
	LidMesh->SetRelativeRotation(FRotator(CurrPitch, 0, 0));
}

void ASItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//将属性同步给所有客户端
	DOREPLIFETIME(ASItemChest, bLidOpened);
}