// Solution_BMJ


#include "FuncItemButtom.h"
#include "CPP_PlayerController.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "FuncButtomItem.h"
#include "Engine/Texture2D.h"

void UFuncItemButtom::NativeOnListItemObjectSet(UObject* InListItemObject)
{
    ListItemObject = InListItemObject;

	UFuncButtomItem* DataObj = Cast<UFuncButtomItem>(InListItemObject);
    if (!DataObj) return;

    ButtonData = DataObj->ButtonInfo;

    OnButtonDataReady();

    FuncButton->OnClicked.AddDynamic(this, &UFuncItemButtom::OnClick);
}

void UFuncItemButtom::OnClick()
{
    // 触发TileView的Item点击事件
    if (ParentTileView && ListItemObject)
    {
        ParentTileView->OnItemClicked().Broadcast(ListItemObject);
    }
}
