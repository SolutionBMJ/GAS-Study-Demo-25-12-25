// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "PlayerAttributeSet.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float, CurrentStamina, float, MaxStamina);

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class GAS_SYSTEM_STUDY_API UPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:

	//UPlayerAttributeSet();

	UPROPERTY(BlueprintReadWrite, Category = "Health")              // 生命值
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Health);

	UPROPERTY(BlueprintReadWrite, Category = "MaxHealth")           // 最大生命值
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadWrite, Category = "Attack")              // 攻击力
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Attack);

	UPROPERTY(BlueprintReadWrite, Category = "Defence")             // 防御力
	FGameplayAttributeData Defence;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Defence);

	UPROPERTY(BlueprintReadWrite, Category = "CriticalValue")       // 暴击伤害
	FGameplayAttributeData CriticalValue;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, CriticalValue);

	UPROPERTY(BlueprintReadWrite, Category = "CriticalRate")        // 暴击率
	FGameplayAttributeData CriticalRate;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, CriticalRate);

	UPROPERTY(BlueprintReadOnly, Category = "Stamina")
    FGameplayAttributeData Stamina;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Stamina);

    UPROPERTY(BlueprintReadOnly, Category = "Stamina")
    FGameplayAttributeData MaxStamina;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxStamina);

	UPROPERTY(BlueprintAssignable, Category = "Attribute Events")
    FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Attribute Events")
	FOnStaminaChanged OnStaminaChanged;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	// 是否正在更新 MaxHealth
	bool bIsUpdatingMaxHealth = false;

	// 临时保存新的 MaxHealth 值
	float TempNewMaxHealth = 0.0f;
};
