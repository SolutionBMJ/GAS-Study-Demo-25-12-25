// Solution_BMJ


#include "DamageCalculation.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CPP_EnemyCharacter.h"
#include "GameplayEffect.h"

// 定义静态结构体，用于存放所有属性捕获定义
struct FDamageStatics
{
    // 声明玩家属性集的捕获定义
    DECLARE_ATTRIBUTE_CAPTUREDEF(Attack);
    DECLARE_ATTRIBUTE_CAPTUREDEF(Defence);
    DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
    
    // 声明敌人属性集的捕获定义
    DECLARE_ATTRIBUTE_CAPTUREDEF(EnemyAttack);
    DECLARE_ATTRIBUTE_CAPTUREDEF(EnemyDefence);
    DECLARE_ATTRIBUTE_CAPTUREDEF(EnemyHealth);

    FDamageStatics()
    {
        // 定义这些捕获定义
        // 玩家属性
        DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, Attack, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, Defence, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, Health, Target, false);
        
        // 敌人属性
        DEFINE_ATTRIBUTE_CAPTUREDEF(UEnemyAttributeSet, EnemyAttack, Source, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UEnemyAttributeSet, EnemyDefence, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UEnemyAttributeSet, EnemyHealth, Target, false);
    }
};

// 获取静态结构体的辅助函数
static const FDamageStatics& DamageStatics()
{
    static FDamageStatics DmgStatics;
    return DmgStatics;
}

UDamageCalculation::UDamageCalculation()
{
	RelevantAttributesToCapture.Add(DamageStatics().AttackDef);
    RelevantAttributesToCapture.Add(DamageStatics().DefenceDef);
    RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
    
    RelevantAttributesToCapture.Add(DamageStatics().EnemyAttackDef);
    RelevantAttributesToCapture.Add(DamageStatics().EnemyDefenceDef);
    RelevantAttributesToCapture.Add(DamageStatics().EnemyHealthDef);
}

void UDamageCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    // Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

    // 获取 攻击者 和 受击者 的ASC
    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

    if (TargetASC == nullptr || SourceASC == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Target or Source ASC is Not Valid"));
        return;
    }

    const FGameplayEffectSpec& OwningSpec = ExecutionParams.GetOwningSpec();

    // 2. 定义SetByCaller标签（必须和HitActor中一致：Data.DamageRate）
    FGameplayTag DamageRateTag = FGameplayTag::RequestGameplayTag(FName("Data.DamageRate"));

    // 3. 正确读取SetByCaller值（第二个参数是默认值）
    float DamageRate = OwningSpec.GetSetByCallerMagnitude(DamageRateTag, 1.0f);
    
    // 新增日志：验证读取结果
    UE_LOG(LogTemp, Log, TEXT("UDamageCalculation - Get DamageRate From SetByCaller: %.2f"), DamageRate);

    // 捕获攻击力
    float Attack = 0.0f;
    if (UPlayerAttributeSet* SourceFAS = const_cast<UPlayerAttributeSet*>(SourceASC->GetSet<UPlayerAttributeSet>()))
    {
        Attack = SourceFAS->GetAttack();
    }
    else if (UEnemyAttributeSet* SourceSAS = const_cast<UEnemyAttributeSet*>(SourceASC->GetSet<UEnemyAttributeSet>()))
    {
        Attack = SourceSAS->GetEnemyAttack();
    }

    // 捕获防御力
    float Defence = 0.0f;
    if (UPlayerAttributeSet* SourceFAS = const_cast<UPlayerAttributeSet*>(SourceASC->GetSet<UPlayerAttributeSet>()))
    {
        Defence = SourceFAS->GetDefence();
    }
    else if (UEnemyAttributeSet* SourceSAS = const_cast<UEnemyAttributeSet*>(SourceASC->GetSet<UEnemyAttributeSet>()))
    {
        Defence = SourceSAS->GetEnemyDefence();
    }

    // 计算最终伤害（基础公式：攻击力 - 防御力，乘以倍率，确保不小于0）
    float FinalDamage = FMath::Max(0.0f, (Attack - Defence * 0.05f) * DamageRate);
    UE_LOG(LogTemp, Log, TEXT("Calculate Damage : Attack = %.2f，Defence = %.2f，Rate = %.2f，FinalDamage = %.2f"), Attack, Defence, DamageRate, FinalDamage);

    float CriticalRate = 0.0f;
    if (UPlayerAttributeSet* SourceFAS = const_cast<UPlayerAttributeSet*>(SourceASC->GetSet<UPlayerAttributeSet>()))
    {
        CriticalRate = SourceFAS->GetCriticalRate();
    }
    float CriticalValue = 1.0f;
    if (UPlayerAttributeSet* SourceFAS = const_cast<UPlayerAttributeSet*>(SourceASC->GetSet<UPlayerAttributeSet>()))
    {
        CriticalValue = SourceFAS->GetCriticalValue();
    }
    bool bIsCriticalHit = FMath::FRand() <= (CriticalRate / 100.0f);

    if (bIsCriticalHit)
    {
        FinalDamage *= (CriticalValue / 100.0f);
        UE_LOG(LogTemp, Log, TEXT("Critical!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %f"), CriticalValue);
    }

    // 应用伤害到受击者的生命值
    if (FinalDamage > 0)
    {
        OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				DamageStatics().EnemyHealthProperty,
				EGameplayModOp::Additive,
				-FinalDamage
			)
		);
    }

    // 获取攻击等级标签（从DynamicGrantedTags中）
    FGameplayTag AttackLevelTag;
    const FGameplayTagContainer& GrantedTags = ExecutionParams.GetOwningSpec().DynamicGrantedTags;
    // 假设你知道AttackLevel的具体标签，或者通过遍历查找
    // 例如，如果你知道它总是以"Player.character.State.GetHit."开头
    for (const FGameplayTag& Tag : GrantedTags)
    {
        if (Tag.ToString().StartsWith(TEXT("Player.character.State.GetHit.")))
        {
            AttackLevelTag = Tag;
            UE_LOG(LogTemp, Log, TEXT("Find Tag: %s"), *Tag.ToString());
            break;
        }
    }

    if (Cast<ACPP_EnemyCharacter>(TargetASC->GetAvatarActor()))
    {
        ACPP_EnemyCharacter* EnemyActor = Cast<ACPP_EnemyCharacter>(TargetASC->GetAvatarActor());
        if (!IsValid(EnemyActor))
        {
            UE_LOG(LogTemp, Log, TEXT("EnemyActor is Not Valid"));
            return;
        }
        UE_LOG(LogTemp, Log, TEXT("EnemyActor Name: %s"), *EnemyActor->GetName());
        int32 Damage = (int32)FinalDamage;
        FGameplayTagContainer HitLevelTags;
        HitLevelTags.AddTag(AttackLevelTag);
        EnemyActor->GetHit(HitLevelTags, Damage, bIsCriticalHit);
    }

}
