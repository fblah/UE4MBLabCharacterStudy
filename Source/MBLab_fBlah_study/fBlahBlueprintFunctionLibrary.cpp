// © Ajit D'Monte 2019

#include "fBlahBlueprintFunctionLibrary.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "Runtime/Engine/Classes/Animation/SkeletalMeshActor.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"

#include "AssetToolsModule.h"

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

void UfBlahBlueprintFunctionLibrary::ExportAssets(TArray<UObject*> ObjectsToExport, const FString ExportPath)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");

	//AssetToolsModule.Get().ExportAssetsWithDialog(ObjectsToExport, true);
	AssetToolsModule.Get().ExportAssets(ObjectsToExport, ExportPath);

}

bool UfBlahBlueprintFunctionLibrary::IsInEditor(AActor* Actor)
{
	return Actor->GetWorld()->WorldType == EWorldType::Type::Editor;

}