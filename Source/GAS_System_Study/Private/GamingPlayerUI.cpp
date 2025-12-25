// Solution_BMJ

#include "GamingPlayerUI.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/Texture2D.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceConstant.h"
#include "CPP_PlayerState.h"

UGamingPlayerUI::UGamingPlayerUI(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 在构造函数中使用 FObjectFinder 加载资源
    static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> CoolDownMatFinder(TEXT("/Game/GameBP/Image/CoolDownTex.CoolDownTex"));
    if (CoolDownMatFinder.Succeeded())
    {
        CoolDownMat = CoolDownMatFinder.Object;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load CoolDownMat!"));
    }
}

void UGamingPlayerUI::InitializeUI(TArray<ACPP_PlayerCharacter*> TeamData)
{
	if (!TeamData.Num()) return;
	TeamMembers = TeamData;

	if (!HealthBar)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthBar not bound in GamingPlayerUI!"));
	}

	// 加载角色头像
	LoadCharacterIcon();
}

void UGamingPlayerUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (IsValid(CurrentCharacter))
	{
		// 若技能一标签已配置
		if (SkillOneTag.IsValid())
		{
			FGameplayTagContainer SKillOneTagContainer;
			SKillOneTagContainer.AddTag(SkillOneTag);
			TArray<FActiveGameplayEffectHandle> GetGEList = CurrentCharacter->GetAbilitySystemComponent()->GetActiveEffectsWithAllTags(SKillOneTagContainer);
			if (GetGEList.Num())
			{
				const FActiveGameplayEffect* ActiveGE = CurrentCharacter->GetAbilitySystemComponent()->GetActiveGameplayEffect(GetGEList[0]);
				if (ActiveGE)
				{
					float TotalTime = ActiveGE->GetDuration();
					float RemainTime = ActiveGE->GetTimeRemaining(GetWorld()->GetTimeSeconds());

					SkillOneDynamicMaterial->SetTextureParameterValue(FName("SkillIcon"), SkillOneTexture);
					SkillOneDynamicMaterial->SetScalarParameterValue("Value", FMath::Clamp(RemainTime / TotalTime, 0.0f, 1.0f));
					SkillOneDynamicMaterial->SetScalarParameterValue("Instance", 0.2f);

					SkillOneIcon->SetBrushFromMaterial(SkillOneDynamicMaterial);
				}
			}
		}
		// 若技能二标签已配置
		if (SkillTwoTag.IsValid())
		{
			FGameplayTagContainer SKillTwoTagContainer;
			SKillTwoTagContainer.AddTag(SkillTwoTag);
			TArray<FActiveGameplayEffectHandle> GetGEList = CurrentCharacter->GetAbilitySystemComponent()->GetActiveEffectsWithAllTags(SKillTwoTagContainer);
			if (GetGEList.Num())
			{
				const FActiveGameplayEffect* ActiveGE = CurrentCharacter->GetAbilitySystemComponent()->GetActiveGameplayEffect(GetGEList[0]);
				if (ActiveGE)
				{
					float TotalTime = ActiveGE->GetDuration();
					float RemainTime = ActiveGE->GetTimeRemaining(GetWorld()->GetTimeSeconds());

					SkillTwoDynamicMaterial->SetTextureParameterValue(FName("SkillIcon"), SkillTwoTexture);
					SkillTwoDynamicMaterial->SetScalarParameterValue("Value", FMath::Clamp(RemainTime / TotalTime, 0.0f, 1.0f));
					SkillTwoDynamicMaterial->SetScalarParameterValue("Instance", 0.2f);

					SkillTwoIcon->SetBrushFromMaterial(SkillTwoDynamicMaterial);
				}
			}
		}
	}
}

void UGamingPlayerUI::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	if (HealthBar && MaxHealth > 0)
	{
		if (OldHp <= CurrentHealth)        // 若血量上涨，则立刻同步副血条
		{
			BackGroundHP->SetPercent(CurrentHealth / MaxHealth);
		}
		else                               // 若血量下降，设置计时器
		{
			// 设置副血条扣血计时器
			if (GetWorld()->GetTimerManager().GetTimerRemaining(StartAlignTimer) <= 0.0f)
			{
				UE_LOG(LogTemp, Error, TEXT("UGamingPlayerUI::Set Timer!"));
				GetWorld()->GetTimerManager().SetTimer(
				StartAlignTimer,
				this,
				&UGamingPlayerUI::SetBG_HPBar,
				0.6f,
				false
				);
			}
			PlayHitAnim();
		}
		HealthBar->SetPercent(CurrentHealth / MaxHealth);
		OldHp = CurrentHealth;
		TargetBG_HP = CurrentHealth / MaxHealth;
	}
	// 更新生命值文本
    if (Health_TextBlock)
    {
        // 格式化当前生命值为字符串
        FString HealthStr = FString::Printf(TEXT("%.0f"), CurrentHealth);
        Health_TextBlock->SetText(FText::FromString(HealthStr));
    }

    // 更新最大生命值文本
    if (MaxHealth_TextBlock)
    {
        // 格式化最大生命值为字符串
        FString MaxHealthStr = FString::Printf(TEXT("%.0f"), MaxHealth);
        MaxHealth_TextBlock->SetText(FText::FromString(MaxHealthStr));
    }
}

