// © Ajit D'Monte 2019

#include "fBlahBlueprintFunctionLibrary.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "Runtime/Engine/Classes/Animation/SkeletalMeshActor.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Misc/FileHelper.h"

#if WITH_EDITOR	
#include "AssetToolsModule.h"
#endif 	

DEFINE_LOG_CATEGORY(LogfBlah);

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
#if WITH_EDITOR	
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");

	//AssetToolsModule.Get().ExportAssetsWithDialog(ObjectsToExport, true);
	AssetToolsModule.Get().ExportAssets(ObjectsToExport, ExportPath);
#endif
}

bool UfBlahBlueprintFunctionLibrary::IsInEditor(AActor* Actor)
{
	return Actor->GetWorld()->WorldType == EWorldType::Type::Editor;

}

ULevelSequence * UfBlahBlueprintFunctionLibrary::GetCurrentSequence()
{
#if WITH_EDITOR
	TArray <UObject*> Objects = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->GetAllEditedAssets();
	for (UObject *Obj : Objects)
	{
		ULevelSequence * CurrentSequence = Cast<ULevelSequence>(Obj);
		if (CurrentSequence)
		{		
			return CurrentSequence;
		}
	}
#endif WITH_EDITOR
	return nullptr;
}

bool UfBlahBlueprintFunctionLibrary::RenameActorComponent(UActorComponent *ActorComponent, FString NewName)
{
	auto components = ActorComponent->GetOwner()->GetComponents();
	for (auto component : components)
	{
		if ((component->GetFName()).ToString().Equals(NewName, ESearchCase::IgnoreCase))
		{
			return false;
		}
	}	
	return ActorComponent->Rename(*NewName, nullptr, 0);
}

FGuid UfBlahBlueprintFunctionLibrary::AddBinding(ULevelSequence * LevelSequence, UObject *Object)
{
	FGuid Guid;
#if WITH_EDITOR	
	if (LevelSequence && Object)
	{
		for (auto b : LevelSequence->MovieScene->GetBindings())
		{
			FGuid ObjectGuid = b.GetObjectGuid();
			if (ObjectGuid.IsValid())
			{
				TArray<UObject*, TInlineAllocator<1>> Objects;
				LevelSequence->LocateBoundObjects(ObjectGuid, GWorld, Objects);
				if (Objects.Num())
				{
					if (Object == Objects[0])
					{
						Guid = ObjectGuid;
						return Guid;
					}					
				}
			}
		}
		if (Object->IsA(UActorComponent::StaticClass())) 
		{
			FString Left, Right;
			UKismetSystemLibrary::GetDisplayName(Object).Split(TEXT("."), &Left, &Right);
			Guid = LevelSequence->MovieScene->AddPossessable(UKismetSystemLibrary::GetDisplayName(Object), Object->GetClass());
			auto owner = Cast<UActorComponent>(Object)->GetOwner();
			FGuid OwnerGuid;
			for (auto b : LevelSequence->MovieScene->GetBindings())
			{
				FGuid ObjectGuid = b.GetObjectGuid();
				if (ObjectGuid.IsValid())
				{
					TArray<UObject*, TInlineAllocator<1>> Objects;
					LevelSequence->LocateBoundObjects(ObjectGuid, GWorld, Objects);
					if (Objects.Num())
					{
						if (owner == Objects[0])
						{
							OwnerGuid = ObjectGuid;							
						}
					}
				}
			}
			//LevelSequence->MovieScene->FindPossessable(Guid)->SetParent(OwnerGuid);			
			LevelSequence->BindPossessableObject(Guid, *Object, Object->GetWorld());

			TRangeBound <FFrameNumber> OpenBound;
			OpenBound.Open();

			/*
			FName Value("Percentage");
			UMovieSceneTrack * Track = LevelSequence->MovieScene->AddTrack(UMovieSceneFloatTrack::StaticClass(), Guid);
			UMovieSceneFloatTrack* ValueTrack = CastChecked<UMovieSceneFloatTrack>(Track);
			ValueTrack->SetPropertyNameAndPath(Value, Value.ToString());
			UMovieSceneFloatSection* ValueSection = CastChecked<UMovieSceneFloatSection>(ValueTrack->CreateNewSection());
			ValueTrack->AddSection(*ValueSection);

			ValueSection->SetStartFrame(OpenBound);
			ValueSection->SetEndFrame(OpenBound);
			*/
			

			FName Op("FaceOperationString");						

			UMovieSceneTrack * EnumTrack = LevelSequence->MovieScene->AddTrack(UMovieSceneStringTrack::StaticClass(), Guid);
			UMovieSceneStringTrack* OpTrack = CastChecked<UMovieSceneStringTrack>(EnumTrack);			
			OpTrack->SetPropertyNameAndPath(Op, Op.ToString());
			UMovieSceneStringSection* OpSection = CastChecked<UMovieSceneStringSection>(OpTrack->CreateNewSection());
			OpTrack->AddSection(*OpSection);
			
			OpSection->SetStartFrame(OpenBound);
			OpSection->SetEndFrame(OpenBound);		
		}
		else 
		{
			Guid = LevelSequence->MovieScene->AddPossessable(UKismetSystemLibrary::GetDisplayName(Object), Object->GetClass());
			LevelSequence->BindPossessableObject(Guid, *Object, Object->GetWorld());
			return Guid;
		}
	}

#endif // WITH_EDITOR
	return Guid;
}

