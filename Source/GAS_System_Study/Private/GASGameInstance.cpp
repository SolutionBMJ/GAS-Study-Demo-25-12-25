// Solution_BMJ


#include "GASGameInstance.h"
#include "AccountData.h"

UAccountRegisterAndReader* UGASGameInstance::GetAccountRegisterAndReader()
{
	return UAccountRegisterAndReader::GetInstance();
}

UEquipmentManager* UGASGameInstance::GetEquipmentManager()
{
	return CurrentLoginAccount ? CurrentLoginAccount->EquipmentManager : nullptr;
}
