// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AttackAttach.generated.h"

class ACPP_PlayerCharacter;
class ACPP_EnemyCharacter;

/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API UAttackAttach : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void Player_FaceToActor(ACPP_PlayerCharacter* Character);

	UFUNCTION(BlueprintCallable)
	void Enemy_FaceToActor(ACPP_EnemyCharacter* Character);
};
