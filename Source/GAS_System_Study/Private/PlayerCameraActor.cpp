// Solution_BMJ


#include "PlayerCameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"

APlayerCameraActor::APlayerCameraActor()
{
    // 启用Tick（必须，否则无法实时更新位置/旋转）
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 0.016f; // 约60FPS

    // 默认视角旋转（初始朝后）
    CurrentViewRotation = FRotator(0.0f, 180.0f, 0.0f);
    GetCameraComponent()->bConstrainAspectRatio = false;
}

void APlayerCameraActor::BeginPlay()
{
    Super::BeginPlay();

    // 初始化摄像机旋转（初始位置基于跟随目标）
    if (FollowTarget)
    {
        SetActorLocation(CalculateTargetLocation());
        SetActorRotation(CurrentViewRotation);
    }
}

void APlayerCameraActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsTransitioning)
    {
        // 处理过渡逻辑
        TransitionElapsedTime += DeltaTime;
        float Alpha = FMath::Clamp(TransitionElapsedTime / TransitionDuration, 0.0f, 1.0f);

        TransitionTargetLocation = CalculateTargetLocation();

        // 位置过渡
        FVector NewLocation = FMath::VInterpTo(
            TransitionStartLocation,
            TransitionTargetLocation,
            DeltaTime,
            12.0f // 基于过渡时长的速度
        );
        SetActorLocation(NewLocation);

        TransitionStartLocation = NewLocation;

        // 过渡结束
        if (Alpha >= 1.0f)
        {
            bIsTransitioning = false;
        }
        return; // 过渡中不执行正常更新
    }

    if (!FollowTarget) return;

    // 1. 计算目标位置并平滑移动
    FVector TargetLocation = CalculateTargetLocation();
    FVector NewLocation = FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, PositionSmoothSpeed);
    SetActorLocation(NewLocation);

    // 2. 应用视角旋转（左右/上下）
    SetActorRotation(CurrentViewRotation);
}

void APlayerCameraActor::AddViewRotation(const FVector2D& DeltaRotation)
{
    if (DeltaRotation.IsZero()) return;

    // 处理水平旋转
    CurrentViewRotation.Yaw += DeltaRotation.X * RotationSensitivity;

    // 处理垂直旋转
    CurrentViewRotation.Pitch -= DeltaRotation.Y * RotationSensitivity;
    CurrentViewRotation.Pitch = FMath::Clamp(CurrentViewRotation.Pitch, MinPitch, MaxPitch);

    // 确保Roll始终为0，防止摄像机倾斜
    CurrentViewRotation.Roll = 0.0f;
}

void APlayerCameraActor::ResetViewToBehindTarget()
{
    if (!FollowTarget) return;

    // 重置视角旋转：Yaw = 角色Yaw + 180 朝后，Pitch = 0 水平
    CurrentViewRotation.Yaw = FollowTarget->GetActorRotation().Yaw;// +180.0f;
    CurrentViewRotation.Pitch = 0.0f;
    ClampViewRotation(); // 确保Pitch在限制范围内

    // 立即更新位置到目标位置
    SetActorLocation(CalculateTargetLocation());
    SetActorRotation(CurrentViewRotation);
}

FVector APlayerCameraActor::CalculateTargetLocation() const
{
    if (!FollowTarget) return GetActorLocation();

    // 使用完整的当前视角旋转
    FRotator TargetRotation = CurrentViewRotation;
    // 计算相机看向的方向
    FVector ViewDirection = TargetRotation.Vector();
    // 相机位置 = 角色位置 - 看向方向 * 跟随距离
    FVector TargetLocation = FollowTarget->GetActorLocation() - ViewDirection * FollowDistance;
    // 高度偏移
    TargetLocation.Z += FollowHeight;

    FVector StartLocation = FollowTarget->GetActorLocation();
    StartLocation.Z += FollowHeight;
    FVector EndLocation = TargetLocation;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(FollowTarget); // 忽略跟随目标自身
    QueryParams.AddIgnoredActor(this); // 忽略摄像机自身

    FHitResult HitResult;
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECollisionChannel::ECC_Visibility,
        QueryParams
    );

    if (bHit)
    {
        const float Offset = 10.0f; // 碰撞点前的偏移量
        HitResult.Location += ViewDirection * Offset;
        return HitResult.Location;
    }

    return TargetLocation;
}

void APlayerCameraActor::StartTransition(AActor* NewTarget)
{
    if (!NewTarget) return;

    // 切换目标计算位置
    FollowTarget = NewTarget;

    // 启动过渡
    bIsTransitioning = true;
    TransitionElapsedTime = 0.0f;
    TransitionStartLocation = GetActorLocation();
}

void APlayerCameraActor::ClampViewRotation()
{
    // 限制Pitch在MinPitch和MaxPitch之间
    CurrentViewRotation.Pitch = FMath::Clamp(CurrentViewRotation.Pitch, MinPitch, MaxPitch);
    // 确保Roll为0，避免倾斜
    CurrentViewRotation.Roll = 0.0f;
}
