// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentSlotWidget.generated.h"

class UCharacterDetail;
class UAccountData;
/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API UEquipmentSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 设置插槽中的装备
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetSlotEquipment(const FEquipmentData& InEquipment);

	// 更新插槽中的装备
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void UpdateSlotEquipment(const FEquipmentData& InEquipment);

	// 清空插槽中的装备
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void ClearSlotEquipment();

	// 获取当前插槽中的装备
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	const FEquipmentData& GetCurrentEquipment() const { return CurrentEquipment; }

	// 获取插槽类型
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	EEquipmentType GetSlotType() const { return SlotType; }

	// 让CharacterDetail主动传递自身引用
	UFUNCTION(BlueprintCallable, Category = "Widget")
    void SetCharacterDetail(UCharacterDetail* InParentWidget);

	UFUNCTION(BlueprintCallable)
	void OnClickedEquipmentSlot();

protected:
	virtual void NativeConstruct() override;

	// 装备图标显示控件
	UPROPERTY(meta = (BindWidget))
	class UImage* EquipmentIcon;

	// 插槽对应的装备类型
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot Settings")
	EEquipmentType SlotType;

	// 插槽默认图标路径
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FString DefaultIconPath;

private:
	// 当前插槽中的装备
	UPROPERTY()
	FEquipmentData CurrentEquipment;

	// 设置装备图标
	void UpdateEquipmentIcon(const FString& IconPath);

	// CharacterDetail指针
	UCharacterDetail* CharacterDetail;
};
