// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "ShooterAbilityTypes.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "GameplayTagsModule.h"
#include "UI/Menu/Widgets/SShooterPawnGuideWidget.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ShooterBlueprintLibrary.generated.h"

/**
 * Game-specific blueprint library
 * Most games will need to implement one or more blueprint function libraries to expose their native code to blueprints
 */
UCLASS()
class SHOOTERGAME_API UShooterBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UShooterBlueprintLibrary(const FObjectInitializer& ObjectInitializer);

	/** Show the native loading screen, such as on a map transfer. If bPlayUntilStopped is false, it will be displayed for PlayTime and automatically stop */
	UFUNCTION(BlueprintCallable, Category = Loading)
	static void PlayLoadingScreen(bool bPlayUntilStopped, float PlayTime);

	/** Turns off the native loading screen if it is visible. This must be called if bPlayUntilStopped was true */
	UFUNCTION(BlueprintCallable, Category = Loading)
	static void StopLoadingScreen();

	/** Returns true if this is being run from an editor preview */
	UFUNCTION(BlueprintPure, Category = Loading)
	static bool IsInEditor();

	/** Equality operator for ItemSlot */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (ShooterItemSlot)", CompactNodeTitle = "==", Keywords = "== equal"), Category = Inventory)
	static bool EqualEqual_ItemSlot(const FShooterItemSlot& A, const FShooterItemSlot& B);

	/** Inequality operator for ItemSlot */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "NotEqualEqual (ShooterItemSlot)", CompactNodeTitle = "!=", Keywords = "!= not equal"), Category = Inventory)
	static bool NotEqual_ItemSlot(const FShooterItemSlot& A, const FShooterItemSlot& B);

	/** Validity check for ItemSlot */
	UFUNCTION(BlueprintPure, Category = Inventory)
	static bool IsValidItemSlot(const FShooterItemSlot& ItemSlot);

	/** Checks if spec has any effects */
	UFUNCTION(BlueprintPure, Category = Ability)
	static bool DoesEffectContainerSpecHaveEffects(const FShooterGameplayEffectContainerSpec& ContainerSpec);

	/** Checks if spec has any targets */
	UFUNCTION(BlueprintPure, Category = Ability)
	static bool DoesEffectContainerSpecHaveTargets(const FShooterGameplayEffectContainerSpec& ContainerSpec);

	/** Adds targets to a copy of the passed in effect container spec and returns it */
	UFUNCTION(BlueprintCallable, Category = Ability, meta = (AutoCreateRefTerm = "HitResults,TargetActors"))
	static FShooterGameplayEffectContainerSpec AddTargetsToEffectContainerSpec(const FShooterGameplayEffectContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);

	/** Applies container spec that was made from an ability */
	UFUNCTION(BlueprintCallable, Category = Ability)
	static TArray<FActiveGameplayEffectHandle> ApplyExternalEffectContainerSpec(const FShooterGameplayEffectContainerSpec& ContainerSpec);

	/** 获取所有PlayerController列表，然后再找到本地客户端的第一个PlayerController并返回  */
	UFUNCTION(BlueprintCallable, Category = PlayerController)
	static APlayerController* GetLocalPlayerController(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = RegexHelper)
	static bool IsValidIP(const FString& Value, const FString Reg);

	UFUNCTION(BlueprintCallable, Category = LineTraceHelper)
	static void DrawDebugLineTraceSingle(const UWorld* World, const FVector& Start, const FVector& End, EDrawDebugTrace::Type DrawDebugType, bool bHit, const FHitResult& OutHit, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime);

	/** 
	* 根据字符串获取Gameplay Tag 
	*
	* @param TagName  name字符串
	*/
	UFUNCTION()
	static FGameplayTag GetGameplayTag(const FString& TagName);
	
	/**
	* Actor生成模板方法：根据不同的PropsType在世界内生成不同的Actor实例
	*
	* @param World					当前世界实例
	* @param InClass				要生成的实例的类；一般指TSubclassOf<AActor>变量的AActor类
	* @param Loc					生成的实例在世界中的位置-Location
	* @param Rot					生成的实例在世界中的方向-Rotation
	* @param bNoCollisionFail		是否包含Collision
	*/
	template <typename PropsType>
	static FORCEINLINE PropsType* SpawnActor(UWorld* World, UClass* InClass, const FVector& Loc, const FRotator& Rot, AActor* Owner = NULL, APawn* Instigator = NULL) 
	{
		if (!World) return NULL;
		if (!InClass) return NULL;

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Owner = Owner;
		SpawnInfo.Instigator = Instigator;
		SpawnInfo.bDeferConstruction = false;

		return World->SpawnActor<PropsType>(InClass, Loc, Rot, SpawnInfo);
	}

	/**
	* Actor快速生成模板方法：根据不同的PropsType在世界内生成不同的Actor实例
	*
	* @param World					当前世界实例
	* @param InClass				要生成的实例的类；一般指TSubclassOf<AActor>变量的AActor类
	* @param SpawnParameters		该实例生成在世界中时包含的一些信息参数-如方式、所属对象、损害等
	*/
	template <typename PropsType>
	static FORCEINLINE PropsType* SpawnActor(UWorld* World, UClass* InClass, const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters())
	{
		if (!World) return NULL;
		if (!InClass) return NULL;

		return World->SpawnActor<PropsType>(InClass, SpawnParameters);
	}

	template< class UserClass >
	static FORCEINLINE void PlaySoundAndCall(UWorld* World, const FSlateSound& Sound, int32 UserIndex, UserClass* inObj, typename SShooterPawnGuideWidget::FOnBeginPlayMenu::TSPMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		FSlateApplication::Get().PlaySound(Sound, UserIndex);
		if (World)
		{
			const float SoundDuration = FMath::Max(FSlateApplication::Get().GetSoundDuration(Sound), 0.1f);
			FTimerHandle DummyHandle;
			World->GetTimerManager().SetTimer(DummyHandle, FTimerDelegate::CreateSP(inObj, inMethod), SoundDuration, false);
		}
		else
		{
			FTimerDelegate D = FTimerDelegate::CreateSP(inObj, inMethod);
			D.ExecuteIfBound();
		}
	}

	////////////////////////// Asset File //////////////////////////

	/**
	*  验证 or 创建文件
	*/
	static FORCEINLINE bool VerifyOrCreateDirectory(const FString& Path)
	{
		// Every function call, unless the function is inline, adds a small
		// overhead which we can avoid by creating a local variable like so.
		// But beware of making every function inline!
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

		// Directory Exists?
		if (!PlatformFile.DirectoryExists(*Path))
		{
			PlatformFile.CreateDirectory(*Path);

			if (!PlatformFile.DirectoryExists(*Path))
			{
				return false;
			}
		}
		return true;
	}

	/**
	*  验证 or 创建文件
	*/
	static FORCEINLINE int64 GetFileSize(const FString& Path)
	{
		if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*Path))
		{
			return 0;
		}
		return FPlatformFileManager::Get().GetPlatformFile().FileSize(*Path);
	}

	/**
	*  移动 or 重命名文件
	*/
	static FORCEINLINE bool MoveAndRenameFile(const FString& OriginalPath, const FString& NewPath)
	{
		if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*OriginalPath))
		{
			return false;
		}
		return FPlatformFileManager::Get().GetPlatformFile().MoveFile(*NewPath, *OriginalPath);
	}

	/**
	*  Del 文件
	*/
	static FORCEINLINE bool RemoveFile(const FString& Path)
	{
		if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*Path))
		{
			return false;
		}
		return FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*Path);
	}
	
	/**
	*  字符串写入到文本文件
	*/
	static FORCEINLINE bool SaveStringToFile(const FString& SaveDirectory, const FString& FileName, const FString& Value, bool AllowOverwriting = false)
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

		// CreateDirectoryTree returns true if the destination
		// directory existed prior to call or has been created
		// during the call.
		if (PlatformFile.CreateDirectoryTree(*SaveDirectory))
		{
			// Get file path
			FString FilePath = SaveDirectory + "/" + FileName;

			// Allow overwriting or file doesn't already exist
			if (AllowOverwriting || !PlatformFile.FileExists(*FilePath))
			{
				FFileHelper::SaveStringToFile(Value, *FilePath);
			}
		}
		return false;
	}

	/**
	*  字符串写入二进制文件
	*/
	static FORCEINLINE void WriteToBinaryFile(const FString& SaveDirectory, const FString& FileName, const FString& Value, bool AllowOverwriting = false)
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

		// Get file path
		FString FilePath = SaveDirectory + "/" + FileName;

		IFileHandle* FileHandle = PlatformFile.OpenWrite(*FilePath);
		if (FileHandle)
		{
			//FString Guid = FString(TEXT("// This file is written to disk\n")TEXT("// GUID = ")) + FGuid::NewGuid().ToString();

			FileHandle->Write((const uint8*)TCHAR_TO_ANSI(*Value), Value.Len());

			delete FileHandle;
		}
	}

	/**
	*  读取二进制文件
	*  注： UE4函数仅支持uint8 *的数组
	*/
	static FORCEINLINE void ReadFromBinaryFile(const FString& SaveDirectory, const FString& FileName, int32 Length)
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

		// Get file path
		FString FilePath = SaveDirectory + "/" + FileName;

		IFileHandle* FileHandle = PlatformFile.OpenWrite(*FilePath);
		if (FileHandle)
		{
			// Create a pointer to MyInteger
			int32* IntPointer = &Length;
			// Reinterpret the pointer for the Read function
			uint8* ByteBuffer = reinterpret_cast<uint8*>(IntPointer);

			// Read the integer from file into our reinterpret pointer
			FileHandle->Read(ByteBuffer, sizeof(int32));

			delete FileHandle;
		}
	}

	/**
	*  相对路径的文件中读取字符串
	*/
	static FORCEINLINE bool LoadStringFromFile(const FString& Directory, const FString& FileName, FString& ResultStr)
	{
		if (!FileName.IsEmpty())
		{
			//获取绝对路径
			FString AbsoPath = FPaths::ProjectContentDir() + Directory + FileName;
			//判断文件是否存在
			if (FPaths::FileExists(AbsoPath))
			{
				if (FFileHelper::LoadFileToString(ResultStr, *AbsoPath))
				{
					return true;
				}
				else
				{
					//加载不成功
					return false;
				}
			}
			else
			{
				//输出文件不存在
				return false;
			}
		}
		return false;
	}

	/**
	*  读取JSON文件
	*/
	static FORCEINLINE bool LoadJSONFromFile(const FString& Directory, const FString& FileName, TSharedPtr<FJsonObject>& JsonObj)
	{
		FString JsonStr;
		bool bSucceed = LoadStringFromFile(Directory, FileName, JsonStr);
		if (bSucceed)
		{
			TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonStr);
			return FJsonSerializer::Deserialize(JsonReader, JsonObj);
		}
		return bSucceed;
	}

	/**
	*  JSON数据写入文件
	*/
	static FORCEINLINE bool WriteJsonToFile(const FString& SaveDirectory, const FString& FileName, const TSharedPtr<FJsonObject>& JsonObj, FString& JsonStr)
	{
		if (JsonObj.IsValid() && JsonObj->Values.Num() > 0)
		{
			TSharedRef<TJsonWriter<TCHAR>> JsonWrite = TJsonWriterFactory<TCHAR>::Create(&JsonStr);
			bool bSerialize = FJsonSerializer::Serialize(JsonObj.ToSharedRef(), JsonWrite);
			if (bSerialize)
			{
				return SaveStringToFile(SaveDirectory, FileName, JsonStr);
			}
		}
		return false;
	}

	/**
	*  读取DataTable文件
	*/
	static FORCEINLINE bool LoadDataFromTable(const FString& Directory, const FString& TableName, UDataTable& DataSource)
	{

		return false;
	}

	/**
	*  FStreamableManager 异步加载资源
	*/
	static FORCEINLINE bool AsyncLoadAssetData(const FString& Directory, const FString& AssetName)
	{

		return false;
	}

	/**
	*  FStreamableManager 同加载资源
	*/
	static FORCEINLINE bool SyncLoadAssetData(const FString& Directory, const FString& AssetName)
	{

		return false;
	}

};


