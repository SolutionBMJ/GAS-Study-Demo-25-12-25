// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHPBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API UEnemyHPBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;

public:
    // 更新HP条显示
    UFUNCTION(BlueprintCallable, Category = "HP")
    void UpdateHP(float CurrentHP, float MaxHP);

	UFUNCTION(BlueprintImplementableEvent)
	void GetHitShake();

private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar_BG;

	float TargetBG_HP;
	float OldHp;

	FTimerHandle StartAlignTimer;

	FTimerHandle AlignTimer;

	void SetHPBar_BG();

	void UpdateHPBar_BG();
};
