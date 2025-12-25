// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TeamManager.h"
#include "Engine/DataTable.h"
#include "CPP_PlayerState.generated.h"

class ACPP_PlayerCharacter;

UCLASS()
class GAS_SYSTEM_STUDY_API ACPP_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ACPP_PlayerState();

	UFUNCTION(BlueprintCallable, Category = "Attack")
	bool IsAttackInputRequested() const { return bAttackInputRequested; }                  //获取是否由于输入

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void SetAttackInputRequested(bool bRequested); //设置是否由于输入

	UPROPERTY(BlueprintReadOnly, Category = "Party", Replicated)
    UTeamManager* TeamManager;

	UFUNCTION(BlueprintCallable)
	bool GetRunningState() { return bIsRunning; }

	UFUNCTION(BlueprintCallable)
	bool GetWalkingState() { return bIsWalking; }

	UFUNCTION(BlueprintCallable)
	void SetRunningState(bool isRun) { bIsRunning = isRun; }

	UFUNCTION(BlueprintCallable)
	void SetWalkingState(bool isWalk) { bIsWalking = isWalk; }

	// 重载出战队伍
	UFUNCTION(BlueprintCallable, Category = "Party")
	void ReloadCurrentTeam();

	void StartPreInputTimer();
    void ClearPreInputTimer();

	// 清空预输入
	void ClearAttackInputRequest();

protected:

	virtual void BeginPlay() override;
	
private:
	UPROPERTY(Replicated)
	bool bAttackInputRequested = false;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FTimerHandle PreInputResetTimer;
    UPROPERTY(EditDefaultsOnly, Category = "Attack")
    float PreInputResetDelay = 0.3f; // 预输入有效时间

	UPROPERTY(EditAnywhere)
    UDataTable* CharacterDataTable;

	bool bIsRunning = false;
	bool bIsWalking = false;
};

