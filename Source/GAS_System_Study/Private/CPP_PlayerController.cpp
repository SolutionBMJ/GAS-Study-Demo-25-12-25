// Solution_BMJ


#include "CPP_PlayerController.h"
#include "AttackAttach.h"
#include "GamingPlayerUI.h"
#include "PauseMenuWidget.h"
#include "TeamMakerWidget.h"
#include "PlayerAttributeSet.h"
#include "CPP_PlayerCharacter.h"
#include "CharacterDetail.h"
#include "Attack_GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimInstance.h"
#include "PlayerCameraActor.h"
#include "GASGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"    // 用于获取角色并处理移动/跳跃
#include "Kismet/GameplayStatics.h"

// 初始化Controller
// *按路径查找UI类
ACPP_PlayerController::ACPP_PlayerController()
{
    static ConstructorHelpers::FClassFinder<UGamingPlayerUI> TempUIClass(TEXT("/Game/GameBP/GameUI/GamingUI"));
    if (TempUIClass.Succeeded())
    {
        PlayerUIClass = TempUIClass.Class;
    }
    static ConstructorHelpers::FClassFinder<UPauseMenuWidget> PauseMenuFinder(TEXT("/Game/GameBP/GameUI/PauseMenuUI"));
    if (PauseMenuFinder.Succeeded())
    {
        PauseMenuClass = PauseMenuFinder.Class;
    }
    static ConstructorHelpers::FClassFinder<UTeamMakerWidget> TeamMakerFinder(TEXT("/Game/GameBP/GameUI/TeamMakerUI"));
    if (TeamMakerFinder.Succeeded())
    {
        TeamMakerClass = TeamMakerFinder.Class;
    }
    static ConstructorHelpers::FClassFinder<UCharacterDetail> CharacterDetailFinder(TEXT("/Game/GameBP/GameUI/CharacterInfoUI"));
    if (CharacterDetailFinder.Succeeded())
    {
        CharacterDetailClass = CharacterDetailFinder.Class;
    }
}

// *创建PlayerCameraActor
void ACPP_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    UGASGameInstance* AGameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!IsValid(AGameInstance))
    {
        UE_LOG(LogTemp, Error, TEXT("ACPP_PlayerController::GAMEINSTANCE is Not Valid!!!"));
        return;
    }
    if (AGameInstance->bIsPlayerLogin)
    {
        // 1. 创建FollowCameraActor
        CurrentFollowCamera = GetWorld()->SpawnActor<APlayerCameraActor>(APlayerCameraActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = false; // 隐藏鼠标光标
        UE_LOG(LogTemp, Error, TEXT("ACPP_PlayerController::Create Camera!!!"));

        ACPP_PlayerCharacter* ACharacter = Cast<ACPP_PlayerCharacter>(GetPawn());
        if (!IsValid(ACharacter)) return;
        ACPP_PlayerState* APlayerState = Cast<ACPP_PlayerState>(ACharacter->GetPlayerState());
        if (!IsValid(APlayerState)) return;

        AGameInstance->CurrentPlayerState = APlayerState;
        UE_LOG(LogTemp, Error, TEXT("ACPP_PlayerController::State Address: %p"), APlayerState);

    }
    else
    {
        OpenLoginUI();
    }
}

void ACPP_PlayerController::OpenLoginUI()
{
    if (MainMenuWidgetClass)
    {
        CurrentUIWidget = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
        if (CurrentUIWidget)
        {
            CurrentUIWidget->AddToViewport(); // 将UI添加到屏幕
            UE_LOG(LogTemp, Warning, TEXT("ACPP_PlayerController::MainMenuUI Add to Viewport"));
            // 设置输入模式：UI+鼠标（可点击UI，显示光标）
            SetInputMode(FInputModeUIOnly());
            bShowMouseCursor = true; // 显示鼠标光标
        }
    }
}

