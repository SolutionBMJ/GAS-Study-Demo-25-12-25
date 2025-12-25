// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AccountRegisterAndReader.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API UAccountRegisterAndReader : public UObject
{
	GENERATED_BODY()

public:
	// 获取单例实例
    UFUNCTION(BlueprintCallable, Category = "Account")
    static UAccountRegisterAndReader* GetInstance();

    UFUNCTION(BlueprintCallable, Category = "Account")
    void Initialize();

    // 注册新账号
    UFUNCTION(BlueprintCallable, Category = "Account")
    bool RegisterAccount(const FString& Username, const FString& Password);

    // 登录账号
    UFUNCTION(BlueprintCallable, Category = "Account")
    bool LoginAccount(const FString& Username, const FString& Password, UAccountData* OutAccountData, UGASGameInstance* GameInstance);

    // 保存账号数据到本地
    UFUNCTION(BlueprintCallable, Category = "Account")
    bool SaveAccountData(UAccountData* AccountData);

    // 加载账号数据
    UFUNCTION(BlueprintCallable, Category = "Account")
    UAccountData* LoadAccountData(const FString& Username);

    // 获取当前登录账号
    UFUNCTION(BlueprintCallable)
    FString GetCurrentUserName() { return CurUserName; }

private:
    // 单例实例
    static UAccountRegisterAndReader* Instance;

    // 账号列表的SaveGame槽名
    const FString AccountListSlotName = "AccountList";

    // 检查用户名是否已存在
    bool IsUsernameExists(const FString& Username);

    // 保存账号列表
    bool SaveAccountList(class UAccountManager* AccountListData);

    class UAccountManager* AccountListData;

    // 记录当前登录账号名
    FString CurUserName;

};
