// Solution_BMJ


#include "CPP_EnemyCharacter.h"
#include "Animation/AnimMontage.h"
#include "CPP_PlayerCharacter.h"
#include "CPP_PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h" // 必需头文件
#include "EnemyAttributeSet.h"
#include "EnemyHPBarWidget.h"
#include "GetHitGameplayAbility.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCameraActor.h"


ACPP_EnemyCharacter::ACPP_EnemyCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    // 创建WidgetComponent
    EnemyHPBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));

    // 将这个组件附加到角色的根组件上
    EnemyHPBar->SetupAttachment(GetRootComponent());

    // 设置显示空间为世界空间
    EnemyHPBar->SetWidgetSpace(EWidgetSpace::Screen);

    EnemyHPBar->bReceivesDecals = false;

    EnemyHPBar->SetUsingAbsoluteRotation(true);

    // 设置这个组件将要显示的Widget类
    EnemyHPBar->SetWidgetClass(UEnemyHPBarWidget::StaticClass());

    // 调整WidgetComponent的相对位置
    EnemyHPBar->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));

    // 设置UI的绘制大小
    EnemyHPBar->SetDrawSize(FVector2D(180.0f, 7.0f));
}

void ACPP_EnemyCharacter::OnHealthChanged(float CurrentHP, float MaxHP)
{
    if (EnemyHPBar)
    {
        EnemyHPBar->SetVisibility(true);
        UUserWidget* WidgetInstance = EnemyHPBar->GetUserWidgetObject();
        if (WidgetInstance)
        {
            // 将通用的UUserWidget转换为你具体的血条类
            UEnemyHPBarWidget* HealthBarWidget = Cast<UEnemyHPBarWidget>(WidgetInstance);
            if (HealthBarWidget)
            {
                // 调用你血条UI类里自定义的更新函数
                HealthBarWidget->UpdateHP(CurrentHP, MaxHP);
            }
        }
    }

    if (bIsDead) return;
    if (UEnemyAttributeSet* EnemyAS = const_cast<UEnemyAttributeSet*>(AbilitySystemComponent->GetSet<UEnemyAttributeSet>()))
    {
        if (EnemyAS->GetEnemyHealth() <= 0.0f)
        {
            Die();
        }
    }
}

void ACPP_EnemyCharacter::OnDeathEffectNotify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, const FAnimNotifyEvent& NotifyEvent)
{
    // 确保是死亡特效通知（可根据NotifyName筛选特定通知）
    if (NotifyEvent.NotifyName == FName("DeathDissolveEffect"))
    {
        // 生成消散粒子特效（以网格体为父节点附着）
        if (DissolveNiagaraSystem && MeshComp)
        {
            UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
                DissolveNiagaraSystem,
                MeshComp,
                NAME_None,
                MeshComp->GetComponentLocation(),
                MeshComp->GetComponentRotation(),
                EAttachLocation::SnapToTargetIncludingScale,
                true
            );
            // 设置粒子生命周期结束后自动销毁
            if (NiagaraComp)
            {
                NiagaraComp->SetAutoDestroy(true);
            }
        }

        // 应用溶解材质
        if (DissolveMaterial && MeshComp)
        {
            // 替换网格体所有材质槽为溶解材质
            for (int32 MatIndex = 0; MatIndex < MeshComp->GetNumMaterials(); MatIndex++)
            {
                MeshComp->SetMaterial(MatIndex, DissolveMaterial);
            }
        }

        // 延迟销毁角色（等待特效播放完成）
        GetWorld()->GetTimerManager().SetTimer(
            DestroyTimerHandle,
            FTimerDelegate::CreateUObject(this, &ACPP_EnemyCharacter::DestroyEnemy),
            2.0f, // 延迟时间（根据特效长度调整）
            false
        );
    }
}

void ACPP_EnemyCharacter::DestroyEnemy()
{

}

UAbilitySystemComponent* ACPP_EnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACPP_EnemyCharacter::Die()
{
    bIsDead = true;

    DieBlueprint();
}

void ACPP_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	GiveStartupAbilities();

    const_cast<UEnemyAttributeSet*>(AbilitySystemComponent->GetSet<UEnemyAttributeSet>())->OnEnemyHealthChanged.AddDynamic(this, &ACPP_EnemyCharacter::OnHealthChanged);
    // 初始更新一次HP
    OnHealthChanged(GetAbilitySystemComponent()->GetSet<UEnemyAttributeSet>()->GetEnemyHealth(),
        GetAbilitySystemComponent()->GetSet<UEnemyAttributeSet>()->GetEnemyMaxHealth());
}

void ACPP_EnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateHealthBarRotation();
}

void ACPP_EnemyCharacter::HitActor(ACPP_BaseCharacter* Target, FGameplayTag HitLevel, float DamageRate)
{
	if (!Target) return;

    HandleHit(Target);
}

void ACPP_EnemyCharacter::UpdateHealthBarRotation()
{
    if (!EnemyHPBar) return;

    ACPP_PlayerController* PC = Cast<ACPP_PlayerController>(GetWorld()->GetFirstPlayerController());
    if (!PC || !PC->PlayerCameraManager) return;

    FVector CameraLoc = PC->GetCurrentCamera()->GetActorLocation();
    FVector WidgetLoc = EnemyHPBar->GetComponentLocation();

    // 计算旋转，让血条的正面( -Z 轴)指向摄像机
    FRotator NewRot = FRotationMatrix::MakeFromX(CameraLoc - WidgetLoc).Rotator();
    // 调整旋转轴，因为UI通常是X轴朝上，但我们要的是Y轴...
    // 实际上WidgetComponent在World Space 下的标准朝向是：-Z 轴指向摄像机
    NewRot = (CameraLoc - WidgetLoc).GetSafeNormal().Rotation();
    
    EnemyHPBar->SetWorldRotation(NewRot);
}
