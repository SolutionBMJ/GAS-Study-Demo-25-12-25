// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterDamageDA.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class GAS_SYSTEM_STUDY_API UCharacterDamageDA : public UDataAsset
{
	GENERATED_BODY()
	
public:
    // 该技能所有段数的伤害配置
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (DisplayName = "Section - DamageRate"))
    TMap<int32, float> DamageDataMap;

    // 根据段ID获取对应倍率
    UFUNCTION(BlueprintCallable, Category = "Damage")
    float GetDamageRate(int32 Index);
};
