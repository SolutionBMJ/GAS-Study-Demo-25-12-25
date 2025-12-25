// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TeamManager.generated.h"

class ACPP_PlayerCharacter;
class ACPP_PlayerController;
class APlayerController;
/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API UTeamManager : public UObject
{
	GENERATED_BODY()
	
public:
    // 初始化管理器，传入玩家控制器和初始的角色类列表
    UFUNCTION(BlueprintCallable, Category = "Party")
    void Initialize(APlayerController* InPlayerController, const TArray<TSubclassOf<ACPP_PlayerCharacter>>& InitialCharacterClasses);

    // 切换到指定索引的角色 (0, 1, 2)
    UFUNCTION(BlueprintCallable, Category = "Party")
    void SwitchToCharacter(int32 TargetIndex);

    // 获取当前活跃的角色
    UFUNCTION(BlueprintPure, Category = "Party")
    ACPP_PlayerCharacter* GetCurrentCharacter() const { return CurrentCharacter; }

    UFUNCTION(BlueprintPure, Category = "Party")
    int32 GetCurrentCharacterIndex() const { return CurrentIndex; }

    // 获取编队中的所有角色实例
    UFUNCTION(BlueprintPure, Category = "Party")
    const TArray<ACPP_PlayerCharacter*>& GetPartyMembers() const { return PartyMembers; }

    UPROPERTY()
    ACPP_PlayerController* CurController;

    FVector OldLocation = FVector::ZeroVector;
    FRotator OldRotation = FRotator::ZeroRotator;
    int32 OldMemberIndex = -1;

private:

    // 持有玩家控制器的引用，用于切换Possess目标
    UPROPERTY()
    APlayerController* PlayerController;

    // 存储编队中所有角色的实例
    UPROPERTY()
    TArray<ACPP_PlayerCharacter*> PartyMembers;

    // 当前活跃的角色
    UPROPERTY()
    ACPP_PlayerCharacter* CurrentCharacter;

    // 当前活跃角色的索引
    int32 CurrentIndex = -1;
};
