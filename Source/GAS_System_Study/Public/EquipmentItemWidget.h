// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/TileView.h"
#include "EquipmentManager.h"
#include "EquipmentItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API UEquipmentItemWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	// 当列表项数据被设置时调用
	virtual void NativeOnListItemObjectSet(UObject* InListItemObject) override;

	// 当前装备数据
	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	FEquipmentData CurrentEquipment;

	UObject* EquipmentObject;

	// 父级UTileView
    UPROPERTY(BlueprintReadWrite)
    UTileView* ParentTileView;

protected:
	// 装备图标显示控件
	UPROPERTY(meta = (BindWidget))
	class UImage* EquipmentIcon;

	// 装备图标的默认路径
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString DefaultIconPath = TEXT("/Game/UI/DefaultEquipmentIcon.DefaultEquipmentIcon");

private:
	// 加载并设置装备图标
	void SetEquipmentIcon(const FString& IconPath);

	UFUNCTION(BlueprintCallable)
	void OnClickEquipment();
};
