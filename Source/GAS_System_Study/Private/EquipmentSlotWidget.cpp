// Solution_BMJ


#include "EquipmentSlotWidget.h"
#include "AccountData.h"
#include "Components/Image.h"
#include "CharacterDetail.h"
#include "Engine/Texture2D.h"
#include "GASGameInstance.h"
#include "EquipmentManager.h"
#include "Kismet/GameplayStatics.h"

void UEquipmentSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 初始化时显示默认图标
	if (EquipmentIcon)
	{
		UpdateEquipmentIcon(DefaultIconPath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EquipmentIcon not bound in EquipmentSlotWidget!"));
	}
}

void UEquipmentSlotWidget::SetSlotEquipment(const FEquipmentData& InEquipment)
{
	// 检查装备类型是否与插槽匹配
	if (InEquipment.Type != SlotType) return;

	UGASGameInstance* GameInstance = Cast<UGASGameInstance>(GetWorld()->GetGameInstance());
	UE_LOG(LogTemp, Log, TEXT("Try to Change Equipment State"));
    if (!GameInstance) return;

        // 获取当前玩家的账号数据
    UAccountData* CurrentAccount = GameInstance->CurrentLoginAccount;
	if (!CurrentAccount) return;

	// 通过EquipmentManager设置装备为"已装备"状态
	UEquipmentManager* EquipmentManager = GameInstance->GetEquipmentManager();
	if (EquipmentManager)
	{
		UE_LOG(LogTemp, Log, TEXT("EquipmentManager is Valid"));
		UWorld* World = GetWorld();
		EquipmentManager->SetEquipmentEquipped(CurrentAccount, InEquipment.EquipmentID, true, World, CharacterDetail->CurrentCharacter->CharacterName);
		CharacterDetail->EquipStateChanged();
	}

	FEquipmentData* LatestEquipData = GameInstance->CurrentLoginAccount->OwnedEquipments.FindByPredicate([&](const FEquipmentData& Equip) {
        return Equip.EquipmentID == InEquipment.EquipmentID;
    });
	if (LatestEquipData)
	{
		UpdateSlotEquipment(*LatestEquipData);
	}
}

void UEquipmentSlotWidget::UpdateSlotEquipment(const FEquipmentData& InEquipment)
{
	CurrentEquipment = InEquipment;
	UpdateEquipmentIcon(InEquipment.IconPath);
}

void UEquipmentSlotWidget::ClearSlotEquipment()
{
	FString Name;
	GetName(Name);
	UE_LOG(LogTemp, Error, TEXT("Clear Slot! %s"), *Name);
	CurrentEquipment = FEquipmentData(); // 清空装备数据
	UpdateEquipmentIcon(DefaultIconPath); // 显示默认图标
}

void UEquipmentSlotWidget::SetCharacterDetail(UCharacterDetail* InParentWidget)
{
	if (InParentWidget)
    {
        CharacterDetail = InParentWidget;
		UE_LOG(LogTemp, Error, TEXT("Bind CharacterDetail!"));
    }
}

void UEquipmentSlotWidget::OnClickedEquipmentSlot()
{
	if (CharacterDetail)
	{
		UEquipmentItem* EquipmentStruct = NewObject<UEquipmentItem>();
		EquipmentStruct->SingleEquipmentItem = CurrentEquipment;

		UE_LOG(LogTemp, Error, TEXT("UEquipmentSlotWidget: Equipment State: %s"), CurrentEquipment.bIsEquipped ? TEXT("True") : TEXT("False"));

		UObject* EquipmentObject = Cast<UObject>(EquipmentStruct);
		CharacterDetail->EquipmentDisplayTileView->OnItemClicked().Broadcast(EquipmentObject);
	}
}

void UEquipmentSlotWidget::UpdateEquipmentIcon(const FString& IconPath)
{
	if (!EquipmentIcon)
	{
		UE_LOG(LogTemp, Error, TEXT("EquipmentIcon is null in UpdateEquipmentIcon!"));
		return;
	}

	UTexture2D* IconTexture = nullptr;
	
	// 尝试加载指定图标
	if (!IconPath.IsEmpty())
	{
		IconTexture = LoadObject<UTexture2D>(nullptr, *IconPath);
	}

	// 如果加载失败，使用默认图标
	if (!IconTexture)
	{
		IconTexture = LoadObject<UTexture2D>(nullptr, *DefaultIconPath);
		if (!IconTexture)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load default slot icon! Path: %s"), *DefaultIconPath);
			return;
		}
	}

	// 设置图标
	EquipmentIcon->SetBrushFromTexture(IconTexture);
}


