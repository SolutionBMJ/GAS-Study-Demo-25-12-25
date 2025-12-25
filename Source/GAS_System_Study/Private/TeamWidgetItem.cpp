// Solution_BMJ


#include "TeamWidgetItem.h"
#include "Components/Button.h"
#include "CPP_PlayerController.h"
#include "CPP_PlayerCharacter.h"
#include "TeamMakerWidget.h"


void UTeamWidgetItem::NativeOnListItemObjectSet(UObject* InListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(InListItemObject);

	CurCharacter = Cast<ACPP_PlayerCharacter>(InListItemObject);
	Select->OnClicked.AddDynamic(this, &UTeamWidgetItem::SelectCharacter);

}

void UTeamWidgetItem::SelectCharacter()
{
	if (!CurCharacter) return;
	ACPP_PlayerController* PC = Cast<ACPP_PlayerController>(GetOwningPlayer());
	if (PC && PC->TeamMakerUI)
	{
		PC->TeamMakerUI->ChangeSelectCharacter(CurCharacter->CharacterName);
	}
	if (PC && PC->CharacterDetailUI)
	{
		PC->CharacterDetailUI->SpawnDisplayCharacterAtAnchor(CurCharacter->CharacterName);
	}
}
