// Solution_BMJ


#include "CharacterDamageDA.h"

float UCharacterDamageDA::GetDamageRate(int32 Index)
{
    const float* DamageRatePtr = DamageDataMap.Find(Index);
    return DamageRatePtr ? *DamageRatePtr : 1.0f; // 找不到段ID时默认倍率
}
