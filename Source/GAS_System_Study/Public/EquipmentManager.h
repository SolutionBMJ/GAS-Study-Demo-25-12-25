// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "AccountData.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentManager.generated.h"

/**
 * 
 */

// 当前账户所有装备管理器
UCLASS()
class GAS_SYSTEM_STUDY_API UEquipmentManager : public UObject
{
	GENERATED_BODY()

public:
    // 构造函数
    UEquipmentManager();

    // 加载默认装备配置
    UFUNCTION(BlueprintCallable, Category = "Equipment")
    void LoadEquipmentConfig(UDataTable* EquipmentTable, UAccountData* PlayerAccount);

    // 获取所有装备
    UFUNCTION(BlueprintCallable, Category = "Equipment")
    TArray<FEquipmentData>& GetAllEquipment(UAccountData* PlayerAccount);

    // 装备/卸下装备
    UFUNCTION(BlueprintCallable, Category = "Equipment")
    void SetEquipmentEquipped(UAccountData* PlayerAccount, const FString& EquipmentID, bool bEquipped, UWorld* World, const FString& CharacterName = "");

    UFUNCTION(BlueprintCallable, Category = "Equipment")
    void SaveEquipmentData(UAccountData* PlayerAccount);

    UFUNCTION(BlueprintImplementableEvent)
    void EquipmentStateChange();
};
