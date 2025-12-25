// Solution_BMJ


#include "CharacterDetail.h"
#include "AbilitySystemComponent.h"
#include "GASGameInstance.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "CPP_PlayerController.h"
#include "CPP_PlayerCharacter.h"
#include "Engine/World.h"
#include "EquipmentItemWidget.h"
#include "GameFramework/PlayerController.h"
#include "GamingPlayerUI.h"
#include "PlayerCameraActor.h"
#include "PlayerAttributeSet.h"
#include "UObject/Object.h"
#include "Kismet/GameplayStatics.h"

void UCharacterDetail::OpenCharacterDetail()
{
    if (!bIsCharacterDetailOpen)
	{
		AddToViewport();
		if (!IsValid(GetWorld())) return;

        // 加载流式关卡CharacterDisplay
        FName LevelName = FName("CharacterDisplay");
        FLatentActionInfo LatentInfo;
        LatentInfo.CallbackTarget = this;
        LatentInfo.UUID = 1;
        LatentInfo.Linkage = 0;
        LatentInfo.ExecutionFunction = FName("OnLevelLoaded");

        UGameplayStatics::LoadStreamLevel(
            GetWorld(),
            LevelName,
            true,
            false,
            LatentInfo
        );

		PC = GetOwningPlayer();
        if (PC)
        {
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
            ACPP_PlayerController* CPPC = Cast<ACPP_PlayerController>(PC);
            if (CPPC)
            {
                CPPC->CharacterDetailUI = this;

                // 隐藏GamingPlayerUI
                if (CPPC->PlayerUI)
                {
                    CPPC->PlayerUI->SetVisibility(ESlateVisibility::Hidden);
                }
            }
        }

        LoadPlayerOwnedCharacter();

        bIsCharacterDetailOpen = true;

        UAccountData* CurPlayer = GetCurrentPlayer();
        if (!IsValid(CurPlayer)) return;
	}
    else
    {
        bIsCharacterDetailOpen = false;
    }
    UE_LOG(LogTemp, Warning, TEXT("Start Refresh EquipmentList"));

    EquipmentSlotHead->SetCharacterDetail(this);
    EquipmentSlotHand->SetCharacterDetail(this);
    EquipmentSlotChest->SetCharacterDetail(this);
    EquipmentSlotLeg->SetCharacterDetail(this);

    RefreshUnEquippedList();

    GoBack->OnClicked.AddDynamic(this, &UCharacterDetail::CloseCharacterDetail);
    UE_LOG(LogTemp, Warning, TEXT("Bind GoBack Dynamic!!!!!!!!!!"));
}

void UCharacterDetail::CloseCharacterDetail()
{
    if (bIsCharacterDetailOpen)
	{
        RemoveFromParent();

        // 1. 恢复原摄像机视角
        RestoreOriginalCamera();
        // 2. 销毁展示角色副本
        DestroyDisplayCharacter();

        if (IsValid(GetWorld()))
        {
            // 卸载流式关卡CharacterDisplay
            FName LevelName = FName("CharacterDisplay");
            FLatentActionInfo LatentInfo;
            LatentInfo.CallbackTarget = this;
            LatentInfo.UUID = 0;
            LatentInfo.Linkage = 0;
            LatentInfo.ExecutionFunction = NAME_None;

            UGameplayStatics::UnloadStreamLevel(
                GetWorld(),
                LevelName,
                LatentInfo,
                false // 不强制立即卸载
            );
        }

		if (PC)
        {
            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = false;
            ACPP_PlayerController* CPPC = Cast<ACPP_PlayerController>(PC);
            if (CPPC)
            {
                CPPC->CharacterDetailUI = nullptr;
                // 隐藏GamingPlayerUI
                if (CPPC->PlayerUI)
                {
                    CPPC->PlayerUI->SetVisibility(ESlateVisibility::Visible);
                }
                ACPP_PlayerCharacter* Player = Cast<ACPP_PlayerCharacter>(CPPC->GetCharacter());
                if (Player)
                {
                    Player->ApplyEquipmentData();
                    CPPC->PlayerUI->UpdateHealth(
                        Player->GetCharacterAS()->GetHealth(),
                        Player->GetCharacterAS()->GetMaxHealth()
                    );
                }
            }
        }
        PC = nullptr;
        
        bIsCharacterDetailOpen = false;
	}

    GoBack->OnClicked.RemoveAll(this);
}

