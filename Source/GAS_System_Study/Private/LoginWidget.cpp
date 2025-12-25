// Solution_BMJ


#include "LoginWidget.h"
#include "GASGameInstance.h"
#include "TimerManager.h"
#include "CPP_PlayerState.h"
#include "AccountData.h"
#include "AccountRegisterAndReader.h"
#include "Kismet/GameplayStatics.h"

void ULoginWidget::BeginGame()
{
    UGASGameInstance* AGameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!IsValid(AGameInstance))
    {
        UE_LOG(LogTemp, Error, TEXT("ULoginWidget::GAMEINSTANCE is Not Valid!!!"));
        return;
    }
    AGameInstance->bIsPlayerLogin = true;
    RemoveFromParent();

    // 加载目标关卡
    UGameplayStatics::OpenLevel(GetWorld(), TEXT("Default_Map"));
}

bool ULoginWidget::TryLogin()
{
    FString UserName = UserNameInput->GetText().ToString();
    FString Password = PasswordInput->GetText().ToString();

    if (!UserName.Len() || !Password.Len())
    {
        UE_LOG(LogTemp, Log, TEXT("UserName or Password is Null!!!"));
        return false;
    }
    UE_LOG(LogTemp, Log, TEXT("UserName: %s , Password: %s"), *UserName, *Password);

    UGASGameInstance* GameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!IsValid(GameInstance))
    {
        UE_LOG(LogTemp, Log, TEXT("GameInstance is Not Valid"));
        return false;
    }
    UAccountData* UserData = nullptr;
    if (GameInstance->GetAccountRegisterAndReader()->LoginAccount(UserName, Password, UserData, GameInstance))
    {
        GameInstance->CurAccountName = GameInstance->GetAccountRegisterAndReader()->GetCurrentUserName();
        UE_LOG(LogTemp, Log, TEXT("Login!!!"));
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("UserName or Password Error!!!"));
        return false;
    }
}

bool ULoginWidget::TryRegist()
{
    FString UserName = UserNameInput->GetText().ToString();
    FString Password = PasswordInput->GetText().ToString();

    if (!UserName.Len() || !Password.Len())
    {
        UE_LOG(LogTemp, Log, TEXT("请输入文本"));
        return false;
    }
    UE_LOG(LogTemp, Log, TEXT("UserName: %s , Password: %s"), *UserName, *Password);

    UGASGameInstance* GameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!IsValid(GameInstance))
    {
        UE_LOG(LogTemp, Log, TEXT("GameInstance is Not Valid"));
        return false;
    }
    if (GameInstance->GetAccountRegisterAndReader()->RegisterAccount(UserName, Password))
    {
        UE_LOG(LogTemp, Log, TEXT("Resisted Successes!!! We Will Auto Login For You!"));
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Resisted Field!!! UserName Exist!"));
        return false;
    }
}

void ULoginWidget::FastBoot()
{
    UGASGameInstance* GameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!IsValid(GameInstance))
    {
        UE_LOG(LogTemp, Error, TEXT("FastBoot: GameInstance is invalid!"));
        return;
    }

    UAccountData* UserData = nullptr;

    if (!GameInstance->GetAccountRegisterAndReader())
    {
        return;
    }

    // 先尝试登录已有的"123"账号
    if (GameInstance->GetAccountRegisterAndReader()->LoginAccount("123", "123", UserData, GameInstance))
    {
        GameInstance->CurAccountName = GameInstance->GetAccountRegisterAndReader()->GetCurrentUserName();
        BeginGame();
        return;
    }

    // 登录失败（账号不存在），再尝试注册
    if (GameInstance->GetAccountRegisterAndReader()->RegisterAccount("123", "123"))
    {
        // 注册成功后再次登录
        if (GameInstance->GetAccountRegisterAndReader()->LoginAccount("123", "123", UserData, GameInstance))
        {
            GameInstance->CurAccountName = GameInstance->GetAccountRegisterAndReader()->GetCurrentUserName();
            BeginGame();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("FastBoot: Failed to login after registration!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("FastBoot: Registration failed (account may exist but login failed)!"));
    }
}

FReply ULoginWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    const FKey PressedKey = InKeyEvent.GetKey();

    // 1. 新增：按下X键触发快速登录（FastBoot）
    if (PressedKey == EKeys::X)
    {
        FastBoot(); // 调用快速登录函数
        return FReply::Handled(); // 消耗事件，防止穿透
    }

    // 2. 原有：Enter键尝试登录
    if (PressedKey == EKeys::Enter)
    {
        if (TryLogin())
        {
            BeginGame();  // 登录成功则进入游戏
        }
        return FReply::Handled();  // 消耗事件，防止穿透
    }

    // 3. 原有：Tab键切换输入框焦点
    if (PressedKey == EKeys::Tab)
    {
        if (UserNameInput->HasKeyboardFocus())
        {
            // 当前焦点在用户名框，切换到密码框
            PasswordInput->SetKeyboardFocus();
        }
        else if (PasswordInput->HasKeyboardFocus())
        {
            // 当前焦点在密码框，切换到用户名框
            UserNameInput->SetKeyboardFocus();
        }
        return FReply::Handled();  // 消耗事件，防止默认行为
    }

    // 其他按键交给父类处理
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void ULoginWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 确保输入框有效后设置焦点
    if (IsValid(UserNameInput))
    {
        UserNameInput->SetKeyboardFocus();
    }
}
