// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CPP_PlayerCharacter.h"
#include "PlayerAttributeSet.h"
#include "GamingPlayerUI.generated.h"

class UProgressBar;
class UImage;
class UTexture2D;
class UTextBlock;
/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API UGamingPlayerUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UGamingPlayerUI(const FObjectInitializer& ObjectInitializer);
	// 初始化UI
	void InitializeUI(TArray<ACPP_PlayerCharacter*> TeamData);
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// 更新生命值显示
	UFUNCTION()
	void UpdateHealth(float CurrentHealth, float MaxHealth);

	UFUNCTION()
	// 更新精力值显示
	void UpdateStamina(float CurrentStamina, float MaxStamina);

	// 切换角色时更新UI
	void OnCharacterSwitched(ACPP_PlayerCharacter* NewCharacter, int32 CharacterID);

	// 加载编队显示
	void LoadCharacterIcon();

	// 加载技能图标
	void LoadAbilityIcons(ACPP_PlayerCharacter* Character);

	//  更新CD
	void UpdateSkillCooldown(int32 SkillIndex, float CurrentCooldown, float MaxCooldown);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayHitAnim();

	UPROPERTY(BlueprintReadOnly)
	int32 CurCharacterID = 0;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag SkillOneTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag SkillTwoTag;

	void SetBG_HPBar();

	void UpdateBG_HP();

	FTimerHandle StartAlignTimer;

	FTimerHandle AlignTimer;

	// 当前角色引用
	UPROPERTY(BlueprintReadOnly)
	ACPP_PlayerCharacter* CurrentCharacter;

protected:
	virtual void NativeConstruct() override;

    TArray<ACPP_PlayerCharacter*> TeamMembers;

private:
	// 绑定角色属性变化事件
	void BindAttributeEvents();

	// 清除属性绑定
	void UnbindAttributeEvents();

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* BackGroundHP;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StrengthBar;

	UPROPERTY(meta = (BindWidget))
	UImage* NormalAttackIcon;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillOneIcon;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillTwoIcon;

	UPROPERTY()
	class UMaterialInstanceConstant* CoolDownMat; // 声明材质资源变量

	UPROPERTY()
	UMaterialInstanceDynamic* SkillOneDynamicMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* SkillTwoDynamicMaterial;

	UPROPERTY(meta = (BindWidget))
	UImage* Character1;

	UPROPERTY(meta = (BindWidget))
	UImage* Character2;

	UPROPERTY(meta = (BindWidget))
	UImage* Character3;

	UPROPERTY(meta = (BindWidget))
	UImage* CharacterSelect1;

	UPROPERTY(meta = (BindWidget))
	UImage* CharacterSelect2;

	UPROPERTY(meta = (BindWidget))
	UImage* CharacterSelect3;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Health_TextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxHealth_TextBlock;

	float TargetBG_HP;
	float OldHp;

	// 技能图标纹理
	UPROPERTY()
	UTexture2D* NormalAttackTexture;

	UPROPERTY()
	UTexture2D* SkillOneTexture;

	UPROPERTY()
	UTexture2D* SkillTwoTexture;

	UPROPERTY()
	UTexture2D* Character1Texture;

	UPROPERTY()
	UTexture2D* Character2Texture;

	UPROPERTY()
	UTexture2D* Character3Texture;

};
