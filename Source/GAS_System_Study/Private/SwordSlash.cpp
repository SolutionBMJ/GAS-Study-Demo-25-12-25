// Solution_BMJ


#include "SwordSlash.h"
#include "AttackTrace.h"

#include "NiagaraComponent.h"
#include "CPP_EnemyCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASwordSlash::ASwordSlash()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    BoxCollision->SetCollisionProfileName(TEXT("Weapon"));              // 自定义碰撞配置：仅检测敌人
    BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 只与Pawn重叠
    BoxCollision->SetHiddenInGame(true);                                // 隐形，仅做判定
    BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
    RootComponent = BoxCollision;

    // 创建特效组件
    SlashFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SlashFXComponent"));
    SlashFXComponent->SetupAttachment(RootComponent);
    SlashFXComponent->SetAutoActivate(false);

    // 绑定重叠事件
    BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ASwordSlash::OnBoxOverlap);
    BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ASwordSlash::OnBoxEndOverlap);
}

// Called when the game starts or when spawned
void ASwordSlash::BeginPlay()
{
	Super::BeginPlay();
	
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ASwordSlash::DestroyByLifeTime);
}

void ASwordSlash::OnBoxOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 是否是敌人类型
    if (!OtherActor->IsA(ACPP_EnemyCharacter::StaticClass())) return;

    // 是自身
    if (OtherActor == GetOwner()) return;

    ACPP_EnemyCharacter* Enemy = Cast<ACPP_EnemyCharacter>(OtherActor);
    if (!IsValid(Enemy)) return;

    if (!MultiDamage)
    {
        if (IsValid(OwnerAttackTrace))
        {
            OwnerAttackTrace->HandleAttack(Enemy);
        }
    }
    else
    {
        if (Enemy && !OverlappingEnemies.Contains(Enemy))
        {
            OverlappingEnemies.Add(Enemy);

            // 如果定时器未启动，则启动周期性伤害
            if (!GetWorld()->GetTimerManager().IsTimerActive(DamageTimerHandle))
            {
                GetWorld()->GetTimerManager().SetTimer(
                    DamageTimerHandle,
                    this,
                    &ASwordSlash::ApplyPeriodicDamage,
                    DamageInterval,
                    true // 循环执行
                );
            }
        }
    }
}

void ASwordSlash::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // 若没有多段伤害，则跳出
    if (!MultiDamage) return;

    ACPP_EnemyCharacter* Enemy = Cast<ACPP_EnemyCharacter>(OtherActor);
    if (Enemy && OverlappingEnemies.Contains(Enemy))
    {
        OverlappingEnemies.Remove(Enemy);

        // 如果没有敌人了，停止定时器
        if (OverlappingEnemies.Num() == 0 && GetWorld()->GetTimerManager().IsTimerActive(DamageTimerHandle))
        {
            GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);
        }
    }
}

void ASwordSlash::ApplyPeriodicDamage()
{
    if (OverlappingEnemies.Num() == 0)
    {
        GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);
        return;
    }

    // 遍历所有重叠的敌人，逐个结算伤害
    for (ACPP_EnemyCharacter* Enemy : OverlappingEnemies)
    {
        if (IsValid(Enemy)) // 确保敌人仍有效
        {
            DealDamageToEnemy(Enemy);
        }
    }
}

void ASwordSlash::DealDamageToEnemy(ACPP_EnemyCharacter* Enemy)
{
    if (!Enemy) return;

    if (IsValid(OwnerAttackTrace))
    {
        OwnerAttackTrace->HandleAttack(Enemy);
    }
}

void ASwordSlash::DestroyByLifeTime()
{
    GetWorld()->GetTimerManager().SetTimer(
        DestroySlash,
        this,
        &ASwordSlash::DestroySelf,
        LifeTime,
        false
    );
}

void ASwordSlash::DestroySelf()
{
    Destroy();
}

// Called every frame
void ASwordSlash::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (!MoveDirection.IsZero())
    {
        FVector NewLocation = GetActorLocation() + MoveDirection * MoveSpeed * DeltaTime;
        SetActorLocation(NewLocation);
    }
}

void ASwordSlash::OpenCollision()
{
        // 启用碰撞检测
    BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    UE_LOG(LogTemp, Warning, TEXT("collision enabled"));
}

