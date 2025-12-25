// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "SwordSlash.generated.h"

class UNiagaraComponent;
class ACPP_EnemyCharacter;
class UAttackTrace;

UCLASS()
class GAS_SYSTEM_STUDY_API ASwordSlash : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASwordSlash();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

    // 移动方向
    FVector MoveDirection;

    // 移动速度
    float MoveSpeed;

    // 关联生成当前投射物的攻击动画通知
    UPROPERTY()
    UAttackTrace* OwnerAttackTrace;

    void OpenCollision();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    FTimerHandle DestroySlash;

	// 碰撞体
    UPROPERTY(EditDefaultsOnly, Category = "Collision")
    UBoxComponent* BoxCollision;

    // 是否多次结算伤害
    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    bool MultiDamage;

	// 剑气特效组件
    UPROPERTY(EditDefaultsOnly, Category = "FX")
    UNiagaraComponent* SlashFXComponent;

    // 伤害间隔时间
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float DamageInterval;

    // 存储当前重叠的敌人
    UPROPERTY()
    TSet<class ACPP_EnemyCharacter*> OverlappingEnemies;

    // 定时器句柄（周期伤害）
    FTimerHandle DamageTimerHandle;

    // 已命中的敌人
    TSet<AActor*> HitEnemies;

    // 剑气生命周期
    UPROPERTY(EditDefaultsOnly, Category = "LifeTime")
    float LifeTime = 3.0f;

    // 方形碰撞重叠事件
    UFUNCTION()
    void OnBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    // 重叠结束时的回调
    UFUNCTION()
    void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    // 周期性伤害结算函数
    void ApplyPeriodicDamage();

    // 对单个敌人应用伤害
    void DealDamageToEnemy(ACPP_EnemyCharacter* Enemy);

    // 设置超时销毁
    void DestroyByLifeTime();

    // 开始销毁自身
    void DestroySelf();
};
