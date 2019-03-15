// © Ajit D'Monte 2019

#include "fBlahBlueprintFunctionLibrary.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "Runtime/Engine/Classes/Animation/SkeletalMeshActor.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"

TArray<FString> UfBlahBlueprintFunctionLibrary::GetCharacterMorphs(USkeletalMeshComponent* mesh) {
	TArray<FString> outputArray;
	USkeletalMesh * skelMesh = mesh->SkeletalMesh;
	TMap<FName, int32> myMap = skelMesh->MorphTargetIndexMap;

	for (TPair<FName, int32> Entry : myMap) {
		outputArray.Add(Entry.Key.ToString());
	}

	return outputArray;
}

void UfBlahBlueprintFunctionLibrary::SetCharacterMorphTarget(USkeletalMeshComponent* SkeletalMeshComponent, FName TargetName, float Weight) {
	if (SkeletalMeshComponent != nullptr)
	{
		TMap <FName, float> MorphTargetCurves;
		MorphTargetCurves.Add(TargetName, Weight);
		USkeletalMesh *InSkeletalMesh = SkeletalMeshComponent->SkeletalMesh;
		SkeletalMeshComponent->MorphTargetWeights.SetNumZeroed(InSkeletalMesh->MorphTargets.Num());
		if (FMath::Abs(Weight) > SMALL_NUMBER)
		{
			int32 MorphId = INDEX_NONE;
			UMorphTarget* Target = InSkeletalMesh ? InSkeletalMesh->FindMorphTargetAndIndex(TargetName, MorphId) : nullptr;
			if (Target != nullptr)
			{
				int32 MorphIndex = INDEX_NONE;
				for (int32 i = 0; i < SkeletalMeshComponent->ActiveMorphTargets.Num(); i++)
				{
					if (SkeletalMeshComponent->ActiveMorphTargets[i].MorphTarget == Target)
					{
						MorphIndex = i;
					}
				}
				if (MorphIndex == INDEX_NONE)
				{
					SkeletalMeshComponent->ActiveMorphTargets.Add(FActiveMorphTarget(Target, MorphId));
					SkeletalMeshComponent->MorphTargetWeights[MorphId] = Weight;
				}
				else
				{
					check(MorphId == SkeletalMeshComponent->ActiveMorphTargets[MorphIndex].WeightIndex);
					float& CurrentWeight = SkeletalMeshComponent->MorphTargetWeights[MorphId];
					CurrentWeight = FMath::Max<float>(CurrentWeight, Weight);
				}
			}
		}
	}

}

void UfBlahBlueprintFunctionLibrary::UpdateMorphTarget(USkeletalMeshComponent* SkeletalMeshComponent)
{
	if (SkeletalMeshComponent != nullptr && !SkeletalMeshComponent->GetWorld()->IsGameWorld())
	{
		SkeletalMeshComponent->TickAnimation(0.f, false);
		SkeletalMeshComponent->RefreshBoneTransforms();
		SkeletalMeshComponent->RefreshSlaveComponents();
		SkeletalMeshComponent->UpdateComponentToWorld();
		SkeletalMeshComponent->FinalizeBoneTransform();
		SkeletalMeshComponent->MarkRenderTransformDirty();
		SkeletalMeshComponent->MarkRenderDynamicDataDirty();
	}
}

void UfBlahBlueprintFunctionLibrary::String__ExplodeString(TArray<FString>& OutputStrings, FString InputString, FString Separator, int32 limit, bool bTrimElements)
{
	OutputStrings.Empty();
	//~~~~~~~~~~~

	if (InputString.Len() > 0 && Separator.Len() > 0) {
		int32 StringIndex = 0;
		int32 SeparatorIndex = 0;

		FString Section = "";
		FString Extra = "";

		int32 PartialMatchStart = -1;

		while (StringIndex < InputString.Len()) {

			if (InputString[StringIndex] == Separator[SeparatorIndex]) {
				if (SeparatorIndex == 0) {
					//A new partial match has started.
					PartialMatchStart = StringIndex;
				}
				Extra.AppendChar(InputString[StringIndex]);
				if (SeparatorIndex == (Separator.Len() - 1)) {
					//We have matched the entire separator.
					SeparatorIndex = 0;
					PartialMatchStart = -1;
					if (bTrimElements == true) {
						OutputStrings.Add(FString(Section).TrimStart().TrimEnd());
					}
					else {
						OutputStrings.Add(FString(Section));
					}

					//if we have reached the limit, stop.
					if (limit > 0 && OutputStrings.Num() >= limit)
					{
						return;
						//~~~~
					}

					Extra.Empty();
					Section.Empty();
				}
				else {
					++SeparatorIndex;
				}
			}
			else {
				//Not matched.
				//We should revert back to PartialMatchStart+1 (if there was a partial match) and clear away extra.
				if (PartialMatchStart >= 0) {
					StringIndex = PartialMatchStart;
					PartialMatchStart = -1;
					Extra.Empty();
					SeparatorIndex = 0;
				}
				Section.AppendChar(InputString[StringIndex]);
			}

			++StringIndex;
		}

		//If there is anything left in Section or Extra. They should be added as a new entry.
		if (bTrimElements == true) {
			OutputStrings.Add(FString(Section + Extra).TrimStart().TrimEnd());
		}
		else {
			OutputStrings.Add(FString(Section + Extra));
		}

		Section.Empty();
		Extra.Empty();
	}
}