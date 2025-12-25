// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "Blueprint/UserWidget.h"
#include "FuncItemButtom.h"
#include "FuncButtomItem.h"
#include "PauseMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // 打开暂停菜单
    UFUNCTION(BlueprintCallable)
    void OpenPauseMenu();
    
    // 关闭暂停菜单
    UFUNCTION(BlueprintCallable)
    void ClosePauseMenu();
    
    // 返回主菜单
    UFUNCTION(BlueprintCallable)
    void ReturnToMainMenu();

    UPROPERTY(BlueprintReadOnly, Category = "Pause Menu")
    bool bIsPauseMenuOpen = false;

    // 页面切换函数
    UFUNCTION(BlueprintImplementableEvent, Category = "SwitchPage")
    void SwitchToPage(FName PageName) const;

    // 离开页面
    UFUNCTION(BlueprintImplementableEvent, Category = "CloseMenu")
    void CloseMenu();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
    UDataTable* FunctionButtonDataTable;

    UPROPERTY(meta = (BindWidget))
    UButton* BackToLogin;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* UserName;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* UserLevel;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTileView* FuncTile;

    // 处理功能按钮点击
    UFUNCTION()
    void OnFuncItemClicked(UObject* Item);

    // 根据FunctionName执行对应的函数
    void ExecuteFunction(FName FunctionName);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    // 输入处理
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
    // 存储所有功能按钮数据
    TArray<UFuncButtomItem*> FunctionButtonsList;

    // 功能映射表
    TMap<FName, TFunction<void()>> FunctionMap;

    // 初始化功能映射
    void InitFunctionMap();

    // 处理快捷键
    void HandleShortcutKey(const FKey& Key);

    TWeakObjectPtr<APlayerController> PC; // 使用弱指针确保生命周期内PC对象有效

    class UAccountData* CurAccount;
};
