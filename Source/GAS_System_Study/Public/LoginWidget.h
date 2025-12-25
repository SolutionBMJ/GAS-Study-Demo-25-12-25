// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Input/Reply.h"  // 新增：用于键盘事件返回值
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void BeginGame();

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UEditableTextBox* UserNameInput;

    UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
    UEditableTextBox* PasswordInput;

    UFUNCTION(BlueprintCallable)
    bool TryLogin();

    UFUNCTION(BlueprintCallable)
    bool TryRegist();

    UFUNCTION(BlueprintCallable)
    void FastBoot();

protected:
    // 新增：重写键盘按下事件
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    // 新增：初始化时设置焦点
    virtual void NativeConstruct() override;
};
