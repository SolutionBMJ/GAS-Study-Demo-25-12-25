// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "TeamWidgetItem.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API UTeamWidgetItem : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
    virtual void NativeOnListItemObjectSet(UObject* InListItemObject) override;

	UPROPERTY(BlueprintReadOnly, Category = "TileEntry")
    class ACPP_PlayerCharacter* CurCharacter;

	UPROPERTY(meta = (BindWidget))
	class UButton* Select;

	UFUNCTION()
	void SelectCharacter();
};
