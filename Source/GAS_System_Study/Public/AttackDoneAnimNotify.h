// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "CPP_PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "CPP_PlayerController.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AttackDoneAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API UAttackDoneAnimNotify : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

};
