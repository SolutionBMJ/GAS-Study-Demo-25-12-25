// Solution_BMJ


#include "TeamMakerCharacterIconUI.h"
#include "Components/CanvasPanelSlot.h"
#include "TeamMakerWidget.h"

FReply UTeamMakerCharacterIconUI::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        bIsMouseDown = true;
        bIsDragging = false;
        
        // 记录鼠标按下的位置
        MouseDownPosition = InMouseEvent.GetScreenSpacePosition();
        
        // 捕获鼠标以便在控件外也能接收移动事件
        return FReply::Handled().CaptureMouse(TakeWidget());
    }
    
    return FReply::Unhandled();
}

FReply UTeamMakerCharacterIconUI::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (bIsMouseDown && !bIsDragging)
    {
        FVector2D CurrentPosition = InMouseEvent.GetScreenSpacePosition();
        float Distance = FVector2D::Distance(MouseDownPosition, CurrentPosition);
        
        // 如果移动距离超过阈值，触发拖拽
        if (Distance >= 5.0f)
        {
            bIsDragging = true;
            
            // 这会自动调用蓝图中的 OnDragDetected 事件
            FKey LeftMouse = EKeys::LeftMouseButton;
            return FReply::Handled().DetectDrag(TakeWidget(), LeftMouse);
        }
    }
    
    return FReply::Unhandled();
}

FReply UTeamMakerCharacterIconUI::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (bIsMouseDown && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        // 释放鼠标捕获
        FReply Reply = FReply::Handled().ReleaseMouseCapture();
        
        // 如果没有开始拖拽，视为点击
        if (!bIsDragging)
        {
            // 检查是否仍在控件范围内
            FVector2D CurrentPosition = InMouseEvent.GetScreenSpacePosition();
            float Distance = FVector2D::Distance(MouseDownPosition, CurrentPosition);
            
            if (Distance < 5.0f)
            {
                OnClickCharacter();
            }
        }
        
        // 重置状态
        bIsMouseDown = false;
        bIsDragging = false;
        
        return Reply;
    }
    
    return FReply::Unhandled();
}

void UTeamMakerCharacterIconUI::OnClickCharacter()
{
	if (ParentWidget)
	{
		ParentWidget->SelectCharacter(SlotID);
	}
}

void UTeamMakerCharacterIconUI::GetDropCharacter(int32 CharacterIndex)
{
    if (ParentWidget)
	{
        ParentWidget->ChangeCharacterByDrag(SlotID, CharacterIndex);
	}
}
