// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_BaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "GetHitGameplayAbility.h"


// Sets default values
ACPP_BaseCharacter::ACPP_BaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 初始化球形碰撞组件
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));

	// 设置碰撞组件的半径
	SphereCollision->InitSphereRadius(380.0f);
	// 将球形碰撞组件附加到角色根组件
	SphereCollision->SetupAttachment(GetRootComponent());

	// 配置碰撞响应
	SphereCollision->SetCollisionProfileName(TEXT("OverlapAll")); // 与所有对象重叠

}

void ACPP_BaseCharacter::HitActor(ACPP_BaseCharacter* Target, FGameplayTag HitLevel, float DamageRate)
{
    return;
}

UAbilitySystemComponent* ACPP_BaseCharacter::GetAbilitySystemComponent() const
{
	return nullptr;
}

void ACPP_BaseCharacter::SetSphereCollisionRadius(float NewRadius)
{
    if (SphereCollision)
    {
        SphereCollision->SetSphereRadius(NewRadius, true); 
    }
}

bool ACPP_BaseCharacter::TryActivateHitAbility(TSubclassOf<UGetHitGameplayAbility> GetHitAbilityClass)
{
    UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	if (!AbilitySystemComponent || !GetHitAbilityClass) return false;

    // 获取新受击GA的优先级
    UGetHitGameplayAbility* NewHitReactAbility = GetHitAbilityClass->GetDefaultObject<UGetHitGameplayAbility>();
    if (!NewHitReactAbility) return false;

    // 检查是否能打断当前受击GA
    if (CurrentHitReactAbility && !CanInterruptCurrentHitReact(NewHitReactAbility->HitReactPriorityTag))
    {
        return false; // 优先级不足，无法激活
    }

    // 打断当前受击GA（如果存在）
    if (CurrentHitReactAbility)
    {
        InterruptCurrentHitReact();
    }

    // 激活新的受击GA
    FGameplayAbilitySpecHandle SpecHandle = AbilitySystemComponent->FindAbilitySpecFromClass(GetHitAbilityClass)->Handle;
    AbilitySystemComponent->TryActivateAbility(SpecHandle);

    // 更新当前受击GA状态
    CurrentHitReactAbility = NewHitReactAbility;
    CurrentHitReactPriority = NewHitReactAbility->HitReactPriorityTag;

    return true;
}

bool ACPP_BaseCharacter::CanInterruptCurrentHitReact(FGameplayTag NewPriorityTag) const
{
	// 优先级比较
    if (NewPriorityTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Player.character.State.GetHit.High"))))
    {
        // High 优先级可以打断任何
        return !NewPriorityTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Player.character.State.GetHit.High")));
    }
    else if (NewPriorityTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Player.character.State.GetHit.Medium"))))
    {
        // Medium 优先级可以打断 Low
        return CurrentHitReactPriority.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Player.character.State.GetHit.Low"))) ||
            CurrentHitReactPriority.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Player.character.State.GetHit.Medium")));
    }
    else if (NewPriorityTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Player.character.State.GetHit.Low"))))
    {
        // Low 优先级不能打断任何
        return CurrentHitReactPriority.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Player.character.State.GetHit.Low")));
    }
    
    return false;
}

void ACPP_BaseCharacter::InterruptCurrentHitReact()
{
    UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	if (CurrentHitReactAbility && AbilitySystemComponent)
    {
        FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(CurrentHitReactAbility->GetClass());
        if (Spec)
        {
            AbilitySystemComponent->CancelAbility(Spec->Ability);
        }
        CurrentHitReactAbility = nullptr;
        CurrentHitReactPriority = FGameplayTag();
    }
}

bool ACPP_BaseCharacter::IsAttacking() const
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (!ASC) return false;
    
    // 检查是否有攻击相关标签
    return ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.Character.Attack.Normal"))) ||
           ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.Character.Attack.SkillOne"))) ||
           ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.Character.Attack.SkillTwo")));
}

void ACPP_BaseCharacter::GiveStartupAbilities()
{
	if (!IsValid(GetAbilitySystemComponent())) return;
	for (const auto& Ability : GamePlayAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability);
		GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
	}
	for (const auto& Ability : GetHitAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability);
		GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
	}
}
