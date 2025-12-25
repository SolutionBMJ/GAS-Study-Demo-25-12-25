// Solution_BMJ


#include "AccountData.h"
#include "EquipmentManager.h"
#include "Kismet/GameplayStatics.h"

UAccountData::UAccountData()
{
	Teams.SetNum(7);
    EquipmentManager = NewObject<UEquipmentManager>(this, TEXT("EquipmentManager"));
}

void UAccountData::RegistPlayer(FString AAccountName, FString PPassward)
{
	AccountName = AAccountName;
	Password = PPassward;
	PlayerName = AAccountName;
	PlayerLevel = 1;
    TeamIndex = 0;

	OwnedCharacters.Add("SwordMan");
    OwnedCharacters.Add("MuLi");
    OwnedCharacters.Add("QiuLi");

    Teams[0].Members[0] = "SwordMan";
    Teams[0].Members[1] = "QiuLi";

    if (!EquipmentManager)
    {
        UE_LOG(LogTemp, Error, TEXT("EquipmentManager is Not Valid"));
        return;
    }

    UDataTable* DefaultEquipmentTable = LoadObject<UDataTable>(
        nullptr, 
        TEXT("/Game/Character/DefaultEquipment.DefaultEquipment")
    );
    
    if (DefaultEquipmentTable)
    {
        EquipmentManager->LoadEquipmentConfig(DefaultEquipmentTable, this);
        UE_LOG(LogTemp, Log, TEXT("Default equipment initialized, count: %d"), OwnedEquipments.Num());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load default equipment table"));
    }

    // ±£¥Ê’À∫≈ ˝æ›
    if (UGameplayStatics::SaveGameToSlot(this, AccountName, 0))
    {
        UE_LOG(LogTemp, Log, TEXT("Account %s registered successfully"), *AccountName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save account data during registration"));
    }
}

bool UAccountData::AddCharacter(const FString& CharacterName)
{
	if (HasCharacter(CharacterName))
    {
        UE_LOG(LogTemp, Warning, TEXT("Character %s already owned!"), *CharacterName);
        return false;
    }
    
    OwnedCharacters.Add(CharacterName);
    UE_LOG(LogTemp, Warning, TEXT("Character %s Add Successes!"), *CharacterName);
    return true;
}

bool UAccountData::HasCharacter(const FString& CharacterID) const
{
    return OwnedCharacters.Contains(CharacterID);
}

