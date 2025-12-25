// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "CPP_PlayerCharacter.h"
#include "CharacterInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FAllCharactersInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACPP_PlayerCharacter> CharacterBlueprintClass;
};


UCLASS()
class GAS_SYSTEM_STUDY_API UCharacterInfo : public UObject
{
	GENERATED_BODY()
	
};
