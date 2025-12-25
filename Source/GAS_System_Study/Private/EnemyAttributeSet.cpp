// Solution_BMJ


#include "EnemyAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Actor.h" // 引入Actor头文件

UEnemyAttributeSet::UEnemyAttributeSet()
{

}

void UEnemyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetEnemyMaxHealthAttribute())
    {
        NewValue = FMath::Max(NewValue, 0.0f);

        // 计算当前健康值占旧最大生命值的百分比
        const float OldMaxHealth = GetEnemyMaxHealth();
        const float CurrentHealth = GetEnemyHealth();
        const float HealthPercent = (OldMaxHealth > 0.0f) ? (CurrentHealth / OldMaxHealth) : 1.0f;
        
        // 按比例计算新的健康值，并限制在0到新最大生命值之间
        const float NewHealth = FMath::Clamp(HealthPercent * NewValue, 0.0f, NewValue);
        SetEnemyHealth(NewHealth);
    }
    else if (Attribute == GetEnemyHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetEnemyMaxHealth());
    }
}

void UEnemyAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetEnemyHealthAttribute())
	{
		SetEnemyHealth(FMath::Clamp(GetEnemyHealth(), 0.0f, GetEnemyMaxHealth()));
		OnEnemyHealthChanged.Broadcast(GetEnemyHealth(), GetEnemyMaxHealth());
	}
	else if (Data.EvaluatedData.Attribute == GetEnemyMaxHealthAttribute())
    {
        // 确保当前血量不会超过新的最大血量
        if (GetEnemyHealth() > GetEnemyMaxHealth())
        {
            SetEnemyHealth(GetEnemyMaxHealth());
        }
        // 广播血量变更事件
        OnEnemyHealthChanged.Broadcast(GetEnemyHealth(), GetEnemyMaxHealth());
    }
}
