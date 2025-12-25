// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AttackTrace.generated.h"

class ACPP_BaseCharacter;
class UStaticMeshComponent;
class ASwordSlash;
class UCharacterDamageDA;

UCLASS()
class GAS_SYSTEM_STUDY_API UAttackTrace : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
    TArray<FHitResult> HitResult;

    TArray<AActor*> HitActors;   // 击中的角色

    TArray<AActor*> ActorIgnore;   // 击中的角色

    UPROPERTY(EditAnywhere, Category = "WeaponSocketName")
    FName WeaponSocketName;

    FVector TraceLocationOne;      // 追踪点1
    FVector TraceLocationTwo;      // 追踪点2

    UPROPERTY(EditAnywhere, Category = "Trace")
    FVector TraceHalfExtent = FVector(5, 30, 50);  // 碰撞盒半尺寸

    UPROPERTY(EditAnywhere, Category = "Trace|Filter")
    TArray<TSubclassOf<AActor>> TargetClasses;     // 攻击目标类

    UPROPERTY(EditAnywhere, Category = "Level")
    FGameplayTag AttackLevel;                      // 攻击等级

    // 伤害资产
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackDamage")
    UCharacterDamageDA* DamageData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackDamage")
    int32 DamageIndex;

    // 是否启用抛射物
    UPROPERTY(EditAnywhere, Category = "Projectile")
    bool WillEnableProjectile = false;

    // 发射物类
    UPROPERTY(EditAnywhere, Category = "Projectile", meta = (EditCondition = "WillEnableProjectile"))
    TSubclassOf<ASwordSlash> ProjectileClass;

    // 发射socket名称
    UPROPERTY(EditAnywhere, Category = "Projectile", meta = (EditCondition = "WillEnableProjectile"))
    FName SocketName;

    // 发射速度
    UPROPERTY(EditAnywhere, Category = "Projectile", meta = (EditCondition = "WillEnableProjectile"))
    float Speed = 1500.0f;

    // 旋转偏移
    UPROPERTY(EditAnywhere, Category = "Projectile", meta = (EditCondition = "WillEnableProjectile"))
    FRotator RotationOffset = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, Category = "Projectile", meta = (EditCondition = "WillEnableProjectile"))
    FVector LocationOffset = FVector::ZeroVector;

    void HandleAttack(AActor* HitActor);

protected:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
    ACPP_BaseCharacter* OwnerCharacter;

    USceneComponent* Weapon;  // 存储找到的武器组件

    void FireProjectile();
};
