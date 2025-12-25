// Solution_BMJ


#include "EquipmentItemWidget.h"
#include "AccountData.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Kismet/GameplayStatics.h"

void UEquipmentItemWidget::NativeOnListItemObjectSet(UObject* InListItemObject)
{
	// 调用父类实现
	IUserObjectListEntry::NativeOnListItemObjectSet(InListItemObject);

	EquipmentObject = InListItemObject;

	// 尝试将传入的对象转换为FEquipmentData
	if (InListItemObject)
    {
		if (UEquipmentItem* EquipmentItem = Cast<UEquipmentItem>(InListItemObject))
		{
			CurrentEquipment = EquipmentItem->SingleEquipmentItem;
			SetEquipmentIcon(CurrentEquipment.IconPath);
		}
    }
}

void UEquipmentItemWidget::SetEquipmentIcon(const FString& IconPath)
{
	if (!EquipmentIcon)
	{
		UE_LOG(LogTemp, Error, TEXT("EquipmentIcon not bound in EquipmentItemWidget!"));
		return;
	}

	// 加载图标资源
	UTexture2D* IconTexture = nullptr;
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
			UE_LOG(LogTemp, Warning, TEXT("Failed to load default equipment icon!"));
			return;
		}
	}
	// 设置图标到控件
	EquipmentIcon->SetBrushFromTexture(IconTexture);
}

void UEquipmentItemWidget::OnClickEquipment()
{
	UE_LOG(LogTemp, Warning, TEXT("Select Equipment!"));
	if (ParentTileView && EquipmentObject)
	{
		ParentTileView->OnItemClicked().Broadcast(EquipmentObject);
	}
}