void UfBlahBlueprintFunctionLibrary::ReOpenEditorAssetTab(UObject* Asset)
{
#if WITH_EDITOR
	if (Asset) FAssetEditorManager::Get().CloseAllEditorsForAsset(Asset);
	if (Asset) FAssetEditorManager::Get().OpenEditorForAsset(Asset);
#endif
}

bool UfBlahBlueprintFunctionLibrary::SetStringKey(ULevelSequence* LevelSequence, UObject* Object, FString Value, float Time, const FName Name = FName("FaceOperationString"))
{
#if WITH_EDITOR
	if (LevelSequence && Object)
	{			
		FFrameRate DisplayRate = LevelSequence->GetMovieScene()->GetDisplayRate();		
		float Fps = DisplayRate.AsDecimal();
		
		FGuid Guid;
		if (FindBinding(LevelSequence, Object, Guid))
		{					
			UMovieSceneStringTrack* MovieSceneTrack = LevelSequence->MovieScene->FindTrack<UMovieSceneStringTrack>(Guid, Name);
			
			if (MovieSceneTrack)
			{
				TArray <UMovieSceneSection*> MovieSceneSections = MovieSceneTrack->GetAllSections();
				if (MovieSceneSections.Num())
				{	
					UMovieSceneStringSection * StringSection = nullptr;
					StringSection = CastChecked<UMovieSceneStringSection>(MovieSceneSections[0]);					
					if (StringSection)
					{							
						if (StringSection->TryModify(true)) 
						{							
							TArrayView <FMovieSceneStringChannel*> Channels = StringSection->GetChannelProxy().GetChannels<FMovieSceneStringChannel>();
							FFrameNumber Frame;
							Frame.Value = Time * 1000;
							Frame++;
							Channels[0]->GetData().UpdateOrAddKey(Frame, Value);
							//UE_LOG(LogfBlah, Warning, TEXT("Frame Number: %d, %s"), Frame.Value, *Value);
													
							//UE_LOG(LogfBlah, Warning, TEXT("Const %s"), (isConst(Data) ? TEXT("True") : TEXT("False")));							
						}											
						return true;
					}
				}
			}
		}		
	}
#endif	
	return false;
}

