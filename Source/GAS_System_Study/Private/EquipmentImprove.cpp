// Solution_BMJ


#include "EquipmentImprove.h"

UEquipmentImprove::UEquipmentImprove(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 设置GE为持久化
    DurationPolicy = EGameplayEffectDurationType::Infinite;
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Equipment.Bonus")));
}

void UEquipmentImprove::ClearEquipmentBonus(UAbilitySystemComponent* ASC)
{
    if (!ASC) return;

    // 移除所有装备相关的GE
	TArray<FActiveGameplayEffectHandle> EquipmentEffects;
	EquipmentEffects = ASC->GetActiveEffectsWithAllTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("Equipment.Bonus"))));
	for (const auto& Handle : EquipmentEffects)
	{
		ASC->RemoveActiveGameplayEffect(Handle);
	}
}
