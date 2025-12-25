// Solution_BMJ


#include "PlayerAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h" // 引入Actor头文件

void UPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
    {
        NewValue = FMath::Max(NewValue, 0.0f);
		// 计算当前健康值占旧最大生命值的百分比
        const float OldMaxHealth = GetMaxHealth();
        const float CurrentHealth = GetHealth();
		
        const float HealthPercent = (OldMaxHealth > 0.0f) ? (CurrentHealth / OldMaxHealth) : 1.0f;

        // 按比例计算新的健康值，并限制在0到新最大生命值之间
        const float NewHealth = FMath::Clamp(HealthPercent * NewValue, 0.0f, NewValue);

		bIsUpdatingMaxHealth = true;
        TempNewMaxHealth = NewValue;

		// 设置新生命值
        SetHealth(NewHealth);

		bIsUpdatingMaxHealth = false;
        TempNewMaxHealth = 0.0f;
    }
    else if (Attribute == GetHealthAttribute())
    {
		float MaxHealthToUse = bIsUpdatingMaxHealth ? TempNewMaxHealth : GetMaxHealth();
        NewValue = FMath::Clamp(NewValue, 0.0f, MaxHealthToUse);
    }
    if (Attribute == GetMaxStaminaAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
}

void UPlayerAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("Attribute Change!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
		OnHealthChanged.Broadcast(GetHealth(), GetMaxHealth());
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
    {
        // 确保当前血量不会超过新的最大血量
        if (GetHealth() > GetMaxHealth())
        {
            SetHealth(GetMaxHealth());
        }
        // 广播血量变更事件
        OnHealthChanged.Broadcast(GetHealth(), GetMaxHealth());
    }
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
		OnStaminaChanged.Broadcast(GetStamina(), GetMaxStamina());
	}
	else if (Data.EvaluatedData.Attribute == GetMaxStaminaAttribute())
	{
		if (GetStamina() > GetMaxStamina())
		{
			SetStamina(GetMaxStamina());
		}
		OnStaminaChanged.Broadcast(GetStamina(), GetMaxStamina());
	}
}