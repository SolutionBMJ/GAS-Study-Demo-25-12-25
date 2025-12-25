// Solution_BMJ


#include "EquipmentManager.h"
#include "Engine/DataTable.h"
#include "GASGameInstance.h"

UEquipmentManager::UEquipmentManager()
{
    // 初始化逻辑
}

void UEquipmentManager::LoadEquipmentConfig(UDataTable* EquipmentTable, UAccountData* PlayerAccount)
{
    if (!EquipmentTable) return;

    // 清空现有装备
    PlayerAccount->OwnedEquipments.Empty();

    // 从数据表加载装备
    TArray<FName> RowNames = EquipmentTable->GetRowNames();
    for (const FName& RowName : RowNames)
    {
        FEquipmentData* EquipmentRow = EquipmentTable->FindRow<FEquipmentData>(RowName, TEXT(""));
        if (EquipmentRow)
        {
            // 直接添加到账号数据的装备列表
            PlayerAccount->OwnedEquipments.Add(*EquipmentRow);
        }
    }
    SaveEquipmentData(PlayerAccount);
}

TArray<FEquipmentData>& UEquipmentManager::GetAllEquipment(UAccountData* PlayerAccount)
{
    if (!PlayerAccount)
    {
        static TArray<FEquipmentData> EmptyArray;
        UE_LOG(LogTemp, Error, TEXT("Invalid PlayerAccount in GetAllEquipments"));
        return EmptyArray;
    }
    return PlayerAccount->OwnedEquipments;
}

void UEquipmentManager::SetEquipmentEquipped(UAccountData* PlayerAccount, const FString& EquipmentID, bool bEquipped, UWorld* World, const FString& CharacterName)
{
    
    if (!PlayerAccount) return;

    for (FEquipmentData& Equipment : PlayerAccount->OwnedEquipments)
    {
        if (Equipment.EquipmentID == EquipmentID)
        {
            Equipment.bIsEquipped = bEquipped;
            Equipment.EquippedToCharacter = bEquipped ? CharacterName : FString("");
            UE_LOG(LogTemp, Log, TEXT("Equipment %s equipped state set to: %s"), 
                *EquipmentID, 
                bEquipped ? TEXT("true") : TEXT("false"));
            break;
        }
    }

    if (!World) return;

    UGASGameInstance* GameInstance = Cast<UGASGameInstance>(World->GetGameInstance());
    if (!GameInstance) return;

    // 检查AccountRegisterAndReader是否有效
    UAccountRegisterAndReader* AccountReader = GameInstance->GetAccountRegisterAndReader();
    if (!AccountReader)
    {
        UE_LOG(LogTemp, Error, TEXT("AccountRegisterAndReader is null"));
        return;
    }

    AccountReader->SaveAccountData(PlayerAccount);
    UE_LOG(LogTemp, Error, TEXT("Save Equipment State to Local!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
}

void UEquipmentManager::SaveEquipmentData(UAccountData* PlayerAccount)
{
    UE_LOG(LogTemp, Log, TEXT("Saving equipment data for account: %s"), 
        PlayerAccount ? *PlayerAccount->AccountName : TEXT("Invalid"));
    
    if (PlayerAccount)
    {
        // 保存账号数据
        if (UGameplayStatics::SaveGameToSlot(PlayerAccount, PlayerAccount->AccountName, 0))
        {
            UE_LOG(LogTemp, Log, TEXT("Equipment data saved successfully"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to save equipment data"));
        }
    }
}
