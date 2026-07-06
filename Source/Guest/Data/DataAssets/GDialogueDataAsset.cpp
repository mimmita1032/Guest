// Copyright (c) 2026 Anything Left Behind?. All rights reserved.

#include "Guest/Data/DataAssets/GDialogueDataAsset.h"

const FDialogueNode* UGDialogueDataAsset::FindNode(FName NodeID) const
{
	if (!DialogueTable || NodeID.IsNone()) return nullptr;
	return DialogueTable->FindRow<FDialogueNode>(NodeID, TEXT(""));
}
