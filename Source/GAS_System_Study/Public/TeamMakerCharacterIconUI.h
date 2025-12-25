// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "TeamMakerCharacterIconUI.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API UTeamMakerCharacterIconUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UImage* CharacterIcon;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void SetParentWidget(class UTeamMakerWidget* PWidget) { ParentWidget = PWidget; }

	UFUNCTION(BlueprintCallable)
	void OnClickCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SlotID;

	UFUNCTION(BlueprintCallable)
	void GetDropCharacter(int32 CharacterIndex);

protected:
	class UTeamMakerWidget* ParentWidget;

	bool bIsMouseDown = false;
    bool bIsDragging = false;
    FVector2D MouseDownPosition;
    FVector2D LastMousePosition;
};
