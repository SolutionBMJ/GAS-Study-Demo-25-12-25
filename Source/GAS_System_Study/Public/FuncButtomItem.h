// Solution_BMJ

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "FuncButtomItem.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FFuncButtomInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	// 按钮唯一标识
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ButtonData")
    FName ButtonID;

    // 按钮显示文本
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ButtonData")
    FText ButtonText;

    // 按钮图标路径
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ButtonData")
    FString IconPath;

    // 点击后要执行的功能函数名称
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ButtonData")
    FName FunctionName;
};

UCLASS()
class GAS_SYSTEM_STUDY_API UFuncButtomItem : public UObject
{
	GENERATED_BODY()
	
public:
    // 承载你的按钮信息结构体
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FFuncButtomInfo ButtonInfo;
};
