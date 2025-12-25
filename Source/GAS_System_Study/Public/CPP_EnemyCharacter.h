// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "CPP_BaseCharacter.h"
#include "NiagaraComponent.h"
#include "CPP_EnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API ACPP_EnemyCharacter : public ACPP_BaseCharacter
{
	GENERATED_BODY()
public:
	ACPP_EnemyCharacter();

private:
	UPROPERTY(VisibleAnywhere, Category = "GAS|Abilities")     //能力系统组件声明
	class UAbilitySystemComponent* AbilitySystemComponent;

	// HP变化回调
	UFUNCTION()
    void OnHealthChanged(float CurrentHP, float MaxHP);

	TSet<AActor*> OverlapActor;

	// 死亡状态标记
    UPROPERTY(VisibleAnywhere, Category = "Enemy|Death")
    bool bIsDead = false;

    // 死亡动画蒙太奇
    UPROPERTY(EditDefaultsOnly, Category = "Enemy|Animation")
    UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "DeathEffect")
    UNiagaraSystem* DissolveNiagaraSystem; // 消散粒子系统模板

    UPROPERTY(EditAnywhere, Category = "DeathEffect")
    UMaterialInterface* DissolveMaterial; // 溶解材质

    FTimerHandle DestroyTimerHandle;

    UFUNCTION()
    void OnDeathEffectNotify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent);

    void DestroyEnemy();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    class UWidgetComponent* EnemyHPBar;

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// 死亡处理函数
    UFUNCTION(BlueprintCallable, Category = "Enemy|Death")
    void Die();

	UFUNCTION(BlueprintImplementableEvent)
    void DieBlueprint();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void HitActor(ACPP_BaseCharacter* Target, FGameplayTag HitLevel, float DamageRate) override;

	UFUNCTION(BlueprintImplementableEvent)
	void HandleHit(ACPP_BaseCharacter* Target);

	void UpdateHealthBarRotation();

	FName DeathEffectNotify;
};
