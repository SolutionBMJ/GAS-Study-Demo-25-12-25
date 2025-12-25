// Solution_BMJ


#include "AttackTrace.h"
#include "CPP_EnemyCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "SwordSlash.h"
#include "GameFramework/Controller.h"
#include "CPP_PlayerCharacter.h"
#include "CPP_BaseCharacter.h"




void UAttackTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    OwnerCharacter = Cast<ACPP_BaseCharacter>(MeshComp->GetOwner());
    if (!OwnerCharacter) return;

    // 查找角色身上的武器插槽
    if (WeaponSocketName.IsNone()) return;

    // 尝试从角色组件中找到武器
    USceneComponent* const* AttachComponentPtr = MeshComp->GetAttachChildren().FindByPredicate([&](USceneComponent* ChildComponent)
        {
            return ChildComponent && ChildComponent->GetAttachSocketName() == WeaponSocketName;
        });
    if (AttachComponentPtr) // 先判断指针的指针是否有效
    {
        Weapon = *AttachComponentPtr; // 保存武器组件
    }

    if (!Weapon)
    {
        UE_LOG(LogTemp, Error, TEXT("Didn't Find Weapon!!!"));
        return;
    }
    ActorIgnore = { MeshComp->GetOwner() };
    TraceLocationOne = Weapon->GetSocketLocation("Socket0");
    TraceLocationTwo = Weapon->GetSocketLocation("Socket1");

    if (WillEnableProjectile)
    {
        FireProjectile();
    }
}

void UAttackTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
    
    if (OwnerCharacter)
    {
        UKismetSystemLibrary::BoxTraceMulti(
            OwnerCharacter->GetWorld(),
            TraceLocationOne,
            Weapon->GetSocketLocation("Socket0"),
            TraceHalfExtent,
            Weapon->GetComponentRotation(),
            ETraceTypeQuery::TraceTypeQuery4,
            false,
            ActorIgnore,
            EDrawDebugTrace::None,
            HitResult,
            true
        );
        for (int i = 0; i < HitResult.Num(); i++)
        {
            AActor* CurHitActor = HitResult[i].GetActor();
            for (const TSubclassOf<AActor>& TargetClass : TargetClasses)
            {
                if (CurHitActor->IsA(TargetClass) && !HitActors.Contains(CurHitActor))
                {
                    HandleAttack(CurHitActor);
                    HitActors.Add(CurHitActor);
                }
            }
        }
        UKismetSystemLibrary::BoxTraceMulti(
            OwnerCharacter->GetWorld(),
            TraceLocationTwo,
            Weapon->GetSocketLocation("Socket1"),
            TraceHalfExtent,
            Weapon->GetComponentRotation(),
            ETraceTypeQuery::TraceTypeQuery4,
            false,
            ActorIgnore,
            EDrawDebugTrace::None,
            HitResult,
            true
        );
        for (int i = 0; i < HitResult.Num(); i++)
        {
            AActor* CurHitActor = HitResult[i].GetActor();
            for (const TSubclassOf<AActor>& TargetClass : TargetClasses)
            {
                if (CurHitActor->IsA(TargetClass) && !HitActors.Contains(CurHitActor))
                {
                    HandleAttack(CurHitActor);
                    HitActors.Add(CurHitActor);
                }
            }
        }
        TraceLocationOne = Weapon->GetSocketLocation("Socket0");
        TraceLocationTwo = Weapon->GetSocketLocation("Socket1");
    }
}

void UAttackTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::NotifyEnd(MeshComp, Animation);
    HitActors.Empty();
    HitResult.Empty();
}

void UAttackTrace::HandleAttack(AActor* bHitActor)
{
    if (!bHitActor || !OwnerCharacter) return;

    ACPP_BaseCharacter* Target = Cast<ACPP_BaseCharacter>(bHitActor);
    if (!Target) return;

    // 检查DamageData是否有效，并打印读取到的DamageRate
    if (!DamageData)
    {
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Hit Enemy: %s , Make Damage: %.1f "), *bHitActor->GetName(), DamageData->GetDamageRate(DamageIndex));
    OwnerCharacter->HitActor(Target, AttackLevel, DamageData->GetDamageRate(DamageIndex));

    FVector AttackerLocation = OwnerCharacter->GetActorLocation();
    FVector Direction = (AttackerLocation - Target->GetActorLocation()).GetSafeNormal();

    if (!Direction.IsNearlyZero())
    {
        FRotator TargetRotation = Direction.Rotation();
        TargetRotation.Pitch = 0.0f; // 只旋转Yaw轴
        TargetRotation.Roll = 0.0f;
        Target->SetActorRotation(TargetRotation);
    }
}

void UAttackTrace::FireProjectile()
{
    if (!WillEnableProjectile || !ProjectileClass || !OwnerCharacter) return;

    USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
    if (!Mesh) return;

    FTransform SocketTransform = Mesh->GetSocketTransform(SocketName);

    FVector LocalOffset = LocationOffset;
    FVector WorldOffset = SocketTransform.GetRotation().Rotator().RotateVector(LocalOffset);
    
    // 生成位置偏移
    FVector SpawnLocation = SocketTransform.GetLocation() + WorldOffset;

    // 生成旋转偏移
    FRotator FinalRotation = SocketTransform.GetRotation().Rotator() + RotationOffset;

    UWorld* World = OwnerCharacter->GetWorld();

	if (!World) return;
    
    // 生成剑气
	ASwordSlash* Projectile = World->SpawnActor<ASwordSlash>(
		ProjectileClass,
		SpawnLocation,
        FinalRotation
		);

    if (Projectile)
    {
        
        // 设置剑气移动方向和速度
        Projectile->MoveDirection = FinalRotation.Vector();
        Projectile->MoveSpeed = Speed;
        
        // 设置所有者为当前角色
        Projectile->SetOwner(OwnerCharacter);
        Projectile->OwnerAttackTrace = this;
        Projectile->OpenCollision();
        
    }
}

