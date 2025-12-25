// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CPP_PlayerState.h"
#include "AccountRegisterAndReader.h"
#include "GASGameInstance.generated.h"

class UAccountData;
/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API UGASGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bIsPlayerLogin = false;

	UFUNCTION(BlueprintCallable)
	UAccountRegisterAndReader* GetAccountRegisterAndReader();

	UFUNCTION(BlueprintCallable)
	UEquipmentManager* GetEquipmentManager();

	// 当前登录账号
	UPROPERTY(BlueprintReadOnly)
	UAccountData* CurrentLoginAccount;

	// 当前登录账号名
	UPROPERTY(BlueprintReadOnly)
	FString CurAccountName;

	// 记录当前玩家状态
	UPROPERTY()
    ACPP_PlayerState* CurrentPlayerState;

};
