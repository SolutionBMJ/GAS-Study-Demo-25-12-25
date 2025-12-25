// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/TileView.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "CharacterInfo.h"
#include "CPP_PlayerCharacter.h"
#include "TeamMakerCharacterIconUI.h"
#include "TeamMakerWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAS_SYSTEM_STUDY_API UTeamMakerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void OpenTeamMaker();

	UFUNCTION(BlueprintCallable)
	void CloseTeamMaker();

	// 关闭选择角色界面
	UFUNCTION(BlueprintImplementableEvent)
	void CloseTeamSelect();

	UFUNCTION(BlueprintImplementableEvent)
	void EnterCharacterSelect();

	UPROPERTY(BlueprintReadOnly)
	bool bIsTeamMakerOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    class UDataTable* CharacterDataTable;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	TArray<ACPP_PlayerCharacter*> CharacterObjectList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon")
    FString DefaultIconPath;

	// 获取当前出战队伍
	void GetCurrentTeam(int32 TeamIndex);

	// 更新单个角色队伍图标
	void UpdataSingleCharacterIcon(UAccountData* CurPlayer, int32 TeamIndex, int32 index, UImage* TargetImage);

	// 获取拥有的角色
	void GetOwnedCharacter();

	void SwitchTeamPage(int32 index);

	// 确定选择的角色
	UFUNCTION()
	void ChangeCharacter();

	// 确定出战队伍
	UFUNCTION(BlueprintCallable)
	void ChangeFightTeam();

	// 根据选择角色显示角色信息
	UFUNCTION()
	void ChangeSelectCharacter(FString CharacterName);

	// 设置所选角色
	UFUNCTION()
	void SelectCharacter(int32 CharIndex);

	void ChangeCharacterByDrag(int32 CharOne, int32 CharTwo);

	// 记录选择的按钮
	UButton* BanButtom;

	// 确定配队
	UPROPERTY(meta = (BindWidget))
	UButton* ConfrimTeam;

	// 确定选择角色按钮
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ConfrimCharacter;

	UPROPERTY(meta = (BindWidget))
	UButton* RemoveCharacter;

	// 显示所选角色名称
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CharacterNameText;

	// 显示所选角色信息
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CharacterInfo;

	// 显示所选角色头像
	UPROPERTY(meta = (BindWidget))
	UImage* CharacterIcon;

	// 显示所选角色普攻
	UPROPERTY(meta = (BindWidget))
	UImage* NormalAttackIcon;

	// 显示所选角色技能一
	UPROPERTY(meta = (BindWidget))
	UImage* SkillOneIcon;

	// 显示所选角色技能二
	UPROPERTY(meta = (BindWidget))
	UImage* SkillTwoIcon;

	// 当前所选队伍
	int32 SelectTeam;

	// 当前选择的角色ID
	int32 SelectCharID;

	// 记录选择的角色，若点击ConfrimCharacter，就记录到数组
	FString SelectedCharacter;

	bool bIsChangeTeam;

protected:
	// 输入处理
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UPROPERTY(meta = (BindWidget))
	UTileView* SelectCharacterList;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Team1;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Team2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Team3;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Team4;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Team5;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Team6;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Team7;

	UPROPERTY(meta = (BindWidget))
	UTeamMakerCharacterIconUI* FirstCharacterIcon;

	UPROPERTY(meta = (BindWidget))
	UTeamMakerCharacterIconUI* SecondCharacterIcon;

	UPROPERTY(meta = (BindWidget))
	UTeamMakerCharacterIconUI* ThirdCharacterIcon;

private:
	// 快捷键
    void HandleShortcutKey(const FKey& Key);

	TWeakObjectPtr<APlayerController> PC; // 使用弱指针确保生命周期内PC对象有效

	UFUNCTION()
	void RemoveCharacterOnTeam();

	UFUNCTION()
	void PressTeam1();

	UFUNCTION()
	void PressTeam2();

	UFUNCTION()
	void PressTeam3();

	UFUNCTION()
	void PressTeam4();

	UFUNCTION()
	void PressTeam5();

	UFUNCTION()
	void PressTeam6();

	UFUNCTION()
	void PressTeam7();
};
