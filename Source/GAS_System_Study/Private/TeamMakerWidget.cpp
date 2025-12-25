// Solution_BMJ


#include "TeamMakerWidget.h"
#include "input/Reply.h"
#include "GASGameInstance.h"
#include "AccountData.h"
#include "Engine/Texture2D.h"
#include "CPP_PlayerState.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h" // 这是关键！
#include "Kismet/GameplayStatics.h"
#include "CPP_PlayerController.h"


void UTeamMakerWidget::OpenTeamMaker()
{
	if (!bIsTeamMakerOpen)
	{
		AddToViewport();
		if (!IsValid(GetWorld())) return;
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		PC = GetOwningPlayer();
        if (PC.IsValid())
        {
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
            ACPP_PlayerController* CPPC = Cast<ACPP_PlayerController>(PC);
            if (CPPC)
            {
                CPPC->TeamMakerUI = this;
            }
        }
        GetOwnedCharacter();
        bIsTeamMakerOpen = true;
        UE_LOG(LogTemp, Error, TEXT("bIsTeamMakerOpen: true"));

        UGASGameInstance* GameInstance = Cast<UGASGameInstance>(GetGameInstance());
        if (!GameInstance)
        {
            return;
        }
        UAccountData* CurPlayer = GameInstance->CurrentLoginAccount;
        if (!IsValid(CurPlayer)) return;
        SwitchTeamPage(CurPlayer->TeamIndex);

        Team1->OnClicked.AddDynamic(this, &UTeamMakerWidget::PressTeam1);
        Team2->OnClicked.AddDynamic(this, &UTeamMakerWidget::PressTeam2);
        Team3->OnClicked.AddDynamic(this, &UTeamMakerWidget::PressTeam3);
        Team4->OnClicked.AddDynamic(this, &UTeamMakerWidget::PressTeam4);
        Team5->OnClicked.AddDynamic(this, &UTeamMakerWidget::PressTeam5);
        Team6->OnClicked.AddDynamic(this, &UTeamMakerWidget::PressTeam6);
        Team7->OnClicked.AddDynamic(this, &UTeamMakerWidget::PressTeam7);
        ConfrimCharacter->OnClicked.AddDynamic(this, &UTeamMakerWidget::ChangeCharacter);
        ConfrimTeam->OnClicked.AddDynamic(this, &UTeamMakerWidget::ChangeFightTeam);
        RemoveCharacter->OnClicked.AddDynamic(this, &UTeamMakerWidget::RemoveCharacterOnTeam);
	}

    if (FirstCharacterIcon && SecondCharacterIcon && ThirdCharacterIcon)
    {
        FirstCharacterIcon->SetParentWidget(this);
        SecondCharacterIcon->SetParentWidget(this);
        ThirdCharacterIcon->SetParentWidget(this);
    }

    bIsChangeTeam = false;
}

void UTeamMakerWidget::CloseTeamMaker()
{
    if (bIsTeamMakerOpen)
	{
        RemoveFromParent();

		UGameplayStatics::SetGamePaused(GetWorld(), false);

		if (PC.IsValid())
        {
            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = false;
            ACPP_PlayerController* CPPC = Cast<ACPP_PlayerController>(PC);
            if (CPPC)
            {
                CPPC->TeamMakerUI = nullptr;
            }
        }
        PC.Reset(); // 重置弱指针
        
        bIsTeamMakerOpen = false;
        UE_LOG(LogTemp, Error, TEXT("bIsTeamMakerOpen: false"));
        Team1->OnClicked.RemoveAll(this);
        Team2->OnClicked.RemoveAll(this);
        Team3->OnClicked.RemoveAll(this);
        Team4->OnClicked.RemoveAll(this);
        Team5->OnClicked.RemoveAll(this);
        Team6->OnClicked.RemoveAll(this);
        Team7->OnClicked.RemoveAll(this);
        ConfrimCharacter->OnClicked.RemoveAll(this);
        ConfrimTeam->OnClicked.RemoveAll(this);
        RemoveCharacter->OnClicked.RemoveAll(this);
	}
}

