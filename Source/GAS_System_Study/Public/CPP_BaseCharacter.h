// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterDamageDA.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "CPP_BaseCharacter.generated.h"



UCLASS()
class GAS_SYSTEM_STUDY_API ACPP_BaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACPP_BaseCharacter();

	// 击中目标
	UFUNCTION()
	virtual void HitActor(ACPP_BaseCharacter* Target, FGameplayTag HitLevel, float DamageRate);

	// 被击中
	UFUNCTION(BlueprintImplementableEvent)
	void GetHit(FGameplayTagContainer HitLevel, int32 DamageValue, bool bIsCritical);

	// 角色攻击结束委托
	// 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereCollision;

	UPROPERTY(EditDefaultsOnly, Category = "GAS|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> GamePlayAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "GAS|GetHit")
	TArray<TSubclassOf<UGetHitGameplayAbility>> GetHitAbilities;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "Collision")
	void SetSphereCollisionRadius(float NewRadius);

	void ReSetSphereCollisionRadius() { SphereCollision->SetSphereRadius(380.0f, true); }

	// 尝试激活受击GA
    UFUNCTION(BlueprintCallable, Category = "HitReact")
    bool TryActivateHitAbility(TSubclassOf<UGetHitGameplayAbility> GetHitAbilityClass);

    // 检查新受击GA是否能打断当前激活的受击GA
    bool CanInterruptCurrentHitReact(FGameplayTag NewPriorityTag) const;

    // 打断当前激活的受击GA
    void InterruptCurrentHitReact();

	// 获取当前是否正在攻击
	bool IsAttacking() const;

	void GiveStartupAbilities();

protected:
	// 当前激活的受击GA
    UPROPERTY()
    UGetHitGameplayAbility* CurrentHitReactAbility;

    // 当前激活的受击GA优先级
    UPROPERTY()
    FGameplayTag CurrentHitReactPriority;
};