void UCharacterDetail::UnEquipped(const FEquipmentData& Equipment)
{
    UAccountData* CurPlayer = GetCurrentPlayer();
    if (!CurPlayer) return;

    UGASGameInstance* GameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!GameInstance) return;

    UEquipmentManager* EquipmentManager = GameInstance->GetEquipmentManager();
    if (!EquipmentManager) return;

    EquipmentManager->SetEquipmentEquipped(CurPlayer, Equipment.EquipmentID, false, GetWorld());

    EquipStateChanged();
}

UAccountData* UCharacterDetail::GetCurrentPlayer()
{
	UGASGameInstance* GameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!IsValid(GameInstance)) return nullptr;
    return GameInstance ? GameInstance->CurrentLoginAccount : nullptr;
}

void UCharacterDetail::OnLevelLoaded()
{
    // 查找摄像机和锚点
    FindDisplayLevelActors();
    // 切换到展示摄像机
    SwitchToDisplayCamera();
    // 在锚点生成角色副本
    SpawnDisplayCharacterAtAnchor(TEXT(""));
}

void UCharacterDetail::LoadPlayerOwnedCharacter()
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
                UWorld* World = GetWorld();
                if (World)
                {
                    ACPP_PlayerCharacter* TempCharacter = World->SpawnActor<ACPP_PlayerCharacter>(RowData->CharacterBlueprintClass);
                    if (TempCharacter)
                    {
                        // 初始化能力系统
                        TempCharacter->GetAbilitySystemComponent()->InitAbilityActorInfo(TempCharacter, TempCharacter);
                        CharacterObjectList.Add(TempCharacter);
                    }
                }
            }
        }
        if (CharacterSelect)
        {
            CharacterSelect->SetScrollbarVisibility(ESlateVisibility::Hidden);
            CharacterSelect->SetListItems(CharacterObjectList);
            UE_LOG(LogTemp, Log, TEXT("TileView updated with %d characters"), CharacterObjectList.Num());
        }
    }
}

void UCharacterDetail::OnEquipmentClicked(UObject* Item)
{
    UE_LOG(LogTemp, Error, TEXT("UCharacterDetail::OnEquipmentClicked: Click Item"));
    // 将点击的Item转换为装备数据
    UEquipmentItem* EquipmentStruct = Cast<UEquipmentItem>(Item);
    if (!EquipmentStruct)
    {
        UE_LOG(LogTemp, Error, TEXT("EquipmentWidget is Not Valid"));
        return;
    }

    // 从装备组件中获取具体装备数据
    FEquipmentData* SelectedEquipment = &EquipmentStruct->SingleEquipmentItem;
    if (!SelectedEquipment)
    {
        UE_LOG(LogTemp, Error, TEXT("SelectedEquipment is Not Valid"));
        return;
    }

    // 保存当前选中的装备
    CurrentSelectedEquipment = SelectedEquipment;

    UE_LOG(LogTemp, Error, TEXT("Get EquipmentData!!!!!!!!!!!!!!!!!!!"));

    // 更新装备名称显示
    if (EquipmentName)
    {
        EquipmentName->SetText(FText::FromString(SelectedEquipment->Name));
    }

    // 更新攻击加成显示
    if (EquipmentAttackValue)
    {
        FString AttackText = FString::Printf(TEXT("+%.1f%%"), SelectedEquipment->AttackBonus * 100);
        EquipmentAttackValue->SetText(FText::FromString(AttackText));
    }

    // 更新防御加成显示
    if (EquipmentDefenseValue)
    {
        FString DefenseText = FString::Printf(TEXT("+%.1f%%"), SelectedEquipment->DefenceBonus * 100);
        EquipmentDefenseValue->SetText(FText::FromString(DefenseText));
    }

    // 更新生命值加成显示
    if (EquipmentMaxHealthValue)
    {
        FString HealthText = FString::Printf(TEXT("+%.1f%%"), SelectedEquipment->HealthBonus * 100);
        EquipmentMaxHealthValue->SetText(FText::FromString(HealthText));
    }

    // 检查当前装备是否已装备到当前角色
    bool bIsEquipped = SelectedEquipment->bIsEquipped && 
                      SelectedEquipment->EquippedToCharacter == CurrentCharacter->CharacterName;

    //// 更新装备按钮状态
    if (EquippedUp)
    {
        EquippedUp->SetIsEnabled(!bIsEquipped);
        EquippedUp->OnClicked.RemoveAll(this);
        EquippedUp->OnClicked.AddDynamic(this, &UCharacterDetail::OnEquipClicked);
    }

    //// 更新卸下按钮状态
	if (EquippedDown)
	{
		EquippedDown->SetIsEnabled(bIsEquipped);
		EquippedDown->OnClicked.RemoveAll(this);
		EquippedDown->OnClicked.AddDynamic(this, &UCharacterDetail::OnUnequipClicked);
	}
    UE_LOG(LogTemp, Error, TEXT("Equipment State: %s"), bIsEquipped ? TEXT("True") : TEXT("False"));
}

