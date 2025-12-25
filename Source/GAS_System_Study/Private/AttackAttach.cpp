// Solution_BMJ


#include "AttackAttach.h"
#include "CPP_EnemyCharacter.h"
#include "CPP_PlayerCharacter.h"

void UAttackAttach::Player_FaceToActor(ACPP_PlayerCharacter* Character)
{
    if (!Character)
    {
        return;
    }
	TArray<AActor*> TargetList;
	Character->SphereCollision->GetOverlappingActors(TargetList, ACPP_EnemyCharacter::StaticClass());

	ACPP_EnemyCharacter* ClosestEnemy = nullptr;
    float MinDistance = FLT_MAX;

    for (AActor* Actor : TargetList)
    {
        if (ACPP_EnemyCharacter* Enemy = Cast<ACPP_EnemyCharacter>(Actor))
        {
            // 在这里可以对每个Enemy进行操作，例如寻找最近的敌人
            float Distance = FVector::Dist(Character->GetActorLocation(), Enemy->GetActorLocation());
            if (Distance < MinDistance)
            {
                MinDistance = Distance;
                ClosestEnemy = Enemy;
            }
        }
    }

    if (!ClosestEnemy) return;
    FVector Direction = (ClosestEnemy->GetActorLocation() - Character->GetActorLocation()).GetSafeNormal();
    if (Direction.IsNearlyZero()) return;

    FRotator TargetRotation = Direction.Rotation();
    TargetRotation.Pitch = 0.0f;                       //只旋转Yaw

    Character->SetActorRotation(TargetRotation);
	return;
}

void UAttackAttach::Enemy_FaceToActor(ACPP_EnemyCharacter* Character)
{
	return;
}
