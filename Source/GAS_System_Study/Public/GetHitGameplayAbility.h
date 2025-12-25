// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GetHitGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API UGetHitGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
    UGetHitGameplayAbility();

    // 激活受击GA
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    // 结束受击GA
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    // 受击优先级标签
    UPROPERTY(EditDefaultsOnly, Category = "Priority")
    FGameplayTag HitReactPriorityTag;

    // 受击动画
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* HitReactMontage;
};
