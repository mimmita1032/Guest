#include "GItemDefinition.h"

template <typename T>
const T* UGItemDefinition::FindFragmentByClass() const
{
	for (const auto& Fragment : Fragments)
	{
		if (T* FoundFragment = Cast<T>(Fragment)) return FoundFragment;
	}
	return nullptr;
}
