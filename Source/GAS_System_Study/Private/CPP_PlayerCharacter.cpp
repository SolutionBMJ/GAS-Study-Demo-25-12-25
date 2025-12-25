  // Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_PlayerCharacter.h"
#include "CPP_PlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GamingPlayerUI.h"
#include "GASGameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerAttributeSet.h"

ACPP_PlayerCharacter::ACPP_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

UAbilitySystemComponent* ACPP_PlayerCharacter::GetAbilitySystemComponent() const          //获取能力系统组件
{
	return AbilitySystemComponent;    //若获取到有效玩家状态，返回能力组件，否则返回空指针
}

void ACPP_PlayerCharacter::PossessedBy(AController* NewController)                //该Pawn被持有时
{
	UE_LOG(LogTemp, Error, TEXT("Possesses Character: %s"), *CharacterName);
	Super::PossessedBy(NewController);

	ApplyEquipmentData();

	//如果未获取到能力组件，则返回空
	if (!IsValid(GetAbilitySystemComponent()) || !HasAuthority()) return;

	GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);         //初始化能力对象信息

	GiveStartupAbilities();

	ACPP_PlayerState* APlayerState = Cast<ACPP_PlayerState>(GetPlayerState());            //获取玩家状态

	// 切换时为奔跑状态则同步奔跑，若为行走则同步行走,都不是则正常
	if (APlayerState && APlayerState->GetRunningState()) GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
	else if (APlayerState && APlayerState->GetWalkingState()) GetCharacterMovement()->MaxWalkSpeed = 150.0f;
	else GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	

    ACPP_PlayerController* PlayerController = Cast<ACPP_PlayerController>(NewController);
    if (!IsValid(PlayerController)) return;

	PlayerController->SetCameraFollowTarget(this);
	PlayerController->PlayerUI->UpdateHealth(
		GetCharacterAS()->GetHealth(), 
		GetCharacterAS()->GetMaxHealth()
	);

	if (!IsValid(GameInstance))
	{
		GameInstance = Cast<UGASGameInstance>(GetGameInstance());
	}
	if (!IsValid(GameInstance) || IsValid(GameInstance->CurrentPlayerState)) return;
	GameInstance->CurrentPlayerState = Cast<ACPP_PlayerState>(GetPlayerState());
}

void ACPP_PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (!IsValid(GetAbilitySystemComponent())) return;                               //如果未获取到能力组件，则返回空

	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);         //初始化能力对象信息
}

void ACPP_PlayerCharacter::HitActor(ACPP_BaseCharacter* Target, FGameplayTag HitLevel, float DamageRate)
{
	if (!Target) return;

    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponent();
    UAbilitySystemComponent* TargetASC = Target->GetAbilitySystemComponent();
    if (!SourceASC || !TargetASC) return;

    // 获取伤害GameplayEffect
    if (!IsValid(DamageGEClass)) return;

    FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	Context.AddSourceObject(this); // 显式将当前玩家角色作为源对象绑定到上下文

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
		DamageGEClass,
		1,
		Context // 使用绑定了源对象的上下文
	);

    FGameplayTag DamageMultiplierTag = FGameplayTag::RequestGameplayTag(FName("Data.DamageRate"));
    // 新增：确认设置SetByCaller是否成功
    if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
    {
        SpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageMultiplierTag, DamageRate);
		SpecHandle.Data.Get()->DynamicGrantedTags.AddTag(HitLevel);
    }
    
    // 将效果应用到目标
    AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}

bool ACPP_PlayerCharacter::GetPlayerStateOnRun()
{
	ACPP_PlayerState* APlayerState = Cast<ACPP_PlayerState>(GetPlayerState());            //获取玩家状态
	if (!APlayerState) return false;
	return APlayerState->GetRunningState();
}

UPlayerAttributeSet* ACPP_PlayerCharacter::GetCharacterAS()
{
	return const_cast<UPlayerAttributeSet*>(AbilitySystemComponent->GetSet<UPlayerAttributeSet>());
}

void ACPP_PlayerCharacter::SetStaminaTimer()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(StaminaTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(StaminaTimer);
	}
	GetWorld()->GetTimerManager().SetTimer(
		StaminaTimer,
		this,
		&ACPP_PlayerCharacter::RecoverStamina,
		0.5f,
		true
	);
}

