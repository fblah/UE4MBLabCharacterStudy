// © Ajit D'Monte 2019

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#if WITH_EDITOR	
#include "Editor/UnrealEd/Public/Toolkits/AssetEditorManager.h"
#include "Editor/MovieSceneTools/Public/KeyframeTrackEditor.h"
#endif

#include "Runtime/Engine/Classes/Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"

#include "LevelSequence.h"
#include "MovieScene.h"

#include "MovieSceneStringSection.h"
#include "MovieSceneStringTrack.h"


DECLARE_LOG_CATEGORY_EXTERN(LogfBlah, Log, All);

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
			
	UFUNCTION(BlueprintCallable, Category = "fBlah's Blueprints", meta = (DisplayName = "Export Assets", Keywords = "Export asset objects"))
		static void ExportAssets(TArray<UObject*> ObjectsToExport, const FString ExportPath);

	UFUNCTION(BlueprintPure, Category = "fBlah's Blueprints", meta = (DisplayName = "In Editor?", Keywords = "Is in editor?"))
		static bool IsInEditor(AActor *Actor);

	UFUNCTION(BlueprintPure, Category = "fBlah's Blueprints", meta = (DisplayName = "Get Sequencer", Keywords = "Get current Sequence"))
		static ULevelSequence * GetCurrentSequence();

	UFUNCTION(BlueprintPure, Category = "fBlah's Blueprints", meta = (DisplayName = "Rename Actor Component", Keywords = "Rename Actor Component"))
		static bool RenameActorComponent(UActorComponent *ActorComponent, FString NewName);

	UFUNCTION(BlueprintCallable, Category = "fBlah's Blueprints", meta = (DisplayName = "Add Binding", Keywords = "Add Binding"))
		static FGuid AddBinding(ULevelSequence *LevelSequence, UObject *Object);

	UFUNCTION(BlueprintCallable, Category = "fBlah's Blueprints", meta = (DisplayName = "Reopen Editor Tab for Asset", Keywords = "Reopen Editor Tab for Asset"))
		static void ReOpenEditorAssetTab(UObject* Asset);

	UFUNCTION(BlueprintCallable, Category = "fBlah's Blueprints", meta = (DisplayName = "Set String Key in Sequencer", Keywords = "Set String key in sequencer"))
		static bool SetStringKey(ULevelSequence* LevelSequence, UObject* Object, FString value, float Time, const FName Name);

	UFUNCTION(BlueprintCallable, Category = "fBlah's Blueprints", meta = (DisplayName = "Get String Keys from Sequencer", Keywords = "Get String keys from sequencer"))
		static bool GetStringKeys(ULevelSequence* LevelSequence, UObject* Object, TArray<FString> & Values, TArray<float> &Times, const FName Name);
	
	UFUNCTION(BlueprintCallable, Category = "fBlah's Blueprints", meta = (DisplayName = "Save String to File", Keywords = "Save String to File"))
		static bool FileSaveString(FString SaveTextB, FString FileNameB);

	UFUNCTION(BlueprintPure, Category = "fBlah's Blueprints", meta = (DisplayName = "Load String from File", Keywords = "Load String from File"))
		static bool FileLoadString(FString FileNameA, FString& SaveTextA);

private:
	static bool FindBinding(ULevelSequence *LevelSequence, UObject *Object, FGuid & Guid);
	
	template <typename T> static bool isConst(T& x)
	{
		return false;
	}

	template <typename T> static bool isConst(T const& x)
	{
		return true;
	}

};
