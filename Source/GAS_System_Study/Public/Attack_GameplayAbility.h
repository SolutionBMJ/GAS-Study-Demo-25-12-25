// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "CPP_PlayerCharacter.h"
#include "CPP_PlayerController.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "PlayerAttributeSet.h"
#include "Attack_GameplayAbility.generated.h"

UCLASS()
class GAS_SYSTEM_STUDY_API UAttack_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UAttack_GameplayAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// 播放指定分段的动画
	void PlaySection();

	void RemoveTag();

	void ResetCombo();

	UFUNCTION(BlueprintImplementableEvent)
	void StartCoolDown();

	// 技能优先级标签
	UPROPERTY(EditDefaultsOnly, Category = "Tag | Priority")
    FGameplayTag AbilityPriorityTag;

	UPROPERTY(EditDefaultsOnly, Category = "Attach")
    bool WillFaceToTarget;

	// 该GA是否可循环
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
    bool CanLoop;

	UPROPERTY(EditDefaultsOnly, Category = "LevelSequence")
    bool WillPlayLevelSequence;

	FGameplayTag GetAttackTag() { return AttackTag; }

protected:
	// Montage 动画
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation|ComboList")
	UAnimMontage* ComboMontages;

	// 分段名列表
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation|SectionName")
	TArray<FName> SectionName;

	//连段重置时间
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	float ComboResetTime = 1.0f;

	// GA标签
	UPROPERTY(EditDefaultsOnly, Category = "Tag | Ability Tag")
	FGameplayTag AttackTag;

	// 索敌半径
	UPROPERTY(EditDefaultsOnly, Category = "EnemyDetection", meta = (MinValue = 10.0))
    float GADetectionRadius = 380.0f;

	// 角色指针
	UPROPERTY()
	TWeakObjectPtr<ACPP_PlayerCharacter> CurrentCharacter;

	void RegisterToController();
    void UnregisterFromController();

private:
	FTimerHandle ComboResetTimer;

	// 委托句柄
    FDelegateHandle PlaySectionDelegateHandle;

    // 接收通知的回调函数
	UFUNCTION()
	void OnReceivePlaySectionNotify(const FGameplayTag& InTag, bool IsAttack);

	// 当前组合段索引
	int32 CurrentComboIndex = 0;

	ACPP_PlayerController* PC;
	UAbilitySystemComponent* ASC;
	UPlayerAttributeSet* AS;
	UAnimInstance* AnimInstance;

};