// *添加输入映射上下文
// *绑定输入
// *创建攻击吸附组件
// *将GamingUI添加到视口
//
void ACPP_PlayerController::SetupInputComponent()
{
    UGASGameInstance* AGameInstance = Cast<UGASGameInstance>(GetGameInstance());
    if (!IsValid(AGameInstance))
    {
        UE_LOG(LogTemp, Error, TEXT("ACPP_PlayerController::GAMEINSTANCE is Not Valid!!!"));
        return;
    }
    if (!AGameInstance->bIsPlayerLogin)
    {
        UE_LOG(LogTemp, Error, TEXT("ACPP_PlayerController::Player Not Login!"));
        return;
    }
    UE_LOG(LogTemp, Error, TEXT("ACPP_PlayerController::Player Login!"));
	Super::SetupInputComponent();

    // 获取增强输入子系统
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        // 添加输入映射上下文
        Subsystem->AddMappingContext(MoveMappingContext, 1);
        Subsystem->AddMappingContext(AttackMappingContext, 1);
        Subsystem->AddMappingContext(SwitchMappingContext, 1);
        Subsystem->AddMappingContext(FuncMappingContext, 1);
    }

    // 获取增强输入组件
    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

    if (!IsValid(EnhancedInputComponent)) return;
        
    // 绑定移动输入
    EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::OnJump);
    EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ThisClass::StopJump);
    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::OnMove);
    EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ThisClass::OnMoveStopped);
    EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::OnLook);
    EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ThisClass::StartDashOrRunning);
    EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Canceled, this, &ThisClass::OnDash);
    EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ThisClass::OnRunning);
    EnhancedInputComponent->BindAction(CameraAction, ETriggerEvent::Started, this, &ThisClass::ControlCameraDistance);
    EnhancedInputComponent->BindAction(WalkSwitchAction, ETriggerEvent::Started, this, &ThisClass::SwitchWalkAndRun);

    EnhancedInputComponent->BindAction(NormalAttackAction, ETriggerEvent::Canceled, this, &ThisClass::OnNormalAttackInput);
    EnhancedInputComponent->BindAction(NormalAttackAction, ETriggerEvent::Triggered, this, &ThisClass::OnHoldNormalAttackInput);

    EnhancedInputComponent->BindAction(Skill_One_Action, ETriggerEvent::Canceled, this, &ThisClass::OnSkill_One);
    EnhancedInputComponent->BindAction(Skill_One_Action, ETriggerEvent::Triggered, this, &ThisClass::OnHoldSkill_OneInput);
    EnhancedInputComponent->BindAction(Skill_Two_Action, ETriggerEvent::Canceled, this, &ThisClass::OnSkill_Two);
    EnhancedInputComponent->BindAction(Skill_Two_Action, ETriggerEvent::Triggered, this, &ThisClass::OnHoldSkill_TwoInput);

    EnhancedInputComponent->BindAction(switch1, ETriggerEvent::Started, this, &ThisClass::ChangeToCharacter1);
    EnhancedInputComponent->BindAction(switch2, ETriggerEvent::Started, this, &ThisClass::ChangeToCharacter2);
    EnhancedInputComponent->BindAction(switch3, ETriggerEvent::Started, this, &ThisClass::ChangeToCharacter3);

    EnhancedInputComponent->BindAction(Esc, ETriggerEvent::Started, this, &ThisClass::OnPause);
    EnhancedInputComponent->BindAction(Team, ETriggerEvent::Started, this, &ThisClass::OnTeamMaker);
    EnhancedInputComponent->BindAction(CharacterInfo, ETriggerEvent::Started, this, &ThisClass::OnCharacterDetail);

    AttackAttachComponent = NewObject<UAttackAttach>(this, TEXT("AATC"));
    if (PlayerUIClass)
    {
        PlayerUI = CreateWidget<UGamingPlayerUI>(this, PlayerUIClass);
        if (PlayerUI)
        {
            PlayerUI->AddToViewport(); // 添加到视口显示
            UE_LOG(LogTemp, Error, TEXT("ACPP_PlayerController::GamingUI Add to Viewport"));
        }
    }
}

// -------------
// *获取GA优先级标签后，调用对应函数，并传入GA序号和优先级标签
void ACPP_PlayerController::OnNormalAttackInput()
{
	// 普攻的优先级是Low
    FGameplayTag NewPriority = GetAbilityPriorityByTag(NormalAttackAbilityTag);
    TryActivateAbilityByTag(NormalAttackAbilityTag, NewPriority);
}

