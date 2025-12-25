// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "GameFramework/PlayerController.h"
#include "CPP_PlayerState.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"      // 增强输入子系统
#include "EnhancedInputComponent.h"      // 增强输入组件
#include "InputActionValue.h"            // 输入值类型
#include "PlayerCameraActor.h"
#include "GameplayEffect.h"
#include "CPP_PlayerController.generated.h"

class UAttackAttach;
class UGamingPlayerUI;
class UPauseMenuWidget;
class UTeamMakerWidget;
class UCharacterDetail;
/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API ACPP_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    ACPP_PlayerController();

    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPlaySectionNotify, const FGameplayTag&, bool);
    FOnPlaySectionNotify OnPlaySectionNotify;

    // 切换摄像机跟踪的目标
    void SwitchCameraTarget(ACPP_PlayerCharacter* NewTarget);

    // 记录GA标签
    void SetAttackTags(FGameplayTag AtTag) { AttackTag = AtTag; }

    // 清除GA标签
    void ClearAttackTags() { AttackTag = FGameplayTag(); }

    FGameplayTag GetAbilityTag() { return AttackTag; }

    // 记录GA优先级标签
    void SetPriorityTags(FGameplayTag PriTag) { PriorityTag = PriTag; }

    // 清除GA优先级标签
    void ClearPriorityTags() { PriorityTag = FGameplayTag(); }

    // 收到动画通知
    UFUNCTION()
    void GetAnimNotify();

    UFUNCTION(BlueprintImplementableEvent)
    void TestGE(UAbilitySystemComponent* AASC);

    UFUNCTION()
    void ForceFaceToInput();

    UFUNCTION()
    void FaceToTarget();

    // 返回游戏中的UI
    UGamingPlayerUI* GetGamingUI() { return PlayerUI; }

    // 游戏中的UI
    UGamingPlayerUI* PlayerUI;

    // 暂停菜单UI
    UPauseMenuWidget* PauseMenuUI;

    // 编队界面UI
    UTeamMakerWidget* TeamMakerUI;

    // 角色界面UI
    UCharacterDetail* CharacterDetailUI;

    // 检查新技能是否可以打断当前技能
    bool CanInterruptCurrentAbility(FGameplayTag NewAbilityPriority) const;

    // 设置当前摄像机
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SetCameraFollowTarget(ACPP_PlayerCharacter* Target);

    // 获取摄像机
    UFUNCTION(BlueprintCallable, Category = "Camera")
    APlayerCameraActor* GetCurrentCamera() { return CurrentFollowCamera; }
	
protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void OpenLoginUI();

	virtual void SetupInputComponent() override;

    // 输入映射上下文（在编辑器中创建的资源）
    UPROPERTY(EditAnywhere, Category = "Input")
    class UInputMappingContext* MoveMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
    class UInputAction* JumpAction;       // 跳跃动作

    UPROPERTY(EditAnywhere, Category = "Input")
    class UInputAction* MoveAction;       // 移动动作

    UPROPERTY(EditAnywhere, Category = "Input")
    class UInputAction* LookAction;       // 移动视角

    UPROPERTY(EditAnywhere, Category = "Input")
    class UInputAction* DashAction;       // 移动视角

    UPROPERTY(EditAnywhere, Category = "Input")
    class UInputAction* CameraAction;     // 缩放视角

    UPROPERTY(EditAnywhere, Category = "Input")
    class UInputAction* WalkSwitchAction; // 走跑切换

    UPROPERTY(EditAnywhere, Category = "Attack")
    class UInputMappingContext* AttackMappingContext;

	UPROPERTY(EditAnywhere, Category = "Attack")
    class UInputAction* NormalAttackAction;     // 攻击

    UPROPERTY(EditAnywhere, Category = "Attack")
    class UInputAction* Skill_One_Action;       // 技能1

    UPROPERTY(EditAnywhere, Category = "Attack")
    class UInputAction* Skill_Two_Action;       // 技能2

    UPROPERTY(EditAnywhere, Category = "Attack")
    class UInputMappingContext* SwitchMappingContext;

	UPROPERTY(EditAnywhere, Category = "Attack")
    class UInputAction* switch1;       // 切换角色1

    UPROPERTY(EditAnywhere, Category = "Attack")
    class UInputAction* switch2;       // 切换角色2

    UPROPERTY(EditAnywhere, Category = "Attack")
    class UInputAction* switch3;       // 切换角色3

    UPROPERTY(EditAnywhere, Category = "OtherFunc")
    class UInputMappingContext* FuncMappingContext;

    UPROPERTY(EditAnywhere, Category = "OtherFunc")
    class UInputAction* Esc;            // 暂停/返回上一级

    UPROPERTY(EditAnywhere, Category = "OtherFunc")
    class UInputAction* Team;           // 编队界面

    UPROPERTY(EditAnywhere, Category = "OtherFunc")
    class UInputAction* CharacterInfo;  // 编队界面

    // 保存 FollowCameraActor 的指针
    UPROPERTY()
    APlayerCameraActor* CurrentFollowCamera;

    // 主界面UI类
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> MainMenuWidgetClass;

	// 当前显示的UI实例
    UPROPERTY()
    UUserWidget* CurrentUIWidget;

    // 根据索引激活技能
    void TryActivateAbilityByTag(const FGameplayTag& AbilityTag, FGameplayTag NewAbilityPriority);