void UTeamMakerWidget::GetCurrentTeam(int32 TeamIndex)
{
    SelectTeam = TeamIndex;
    UGASGameInstance* GameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!GameInstance) return;

    UAccountData* CurPlayer = GameInstance->CurrentLoginAccount;

    if (!IsValid(CurPlayer))
    {
        UE_LOG(LogTemp, Log, TEXT("UTeamMakerWidget::CurPlayer is Not Valid!"));
        return;
    }
    UpdataSingleCharacterIcon(CurPlayer, TeamIndex, 0, FirstCharacterIcon->CharacterIcon);
    UpdataSingleCharacterIcon(CurPlayer, TeamIndex, 1, SecondCharacterIcon->CharacterIcon);
    UpdataSingleCharacterIcon(CurPlayer, TeamIndex, 2, ThirdCharacterIcon->CharacterIcon);
	TArray<FString> CurrentTeam = CurPlayer->Teams[TeamIndex].Members;
    bool bIsTeamEmpty = true;
    for (FString CurName : CurrentTeam)
    {
        if (CurName.Len())
        {
            bIsTeamEmpty = false;
            break;
        }
    }

    if (SelectTeam == CurPlayer->TeamIndex && !bIsChangeTeam || bIsTeamEmpty)
    {
        ConfrimTeam->SetIsEnabled(false);
    }
    else
    {
        ConfrimTeam->SetIsEnabled(true);
    }
}

