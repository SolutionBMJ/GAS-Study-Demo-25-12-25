// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "EnhancedInputComponent.h"
#include "PlayerCameraActor.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API APlayerCameraActor : public ACameraActor
{
	GENERATED_BODY()
	
public:
    // 初始化默认参数
    APlayerCameraActor();

    // 每帧处理跟随和视角控制
    virtual void Tick(float DeltaTime) override;

    // 初始化输入绑定视角控制
    UFUNCTION(BlueprintCallable, Category = "CameraControl")
    void AddViewRotation(const FVector2D& DeltaRotation);

    // 立即重置视角到角色正后方
    UFUNCTION(BlueprintCallable, Category = "CameraControl")
    void ResetViewToBehindTarget();

    // 跟随距离
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings", meta = (AllowPrivateAccess = "true", MinValue = 100.0f))
    float FollowDistance = 400.0f;

    // 跟随高度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings", meta = (AllowPrivateAccess = "true", MinValue = 0.0f))
    float FollowHeight = 70.0f;

    UFUNCTION(BlueprintCallable)
    float GetRotationSensitivity() { return RotationSensitivity; }

    // 开始过渡
    void StartTransition(AActor* NewTarget);

    bool IsTransition() { return bIsTransitioning; }

protected:
    // 开始播放时初始化
    virtual void BeginPlay() override;

private:
    // 跟随目标
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings", meta = (AllowPrivateAccess = "true"))
    AActor* FollowTarget = nullptr;

    // 位置平滑移动速度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings", meta = (AllowPrivateAccess = "true", MinValue = 1.0f))
    float PositionSmoothSpeed = 100.0f;

    // 视角旋转灵敏度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings", meta = (AllowPrivateAccess = "true", MinValue = 0.1f))
    float RotationSensitivity = 1.0f;

    // 上下视角限制
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings", meta = (AllowPrivateAccess = "true"))
    float MinPitch = -80.0f; // 最低视角
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraSettings", meta = (AllowPrivateAccess = "true"))
    float MaxPitch = 80.0f;  // 最高视角

    // 当前视角旋转
    FRotator CurrentViewRotation;

    // 计算摄像机的目标位置
    FVector CalculateTargetLocation() const;

    // 限制视角旋转范围
    void ClampViewRotation();

private:
    // 过渡状态管理
    UPROPERTY()
    bool bIsTransitioning = false;                 // 是否处于角色切换过渡中
    FVector TransitionStartLocation;               // 过渡起始位置
    FVector TransitionTargetLocation;              // 过渡目标位置
    float TransitionElapsedTime = 0.0f;            // 过渡已消耗时间

    UPROPERTY(EditAnywhere, Category = "Transition", meta = (AllowPrivateAccess = "true"))
    float TransitionDuration = 0.35f;               // 过渡总时长
};