private:
    // 获取对应GA的优先级标签
    FGameplayTag GetAbilityPriorityByTag(const FGameplayTag& AbilityTag);

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UGamingPlayerUI> PlayerUIClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UPauseMenuWidget> PauseMenuClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UTeamMakerWidget> TeamMakerClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UCharacterDetail> CharacterDetailClass;

    UPROPERTY(EditDefaultsOnly, Category = "Ability Tags")
    FGameplayTag NormalAttackAbilityTag = FGameplayTag::RequestGameplayTag(FName("Player.Character.Attack.Normal"));

    UPROPERTY(EditDefaultsOnly, Category = "Ability Tags")
    FGameplayTag HoldNormalAttackAbilityTag = FGameplayTag::RequestGameplayTag(FName("Player.Character.Attack.NAHold"));

    UPROPERTY(EditDefaultsOnly, Category = "Ability Tags")
    FGameplayTag SkillOneAbilityTag = FGameplayTag::RequestGameplayTag(FName("Player.Character.Attack.SkillOne"));

    UPROPERTY(EditDefaultsOnly, Category = "Ability Tags")
    FGameplayTag HoldSkillOneAbilityTag = FGameplayTag::RequestGameplayTag(FName("Player.Character.Attack.SOHold"));

    UPROPERTY(EditDefaultsOnly, Category = "Ability Tags")
    FGameplayTag SkillTwoAbilityTag = FGameplayTag::RequestGameplayTag(FName("Player.Character.Attack.SkillTwo"));

    UPROPERTY(EditDefaultsOnly, Category = "Ability Tags")
    FGameplayTag HoldSkillTwoAbilityTag = FGameplayTag::RequestGameplayTag(FName("Player.Character.Attack.STHold"));

    UPROPERTY(EditDefaultsOnly, Category = "Ability Tags")
    FGameplayTag DashAbilityTag = FGameplayTag::RequestGameplayTag(FName("Player.Character.State.Running"));

    // 当前 GA 标签
	FGameplayTag AttackTag;

    // 当前 GA 优先级
    FGameplayTag PriorityTag;

    UAttackAttach* AttackAttachComponent;

    FVector InputDir;

    // 判断要切换的角色
    void ChangeToCharacter1();
    void ChangeToCharacter2();
    void ChangeToCharacter3();
    // 开始切换
    void BeginChange(int index);

	// 输入回调函数（处理跳跃）
    void OnJump();
    void StopJump();

    // 输入回调函数（处理移动）
    void OnMove(const FInputActionValue& Value);
    void OnMoveStopped();

    // 输入回调函数（处理视角）
    void OnLook(const FInputActionValue& Value);

    // 判断长短按输入
    void StartDashOrRunning();
    void OnDash();          // 短按冲刺
    void OnRunning();       // 长按奔跑

    void SwitchWalkAndRun();// 走跑切换

    // 释放普攻
	void OnNormalAttackInput();

    // 释放技能一
    void OnSkill_One();

    // 释放技能二
    void OnSkill_Two();

    // 释放普攻长按
    void OnHoldNormalAttackInput();

    // 释放技能一长按
    void OnHoldSkill_OneInput();

    // 释放技能二长按
    void OnHoldSkill_TwoInput();

    void ControlCameraDistance(const FInputActionValue& Value);

public:
    // 暂停
    void OnPause();

    // 打开编队界面
    void OnTeamMaker();

    // 打开角色详情
    void OnCharacterDetail();
};
