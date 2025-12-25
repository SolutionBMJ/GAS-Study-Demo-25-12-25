// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/SaveGame.h"
#include "AccountData.generated.h"

class UEquipmentManager;
/**
 * 
 */

// 装备类型
UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
    Head,
    Chest,
    Hand,
    Leg
};

// 单个装备信息结构体
USTRUCT(BlueprintType)
struct FEquipmentData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()
    // 装备ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    FString EquipmentID;

    // 装备名称
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    FString Name;

    // 装备图标路径
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    FString IconPath;

    // 装备类型
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    EEquipmentType Type;

    // 攻击力加成
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    float AttackBonus;

    // 防御力加成
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    float DefenceBonus;

    // 生命值加成
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    float HealthBonus;

    // 是否装备中
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    bool bIsEquipped;

    // 所装备的角色
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame)
    FString EquippedToCharacter;
};

USTRUCT(BlueprintType)
struct FPlayerTeam
{
    GENERATED_BODY()

public:
    // 队伍成员数组，3个
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Members;

    // 队伍名称
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString TeamName;

    // 构造函数，初始化固定3个空位
    FPlayerTeam()
    {
        Members.SetNum(3); // 预先分配3个空字符串位置
    }

    // 检查队伍是否有效
    bool IsValid() const
    {
        // 例如至少有一个成员
        if (!Members.Num()) return false;
        return true;
    }
};

// 账户数据信息
UCLASS()
class GAS_SYSTEM_STUDY_API UAccountData : public USaveGame
{
	GENERATED_BODY()

public:
     // 创建7个默认的FPlayerTeam
    UAccountData();

    // 注册玩家
	void RegistPlayer(FString AAccountName, FString PPassward);

    // 玩家队伍信息
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
    TArray<FPlayerTeam> Teams;

	// 获取拥有的角色列表
    UFUNCTION(BlueprintCallable, Category = "Account")
    TArray<FString> GetOwnedCharacters() const { return OwnedCharacters; }

	// 添加角色到账号
    UFUNCTION(BlueprintCallable, Category = "Account")
    bool AddCharacter(const FString& CharacterName);
	
    // 玩家是否有某角色
	bool HasCharacter(const FString& CharacterID) const;

public:
	UPROPERTY(VisibleAnywhere)
	FString AccountName;        // 用户名

	UPROPERTY(VisibleAnywhere)
	FString Password;           // 密码

	UPROPERTY(VisibleAnywhere)
	FString PlayerName;         // 玩家昵称

	UPROPERTY(VisibleAnywhere)
	int32 PlayerLevel;          // 玩家等级

    UPROPERTY(VisibleAnywhere, SaveGame)
    TArray<FEquipmentData> OwnedEquipments;    // 玩家拥有的装备信息

    UPROPERTY(VisibleAnywhere)
    int32 TeamIndex;            // 出战队伍

	UPROPERTY(VisibleAnywhere)
    TArray<FString> OwnedCharacters;     // 拥有的角色

    // 装备管理器
    UEquipmentManager* EquipmentManager;

};
