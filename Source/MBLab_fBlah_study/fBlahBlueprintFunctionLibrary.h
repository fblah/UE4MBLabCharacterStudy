// © Ajit D'Monte 2019

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "fBlahBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class MBLAB_FBLAH_STUDY_API UfBlahBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "fBlah's Blueprints", meta = (DisplayName = "Get Morph Target Names", CompactNodeTitle = "Get MT Names", Keywords = "GetMorphTargets"))
		static 	TArray<FString> GetCharacterMorphs(USkeletalMeshComponent* mesh);

	UFUNCTION(BlueprintCallable, Category = "fBlah's Blueprints", meta = (DisplayName = "Set Morph Target", CompactNodeTitle = "Set MT Weight", Keywords = "SetMorphTargets"))
		static void SetCharacterMorphTarget(USkeletalMeshComponent* SkeletalMeshComponent, FName TargetName, float Weight);

	UFUNCTION(BlueprintCallable, Category = "fBlah's Blueprints", meta = (DisplayName = "Update Morph Target", CompactNodeTitle = "Update MT Weight", Keywords = "UpdateMorphTargets"))
		static void UpdateMorphTarget(USkeletalMeshComponent* SkeletalMeshComponent);
};
