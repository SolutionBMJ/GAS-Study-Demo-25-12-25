// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "EnemyAttributeSet.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyHealthChanged, float, CurrentHealth, float, MaxHealth);

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class GAS_SYSTEM_STUDY_API UEnemyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UEnemyAttributeSet();

	UPROPERTY(BlueprintReadWrite, Category = "Health")              // 生命值
	FGameplayAttributeData EnemyHealth;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, EnemyHealth);

	UPROPERTY(BlueprintReadWrite, Category = "MaxHealth")           // 最大生命值
	FGameplayAttributeData EnemyMaxHealth;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, EnemyMaxHealth);

	UPROPERTY(BlueprintReadWrite, Category = "Attack")              // 攻击力
	FGameplayAttributeData EnemyAttack;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, EnemyAttack);

	UPROPERTY(BlueprintReadWrite, Category = "Defence")             // 防御力
	FGameplayAttributeData EnemyDefence;
	ATTRIBUTE_ACCESSORS(UEnemyAttributeSet, EnemyDefence);

	UPROPERTY(BlueprintAssignable, Category = "Attribute Events")
    FOnEnemyHealthChanged OnEnemyHealthChanged;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
