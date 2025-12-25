// Solution_BMJ


#include "TeamManager.h"
#include "CPP_PlayerCharacter.h"
#include "CPP_PlayerController.h"
#include "GamingPlayerUI.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UTeamManager::Initialize(APlayerController* InPlayerController, const TArray<TSubclassOf<ACPP_PlayerCharacter>>& InitialCharacterClasses)
{
    if (OldMemberIndex > -1)
    {
        UE_LOG(LogTemp, Error, TEXT("UTeamManager::Have OldCharacter!!!!"));
        OldLocation = PartyMembers[OldMemberIndex]->GetActorLocation();
        OldRotation = PartyMembers[OldMemberIndex]->GetActorRotation();
    }
    for (ACPP_PlayerCharacter* OldChar : PartyMembers)
    {

        if (OldChar)
        {
            UAbilitySystemComponent* ASC = CurrentCharacter->GetAbilitySystemComponent();
            if (ASC)
            {
                // 结束所有正在进行的能力
                ASC->CancelAllAbilities();
                // 清除所有攻击相关标签
                ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.Character.Attack")));
            }
            OldChar->Destroy(); // 安全销毁
        }
    }
    PartyMembers.Empty();

    PlayerController = InPlayerController;
    if (!PlayerController) return;

    if (OldLocation.IsNearlyZero() && OldRotation.IsNearlyZero())
    {
        // 获取玩家的出生点，作为所有角色的初始生成位置
        AGameModeBase* AGameMode = GetWorld()->GetAuthGameMode();
        if (!AGameMode)
        {
            UE_LOG(LogTemp, Warning, TEXT("GameMode Not Found!"));
            return;
        }
        AActor* StartPoint = AGameMode->FindPlayerStart(PlayerController);
        OldLocation = StartPoint ? StartPoint->GetActorLocation() : FVector::ZeroVector;
        OldRotation = StartPoint ? StartPoint->GetActorRotation() : FRotator::ZeroRotator;
    }

    UE_LOG(LogTemp, Warning, TEXT("UTeamManager::Create Character!"));

    // 根据传入的角色类列表，预生成所有角色
    for (TSubclassOf<ACPP_PlayerCharacter> CharClass : InitialCharacterClasses)
    {
        if (CharClass)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;  //无视碰撞强制生成

            ACPP_PlayerCharacter* NewCharacter = GetWorld()->SpawnActor<ACPP_PlayerCharacter>(
                CharClass,
                OldLocation,
                OldRotation,
                SpawnParams
            );
            if (NewCharacter)
            {
                PartyMembers.Add(NewCharacter);
                NewCharacter->SetActorHiddenInGame(true);                // 默认隐藏
                NewCharacter->SetActorTickEnabled(false);
                NewCharacter->SetActorEnableCollision(false);            //取消碰撞
                UE_LOG(LogTemp, Warning, TEXT("Add a New Character! %s"), *NewCharacter->CharacterName);
            }
        }
    }

    CurController = Cast<ACPP_PlayerController>(PlayerController);
    if (!CurController) return;

    CurController->PlayerUI->InitializeUI(PartyMembers);

    // 切换到第一个角色
    if (PartyMembers.Num() > 0)
    {
        SwitchToCharacter(0);
    }
}

void UTeamManager::SwitchToCharacter(int32 TargetIndex)
{
    OldMemberIndex = TargetIndex;
    if (TargetIndex < 0 || TargetIndex >= PartyMembers.Num()) return;
    ACPP_PlayerCharacter* TargetCharacter = PartyMembers[TargetIndex];
    if (!TargetCharacter || TargetCharacter == CurrentCharacter) return;

    // 获取旧角色
    ACPP_PlayerCharacter* OldCharacter = CurrentCharacter;

	if (CurrentCharacter)
	{
		UE_LOG(LogTemp, Log, TEXT("UTeamManager::SwitchToCharacter:Current Location: %s"), *CurrentCharacter->GetActorLocation().ToString());
	}
    

    bool bIsAttacking = OldCharacter && OldCharacter->IsAttacking();

    if (IsValid(PlayerController->GetPawn()))
    {
        if (bIsAttacking)
        {
            // 攻击中：新角色移动到旧角色右侧
            TargetCharacter->SetActorLocation(OldCharacter->GetActorLocation() + OldCharacter->GetActorRightVector() * 200.0f);
            TargetCharacter->SetActorRotation(OldCharacter->GetActorRotation());
        }
        else
        {
            // 非攻击：新角色原地切换
            TargetCharacter->SetActorLocation(PlayerController->GetPawn()->GetActorLocation());
            TargetCharacter->SetActorRotation(PlayerController->GetPawn()->GetActorRotation());
        }
    }
    else
    {
        // 切换队伍时，将新角色移动到旧角色位置
        TargetCharacter->SetActorLocation(OldLocation); 
        TargetCharacter->SetActorRotation(OldRotation);
    }
    
    FVector OldVelocity = FVector::ZeroVector;

    // 处理之前活跃的角色
    if (CurrentCharacter)
    {
        // 保存旧角色的当前速度
        OldVelocity = CurrentCharacter->GetVelocity();
        
        if (!bIsAttacking)
        {
            // 非攻击：立即隐藏
            OldCharacter->SetActorHiddenInGame(true);
            OldCharacter->SetActorTickEnabled(false);
            OldCharacter->SetActorEnableCollision(false);
            OldCharacter->GetCharacterMovement()->Velocity = FVector::ZeroVector;
        }
    }

    // 激活新的角色
    CurrentCharacter = TargetCharacter;
    CurrentIndex = TargetIndex;


    CurrentCharacter->SetActorHiddenInGame(false);
    CurrentCharacter->SetActorTickEnabled(true);
    CurrentCharacter->SetActorEnableCollision(true);                                     //开启碰撞

    if (!CurController) return;
    CurController->PlayerUI->OnCharacterSwitched(CurrentCharacter, TargetIndex);

    // 切换控制器的Possess目标
    PlayerController->Possess(CurrentCharacter);
    
    CurController->SetCameraFollowTarget(CurrentCharacter);

    if (!OldVelocity.IsZero())
    {
        // 设置新角色的初始速度
        CurrentCharacter->GetCharacterMovement()->Velocity = OldVelocity;
    }
}

