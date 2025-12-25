// Solution_BMJ


#include "PauseMenuWidget.h"
#include "Input/Reply.h" 
#include "AccountData.h"
#include "GASGameInstance.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "CPP_PlayerController.h"

void UPauseMenuWidget::OpenPauseMenu()
{
    if (!bIsPauseMenuOpen)
    {
        AddToViewport();
        if (!IsValid(GetWorld()))
        {
            return;
        }
        // 暂停游戏
        UGameplayStatics::SetGamePaused(GetWorld(), true);
        
        // 设置输入模式
        PC = GetOwningPlayer();
        if (PC.IsValid())
        {
            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
        }
        
        bIsPauseMenuOpen = true;
    }
}

void UPauseMenuWidget::ClosePauseMenu()
{
    if (bIsPauseMenuOpen)
    {
        RemoveFromParent();
        
        // 恢复游戏
        UGameplayStatics::SetGamePaused(GetWorld(), false);
        
        if (PC.IsValid())
        {
            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = false;
        }
        PC.Reset(); // 重置弱指针

        bIsPauseMenuOpen = false;
    }
}

void UPauseMenuWidget::ReturnToMainMenu()
{
    UGASGameInstance* AGameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!IsValid(AGameInstance))
    {
        UE_LOG(LogTemp, Error, TEXT("ULoginWidget::GAMEINSTANCE is Not Valid!!!"));
        return;
    }
    AGameInstance->bIsPlayerLogin = false;
    RemoveFromParent();

    UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenuMap"));
}

void UPauseMenuWidget::OnFuncItemClicked(UObject* Item)
{
    // 将点击的项转换为功能按钮数据对象
    UFuncButtomItem* FuncItem = Cast<UFuncButtomItem>(Item);
    if (FuncItem)
    {
        // 执行对应的功能
        ExecuteFunction(FuncItem->ButtonInfo.FunctionName);
    }
}

void UPauseMenuWidget::ExecuteFunction(FName FunctionName)
{
    // 从映射中查找并执行对应的函数
    if (FunctionMap.Contains(FunctionName))
    {
        FunctionMap[FunctionName]();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Function %s not found in FunctionMap"), *FunctionName.ToString());
    }
}

void UPauseMenuWidget::NativeConstruct()
{
    PC.Reset(); // 额外保险
    Super::NativeConstruct();
    // 初始设置为关闭状态
    bIsPauseMenuOpen = false;

    // 初始化映射，再加载按钮数据
    InitFunctionMap();

    UGASGameInstance* GameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!IsValid(GameInstance)) return;

    FString CurUserName = GameInstance->CurAccountName;
    UE_LOG(LogTemp, Error, TEXT("ULoginWidget::CurUserName is %s"), *CurUserName);

    CurAccount = GameInstance->GetAccountRegisterAndReader()->LoadAccountData(CurUserName);

    UserName->SetText(FText::FromString(CurAccount->PlayerName));
    UserLevel->SetText(FText::AsNumber(CurAccount->PlayerLevel));

    BackToLogin->OnClicked.AddDynamic(this, &UPauseMenuWidget::ReturnToMainMenu);

    if (FunctionButtonDataTable)
    {
        TArray<FFuncButtomInfo*> TempStructList;
        FunctionButtonDataTable->GetAllRows<FFuncButtomInfo>("", TempStructList);

        FunctionButtonsList.Empty();
        for (FFuncButtomInfo* StructPtr : TempStructList)
        {
            if (StructPtr)
            {
                UFuncButtomItem* ItemObj = NewObject<UFuncButtomItem>(this); // 创建 UObject 实例
                ItemObj->ButtonInfo = *StructPtr; // 复制结构体数据到 UObject 中
                FunctionButtonsList.Add(ItemObj);
            }
        }
        
        // 绑定数据到TileView
        if (FuncTile)
        {
            FuncTile->SetListItems(FunctionButtonsList);
            FuncTile->OnItemClicked().AddUObject(this, &UPauseMenuWidget::OnFuncItemClicked);
        }
    }
}

void UPauseMenuWidget::NativeDestruct()
{
    BackToLogin->OnClicked.RemoveAll(this);
    // 确保在销毁时恢复游戏状态
    if (bIsPauseMenuOpen)
    {
        ClosePauseMenu();
    }
    
    Super::NativeDestruct();
}

FReply UPauseMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    HandleShortcutKey(InKeyEvent.GetKey());
    return FReply::Handled();
}

void UPauseMenuWidget::InitFunctionMap()
{
    // 注册功能
    FunctionMap.Add("OnTeamMaker", [this]() 
    {
        ClosePauseMenu();
        // 调用打开编队界面的逻辑
        ACPP_PlayerController* PC = Cast<ACPP_PlayerController>(GetOwningPlayer());
        if (PC)
        {
            PC->OnTeamMaker();
        }
    });
}

void UPauseMenuWidget::HandleShortcutKey(const FKey& Key)
{
    if (Key == EKeys::Escape)
    {
        CloseMenu();
    }
    // 处理快捷键，例如按I打开背包，按C打开角色面板等
    // 具体逻辑可以在蓝图中重写或在这里实现
}
