// Solution_BMJ

#include "AccountRegisterAndReader.h"
#include "AccountData.h"
#include "AccountManager.h"
#include "GASGameInstance.h"
#include "Kismet/GameplayStatics.h"

UAccountRegisterAndReader* UAccountRegisterAndReader::Instance = nullptr;

// 获取实例
UAccountRegisterAndReader* UAccountRegisterAndReader::GetInstance()
{
	if (!Instance)
    {
        Instance = NewObject<UAccountRegisterAndReader>();
        Instance->AddToRoot();
        Instance->Initialize();
    }
    return Instance;
}

void UAccountRegisterAndReader::Initialize()
{
    
    // 尝试加载现有的账户列表
    AccountListData = Cast<UAccountManager>(UGameplayStatics::LoadGameFromSlot(AccountListSlotName, 0));
    
    if (!AccountListData)
    {
        AccountListData = Cast<UAccountManager>(UGameplayStatics::CreateSaveGameObject(UAccountManager::StaticClass()));
        
        if (AccountListData)
        {
            AccountListData->AccountList.Empty();
            SaveAccountList(AccountListData);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create AccountListData!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Loaded existing AccountListData with %d accounts"), AccountListData->AccountList.Num());
    }
}

// 注册账户
bool UAccountRegisterAndReader::RegisterAccount(const FString& Username, const FString& Password)
{
    if (!AccountListData)
    {
        Initialize();
        if (!AccountListData)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to initialize AccountListData!"));
            return false;
        }
    }
    
    // 检查用户名是否存在
    if (IsUsernameExists(Username))
    {
        UE_LOG(LogTemp, Error, TEXT("RegisterAccount: Username '%s' is already taken!"), *Username);
        return false;
    }
    
    // 创建新账户数据
    UAccountData* NewAccount = Cast<UAccountData>(UGameplayStatics::CreateSaveGameObject(UAccountData::StaticClass()));
    if (!NewAccount)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create AccountData for user: %s"), *Username);
        return false;
    }
    NewAccount->RegistPlayer(Username, Password);
    
    // 保存账户数据
    if (!SaveAccountData(NewAccount))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save account data for user: %s"), *Username);
        return false;
    }
    
    // 添加到账户列表
    AccountListData->AccountList.Add(Username);
    
    // 保存账户列表
    if (!SaveAccountList(AccountListData))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save AccountList!"));
        // 回滚：从列表中移除刚刚添加的用户名
        AccountListData->AccountList.Remove(Username);
        return false;
    }
    return true;
}

// 登录账户
bool UAccountRegisterAndReader::LoginAccount(const FString& Username, const FString& Password, UAccountData* OutAccountData, UGASGameInstance* GameInstance)
{
    OutAccountData = LoadAccountData(Username);
    if (!OutAccountData || OutAccountData->Password != Password)
    {
        UE_LOG(LogTemp, Warning, TEXT("UAccountRegisterAndReader::UserName or Password Error!"));
        OutAccountData = nullptr;
        return false;
    }
    
    if (GameInstance)
    {
        GameInstance->CurrentLoginAccount = OutAccountData;
        UE_LOG(LogTemp, Warning, TEXT("UAccountRegisterAndReader::LoginAccount::Address: %p"), OutAccountData);
    }

    CurUserName = Username;
    return true;
}

// 保存账号数据
bool UAccountRegisterAndReader::SaveAccountData(UAccountData* AccountData)
{
    if (!AccountData || AccountData->AccountName.IsEmpty())
    {
        return false;
    }

    bool bSaved = UGameplayStatics::SaveGameToSlot(AccountData, AccountData->AccountName, 0);
    if (bSaved)
    {
        // 保存成功后，更新GameInstance中的CurrentLoginAccount
        UGASGameInstance* GameInstance = Cast<UGASGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        if (GameInstance && GameInstance->CurrentLoginAccount 
            && GameInstance->CurrentLoginAccount->AccountName == AccountData->AccountName)
        {
            GameInstance->CurrentLoginAccount = AccountData;
            UE_LOG(LogTemp, Log, TEXT("CurrentLoginAccount updated to latest saved data for: %s"), *AccountData->AccountName);
        }
    }

    // 用用户名作为SaveGame的SlotName，确保唯一
    return bSaved;
}

UAccountData* UAccountRegisterAndReader::LoadAccountData(const FString& Username)
{
    if (Username.IsEmpty())
    {
        return nullptr;
    }
    return Cast<UAccountData>(UGameplayStatics::LoadGameFromSlot(Username, 0));
}

bool UAccountRegisterAndReader::IsUsernameExists(const FString& Username)
{
    if (!AccountListData)
    {
        Initialize();
        
        if (!AccountListData)
        {
            UE_LOG(LogTemp, Error, TEXT("IsUsernameExists: Failed to initialize AccountListData!"));
            return false;
        }
    }
    bool bExists = AccountListData->AccountList.Contains(Username);
    return bExists;
}

bool UAccountRegisterAndReader::SaveAccountList(class UAccountManager* AAccountListData)
{
    if (!AAccountListData)
    {
        return false;
    }
    return UGameplayStatics::SaveGameToSlot(AAccountListData, AccountListSlotName, 0);
}
