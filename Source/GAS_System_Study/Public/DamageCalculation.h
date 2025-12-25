// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "PlayerAttributeSet.h"
#include "EnemyAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "DamageCalculation.generated.h"

/**
 * 
 */

UCLASS()
class GAS_SYSTEM_STUDY_API UDamageCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UDamageCalculation();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
