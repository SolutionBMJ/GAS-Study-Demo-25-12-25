// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "PlayerAttributeSet.h"
#include "EquipmentImprove.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API UEquipmentImprove : public UGameplayEffect
{
	GENERATED_BODY()

public:
    UEquipmentImprove(const FObjectInitializer& ObjectInitializer);

    // 移除角色所有装备加成GE
    static void ClearEquipmentBonus(UAbilitySystemComponent* ASC);
};