void UGamingPlayerUI::UpdateStamina(float CurrentStamina, float MaxStamina)
{
	UE_LOG(LogTemp, Log, TEXT("Get Stamina Broadcast!!!!!!!! CurrentStamina: %f , MaxStamina: %f "), CurrentStamina, MaxStamina);
	if (StrengthBar && MaxStamina > 0)
	{
		StrengthBar->SetPercent(CurrentStamina / MaxStamina);
		CurrentCharacter->SetStaminaTimer();
	}
}

void UGamingPlayerUI::OnCharacterSwitched(ACPP_PlayerCharacter* NewCharacter, int32 CharacterID)
{
	if (!NewCharacter || CharacterID >= TeamMembers.Num()) return;

	CurCharacterID = CharacterID;

	// 清除之前的绑定
	UnbindAttributeEvents();

	// 更新当前角色
	CurrentCharacter = NewCharacter;

	BindAttributeEvents();

	UpdateStamina(
		NewCharacter->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>()->GetStamina(), 
		NewCharacter->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>()->GetMaxStamina()
	);

	if (GetWorld()->GetTimerManager().GetTimerRemaining(StartAlignTimer) > 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(StartAlignTimer);
	}
	if (GetWorld()->GetTimerManager().GetTimerRemaining(AlignTimer) > 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(AlignTimer);
	}

	BackGroundHP->SetPercent(HealthBar->Percent);
	
	// 重新加载新角色的技能图标
	LoadAbilityIcons(CurrentCharacter);
}

void UGamingPlayerUI::LoadCharacterIcon()
{
	if (!TeamMembers.Num()) return;
	if (TeamMembers.Num() > 0)
	{
		ACPP_PlayerCharacter* CurCharacter = Cast<ACPP_PlayerCharacter>(TeamMembers[0]);
		if (!CurCharacter->HeadIconPath.IsEmpty())
		{
			Character1Texture = LoadObject<UTexture2D>(nullptr, *CurCharacter->HeadIconPath);
			if (Character1Texture && Character1)
			{
				Character1->SetBrushFromTexture(Character1Texture);
			}
		}
		CurrentCharacter = CurCharacter;
	}
	Character2->SetVisibility(ESlateVisibility::Hidden);
	if (TeamMembers.Num() > 1)
	{
		ACPP_PlayerCharacter* CurCharacter = Cast<ACPP_PlayerCharacter>(TeamMembers[1]);
		if (!CurCharacter->HeadIconPath.IsEmpty())
		{
			Character2Texture = LoadObject<UTexture2D>(nullptr, *CurCharacter->HeadIconPath);
			if (Character2Texture && Character2)
			{
				Character2->SetVisibility(ESlateVisibility::Visible);
				Character2->SetBrushFromTexture(Character2Texture);
			}
		}
	}
	Character3->SetVisibility(ESlateVisibility::Hidden);
	if (TeamMembers.Num() > 2)
	{
		ACPP_PlayerCharacter* CurCharacter = Cast<ACPP_PlayerCharacter>(TeamMembers[2]);
		if (!CurCharacter->HeadIconPath.IsEmpty())
		{
			Character3Texture = LoadObject<UTexture2D>(nullptr, *CurCharacter->HeadIconPath);
			if (Character3Texture && Character3)
			{
				Character3->SetVisibility(ESlateVisibility::Visible);
				Character3->SetBrushFromTexture(Character3Texture);
			}
		}
	}
}

void UGamingPlayerUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (CoolDownMat)
    {
        // 初始化技能1图标材质
        if (SkillOneIcon)
        {
            SkillOneDynamicMaterial = UMaterialInstanceDynamic::Create(CoolDownMat, this);
        }
        
        // 初始化技能2图标材质
        if (SkillTwoIcon)
        {
            SkillTwoDynamicMaterial = UMaterialInstanceDynamic::Create(CoolDownMat, this);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load CoolDownMaterial!"));
    }
}

