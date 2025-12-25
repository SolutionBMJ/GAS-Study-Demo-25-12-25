// Solution_BMJ

#include "Attack_GameplayAbility.h"
#include "AttackAttach.h"
#include "CPP_PlayerState.h"
#include "GameplayTagContainer.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

UAttack_GameplayAbility::UAttack_GameplayAbility()
{
	// 确保能力可预测
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UAttack_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	CurrentCharacter = Cast<ACPP_PlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!CurrentCharacter.IsValid()) return;

    PC = Cast<ACPP_PlayerController>(CurrentCharacter->Controller);
	if (!IsValid(PC)) return;

	if (PC)
    {
		PlaySectionDelegateHandle = PC->OnPlaySectionNotify.AddUObject(this, &UAttack_GameplayAbility::OnReceivePlaySectionNotify);
    }

	ASC = CurrentCharacter->GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;

	AS = const_cast<UPlayerAttributeSet*>(ASC->GetSet<UPlayerAttributeSet>());
	if (!IsValid(AS)) return;

	AnimInstance = CurrentCharacter->GetMesh()->GetAnimInstance();
	if (!IsValid(AnimInstance)) return;

	CurrentComboIndex = 0;

	PlaySection();
}

void UAttack_GameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UE_LOG(LogTemp, Warning, TEXT("EndAbility!!!!!!!!!!!!!!!!!!!!"));
	GetWorld()->GetTimerManager().ClearTimer(ComboResetTimer);
	ACPP_PlayerState* APlayerState = Cast<ACPP_PlayerState>(CurrentCharacter->GetPlayerState());
	if (PC && PlaySectionDelegateHandle.IsValid())
    {
		if (CurrentCharacter.IsValid() && ASC)
		{
			if (ASC->HasMatchingGameplayTag(AttackTag))
			{
				ASC->RemoveLooseGameplayTag(AttackTag);
			}
		}
        PC->OnPlaySectionNotify.Remove(PlaySectionDelegateHandle);
        PlaySectionDelegateHandle.Reset();
    }
    if (!APlayerState)
    {
        Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
        return;
    }

	// 强制清除所有攻击相关状态
	
    APlayerState->SetAttackInputRequested(false);

    RemoveTag();

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAttack_GameplayAbility::RemoveTag()
{
	// 尝试通过ASC移除
    if (IsValid(ASC) && ASC->HasMatchingGameplayTag(AttackTag))
    {
        ASC->RemoveLooseGameplayTag(AttackTag);
    }
}

void UAttack_GameplayAbility::PlaySection()
{
	if (CurrentComboIndex >= SectionName.Num())
	{
		return;
	}

	CurrentCharacter->SetSphereCollisionRadius(GADetectionRadius);

	//清除计时器
	GetWorld()->GetTimerManager().ClearTimer(ComboResetTimer);
	// 启动连段重置定时器
    GetWorld()->GetTimerManager().SetTimer(
        ComboResetTimer,
        this,
        &UAttack_GameplayAbility::ResetCombo,
        ComboResetTime,
        false
    );

	PC->ForceFaceToInput();
	if (WillFaceToTarget)
	{
		PC->FaceToTarget();
	}
	if (!ASC->HasMatchingGameplayTag(AttackTag))
	{
		ASC->AddLooseGameplayTag(AttackTag);
	}

	CurrentCharacter->ReSetSphereCollisionRadius();

	// 播放指定分段
	if (AnimInstance->Montage_IsPlaying(ComboMontages))
	{
		AnimInstance->Montage_JumpToSection(SectionName[CurrentComboIndex], ComboMontages);
	}
	else
	{
		AnimInstance->Montage_Play(ComboMontages);
		AnimInstance->Montage_JumpToSection(SectionName[CurrentComboIndex], ComboMontages);
	}
	if (CanLoop)
	{
		// 计算下一段索引
		CurrentComboIndex = (CurrentComboIndex + 1) % SectionName.Num();
	}
	else
	{
		CurrentComboIndex++;
	}

	if (WillPlayLevelSequence)
	{
		CurrentCharacter->TryPlayLevelSequenceByBP(CurrentComboIndex);
		PC->GetCurrentCamera()->ResetViewToBehindTarget();
	}
	if (CurrentComboIndex >= SectionName.Num())
	{
		StartCoolDown();
		return;
	}
}

void UAttack_GameplayAbility::OnReceivePlaySectionNotify(const FGameplayTag& InTag, bool IsAttack)
{
	RemoveTag();
	// 是同一个GA 且 需要进行攻击 且 正在被控制， 则响应
	if (InTag.MatchesTag(AttackTag) && IsAttack && CurrentCharacter->IsPlayerControlled())
    {
		PlaySection();
    }
}

void UAttack_GameplayAbility::ResetCombo()
{
	StartCoolDown();
    //清除计时器
	GetWorld()->GetTimerManager().ClearTimer(ComboResetTimer);
    // 直接结束能力，重置逻辑放在 EndAbility 里
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

}
