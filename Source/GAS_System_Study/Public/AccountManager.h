// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AccountManager.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API UAccountManager : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere)
	TArray<FString> AccountList;
};
