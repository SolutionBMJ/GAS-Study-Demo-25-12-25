// Solution_BMJ


#include "GetHitGameplayAbility.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

UGetHitGameplayAbility::UGetHitGameplayAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGetHitGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 设置Ability Priority Tag
    if (HitReactPriorityTag.IsValid())
    {
        ActorInfo->AbilitySystemComponent->AddLooseGameplayTag(HitReactPriorityTag);
    }

    // 执行Ability
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
    // 播放受击动画
    ACharacter* AvatarCharacter = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (AvatarCharacter)
    {
        UAnimInstance* AnimInstance = AvatarCharacter->GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(HitReactMontage);
        }
    }
}

void UGetHitGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (HitReactPriorityTag.IsValid() && ActorInfo->AbilitySystemComponent.IsValid())
    {
        ActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(HitReactPriorityTag);
    }
    
    // 2. 继续执行EndAbility
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
