// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "FuncButtomItem.h"
#include "Components/TileView.h"
#include "FuncItemButtom.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API UFuncItemButtom : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
    // 绑定数据到UI
    virtual void NativeOnListItemObjectSet(UObject* InListItemObject) override;

    // 保存当前Item对应的Object
    UPROPERTY()
    UObject* ListItemObject;

     // 父级UTileView
    UPROPERTY(BlueprintReadWrite)
    UTileView* ParentTileView;

    // 点击事件回调
    FOnClicked OnButtonClicked;

    // 功能按钮数据
    UPROPERTY(BlueprintReadOnly)
    FFuncButtomInfo ButtonData;

    // 按钮图标
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UImage* ButtonIcon;

    UFUNCTION(BlueprintImplementableEvent, Category = "ButtonData")
    void OnButtonDataReady(); // 蓝图中实现此事件，用于加载图标

private:
    // 按钮点击事件
    UFUNCTION()
    void OnClick();

    // 按钮组件
    UPROPERTY(meta = (BindWidget))
    class UButton* FuncButton;

    // 按钮文本
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ButtonText;
};
