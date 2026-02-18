// Copyright Epic Games, Inc. All Rights Reserved.

#include "OAAnimNotify_Respawn.h"
#include "Obstacle_AvoidanceCharacter.h"

FString UOAAnimNotify_Respawn::GetNotifyName_Implementation() const
{
	return TEXT("OA Respawn");
}

void UOAAnimNotify_Respawn::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	AObstacle_AvoidanceCharacter* Character = Cast<AObstacle_AvoidanceCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		Character->Respawn();
	}
}
