// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "OAAnimNotify_Respawn.generated.h"

/**
 * AnimNotify that respawns the owning character at its start location.
 * Place this at the end of a Death montage.
 */
UCLASS(DisplayName = "OA Respawn")
class UOAAnimNotify_Respawn : public UAnimNotify
{
	GENERATED_BODY()

public:

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};
