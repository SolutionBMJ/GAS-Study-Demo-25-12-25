// Solution_BMJ


#include "EnemyHPBarWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"

void UEnemyHPBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
    // 确保进度条组件已绑定
    
}

void UEnemyHPBarWidget::UpdateHP(float CurrentHP, float MaxHP)
{
    if (HPBar && MaxHP > 0)
    {
        HPBar->SetPercent(CurrentHP / MaxHP);

        if (OldHp <= CurrentHP)        // 若血量上涨，则立刻同步副血条
		{
			HPBar->SetPercent(CurrentHP / MaxHP);
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
				&UEnemyHPBarWidget::SetHPBar_BG,
				0.6f,
				false
				);
			}
			GetHitShake();
		}
		HPBar->SetPercent(CurrentHP / MaxHP);
		OldHp = CurrentHP;
		TargetBG_HP = CurrentHP / MaxHP;
    }
}

void UEnemyHPBarWidget::SetHPBar_BG()
{
	GetWorld()->GetTimerManager().SetTimer(
	AlignTimer,
	this,
	&UEnemyHPBarWidget::UpdateHPBar_BG,
	0.016f,
	true
	);
}

void UEnemyHPBarWidget::UpdateHPBar_BG()
{
	if (!HPBar_BG) return;
	HPBar_BG->SetPercent(HPBar_BG->Percent - 0.005f);
	if (HPBar_BG->Percent <= HPBar->Percent)
	{
		HPBar_BG->SetPercent(HPBar->Percent);
		GetWorld()->GetTimerManager().ClearTimer(AlignTimer);
	}
}
