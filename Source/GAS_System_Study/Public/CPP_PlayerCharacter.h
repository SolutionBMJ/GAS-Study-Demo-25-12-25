// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPP_BaseCharacter.h"
#include "CPP_PlayerState.h"
#include "AbilitySystemInterface.h"
#include "EquipmentImprove.h"
#include "CPP_PlayerCharacter.generated.h"

class UGamingPlayerUI;
class UPlayerAttributeSet;
class UAbilitySystemComponent;

UCLASS()
class GAS_SYSTEM_STUDY_API ACPP_PlayerCharacter : public ACPP_BaseCharacter
{
	GENERATED_BODY()
	
public:
	ACPP_PlayerCharacter();

	UFUNCTION(BlueprintCallable)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;    //获取能力系统组件函数声明

	virtual void PossessedBy(AController* NewController) override;                  //当该角色被操作时

	virtual void OnRep_PlayerState() override;                                      //玩家状态首次触发

	virtual void HitActor(ACPP_BaseCharacter* Target, FGameplayTag HitLevel, float DamageRate) override;

	UFUNCTION(BlueprintCallable)
	bool GetPlayerStateOnRun();

	UFUNCTION(BlueprintCallable)
	UPlayerAttributeSet* GetCharacterAS();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayHitSound();

	// 加载并应用装备数据
	void ApplyEquipmentData();

	// 设置体力恢复计时器
	void SetStaminaTimer();

	// 恢复体力
	void RecoverStamina();

	FTimerHandle StaminaTimer;

	// 尝试隐藏自身
	void HideSelf();

	// 在蓝图中播放关卡序列
	UFUNCTION(BlueprintImplementableEvent)
	void TryPlayLevelSequenceByBP(int32 index);

	// 角色名称
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Name")
	FString CharacterName;

	// 角色信息
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Info", Meta = (MultiLine = "true"))
	FText CharacterInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|DamageGE")
	TSubclassOf<UGameplayEffect> DamageGEClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|StaminaRecover")
	TSubclassOf<UGameplayEffect> StaminaRecoverClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Equipment")
	TSubclassOf<UEquipmentImprove> EquipmentEffectClass;

	// -------- 图标数据 -----------------------------------------
	// 
	// 普通攻击图标路径
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|AbilityIcons")
    FString NormalAttackIconPath;

    // 技能1图标路径
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|AbilityIcons")
    FString SkillOneIconPath;

    // 技能2图标路径
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|AbilityIcons")
    FString SkillTwoIconPath;

	// 头像图标路径
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|CharacterIcons")
    FString HeadIconPath;

	// 组队图标路径
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|CharacterIcons")
    FString TeamIconPath;

protected:
	UPROPERTY(VisibleAnywhere, Category = "GAS|Abilities")                         //能力系统组件
	class UAbilitySystemComponent* AbilitySystemComponent;

	// 装备GE实例
	UPROPERTY()
	FActiveGameplayEffectHandle EquipmentEffectHandle;

private:

	class UGASGameInstance* GameInstance;
};
