// Solution_BMJ

#include "AttackDoneAnimNotify.h"
#include "Attack_GameplayAbility.h"


// 动画通知类的Notify函数
void UAttackDoneAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::Notify(MeshComp, Animation);

    // 获取角色实例
    ACPP_PlayerCharacter* Character = Cast<ACPP_PlayerCharacter>(MeshComp->GetOwner());
    if (!IsValid(Character)) return;
    Character->HideSelf();
    ACPP_PlayerController* PC = Cast<ACPP_PlayerController>(Character->Controller);
    if (PC)
    {
        PC->GetAnimNotify();
    }
}