void ACPP_PlayerCharacter::RecoverStamina()
{
	if (!StaminaRecoverClass) return;
	UPlayerAttributeSet* CharacterAS = GetCharacterAS();
	if (!CharacterAS) return;

	if (CharacterAS->GetStamina() >= CharacterAS->GetMaxStamina())
    {
        GetWorld()->GetTimerManager().ClearTimer(StaminaTimer);
        UE_LOG(LogTemp, Log, TEXT("%s: Stamina is full, stop recovery."), *GetName());
        return;
    }

	FGameplayEffectContextHandle Context = GetAbilitySystemComponent()->MakeEffectContext();
    Context.AddSourceObject(this);  // 确保效果关联当前角色

	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(
        StaminaRecoverClass,
        1,  // 效果等级
        Context
    );

	if (!SpecHandle.IsValid() || !SpecHandle.Data.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Failed to create StaminaRecover Spec!"), *GetName());
        return;
    }

	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void ACPP_PlayerCharacter::HideSelf()
{
    if (IsValid(GetController()))
    {
		return;
    }
    // 动画播放完毕，隐藏旧角色
    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);
    SetActorEnableCollision(false);
    GetCharacterMovement()->Velocity = FVector::ZeroVector;
}

void ACPP_PlayerCharacter::ApplyEquipmentData()
{
	UE_LOG(LogTemp, Log, TEXT("Start Calculate Equipment Improve!"));
	// 从游戏实例获取装备数据
	UGASGameInstance* _GameInstance = Cast<UGASGameInstance>(GetGameInstance());
	if (!_GameInstance) return;

	UAccountData* CurrentAccount = _GameInstance->CurrentLoginAccount;
	if (!CurrentAccount) return;

	UPlayerAttributeSet* PlayerAS = GetCharacterAS();
	if (!PlayerAS) return;

	// 清空旧加成
	if (EquipmentEffectHandle.IsValid() && AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(EquipmentEffectHandle);
		EquipmentEffectHandle.Invalidate();
	}

	// 计算总装备加成
	float TotalAttack = 0.0f;
	float TotalDefence = 0.0f;
	float TotalHealth = 0.0f;

	// 遍历角色已装备的装备
	for (const FEquipmentData& Equip : CurrentAccount->OwnedEquipments)
	{
		if (Equip.bIsEquipped && Equip.EquippedToCharacter == CharacterName)
		{
			TotalAttack += Equip.AttackBonus;
			TotalDefence += Equip.DefenceBonus;
			TotalHealth += Equip.HealthBonus;
		}
	}

	// 如果没有加成，直接返回
    if (TotalAttack == 0.0f && TotalDefence == 0.0f && TotalHealth == 0.0f) return;

	// 计算各项加成具体加成值
	TotalAttack = PlayerAS->GetAttack() * TotalAttack;
	TotalDefence = PlayerAS->GetDefence() * TotalDefence;
	TotalHealth = PlayerAS->GetHealth() * TotalHealth;

	if (!AbilitySystemComponent || !EquipmentEffectClass) return;
    
    // 创建效果上下文
    FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
    Context.AddSourceObject(this);

    // 创建GE Spec并设置SetByCaller数值
    FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EquipmentEffectClass, 1.0f, Context);
    if (SpecHandle.IsValid())
    {
        // 通过SetByCaller传递加成值（标签需与预设GE中的配置一致）
        FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(FName("Equipment.Attack"));
        FGameplayTag DefenseTag = FGameplayTag::RequestGameplayTag(FName("Equipment.Defense"));
        FGameplayTag HealthTag = FGameplayTag::RequestGameplayTag(FName("Equipment.Health"));

        SpecHandle.Data.Get()->SetSetByCallerMagnitude(AttackTag, TotalAttack);
        SpecHandle.Data.Get()->SetSetByCallerMagnitude(DefenseTag, TotalDefence);
        SpecHandle.Data.Get()->SetSetByCallerMagnitude(HealthTag, TotalHealth);

        // 应用GE
        EquipmentEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		ACPP_PlayerController* PC = Cast<ACPP_PlayerController>(GetController());
		if (!PC) return;
    }
}