void UGamingPlayerUI::BindAttributeEvents()
{
	if (CurrentCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("UGamingPlayerUI::Bind AS Event!"));
		// 绑定生命值变化事件
		UPlayerAttributeSet* AttributSet = const_cast<UPlayerAttributeSet*>(CurrentCharacter->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>());
		if (!AttributSet) return;
		AttributSet->OnHealthChanged.AddDynamic(this, &UGamingPlayerUI::UpdateHealth);
		AttributSet->OnStaminaChanged.AddDynamic(this, &UGamingPlayerUI::UpdateStamina);
	}
}

void UGamingPlayerUI::LoadAbilityIcons(ACPP_PlayerCharacter* Character)
{
	UE_LOG(LogTemp, Error, TEXT("UGamingPlayerUI::Load Skill Icon"));
	if (!Character) return;

	// 加载普攻图标
	if (!Character->NormalAttackIconPath.IsEmpty())
	{
		NormalAttackTexture = LoadObject<UTexture2D>(nullptr, *Character->NormalAttackIconPath);
		if (NormalAttackTexture && NormalAttackIcon)
		{
			NormalAttackIcon->SetBrushFromTexture(NormalAttackTexture);
		}
	}
	
	// 加载技能1图标
	if (!Character->SkillOneIconPath.IsEmpty())
	{
		SkillOneTexture = LoadObject<UTexture2D>(nullptr, *Character->SkillOneIconPath);
        if (SkillOneTexture && SkillOneIcon && SkillOneDynamicMaterial)
        {
            // 设置技能1图标到材质
            SkillOneDynamicMaterial->SetTextureParameterValue(FName("SkillIcon"), SkillOneTexture);
			SkillOneDynamicMaterial->SetScalarParameterValue("Value", 0.0f);
            SkillOneDynamicMaterial->SetScalarParameterValue("Instance", 0.0f);
            
            // 设置技能1图标到UI
            SkillOneIcon->SetBrushFromTexture(SkillOneTexture);
        }
	}

	// 加载技能2图标
	if (!Character->SkillTwoIconPath.IsEmpty())
	{
		SkillTwoTexture = LoadObject<UTexture2D>(nullptr, *Character->SkillTwoIconPath);
        if (SkillTwoTexture && SkillTwoIcon && SkillTwoDynamicMaterial)
        {
            // 设置技能2图标到材质
            SkillTwoDynamicMaterial->SetTextureParameterValue(FName("SkillIcon"), SkillTwoTexture);
			SkillTwoDynamicMaterial->SetScalarParameterValue("Value", 0.0f);
            SkillTwoDynamicMaterial->SetScalarParameterValue("Instance", 0.0f);
            
            // 设置技能2图标到UI
            SkillTwoIcon->SetBrushFromTexture(SkillTwoTexture);
        }
	}
}

void UGamingPlayerUI::UpdateSkillCooldown(int32 SkillIndex, float CurrentCooldown, float MaxCooldown)
{
	// 计算冷却比率（1.0表示完全冷却，0.0表示正在冷却）
    float Value = 0.0f;
    if (MaxCooldown > 0.0f)
    {
        Value = 1.0f - (CurrentCooldown / MaxCooldown);
    }
    
    // 更新对应技能的Value参数
    if (SkillIndex == 0 && SkillOneDynamicMaterial)
    {
        SkillOneDynamicMaterial->SetScalarParameterValue(FName("Value"), Value);
    }
    else if (SkillIndex == 1 && SkillTwoDynamicMaterial)
    {
        SkillTwoDynamicMaterial->SetScalarParameterValue(FName("Value"), Value);
    }
}

void UGamingPlayerUI::SetBG_HPBar()
{
	GetWorld()->GetTimerManager().SetTimer(
	AlignTimer,
	this,
	&UGamingPlayerUI::UpdateBG_HP,
	0.016f,
	true
	);
}

void UGamingPlayerUI::UpdateBG_HP()
{
	if (!BackGroundHP) return;
	BackGroundHP->SetPercent(BackGroundHP->Percent - 0.005f);
	if (BackGroundHP->Percent <= HealthBar->Percent)
	{
		BackGroundHP->SetPercent(HealthBar->Percent);
		GetWorld()->GetTimerManager().ClearTimer(AlignTimer);
	}
}

void UGamingPlayerUI::UnbindAttributeEvents()
{
	if (CurrentCharacter)
	{
		UPlayerAttributeSet* AttributSet = const_cast<UPlayerAttributeSet*>(CurrentCharacter->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>());
		if (!AttributSet) return;
		AttributSet->OnHealthChanged.RemoveDynamic(this, &UGamingPlayerUI::UpdateHealth);
		AttributSet->OnStaminaChanged.RemoveDynamic(this, &UGamingPlayerUI::UpdateStamina);
	}
}