void ACPP_PlayerController::OnSkill_One()
{
    // 技能一的优先级是Medium
    FGameplayTag NewPriority = GetAbilityPriorityByTag(SkillOneAbilityTag);
    TryActivateAbilityByTag(SkillOneAbilityTag, NewPriority);
}

void ACPP_PlayerController::OnSkill_Two()
{
    // 技能二的优先级是High
    FGameplayTag NewPriority = GetAbilityPriorityByTag(SkillTwoAbilityTag);
    TryActivateAbilityByTag(SkillTwoAbilityTag, NewPriority);
}

void ACPP_PlayerController::OnHoldNormalAttackInput()
{
    FGameplayTag NewPriority = GetAbilityPriorityByTag(HoldNormalAttackAbilityTag);
    TryActivateAbilityByTag(HoldNormalAttackAbilityTag, NewPriority);
}

void ACPP_PlayerController::OnHoldSkill_OneInput()
{
    FGameplayTag NewPriority = GetAbilityPriorityByTag(HoldSkillOneAbilityTag);
    TryActivateAbilityByTag(HoldSkillOneAbilityTag, NewPriority);
}

void ACPP_PlayerController::OnHoldSkill_TwoInput()
{
    FGameplayTag NewPriority = GetAbilityPriorityByTag(HoldSkillTwoAbilityTag);
    TryActivateAbilityByTag(HoldSkillTwoAbilityTag, NewPriority);
}

// -------------

// *控制摄像机距离
void ACPP_PlayerController::ControlCameraDistance(const FInputActionValue& Value)
{
    if (!CurrentFollowCamera) return;
    float input = Value.GetMagnitude();
    const float DistanceStep = 50.0f;  // 每次调整的距离步长
    const float HeightRatio = 0.18f;   // 高度与距离的比例（基于18/100和105/600的近似值）
    const float HeightStep = DistanceStep * HeightRatio;

    float NewDistance = CurrentFollowCamera->FollowDistance;
    float NewHeight = CurrentFollowCamera->FollowHeight;

    if (input > 0.1)
    {
        NewDistance = FMath::Clamp(NewDistance - DistanceStep, 100.0f, 600.0f);
    }
    else if (input < -0.1)
    {
        NewDistance = FMath::Clamp(NewDistance + DistanceStep, 100.0f, 600.0f);
    }
    CurrentFollowCamera->FollowDistance = NewDistance;
}

// *打开暂停菜单
void ACPP_PlayerController::OnPause()
{
    if (!PauseMenuUI)
    {
        // 创建暂停菜单
        PauseMenuUI = CreateWidget<UPauseMenuWidget>(this, PauseMenuClass);
    }
    
    if (PauseMenuUI)
    {
        if (PauseMenuUI->bIsPauseMenuOpen)
        {
            PauseMenuUI->ClosePauseMenu();
        }
        else
        {
            PauseMenuUI->OpenPauseMenu();
        }
    }
}

// *打开编队界面
void ACPP_PlayerController::OnTeamMaker()
{
    if (TeamMakerUI)
    {
        TeamMakerUI->CloseTeamMaker();
        TeamMakerUI = nullptr;
    }
	TeamMakerUI = CreateWidget<UTeamMakerWidget>(this, TeamMakerClass);

    if (TeamMakerUI)
    {
        if (TeamMakerUI->bIsTeamMakerOpen)
        {
            TeamMakerUI->CloseTeamMaker();
        }
        else
        {
            TeamMakerUI->OpenTeamMaker();
        }
    }
}

void ACPP_PlayerController::OnCharacterDetail()
{
    if (CharacterDetailUI)
    {
        CharacterDetailUI->CloseCharacterDetail();
        CharacterDetailUI = nullptr;
    }
	CharacterDetailUI = CreateWidget<UCharacterDetail>(this, CharacterDetailClass);

    if (CharacterDetailUI)
    {
        if (CharacterDetailUI->bIsCharacterDetailOpen)
        {
            CharacterDetailUI->CloseCharacterDetail();
        }
        else
        {
            CharacterDetailUI->OpenCharacterDetail();
        }
    }
}