void UCharacterDetail::EquipStateChanged()
{
    RefreshEquippedSlots();
    RefreshUnEquippedList();
    CalculateFinalAttributes(CurrentCharacter);
}

void UCharacterDetail::RefreshEquippedSlots()
{
    if (!CurrentCharacter) return; // 确保当前角色有效

    UAccountData* CurPlayer = GetCurrentPlayer();
    if (!CurPlayer) return;

    UGASGameInstance* GameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!GameInstance) return;

    UEquipmentManager* EquipmentManager = GameInstance->GetEquipmentManager();
    if (!EquipmentManager) return;

    TArray<FEquipmentData>& _AllEquipment = EquipmentManager->GetAllEquipment(CurPlayer);
    TMap<EEquipmentType, FEquipmentData> EquippedMap;

    // 筛选出属于当前角色的已装备装备
    for (FEquipmentData& Equip : _AllEquipment)
    {
        if (Equip.bIsEquipped && Equip.EquippedToCharacter == CurrentCharacter->CharacterName)
        {
            EquippedMap.Add(Equip.Type, Equip);
        }
    }

    // 更新装备槽显示
    auto SetSlotEquipment = [&](UEquipmentSlotWidget* SlotWidget)
    {
        if (!SlotWidget) return;

        EEquipmentType SlotType = SlotWidget->GetSlotType();
        if (EquippedMap.Contains(SlotType))
        {
            SlotWidget->UpdateSlotEquipment(EquippedMap[SlotType]);
        }
        else
        {
            SlotWidget->ClearSlotEquipment(); // 未装备时显示默认图标
        }
    };

    SetSlotEquipment(EquipmentSlotHead);
    SetSlotEquipment(EquipmentSlotChest);
    SetSlotEquipment(EquipmentSlotHand);
    SetSlotEquipment(EquipmentSlotLeg);
    UE_LOG(LogTemp, Error, TEXT("Refresh Equipment for current character end: %s"), *CurrentCharacter->CharacterName);
}

void UCharacterDetail::RefreshUnEquippedList()
{
	UAccountData* CurPlayer = GetCurrentPlayer();
    if (!CurPlayer)
    {
        UE_LOG(LogTemp, Error, TEXT("Get Player Failed"));
        return;
    }

    UGASGameInstance* GameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!GameInstance) return;
    UEquipmentManager* EquipmentManager = GameInstance->GetEquipmentManager();
    if (!EquipmentManager) return;

    // 直接从账号数据获取装备列表
    TArray<FEquipmentData>& OwnedEquipment = EquipmentManager->GetAllEquipment(CurPlayer);

    TArray<UObject*> EquipmentList;

    for (FEquipmentData& EquipInfo : OwnedEquipment)
    {
        if (!EquipInfo.bIsEquipped)
        {
            UEquipmentItem* Item = NewObject<UEquipmentItem>(this); // 以当前Widget为Outer，自动管理生命周期
            Item->SingleEquipmentItem = EquipInfo; // 绑定装备数据
            EquipmentList.Add(Item);
        }
    }

    // 更新UI列表
    EquipmentDisplayTileView->OnItemClicked().RemoveAll(this);
    EquipmentDisplayTileView->SetListItems(EquipmentList);
    EquipmentDisplayTileView->OnItemClicked().AddUObject(this, &UCharacterDetail::OnEquipmentClicked);
}