bool UfBlahBlueprintFunctionLibrary::SetFloatKey(ULevelSequence* LevelSequence, UObject* Object, float Value, float Time, const FName Name = FName("FaceOperationString"))
{
#if WITH_EDITOR
	if (LevelSequence && Object)
	{
		FGuid Guid;
		if (FindBinding(LevelSequence, Object, Guid))
		{
			UMovieSceneFloatTrack* MovieSceneTrack = LevelSequence->MovieScene->FindTrack<UMovieSceneFloatTrack>(Guid, Name);

			if (MovieSceneTrack)
			{
				TArray <UMovieSceneSection*> MovieSceneSections = MovieSceneTrack->GetAllSections();
				if (MovieSceneSections.Num())
				{
					UMovieSceneFloatSection * FloatSection = nullptr;
					FloatSection = CastChecked<UMovieSceneFloatSection>(MovieSceneSections[0]);
					if (FloatSection)
					{
						if (FloatSection->TryModify(true))
						{
							TArrayView <FMovieSceneFloatChannel*> Channels = FloatSection->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>();
							FFrameNumber Frame;
							Frame.Value = Time * 1000;
							Frame++;
							Channels[0]->GetData().UpdateOrAddKey(Frame, FMovieSceneFloatValue(Value));
						}
						return true;
					}
				}
			}
		}
	}
#endif	
	return false;
}


bool UfBlahBlueprintFunctionLibrary::GetStringKeys(ULevelSequence* LevelSequence, UObject* Object, TArray<FString> & Values, TArray<float> &Times, const FName Name = FName("FaceOperationString"))
{
#if WITH_EDITOR	
	if (LevelSequence && Object)
	{		
		FGuid Guid;
		if (FindBinding(LevelSequence, Object, Guid))
		{
			UMovieSceneStringTrack* MovieSceneTrack = LevelSequence->MovieScene->FindTrack<UMovieSceneStringTrack>(Guid, Name);

			if (MovieSceneTrack)
			{
				TArray <UMovieSceneSection*> MovieSceneSections = MovieSceneTrack->GetAllSections();
				if (MovieSceneSections.Num())
				{
					UMovieSceneStringSection * StringSection = nullptr;
					StringSection = CastChecked<UMovieSceneStringSection>(MovieSceneSections[0]);
					if (StringSection)
					{
						if (StringSection->TryModify(true))
						{
							TArrayView <FMovieSceneStringChannel*> Channels = StringSection->GetChannelProxy().GetChannels<FMovieSceneStringChannel>();							
							
							TArray <FFrameNumber> TimesTAV;
							TArray <FKeyHandle> KeyHandles;
							//TArray <const FString *> Strings;
							const TRange <FFrameNumber> Range = LevelSequence->MovieScene->GetPlaybackRange();
							Channels[0]->GetKeys(Range, &TimesTAV, &KeyHandles);
							for (int32 i = 0; i < TimesTAV.Num(); i++)
							{
								Times.Add(TimesTAV[i].Value);
								FString * temp = const_cast<FString *>(Channels[0]->Evaluate(TimesTAV[i]));
								Values.Add(*temp);
							}
						}
						return true;
					}
				}
			}
		}
	}
#endif	
	return false;
}

bool UfBlahBlueprintFunctionLibrary::FindBinding(ULevelSequence *LevelSequence, UObject *Object, FGuid & Guid)
{
	if (LevelSequence && Object)
	{
		for (auto b : LevelSequence->MovieScene->GetBindings())
		{
			FGuid ObjectGuid = b.GetObjectGuid();
			if (ObjectGuid.IsValid())
			{
				TArray<UObject*, TInlineAllocator<1>> Objects;
				LevelSequence->LocateBoundObjects(ObjectGuid, GWorld, Objects);
				if (Objects.Num())
				{
					if (Object == Objects[0])
					{
						Guid = ObjectGuid;
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool UfBlahBlueprintFunctionLibrary::FileSaveString(FString SaveTextB, FString FileNameB)
{
	return FFileHelper::SaveStringToFile(SaveTextB, *(FileNameB));
}

bool UfBlahBlueprintFunctionLibrary::FileLoadString(FString FileNameA, FString& SaveTextA)
{
	return FFileHelper::LoadFileToString(SaveTextA, *(FileNameA));
}