void UTeamMakerWidget::UpdataSingleCharacterIcon(UAccountData* CurPlayer, int32 TeamIndex, int32 index, UImage* TargetImage)
{
    TArray<FString> CurrentTeam = CurPlayer->Teams[TeamIndex].Members;
    if (CharacterDataTable)
    {
        FName CharacterName = FName(*CurrentTeam[index]);
        FAllCharactersInfo* RowData = CharacterDataTable->FindRow<FAllCharactersInfo>(CharacterName, TEXT(""));
        if (RowData && RowData->CharacterBlueprintClass->IsChildOf(ACPP_PlayerCharacter::StaticClass()))
        {
            // 获取类的默认对象
            ACPP_PlayerCharacter* DefaultCharacter = Cast<ACPP_PlayerCharacter>(RowData->CharacterBlueprintClass->GetDefaultObject());
            if (DefaultCharacter)
            {
                UTexture2D* CharacterIconTexture = LoadObject<UTexture2D>(nullptr, *DefaultCharacter->TeamIconPath);
                if (CharacterIconTexture && TargetImage)
                {
                    TargetImage->SetBrushFromTexture(CharacterIconTexture);
                }
            }
        }
        else
        {
            UTexture2D* CharacterIconTexture = LoadObject<UTexture2D>(nullptr, *DefaultIconPath);
            if (CharacterIconTexture && TargetImage)
            {
                TargetImage->SetBrushFromTexture(CharacterIconTexture);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UTeamMakerWidget::CharacterDataTable is not assigned"));
    }
}

void UTeamMakerWidget::GetOwnedCharacter()
{
    UGASGameInstance* GameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!IsValid(GameInstance))
    {
         UE_LOG(LogTemp, Error, TEXT("UTeamMakerWidget::GameInstace is No Valid"));
         return;
    }
    TArray<FString> OwnedCharacter = GameInstance->GetAccountRegisterAndReader()->LoadAccountData(GameInstance->CurAccountName)->GetOwnedCharacters();

    CharacterObjectList.Empty();

    if (CharacterDataTable)
    {
        for (FString &Character : OwnedCharacter)
        {
            FName CharacterName = FName(*Character);
            FAllCharactersInfo* RowData = CharacterDataTable->FindRow<FAllCharactersInfo>(CharacterName, TEXT(""));
            if (RowData && RowData->CharacterBlueprintClass->IsChildOf(ACPP_PlayerCharacter::StaticClass()))
            {
                // 获取类的默认对象
                ACPP_PlayerCharacter* DefaultCharacter = Cast<ACPP_PlayerCharacter>(RowData->CharacterBlueprintClass->GetDefaultObject());
                if (DefaultCharacter)
                {
                    CharacterObjectList.Add(DefaultCharacter);
                    UE_LOG(LogTemp, Log, TEXT("Added character default object: %s"), *RowData->CharacterBlueprintClass->GetName());
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Invalid character class for: %s"), *Character);
                }
            }
        }
        if (SelectCharacterList)
        {
            SelectCharacterList->SetListItems(CharacterObjectList);
            UE_LOG(LogTemp, Log, TEXT("TileView updated with %d characters"), CharacterObjectList.Num());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("SelectCharacterList is not bound in Widget Blueprint"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UTeamMakerWidget::CharacterDataTable is not assigned"));
    }
}

void UTeamMakerWidget::SwitchTeamPage(int32 index)
{
    if (IsValid(BanButtom))
    {
        BanButtom->SetIsEnabled(true);
    }
    switch (index)
    {
    case 0:
        Team1->SetIsEnabled(false);
        BanButtom = Team1;
        GetCurrentTeam(0);
        break;
    case 1:
        Team2->SetIsEnabled(false);
        BanButtom = Team2;
        GetCurrentTeam(1);
        break;
    case 2:
        Team3->SetIsEnabled(false);
        BanButtom = Team3;
        GetCurrentTeam(2);
        break;
    case 3:
        Team4->SetIsEnabled(false);
        BanButtom = Team4;
        GetCurrentTeam(3);
        break;
    case 4:
        Team5->SetIsEnabled(false);
        BanButtom = Team5;
        GetCurrentTeam(4);
        break;
    case 5:
        Team6->SetIsEnabled(false);
        BanButtom = Team6;
        GetCurrentTeam(5);
        break;
    case 6:
        Team7->SetIsEnabled(false);
        BanButtom = Team7;
        GetCurrentTeam(6);
        break;
    }
}

void UTeamMakerWidget::ChangeCharacter()
{
    // 保存存档修改
    UGASGameInstance* GameInstace = Cast<UGASGameInstance>(GetGameInstance());
    if (!IsValid(GameInstace)) return;

    // 将所选角色更改应用到存档
    UAccountData* CurPlayer = GameInstace->CurrentLoginAccount;
    CurPlayer->Teams[SelectTeam].Members[SelectCharID] = SelectedCharacter;

    GameInstace->GetAccountRegisterAndReader()->SaveAccountData(CurPlayer);

    SelectedCharacter = TEXT("");

    if (SelectTeam == CurPlayer->TeamIndex)
    {
        bIsChangeTeam = true;
    }

    CloseTeamSelect();

    // 刷新队伍界面
    GetCurrentTeam(SelectTeam);
}

void UTeamMakerWidget::ChangeFightTeam()
{
    CloseTeamMaker();

    UGASGameInstance* GameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!IsValid(GameInstance)) return;

    UAccountData* CurPlayer = GameInstance->CurrentLoginAccount;
    if (!IsValid(CurPlayer)) return;

    CurPlayer->TeamIndex = SelectTeam;

    GameInstance->GetAccountRegisterAndReader()->SaveAccountData(CurPlayer);

    GameInstance->CurrentPlayerState->ReloadCurrentTeam();
}

void UTeamMakerWidget::ChangeSelectCharacter(FString CharacterName)
{
    SelectedCharacter = CharacterName;
    UGASGameInstance* GameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!IsValid(GameInstance)) return;

    TArray<FString> OwnedCharacter = GameInstance->CurrentLoginAccount->GetOwnedCharacters();

    UAccountData* CurPlayer = GameInstance->CurrentLoginAccount;
    if (!IsValid(CurPlayer)) return;

	TArray<FString> CurrentTeam = CurPlayer->Teams[SelectTeam].Members;
    ConfrimCharacter->SetIsEnabled(true);
    for (FString CurName : CurrentTeam)
    {
        if (CurName == SelectedCharacter)
        {
            ConfrimCharacter->SetIsEnabled(false);
        }
    }

    if (CharacterDataTable)
    {
        FAllCharactersInfo* RowData = CharacterDataTable->FindRow<FAllCharactersInfo>(FName(*SelectedCharacter), TEXT(""));
        if (RowData && RowData->CharacterBlueprintClass->IsChildOf(ACPP_PlayerCharacter::StaticClass()))
        {
            // 获取类的默认对象
            ACPP_PlayerCharacter* DefaultCharacter = Cast<ACPP_PlayerCharacter>(RowData->CharacterBlueprintClass->GetDefaultObject());
            if (DefaultCharacter)
            {
                FString CharName = DefaultCharacter->CharacterName;
                if (CharName.Len() && CharacterNameText)
                {
                    CharacterNameText->SetText(FText::FromString(CharName));
                }
                FText CharInfo = DefaultCharacter->CharacterInfo;
                if (CharacterInfo)
                {
                    CharacterInfo->SetText(CharInfo);
                }
                UTexture2D* CharacterIconTexture = LoadObject<UTexture2D>(nullptr, *DefaultCharacter->HeadIconPath);
                if (CharacterIconTexture && CharacterIcon)
                {
                    CharacterIcon->SetBrushFromTexture(CharacterIconTexture);
                }
                UTexture2D* NA_Icon = LoadObject<UTexture2D>(nullptr, *DefaultCharacter->NormalAttackIconPath);
                if (NA_Icon && NormalAttackIcon)
                {
                    NormalAttackIcon->SetBrushFromTexture(NA_Icon);
                }
                UTexture2D* SO_Icon = LoadObject<UTexture2D>(nullptr, *DefaultCharacter->SkillOneIconPath);
                if (SO_Icon && SkillOneIcon)
                {
                    SkillOneIcon->SetBrushFromTexture(SO_Icon);
                }
                UTexture2D* ST_Icon = LoadObject<UTexture2D>(nullptr, *DefaultCharacter->SkillTwoIconPath);
                if (ST_Icon && SkillTwoIcon)
                {
                    SkillTwoIcon->SetBrushFromTexture(ST_Icon);
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Invalid character class for: %s"), *SelectedCharacter);
        }
    }
}

FReply UTeamMakerWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    HandleShortcutKey(InKeyEvent.GetKey());
    return FReply::Handled();
}

void UTeamMakerWidget::HandleShortcutKey(const FKey& Key)
{
    if (Key == EKeys::L)
    {
        CloseTeamMaker();
    }
}

void UTeamMakerWidget::RemoveCharacterOnTeam()
{
    // 保存存档修改
    UGASGameInstance* GameInstace = Cast<UGASGameInstance>(GetGameInstance());
    if (!IsValid(GameInstace)) return;

    // 将所选角色更改应用到存档
    UAccountData* CurPlayer = GameInstace->CurrentLoginAccount;
    CurPlayer->Teams[SelectTeam].Members[SelectCharID] = TEXT("");

    GameInstace->GetAccountRegisterAndReader()->SaveAccountData(CurPlayer);

    SelectedCharacter = TEXT("");

    if (SelectTeam == CurPlayer->TeamIndex)
    {
        bIsChangeTeam = true;
    }

    UE_LOG(LogTemp, Error, TEXT("UTeamMakerWidget::PlayerState Address: %p"), GameInstace->CurrentPlayerState);

    CloseTeamSelect();

    // 刷新队伍界面
    GetCurrentTeam(SelectTeam);
}

void UTeamMakerWidget::PressTeam1()
{
    SwitchTeamPage(0);
}

void UTeamMakerWidget::PressTeam2()
{
    SwitchTeamPage(1);
}

void UTeamMakerWidget::PressTeam3()
{
    SwitchTeamPage(2);
}

void UTeamMakerWidget::PressTeam4()
{
    SwitchTeamPage(3);
}

void UTeamMakerWidget::PressTeam5()
{
    SwitchTeamPage(4);
}

void UTeamMakerWidget::PressTeam6()
{
    SwitchTeamPage(5);
}

void UTeamMakerWidget::PressTeam7()
{
    SwitchTeamPage(6);
}

void UTeamMakerWidget::SelectCharacter(int32 CharIndex)
{
    EnterCharacterSelect();
    SelectCharID = CharIndex;
}

void UTeamMakerWidget::ChangeCharacterByDrag(int32 CharOne, int32 CharTwo)
{
    UGASGameInstance* GameInstace = Cast<UGASGameInstance>(GetGameInstance());
    if (!IsValid(GameInstace)) return;

    // 获取涉及角色的名字
    UAccountData* CurPlayer = GameInstace->CurrentLoginAccount;
    FString FirstCharacter = CurPlayer->Teams[SelectTeam].Members[CharOne];
    FString SecondCharacter = CurPlayer->Teams[SelectTeam].Members[CharTwo];

    // 对调账号中的组队信息
    CurPlayer->Teams[SelectTeam].Members[CharTwo] = FirstCharacter;
    CurPlayer->Teams[SelectTeam].Members[CharOne] = SecondCharacter;

    // 将所选角色更改应用到存档
    GameInstace->GetAccountRegisterAndReader()->SaveAccountData(CurPlayer);

    if (SelectTeam == CurPlayer->TeamIndex)
    {
        bIsChangeTeam = true;
    }

    // 刷新队伍界面
    GetCurrentTeam(SelectTeam);
}