void UCharacterDetail::CalculateFinalAttributes(ACPP_PlayerCharacter* CurChar)
{
    if (!CurChar) return;

    UAbilitySystemComponent* ASC = CurChar->GetAbilitySystemComponent();
    if (!ASC) return;
    UPlayerAttributeSet* AttributeSet = const_cast<UPlayerAttributeSet*>(ASC->GetSet<UPlayerAttributeSet>());
    if (!AttributeSet) return;

    UGASGameInstance* GameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!GameInstance) return;
    UEquipmentManager* EquipmentManager = GameInstance->GetEquipmentManager();
    if (!EquipmentManager) return;

    float Attack = CurChar->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>()->GetAttack();
    float Defense = CurChar->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>()->GetDefence();
    float Health = CurChar->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>()->GetHealth();
    float CriticalR = CurChar->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>()->GetCriticalRate();
    float CriticalV = CurChar->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>()->GetCriticalValue();

    UAccountData* CurPlayer = GetCurrentPlayer();

    TArray<FEquipmentData>& _AllEquipment = EquipmentManager->GetAllEquipment(CurPlayer);
    for (auto& Equip : _AllEquipment)
    {
        if (Equip.bIsEquipped && Equip.EquippedToCharacter == CurChar->CharacterName)
        {
            Attack += (Equip.AttackBonus * Attack);
            Health += (Equip.HealthBonus * Health);
            Defense += (Equip.DefenceBonus * Defense);
        }
    }


    if (AttackValueText)
    {
        FString AttackStr = FString::Printf(TEXT("%.0f"), Attack);
        AttackValueText->SetText(FText::FromString(AttackStr));
    }
    if (DefenseValueText)
    {
        FString DefenseStr = FString::Printf(TEXT("%.0f"), Defense);
        DefenseValueText->SetText(FText::FromString(DefenseStr));
    }
    if (MaxHealthValueText)
    {
        FString HealthStr = FString::Printf(TEXT("%.0f"), Health);
        MaxHealthValueText->SetText(FText::FromString(HealthStr));
    }
    if (CriticalRate)
    {
        FString CriticalRStr = FString::Printf(TEXT("%.0f"), CriticalR);
        CriticalRate->SetText(FText::FromString(CriticalRStr));
    }
    if (CriticalValue)
    {
        FString CriticalVStr = FString::Printf(TEXT("%.0f"), CriticalV);
        CriticalValue->SetText(FText::FromString(CriticalVStr));
    }
}

void UCharacterDetail::FindDisplayLevelActors()
{
    if (!IsValid(GetWorld())) return;

    // 查找展示摄像机
    TArray<AActor*> CameraActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("DisplayCameraTag"), CameraActors);
    if (CameraActors.Num() > 0)
    {
        DisplayCamera = Cast<ACameraActor>(CameraActors[0]);
    }

    // 查找角色生成锚点
    TArray<AActor*> SpawnPoints;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("CharacterPoint"), SpawnPoints);
    if (SpawnPoints.Num() > 0)
    {
        CharacterSpawnPoint = SpawnPoints[0];
    }
}

void UCharacterDetail::SwitchToDisplayCamera()
{
    if (!PC || !IsValid(DisplayCamera)) return;

    // 保存原摄像机管理器
    ACPP_PlayerController* CPPC = Cast<ACPP_PlayerController>(PC);
    OriginalViewTarget = CPPC->GetCurrentCamera();
    // 强制切换到展示摄像机
    PC->SetViewTargetWithBlend(DisplayCamera, 0.2f);
    UCameraComponent* CameraComp = DisplayCamera->GetCameraComponent();
    CameraComp->bConstrainAspectRatio = false;
}