// *依靠当前传入的优先级标签查看是否能打断，并尝试激活序号对应GA
void ACPP_PlayerController::TryActivateAbilityByTag(const FGameplayTag& AbilityTag, FGameplayTag NewPriority)
{
    ACPP_PlayerCharacter* PlayerCharacter = Cast<ACPP_PlayerCharacter>(GetPawn());
    if (!IsValid(PlayerCharacter))
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter is Not Valid"));
        return;
    }

    UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
    if (!IsValid(ASC))
    {
        UE_LOG(LogTemp, Warning, TEXT("ASC is Not Valid"));
        return;
    }

    ACPP_PlayerState* APlayerState = Cast<ACPP_PlayerState>(PlayerCharacter->GetPlayerState());
    if (!IsValid(APlayerState))
    {
        UE_LOG(LogTemp, Warning, TEXT("APlayerState is Not Valid"));
        return;
    }

    //  优先级低于当前 GA 且攻击仍在持续，则无视
    if (!CanInterruptCurrentAbility(NewPriority) && ASC->HasMatchingGameplayTag(AttackTag))
    {
        UE_LOG(LogTemp, Warning, TEXT("Priority is Lower, Block it!!!"));
        return;
    }
    
    UAttack_GameplayAbility* AttackAbilityInstance = nullptr;

    // 尝试激活对应 GA
    TArray<FGameplayAbilitySpec> AbilitySpecs = ASC->GetActivatableAbilities();
    bool bIsActive = true;
    for (const FGameplayAbilitySpec& Spec : AbilitySpecs)
    {
        if (Spec.Ability)
        {
            if (Spec.Ability->AbilityTags.HasTagExact(AbilityTag))
            {
                // 激活找到的 GA
                AttackAbilityInstance = Cast<UAttack_GameplayAbility>(Spec.Ability);
                AttackTag = AttackAbilityInstance->GetAttackTag();
                PriorityTag = AttackAbilityInstance->AbilityPriorityTag;
                bIsActive = ASC->TryActivateAbility(Spec.Handle);
                break;
            }
        }
    }
    // 若未成功激活，由于能运行到这里代表着大概率是不同 GA，故能直接覆盖；但若激活失败，代表肯定是同一个GA
    if (!bIsActive && AttackTag.IsValid())
    {
        // 在攻击窗口内
        if (ASC->HasMatchingGameplayTag(AttackTag))
        {
        // 添加预输入
            UE_LOG(LogTemp, Warning, TEXT("Already has Attack, add Request"));
            APlayerState->SetAttackInputRequested(true);
            return;
        }
        // 不在攻击窗口内
        else
        {
        // 通知 GA 攻击
            UE_LOG(LogTemp, Warning, TEXT("Broadcast Attack"));
            OnPlaySectionNotify.Broadcast(AttackAbilityInstance->GetAttackTag(), true);
        }
    }
    else if (!AttackTag.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("AttackTag is Not Valid"));
    }
}

void ACPP_PlayerController::GetAnimNotify()
{
    ACPP_PlayerCharacter* ACharacter = Cast<ACPP_PlayerCharacter>(GetPawn());
    if (!IsValid(ACharacter)) return;
	ACPP_PlayerState* APlayerState = Cast<ACPP_PlayerState>(ACharacter->GetPlayerState());
    if (!IsValid(APlayerState)) return;
    if (APlayerState->IsAttackInputRequested())
    {
        APlayerState->SetAttackInputRequested(false);
        // 通知 GA 攻击
        OnPlaySectionNotify.Broadcast(AttackTag, true);
    }
    else
    {
        // 通知 GA 无攻击
        OnPlaySectionNotify.Broadcast(AttackTag, false);
    }
}

void ACPP_PlayerController::ForceFaceToInput()
{
    ACPP_PlayerCharacter* PlayerCharacter = Cast<ACPP_PlayerCharacter>(GetPawn());
    if (!IsValid(PlayerCharacter)) return;

    if (!InputDir.IsNearlyZero())
    {
        InputDir.Normalize();
        FRotator InputRotation = InputDir.Rotation();
        PlayerCharacter->SetActorRotation(InputRotation);
    }
    return;
}

void ACPP_PlayerController::FaceToTarget()
{
    ACPP_PlayerCharacter* PlayerCharacter = Cast<ACPP_PlayerCharacter>(GetPawn());
    if (!IsValid(PlayerCharacter)) return;
    AttackAttachComponent->Player_FaceToActor(PlayerCharacter);
    return;
}

