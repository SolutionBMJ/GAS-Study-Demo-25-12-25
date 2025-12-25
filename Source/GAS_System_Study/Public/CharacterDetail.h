// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "AccountData.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/DragDropOperation.h"
#include "CharacterInfo.h"
#include "Components/HorizontalBox.h"
#include "Components/ListView.h"
#include "Components/TileView.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "EquipmentManager.h"
#include "EquipmentSlotWidget.h"
#include "CPP_PlayerCharacter.h"
#include "CharacterDetail.generated.h"

// 临时包装单个装备
UCLASS()
class GAS_SYSTEM_STUDY_API UEquipmentItem : public UObject
{
    GENERATED_BODY()

public:
    FEquipmentData SingleEquipmentItem;
};

// 角色详情展示页面
UCLASS()
class GAS_SYSTEM_STUDY_API UCharacterDetail : public UUserWidget
{
    GENERATED_BODY()

public:
    // 打开角色详情页
    UFUNCTION()
    void OpenCharacterDetail();

    // 关闭角色详情页
    UFUNCTION()
    void CloseCharacterDetail();

    // 装备状态改变回调
    void EquipStateChanged();

    // 在锚点生成角色副本
    void SpawnDisplayCharacterAtAnchor(FString CharacterName);

    // 装备点击事件
    UFUNCTION()
    void OnEquipmentClicked(UObject* Item);

    // 角色详情页面打开状态
    bool bIsCharacterDetailOpen = false;

     // 当前显示的角色
    UPROPERTY()
    ACPP_PlayerCharacter* CurrentCharacter;

    // 当前选中的装备
    FEquipmentData* CurrentSelectedEquipment;

    // 装备选择槽
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTileView* EquipmentDisplayTileView;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    class UDataTable* CharacterDataTable;

    // 玩家控制器
    APlayerController* PC;

    // 属性展示------------------------------------------
    // 最终攻击力
    UPROPERTY(meta = (BindWidget))
    UTextBlock* AttackValueText;

    // 最终防御力
    UPROPERTY(meta = (BindWidget))
    UTextBlock* DefenseValueText;

    // 最终最大生命值
    UPROPERTY(meta = (BindWidget))
    UTextBlock* MaxHealthValueText;

    // 暴击率
    UPROPERTY(meta = (BindWidget))
    UTextBlock* CriticalRate;

    // 暴击伤害
    UPROPERTY(meta = (BindWidget))
    UTextBlock* CriticalValue;
    //---------------------------------------------------

    // 返回键
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* GoBack;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CharacterNameBlock;

    // 角色选择槽
    UPROPERTY(meta = (BindWidget))
    UListView* CharacterSelect;

    // 装备槽-----------------------------------
    UPROPERTY(meta = (BindWidget))
    UEquipmentSlotWidget* EquipmentSlotHead;

    UPROPERTY(meta = (BindWidget))
    UEquipmentSlotWidget* EquipmentSlotHand;

    UPROPERTY(meta = (BindWidget))
    UEquipmentSlotWidget* EquipmentSlotChest;

    UPROPERTY(meta = (BindWidget))
    UEquipmentSlotWidget* EquipmentSlotLeg;
    //-----------------------------------------------

    // 装备名称
    UPROPERTY(meta = (BindWidget))
    UTextBlock* EquipmentName;

    // 装备攻击力提升
    UPROPERTY(meta = (BindWidget))
    UTextBlock* EquipmentAttackValue;

    // 装备防御力提升
    UPROPERTY(meta = (BindWidget))
    UTextBlock* EquipmentDefenseValue;

    // 装备最大生命值提升
    UPROPERTY(meta = (BindWidget))
    UTextBlock* EquipmentMaxHealthValue;

    // 装备暴击率提升
    UPROPERTY(meta = (BindWidget))
    UTextBlock* EquipmentCriticalRate;

    // 装备暴击伤害提升
    UPROPERTY(meta = (BindWidget))
    UTextBlock* EquipmentCriticalValue;

    // 确认装备
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* EquippedUp;

    // 卸下装备
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* EquippedDown;

    // 拥有的角色数组
    UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<ACPP_PlayerCharacter*> CharacterObjectList;

    // 装备数据缓存
    UPROPERTY()
    TArray<FEquipmentData> AllEquipment; // 玩家所有装备

    UPROPERTY()
    TMap<EEquipmentType, FEquipmentData> EquippedSlots; // 已装备的槽位

    UPROPERTY()
    TArray<FEquipmentData> UnEquippedEquipment; // 未装备的装备

    // 角色数据
    UPROPERTY()
    TArray<ACPP_PlayerCharacter*> OwnedCharacters; // 玩家拥有的角色

private:
    UFUNCTION(BlueprintCallable)
    void UnEquipped(const FEquipmentData& Equipment);

    UPROPERTY()
    ACameraActor* DisplayCamera; // 展示关卡的摄像机

    UPROPERTY()
    AActor* CharacterSpawnPoint; // 角色生成锚点

    UPROPERTY()
    ACPP_PlayerCharacter* DisplayCharacter; // 展示用的角色副本

    UPROPERTY()
    class APlayerCameraActor* OriginalViewTarget; // 保存切换前的原视角目标

    // 获取当前登录玩家
	UAccountData* GetCurrentPlayer();

    // 关卡加载完成后的回调函数
    UFUNCTION()
    void OnLevelLoaded();

    // 获取当前登录玩家拥有角色
    UFUNCTION()
    void LoadPlayerOwnedCharacter();

    // 刷新左侧装备槽显示
    void RefreshEquippedSlots();

    // 刷新右侧未装备列表
    void RefreshUnEquippedList(); 

    // 计算角色最终属性
    void CalculateFinalAttributes(ACPP_PlayerCharacter* CurChar);

    // 查找展示关卡的摄像机和锚点
    void FindDisplayLevelActors();

    // 切换到展示摄像机视角
    void SwitchToDisplayCamera();

    // 恢复原摄像机视角
    void RestoreOriginalCamera();

    // 销毁展示用的角色副本
    void DestroyDisplayCharacter();

    // 点击装备按钮
    UFUNCTION()
    void OnEquipClicked();

    // 点击卸下按钮
    UFUNCTION()
    void OnUnequipClicked();
};
