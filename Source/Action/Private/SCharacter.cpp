// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "SInteractionComponent.h"
#include "Kismet/GameplayStatics.h"

ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//为弹簧臂创建对象并且绑定在根组件上
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	//开启使用pawn控制旋转，这样在区分摄像机和角色旋转的同时，我们也不能再控制角色旋转
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);
	
	//为相机创建对象并且绑定在弹簧臂上
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);
	
	//获取角色移动组件，开启将旋转朝向移动，在左右移动时角色会转向相应的方向，而不是侧着身子漂移
	GetCharacterMovement()->bOrientRotationToMovement = true;
	//关闭使用控制器旋转Yaw，使得鼠标控制旋转和角色旋转分开控制
	bUseControllerRotationYaw = false;

	//添加交互和属性组件
	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");

	//受击特效参数，手部位置的名称
	TimeToHitParamName = "TimeToHit";
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//绑定死亡事件
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//绑定轴映射，绑定前后移动操作，-1到1表示后退和前进，绑定this人物对象，并且调用自己实现向前移动的函数
	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	//绑定轴映射，通过引擎内输入设置绑定鼠标X，实现左右转动头部效果
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	
	//绑定操作映射，通过点击鼠标左键发射出魔法粒子
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	//绑定跳跃
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//绑定主要交互功能
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);

	//绑定黑洞和传送技能
	PlayerInputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ASCharacter::BlackHoleAttack);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ASCharacter::Dash);

	//开始冲刺和结束冲刺
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::SprintStop);
}

void ASCharacter::HealSelf(float Amount /* = 100 */)
{
	AttributeComp->ApplyHealthChange(this, Amount);
}

//主要交互
void ASCharacter::PrimaryInteract()
{
	if(InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

//开始冲刺
void ASCharacter::SprintStart()
{
	ActionComp->StartActionByName(this, "Sprint");
}

//结束冲刺
void ASCharacter::SprintStop()
{
	ActionComp->StopActionByName(this, "Sprint");
}

//主要攻击
void ASCharacter::PrimaryAttack()
{
	ActionComp->StartActionByName(this, "PrimaryAttack");
	//播放攻击动画
	//StartAttackEffects();

	//设置定时器，用于使攻击动画和攻击粒子产生时间同步
	//GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimeElapsed, 0.2f);

	//用于清理计时器，例如角色在释放动画中途死亡
	//GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);

}

//黑洞攻击
void ASCharacter::BlackHoleAttack()
{
	ActionComp->StartActionByName(this, "BlackHole");
}

//闪现
void ASCharacter::Dash()
{
	ActionComp->StartActionByName(this, "Dash");
}

//生命值发生变化事件
void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	//判断为受到伤害，添加受击特效
	if (Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		// 添加受击怒气，怒气值为伤害的绝对值
		float RageDelta = FMath::Abs(Delta);
		AttributeComp->ApplyRage(InstigatorActor, RageDelta);
	}
	if(NewHealth <= 0.0f && Delta < 0.0f)
	{
		//生命值小于等于0时获得角色的controller并禁用
		APlayerController *PC = Cast<APlayerController>(GetController());
		DisableInput(PC);

		//设置生命期
		SetLifeSpan(5.0f);
	}
}

//重载pawn的视线为摄像机所在位置，便于交互功能瞄准
FVector ASCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

void ASCharacter::MoveForward(float Value)
{
	//获得控制器的向量，使得角色前进方向为控制器方向，而不是原来的方向（也就是向当前摄像机朝向方向前进，而不是原来角色朝向）
	FRotator ControlRot = GetControlRotation();
	//确保控制方向在上下为0，防止进入地板或者飞天
	//ControlRot.Pitch = 0.0f;
	//ControlRot.Roll = 0.0f;
	
	//前后移动函数的实现
	AddMovementInput(ControlRot.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	//同理，使角色左右转的方向为控制器方向，也即是摄像机朝向方向
	FRotator ControlRot = GetControlRotation();
	//确保控制方向在上下为0，防止进入地板或者飞天
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	//X： Forward（红色），Y： Right（绿色），Z：Up（蓝色）
	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	
	//左右移动函数的实现
	AddMovementInput(RightVector, Value);
}