bool ACPP_PlayerController::CanInterruptCurrentAbility(FGameplayTag NewAbilityPriority) const
{
    if (PriorityTag.IsValid() == false)
    {
        return true; // 无当前技能，直接允许激活
    }

    // High 可以打断 Medium 和 Low
    if (NewAbilityPriority.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Player.Priority.High"))))
    {
        return true && !PriorityTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Player.Priority.High")));
    }
    // Medium 只能打断普攻 Low
    else if (NewAbilityPriority.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Player.Priority.Medium"))))
    {
        return PriorityTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Player.Priority.Medium"))) || 
            PriorityTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Player.Priority.Low")));
    }
    // Low 不能打断任何技能
    else if (NewAbilityPriority.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Player.Priority.Low"))))
    {
        return PriorityTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Player.Priority.Low")));
    }

    return false;
}

void ACPP_PlayerController::SetCameraFollowTarget(ACPP_PlayerCharacter* Target)
{
    // 将 PlayerController 的视角目标设置为这个新相机
    if (!CurrentFollowCamera) return;

    AActor* TargetCharacter = Cast<AActor>(Target);
    if (!TargetCharacter) return;

    CurrentFollowCamera->StartTransition(TargetCharacter);

    SetViewTarget(CurrentFollowCamera);
}

FGameplayTag ACPP_PlayerController::GetAbilityPriorityByTag(const FGameplayTag& AbilityTag)
{
    ACPP_PlayerCharacter* PlayerCharacter = Cast<ACPP_PlayerCharacter>(GetPawn());
    if (!IsValid(PlayerCharacter)) return FGameplayTag();

    UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
    if (!IsValid(ASC)) return FGameplayTag();

    // 通过 Tag 查找对应的 GA
    TArray<FGameplayAbilitySpec*> AbilitySpecs;
    ASC->GetActivatableGameplayAbilitySpecsByAllMatchingTags(FGameplayTagContainer(AbilityTag), AbilitySpecs);

    for (FGameplayAbilitySpec* Spec : AbilitySpecs)
    {
        if (Spec && Spec->Ability)
        {
            // 检查 GA 是否包含目标 Tag（假设 GA 的 AbilityTags 中定义了对应 Tag）
            if (Spec->Ability->AbilityTags.HasTagExact(AbilityTag))
            {
                UAttack_GameplayAbility* TargetGA = Cast<UAttack_GameplayAbility>(Spec->Ability);
                if (IsValid(TargetGA) && TargetGA->AbilityPriorityTag.IsValid())
                {
                    return TargetGA->AbilityPriorityTag; // 返回GA配置的优先级标签
                }
                else
                {
                    return FGameplayTag();
                }
            }
        }
    }
    return FGameplayTag();
}

void ACPP_PlayerController::ChangeToCharacter1()
{
    BeginChange(0);
}

void ACPP_PlayerController::ChangeToCharacter2()
{
    BeginChange(1);
}

void ACPP_PlayerController::ChangeToCharacter3()
{
    BeginChange(2);
}

void ACPP_PlayerController::BeginChange(int32 index)
{
    ACPP_PlayerCharacter* PlayerCharacter = Cast<ACPP_PlayerCharacter>(GetPawn());
    if (!IsValid(PlayerCharacter)) return;
    ACPP_PlayerState* APlayerState = Cast<ACPP_PlayerState>(PlayerCharacter->GetPlayerState());
    if (!IsValid(APlayerState)) return;

    if (APlayerState->TeamManager->GetCurrentCharacterIndex() == index) return;
    
    APlayerState->TeamManager->SwitchToCharacter(index);

    AttackTag = FGameplayTag();
    PriorityTag = FGameplayTag();
}

void ACPP_PlayerController::OnJump()
{
    if (!IsValid(GetCharacter())) return;
    GetCharacter()->Jump();
    ACPP_PlayerCharacter* PlayerCharacter = Cast<ACPP_PlayerCharacter>(GetCharacter());
    if (!PlayerCharacter) return;

    UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();
    if (!ASC) return;
    TestGE(ASC);
}

void ACPP_PlayerController::StopJump()
{
    if (!IsValid(GetCharacter())) return;
    GetCharacter()->StopJumping();
}

void ACPP_PlayerController::OnMove(const FInputActionValue& Value)
{
    if (!IsValid(GetPawn())) return;

    // 获取玩家状态和能力系统组件
    ACPP_PlayerCharacter* ACharacter = Cast<ACPP_PlayerCharacter>(GetPawn());
    UAnimInstance* AnimInstance = ACharacter->GetMesh()->GetAnimInstance();
    ACPP_PlayerState* APlayerState = Cast<ACPP_PlayerState>(ACharacter->GetPlayerState());
    UAbilitySystemComponent* ASC = ACharacter->GetAbilitySystemComponent();

    if (!IsValid(ACharacter) || !IsValid(APlayerState) || !IsValid(ASC)) return;


    FVector2D MoveVector = Value.Get<FVector2D>();

    if (!CurrentFollowCamera) return;
    FRotator CameraRotation = CurrentFollowCamera->GetActorRotation();
    CameraRotation.Pitch = 0.0f;
    CameraRotation.Roll = 0.0f;

    FVector WorldDirection(MoveVector.Y, MoveVector.X, 0.0f);
    WorldDirection = CameraRotation.RotateVector(WorldDirection);

    InputDir = WorldDirection;

    if (ASC->HasMatchingGameplayTag(AttackTag))
    {
        return;
    }

    AttackTag = FGameplayTag();
    PriorityTag = FGameplayTag();

    AnimInstance->StopAllMontages(0.3f);
    ACharacter->AddMovementInput(WorldDirection, 1.0f);
}

void ACPP_PlayerController::OnMoveStopped()
{
    ACPP_PlayerCharacter* PlayerCharacter = Cast<ACPP_PlayerCharacter>(GetPawn());
    if (!IsValid(PlayerCharacter)) return;

    ACPP_PlayerState* APlayerState = Cast<ACPP_PlayerState>(PlayerCharacter->GetPlayerState());
    if (!IsValid(APlayerState)) return;

    InputDir = FVector::ZeroVector;

    APlayerState->SetRunningState(false);
    // 修改移动速度
    PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void ACPP_PlayerController::OnLook(const FInputActionValue& Value)
{
    FVector2D LookVector = Value.Get<FVector2D>();

    // 是否有摄像机
    if (CurrentFollowCamera)
    {
        // 输入向量给摄像机旋转函数
        CurrentFollowCamera->AddViewRotation(LookVector);
    }
}

void ACPP_PlayerController::StartDashOrRunning()
{
    // 冲刺的优先级是High
    ForceFaceToInput();
    FGameplayTag NewPriority = GetAbilityPriorityByTag(DashAbilityTag);
    TryActivateAbilityByTag(DashAbilityTag, NewPriority);
    return;
}

void ACPP_PlayerController::OnDash()
{
    return;
}

void ACPP_PlayerController::OnRunning()
{
    ACPP_PlayerCharacter* PlayerCharacter = Cast<ACPP_PlayerCharacter>(GetPawn());
    if (!IsValid(PlayerCharacter)) return;

    // 设置跑步状态为true
    ACPP_PlayerState* APlayerState = Cast<ACPP_PlayerState>(PlayerCharacter->GetPlayerState());
    if (!IsValid(APlayerState)) return;

    APlayerState->SetRunningState(true);
    APlayerState->SetWalkingState(false);

    // 修改移动速度
    PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
}

void ACPP_PlayerController::SwitchWalkAndRun()
{
    ACPP_PlayerCharacter* PlayerCharacter = Cast<ACPP_PlayerCharacter>(GetPawn());
    if (!IsValid(PlayerCharacter)) return;

    // 设置走路状态为true
    ACPP_PlayerState* APlayerState = Cast<ACPP_PlayerState>(PlayerCharacter->GetPlayerState());
    if (!IsValid(APlayerState)) return;

    if (APlayerState->GetWalkingState())
    {
        APlayerState->SetWalkingState(false);
        PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
    }
    else
    {
        APlayerState->SetWalkingState(true);
        APlayerState->SetRunningState(false);
        PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = 150.0f;
    }
}
