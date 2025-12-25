// Solution_BMJ


#include "CPP_PlayerState.h"
#include "GASGameInstance.h"
#include "AccountData.h"
#include "CharacterInfo.h"
#include "Attack_GameplayAbility.h"
#include "CPP_PlayerCharacter.h"
#include "Net/UnrealNetwork.h"


ACPP_PlayerState::ACPP_PlayerState()
{
	NetUpdateFrequency = 100.f;
}

void ACPP_PlayerState::BeginPlay()
{
    UGASGameInstance* AGameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!IsValid(AGameInstance))
    {
        UE_LOG(LogTemp, Error, TEXT("ACPP_PlayerState::GAMEINSTANCE is Not Valid!!!"));
        return;
    }
    if (!AGameInstance->bIsPlayerLogin)
    {
        UE_LOG(LogTemp, Error, TEXT("ACPP_PlayerState::Player Not Login!"));
        return;
    }
    Super::BeginPlay();

    TeamManager = NewObject<UTeamManager>(this, TEXT("TeamManager"));

    if (GetWorld())
    {
        ReloadCurrentTeam();
    }
}

void ACPP_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 注册需要复制的属性
    DOREPLIFETIME(ACPP_PlayerState, bAttackInputRequested);
    DOREPLIFETIME(ACPP_PlayerState, TeamManager); // 添加上下文复制
}

void ACPP_PlayerState::SetAttackInputRequested(bool bRequested)
{
    bAttackInputRequested = bRequested;
    if (bRequested)
    {
        StartPreInputTimer();
    }
    else
    {
        ClearPreInputTimer();
    }
}

void ACPP_PlayerState::ReloadCurrentTeam()
{
    if (!IsValid(TeamManager)) return;

    UE_LOG(LogTemp, Error, TEXT("ACPP_PlayerState::Begin Reload Team!!!!"));

    UGASGameInstance* AGameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!AGameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("ReloadCurrentTeam: GameInstance is not valid"));
        return;
    }

    UAccountData* PlayerAccount = AGameInstance->CurrentLoginAccount;

    // 检查AccountName是否为空
    if (!PlayerAccount || PlayerAccount->AccountName.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("ReloadCurrentTeam: AccountName is Empty"));
        return;
    }

    int32 CurrentTeamIndex = PlayerAccount->TeamIndex;

    // 从 DataTable 构建新的角色类数组
    TArray<FString> TeamCharacterNames = PlayerAccount->Teams[CurrentTeamIndex].Members;
    TArray<TSubclassOf<ACPP_PlayerCharacter>> NewClasses;

    if (CharacterDataTable)
    {
        for (const FString& CharName : TeamCharacterNames)
        {
            if (CharName.IsEmpty()) continue;
            FAllCharactersInfo* Row = CharacterDataTable->FindRow<FAllCharactersInfo>(FName(*CharName), TEXT(""));
            if (Row && Row->CharacterBlueprintClass)
            {
                NewClasses.Add(Row->CharacterBlueprintClass);
            }
        }
    }

    // 调用 TeamManager 重新初始化
    ACPP_PlayerController* PC = Cast<ACPP_PlayerController>(GetWorld()->GetFirstPlayerController());
    if (PC && TeamManager)
    {
        TeamManager->Initialize(PC, NewClasses);
    }
}

void ACPP_PlayerState::StartPreInputTimer()
{
    GetWorld()->GetTimerManager().ClearTimer(PreInputResetTimer);
    GetWorld()->GetTimerManager().SetTimer(
        PreInputResetTimer,
        this,
        &ACPP_PlayerState::ClearAttackInputRequest,
        PreInputResetDelay,
        false
    );
}

void ACPP_PlayerState::ClearPreInputTimer()
{
    GetWorld()->GetTimerManager().ClearTimer(PreInputResetTimer);
}

void ACPP_PlayerState::ClearAttackInputRequest()
{
    SetAttackInputRequested(false);
}