void UCharacterDetail::SpawnDisplayCharacterAtAnchor(FString _CharacterName)
{
    if (!IsValid(GetWorld()) || !IsValid(CharacterSpawnPoint) || !PC) return;

    if (CurrentCharacter && CurrentCharacter->CharacterName == _CharacterName) return;

    DestroyDisplayCharacter();

    ACPP_PlayerCharacter* OriginalCharacter = nullptr;

    if (!_CharacterName.Len())
    {
        // 获取原角色
        OriginalCharacter = Cast<ACPP_PlayerCharacter>(PC->GetPawn());
    }
    else
    {
        if (CharacterDataTable)
        {
            FAllCharactersInfo* RowData = CharacterDataTable->FindRow<FAllCharactersInfo>(FName(*_CharacterName), TEXT(""));
            if (RowData && RowData->CharacterBlueprintClass->IsChildOf(ACPP_PlayerCharacter::StaticClass()))
            {
                // 获取类的默认对象
                OriginalCharacter = Cast<ACPP_PlayerCharacter>(RowData->CharacterBlueprintClass->GetDefaultObject());
            }
        }
    }
    if (!IsValid(OriginalCharacter)) return;

    // 生成角色副本
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 用锚点的位置/旋转生成
    DisplayCharacter = GetWorld()->SpawnActor<ACPP_PlayerCharacter>(
        OriginalCharacter->GetClass(),
        CharacterSpawnPoint->GetActorLocation(),
        CharacterSpawnPoint->GetActorRotation(),
        SpawnParams
    );

    CurrentCharacter = DisplayCharacter;

    if (CharacterNameBlock)
    {
        CharacterNameBlock->SetText(FText::FromString(CurrentCharacter->CharacterName));
    }

    // 关闭副本的碰撞/输入/物理
    if (IsValid(DisplayCharacter))
    {
        DisplayCharacter->SetActorEnableCollision(false);
        if (DisplayCharacter->InputComponent)
        {
            DisplayCharacter->InputComponent->SetActive(false); // 直接禁用输入组件
        }
        DisplayCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (DisplayCharacter)
    {
        UAbilitySystemComponent* ASC = DisplayCharacter->GetAbilitySystemComponent();
        if (ASC)
        {
            ASC->InitAbilityActorInfo(DisplayCharacter, DisplayCharacter);
            DisplayCharacter->GiveStartupAbilities(); // 确保属性集被初始化
        }
        // 调用属性计算
        EquipStateChanged();
    }
}

void UCharacterDetail::RestoreOriginalCamera()
{
    if (!PC || !IsValid(OriginalViewTarget)) return;
    PC->SetViewTargetWithBlend(OriginalViewTarget, 0.2f);
}

void UCharacterDetail::DestroyDisplayCharacter()
{
    if (IsValid(DisplayCharacter))
    {
        DisplayCharacter->Destroy();
        DisplayCharacter = nullptr;
    }
}

void UCharacterDetail::OnEquipClicked()
{
    // 获取游戏实例和账号数据
    UGASGameInstance* GameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!GameInstance) return;

    UAccountData* CurrentAccount = GameInstance->CurrentLoginAccount;
    if (!CurrentAccount) return;

    // 获取当前选中的待装备物品
    if (!CurrentSelectedEquipment)
    {
        UE_LOG(LogTemp, Warning, TEXT("No equipment selected to equip"));
        return;
    }

    FEquipmentData* EquipData = CurrentAccount->OwnedEquipments.FindByPredicate([&](const FEquipmentData& Equip) {
        return Equip.EquipmentID == CurrentSelectedEquipment->EquipmentID;
    });
    if (!EquipData) return;

    // 获取装备管理器并执行装备操作
    UEquipmentManager* EquipmentManager = GameInstance->GetEquipmentManager();
    if (EquipmentManager && CurrentCharacter)
    {
        UEquipmentSlotWidget* TargetSlot = nullptr;
        switch (EquipData->Type)
        {
            case EEquipmentType::Head:
                TargetSlot = EquipmentSlotHead;
        	    break;
            case EEquipmentType::Hand:
                TargetSlot = EquipmentSlotHand;
        	    break;
            case EEquipmentType::Chest:
                TargetSlot = EquipmentSlotChest;
        	    break;
            case EEquipmentType::Leg:
                TargetSlot = EquipmentSlotLeg;
        	    break;
        }

        if (TargetSlot)
        {
            TargetSlot->SetSlotEquipment(*EquipData); // 调用槽位的装备设置函数
        }
        
        // 通知装备状态变更并刷新UI
        EquipStateChanged();
    }
}

void UCharacterDetail::OnUnequipClicked()
{
    // 获取游戏实例和账号数据
    UGASGameInstance* GameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!GameInstance) return;

    UAccountData* CurrentAccount = GameInstance->CurrentLoginAccount;
    if (!CurrentAccount) return;

    // 获取当前槽位的装备数据
    FEquipmentData* EquipData = CurrentSelectedEquipment;
    if (!EquipData || !EquipData->bIsEquipped)
    {
        UE_LOG(LogTemp, Warning, TEXT("No equipped item in current slot"));
        return;
    }

    // 获取装备管理器并执行卸下操作
    UEquipmentManager* EquipmentManager = GameInstance->GetEquipmentManager();
    if (EquipmentManager)
    {
        // 设置装备状态为未装备，清除角色关联
        EquipmentManager->SetEquipmentEquipped(CurrentAccount, EquipData->EquipmentID, false, GetWorld(), "");
        
        // 通知装备状态变更并刷新UI
        EquipStateChanged();
    }
}
