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

	/**
	*
	* from RAMA VictoryBP
	* Contributed by: Mindfane 
	*
	* Split a string into an array of substrings based on the given delimitter.
	* Unlike ParseIntoArray() function which expects single character delimitters, this function can accept a delimitter that is also a string.
	*
	* @param InputString - The string that is to be exploded.
	* @param Separator - The delimitter that is used for splitting (multi character strings are allowed)
	* @param limit - If greater than zero, returns only the first x strings. Otherwsie returns all the substrings
	* @param bTrimElelements - If True, then each subsctring is processed and any leading or trailing whitespcaes are trimmed.
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Victory BP Library|String", Keywords = "split explode string"), Category = String)
		static void String__ExplodeString(TArray<FString>& OutputStrings, FString InputString, FString Separator = ",", int32 limit = 0, bool bTrimElements = false);
	
	UFUNCTION(BlueprintCallable, Category = "fBlah's Blueprints", meta = (DisplayName = "Export Assets", Keywords = "Export asset objects"))
		static void ExportAssets(TArray<UObject*> ObjectsToExport, const FString ExportPath);

	UFUNCTION(BlueprintPure, Category = "fBlah's Blueprints", meta = (DisplayName = "In Editor?", Keywords = "Is in editor?"))
		static bool IsInEditor(AActor *Actor);
};
