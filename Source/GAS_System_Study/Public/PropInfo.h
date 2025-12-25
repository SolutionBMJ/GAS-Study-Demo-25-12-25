// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PropInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FPropInfoStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FString Name;

	UPROPERTY(EditDefaultsOnly)
	int32 Amount;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* PropImage;
};

UCLASS()
class GAS_SYSTEM_STUDY_API UPropInfo : public UObject
{
	GENERATED_BODY()
	
};
