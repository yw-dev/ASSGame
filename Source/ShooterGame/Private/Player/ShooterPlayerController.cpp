// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "ShooterTypes.h"
#include "ShooterAssetManager.h"
#include "ShooterBlueprintLibrary.h"
#include "Items/ShooterItem.h"
#include "Player/ShooterPlayerController.h"
#include "Player/ShooterPlayerCameraManager.h"
#include "Player/ShooterCheatManager.h"
#include "Player/ShooterLocalPlayer.h"
#include "Online/ShooterPlayerState.h"
#include "Weapons/ShooterWeaponBase.h"
#include "Weapons/ShooterRangedWeapon.h"
#include "Weapons/ShooterMeleeWeapon.h"
#include "UI/Menu/ShooterIngameMenu.h"
#include "UI/Menu/ShooterShopMenu.h"
#include "UI/Style/ShooterStyle.h"
#include "UI/ShooterHUD.h"
#include "UI/ShooterLoginHUD.h"
#include "UI/ShooterMenuHUD.h"
#include "Online.h"
#include "OnlineAchievementsInterface.h"
#include "OnlineEventsInterface.h"
#include "OnlineIdentityInterface.h"
#include "OnlineSessionInterface.h"
#include "ShooterGameInstance.h"
#include "ShooterLeaderboards.h"
#include "ShooterGameViewportClient.h"
#include "Sound/SoundNodeLocalPlayer.h"
#include "AudioThread.h"

#define  ACH_FRAG_SOMEONE	TEXT("ACH_FRAG_SOMEONE")
#define  ACH_SOME_KILLS		TEXT("ACH_SOME_KILLS")
#define  ACH_LOTS_KILLS		TEXT("ACH_LOTS_KILLS")
#define  ACH_FINISH_MATCH	TEXT("ACH_FINISH_MATCH")
#define  ACH_LOTS_MATCHES	TEXT("ACH_LOTS_MATCHES")
#define  ACH_FIRST_WIN		TEXT("ACH_FIRST_WIN")
#define  ACH_LOTS_WIN		TEXT("ACH_LOTS_WIN")
#define  ACH_MANY_WIN		TEXT("ACH_MANY_WIN")
#define  ACH_SHOOT_BULLETS	TEXT("ACH_SHOOT_BULLETS")
#define  ACH_SHOOT_ROCKETS	TEXT("ACH_SHOOT_ROCKETS")
#define  ACH_GOOD_SCORE		TEXT("ACH_GOOD_SCORE")
#define  ACH_GREAT_SCORE	TEXT("ACH_GREAT_SCORE")
#define  ACH_PLAY_SANCTUARY	TEXT("ACH_PLAY_SANCTUARY")
#define  ACH_PLAY_HIGHRISE	TEXT("ACH_PLAY_HIGHRISE")

static const int32 SomeKillsCount = 10;
static const int32 LotsKillsCount = 20;
static const int32 LotsMatchesCount = 5;
static const int32 LotsWinsCount = 3;
static const int32 ManyWinsCount = 5;
static const int32 LotsBulletsCount = 100;
static const int32 LotsRocketsCount = 10;
static const int32 GoodScoreCount = 10;
static const int32 GreatScoreCount = 15;

AShooterPlayerController::AShooterPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AShooterPlayerCameraManager::StaticClass();
	CheatClass = UShooterCheatManager::StaticClass();
	bAllowGameActions = true;
	bGameEndedFrame = false;
	LastDeathLocation = FVector::ZeroVector;

	ServerSayString = TEXT("Say");
	ShooterFriendUpdateTimer = 0.0f;
	bHasSentStartEvents = false;

	bStoreVisible = false;

	StatMatchesPlayed = 0;
	StatKills = 0;
	StatDeaths = 0;
	bHasFetchedPlatformData = false;
}

void AShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// UI input
	InputComponent->BindAction("InGameMenu", IE_Pressed, this, &AShooterPlayerController::OnToggleInGameMenu);
	InputComponent->BindAction("ShopMenu", IE_Pressed, this, &AShooterPlayerController::OnToggleShopMenu);
	InputComponent->BindAction("Scoreboard", IE_Pressed, this, &AShooterPlayerController::OnShowScoreboard);
	InputComponent->BindAction("Scoreboard", IE_Released, this, &AShooterPlayerController::OnHideScoreboard);
	InputComponent->BindAction("ConditionalCloseScoreboard", IE_Pressed, this, &AShooterPlayerController::OnConditionalCloseScoreboard);
	InputComponent->BindAction("ToggleScoreboard", IE_Pressed, this, &AShooterPlayerController::OnToggleScoreboard);

	// voice chat
	InputComponent->BindAction("PushToTalk", IE_Pressed, this, &APlayerController::StartTalking);
	InputComponent->BindAction("PushToTalk", IE_Released, this, &APlayerController::StopTalking);

	InputComponent->BindAction("ToggleChat", IE_Pressed, this, &AShooterPlayerController::ToggleChatWindow);
}

void AShooterPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	FShooterStyle::Initialize();
	ShooterFriendUpdateTimer = 0;
	LoadInventory();

	//OnInventoryItemChanged.AddDynamic(this, &AShooterPlayerController::OnInventoryItemChange);
	//OnSlottedItemChanged.AddDynamic(this, &AShooterPlayerController::OnItemSlotChanged);
	OnInventoryItemChangedNative.AddUObject(this, &AShooterPlayerController::OnInventoryItemChange);
	//OnSlottedItemChangedNative.AddUObject(this, &AShooterPlayerController::OnItemSlotChanged);

}

void AShooterPlayerController::ClearLeaderboardDelegate()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineLeaderboardsPtr Leaderboards = OnlineSub->GetLeaderboardsInterface();
		if (Leaderboards.IsValid())
		{
			Leaderboards->ClearOnLeaderboardReadCompleteDelegate_Handle(LeaderboardReadCompleteDelegateHandle);
		}
	}
}

void AShooterPlayerController::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	if (IsGameMenuVisible())
	{
		if (ShooterFriendUpdateTimer > 0)
		{
			ShooterFriendUpdateTimer -= DeltaTime;
		}
		else
		{
			TSharedPtr<class FShooterFriends> ShooterFriends = ShooterIngameMenu->GetShooterFriends();
			ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
			if (ShooterFriends.IsValid() && LocalPlayer && LocalPlayer->GetControllerId() >= 0)
			{
				ShooterFriends->UpdateFriends(LocalPlayer->GetControllerId());
			}
			
			// Make sure the time between calls is long enough that we won't trigger (0x80552C81) and not exceed the web api rate limit
			// That value is currently 75 requests / 15 minutes.
			ShooterFriendUpdateTimer = 15;

		}
	}

	// Is this the first frame after the game has ended
	if(bGameEndedFrame)
	{
		bGameEndedFrame = false;

		// ONLY PUT CODE HERE WHICH YOU DON'T WANT TO BE DONE DUE TO HOST LOSS

		// Do we need to show the end of round scoreboard?
		if (IsPrimaryPlayer())
		{
			AShooterHUD* ShooterHUD = GetShooterHUD();
			if (ShooterHUD)
			{
				ShooterHUD->ShowScoreboard(true, true);
			}
		}
	}

	const bool bLocallyControlled = IsLocalController();
	const uint32 UniqueID = GetUniqueID();
	FAudioThread::RunCommandOnAudioThread([UniqueID, bLocallyControlled]()
	{
		USoundNodeLocalPlayer::GetLocallyControlledActorCache().Add(UniqueID, bLocallyControlled);
	});
};

void AShooterPlayerController::BeginDestroy()
{
	Super::BeginDestroy();
	ClearLeaderboardDelegate();

	if (!GExitPurge)
	{
		const uint32 UniqueID = GetUniqueID();
		FAudioThread::RunCommandOnAudioThread([UniqueID]()
		{
			USoundNodeLocalPlayer::GetLocallyControlledActorCache().Remove(UniqueID);
		});
	}
}

void AShooterPlayerController::SetPlayer( UPlayer* InPlayer )
{
	Super::SetPlayer( InPlayer );

	if (ULocalPlayer* const LocalPlayer = Cast<ULocalPlayer>(Player))
	{
		//Build menu only after game is initialized
		ShooterIngameMenu = MakeShareable(new FShooterIngameMenu());
		ShooterIngameMenu->Construct(Cast<ULocalPlayer>(Player));
		ShooterShopMenu = MakeShareable(new FShooterShopMenu());
		ShooterShopMenu->Construct(Cast<ULocalPlayer>(Player));

		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
}

void AShooterPlayerController::QueryAchievements()
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerController::QueryAchievements()"));
	// precache achievements
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer && LocalPlayer->GetControllerId() != -1)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if(OnlineSub)
		{
			IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
			if (Identity.IsValid())
			{
				TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->GetControllerId());

				if (UserId.IsValid())
				{
					IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();

					if (Achievements.IsValid())
					{
						Achievements->QueryAchievements( *UserId.Get(), FOnQueryAchievementsCompleteDelegate::CreateUObject( this, &AShooterPlayerController::OnQueryAchievementsComplete ));
					}
				}
				else
				{
					UE_LOG(LogOnline, Warning, TEXT("No valid user id for this controller."));
				}
			}
			else
			{
				UE_LOG(LogOnline, Warning, TEXT("No valid identity interface."));
			}
		}
		else
		{
			UE_LOG(LogOnline, Warning, TEXT("No default online subsystem."));
		}
	}
	else
	{
		UE_LOG(LogOnline, Warning, TEXT("No local player, cannot read achievements."));
	}
}

void AShooterPlayerController::OnQueryAchievementsComplete(const FUniqueNetId& PlayerId, const bool bWasSuccessful )
{
	UE_LOG(LogOnline, Display, TEXT("AShooterPlayerController::OnQueryAchievementsComplete(bWasSuccessful = %s)"), bWasSuccessful ? TEXT("TRUE") : TEXT("FALSE"));
}

void AShooterPlayerController::OnLeaderboardReadComplete(bool bWasSuccessful)
{
	if (ReadObject.IsValid() && ReadObject->ReadState == EOnlineAsyncTaskState::Done && !bHasFetchedPlatformData)
	{
		bHasFetchedPlatformData = true;
		ClearLeaderboardDelegate();

		// We should only have one stat.
		if (bWasSuccessful && ReadObject->Rows.Num() == 1)
		{
			FOnlineStatsRow& RowData = ReadObject->Rows[0];
			if (const FVariantData* KillData = RowData.Columns.Find(LEADERBOARD_STAT_KILLS))
			{
				KillData->GetValue(StatKills);
			}

			if (const FVariantData* DeathData = RowData.Columns.Find(LEADERBOARD_STAT_DEATHS))
			{
				DeathData->GetValue(StatDeaths);
			}

			if (const FVariantData* MatchData = RowData.Columns.Find(LEADERBOARD_STAT_MATCHESPLAYED))
			{
				MatchData->GetValue(StatMatchesPlayed);
			}

			UE_LOG(LogOnline, Log, TEXT("Fetched player stat data. Kills %d Deaths %d Matches %d"), StatKills, StatDeaths, StatMatchesPlayed);
		}
	}
}

void AShooterPlayerController::QueryStats()
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerController::QueryStats()"));
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer && LocalPlayer->GetControllerId() != -1)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
			if (Identity.IsValid())
			{
				TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->GetControllerId());

				if (UserId.IsValid())
				{
					IOnlineLeaderboardsPtr Leaderboards = OnlineSub->GetLeaderboardsInterface();
					if (Leaderboards.IsValid() && !bHasFetchedPlatformData)
					{
						TArray<TSharedRef<const FUniqueNetId>> QueryPlayers;
						QueryPlayers.Add(UserId.ToSharedRef());

						LeaderboardReadCompleteDelegateHandle = Leaderboards->OnLeaderboardReadCompleteDelegates.AddUObject(this, &AShooterPlayerController::OnLeaderboardReadComplete);
						ReadObject = MakeShareable(new FShooterAllTimeMatchResultsRead());
						FOnlineLeaderboardReadRef ReadObjectRef = ReadObject.ToSharedRef();
						if (Leaderboards->ReadLeaderboards(QueryPlayers, ReadObjectRef))
						{
							UE_LOG(LogOnline, Log, TEXT("Started process to fetch stats for current user."));
						}
						else
						{
							UE_LOG(LogOnline, Warning, TEXT("Could not start leaderboard fetch process. This will affect stat writes for this session."));
						}
						
					}
				}
			}
		}
	}
}

void AShooterPlayerController::UnFreeze()
{
	ServerRestartPlayer();
}

void AShooterPlayerController::FailedToSpawnPawn()
{
	if(StateName == NAME_Inactive)
	{
		BeginInactiveState();
	}
	Super::FailedToSpawnPawn();
}

void AShooterPlayerController::PawnPendingDestroy(APawn* P)
{
	LastDeathLocation = P->GetActorLocation();
	FVector CameraLocation = LastDeathLocation + FVector(0, 0, 300.0f);
	FRotator CameraRotation(-90.0f, 0.0f, 0.0f);
	FindDeathCameraSpot(CameraLocation, CameraRotation);

	Super::PawnPendingDestroy(P);

	ClientSetSpectatorCamera(CameraLocation, CameraRotation);
}

void AShooterPlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);
}

void AShooterPlayerController::ClientSetSpectatorCamera_Implementation(FVector CameraLocation, FRotator CameraRotation)
{
	SetInitialLocationAndRotation(CameraLocation, CameraRotation);
	SetViewTarget(this);
}

bool AShooterPlayerController::FindDeathCameraSpot(FVector& CameraLocation, FRotator& CameraRotation)
{
	const FVector PawnLocation = GetPawn()->GetActorLocation();
	FRotator ViewDir = GetControlRotation();
	ViewDir.Pitch = -45.0f;

	const float YawOffsets[] = { 0.0f, -180.0f, 90.0f, -90.0f, 45.0f, -45.0f, 135.0f, -135.0f };
	const float CameraOffset = 600.0f;
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(DeathCamera), true, GetPawn());

	FHitResult HitResult;
	for (int32 i = 0; i < ARRAY_COUNT(YawOffsets); i++)
	{
		FRotator CameraDir = ViewDir;
		CameraDir.Yaw += YawOffsets[i];
		CameraDir.Normalize();

		const FVector TestLocation = PawnLocation - CameraDir.Vector() * CameraOffset;
		
		const bool bBlocked = GetWorld()->LineTraceSingleByChannel(HitResult, PawnLocation, TestLocation, ECC_Camera, TraceParams);

		if (!bBlocked)
		{
			CameraLocation = TestLocation;
			CameraRotation = CameraDir;
			return true;
		}
	}

	return false;
}

bool AShooterPlayerController::ServerCheat_Validate(const FString& Msg)
{
	return true;
}

void AShooterPlayerController::ServerCheat_Implementation(const FString& Msg)
{
	if (CheatManager)
	{
		ClientMessage(ConsoleCommand(Msg));
	}
}

void AShooterPlayerController::SimulateInputKey(FKey Key, bool bPressed)
{
	InputKey(Key, bPressed ? IE_Pressed : IE_Released, 1, false);
}

void AShooterPlayerController::OnKill()
{
	UpdateAchievementProgress(ACH_FRAG_SOMEONE, 100.0f);

	const auto Events = Online::GetEventsInterface();
	const auto Identity = Online::GetIdentityInterface();

	if (Events.IsValid() && Identity.IsValid())
	{
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
		if (LocalPlayer)
		{
			int32 UserIndex = LocalPlayer->GetControllerId();
			TSharedPtr<const FUniqueNetId> UniqueID = Identity->GetUniquePlayerId(UserIndex);			
			if (UniqueID.IsValid())
			{			
				AShooterCharacter* ShooterChar = Cast<AShooterCharacter>(GetCharacter());
				// If player is dead, use location stored during pawn cleanup.
				FVector Location = ShooterChar ? ShooterChar->GetActorLocation() : LastDeathLocation;
				AShooterWeaponBase* Weapon = ShooterChar ? ShooterChar->GetWeapon() : 0;
				int32 WeaponType = Weapon ? (int32)((AShooterRangedWeapon*)Weapon)->GetAmmoType() : 0;

				FOnlineEventParms Params;		

				Params.Add( TEXT( "SectionId" ), FVariantData( (int32)0 ) ); // unused
				Params.Add( TEXT( "GameplayModeId" ), FVariantData( (int32)1 ) ); // @todo determine game mode (ffa v tdm)
				Params.Add( TEXT( "DifficultyLevelId" ), FVariantData( (int32)0 ) ); // unused

				Params.Add( TEXT( "PlayerRoleId" ), FVariantData( (int32)0 ) ); // unused
				Params.Add( TEXT( "PlayerWeaponId" ), FVariantData( (int32)WeaponType ) );
				Params.Add( TEXT( "EnemyRoleId" ), FVariantData( (int32)0 ) ); // unused
				Params.Add( TEXT( "EnemyWeaponId" ), FVariantData( (int32)0 ) ); // untracked			
				Params.Add( TEXT( "KillTypeId" ), FVariantData( (int32)0 ) ); // unused
				Params.Add( TEXT( "LocationX" ), FVariantData( Location.X ) );
				Params.Add( TEXT( "LocationY" ), FVariantData( Location.Y ) );
				Params.Add( TEXT( "LocationZ" ), FVariantData( Location.Z ) );
			
				Events->TriggerEvent(*UniqueID, TEXT("KillOponent"), Params);				
			}
		}
	}
}

void AShooterPlayerController::OnDeathMessage(class AShooterPlayerState* KillerPlayerState, class AShooterPlayerState* KilledPlayerState, const UDamageType* KillerDamageType) 
{
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if (ShooterHUD)
	{
		ShooterHUD->ShowDeathMessage(KillerPlayerState, KilledPlayerState, KillerDamageType);		
	}

	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer && LocalPlayer->GetCachedUniqueNetId().IsValid() && KilledPlayerState->UniqueId.IsValid())
	{
		// if this controller is the player who died, update the hero stat.
		if (*LocalPlayer->GetCachedUniqueNetId() == *KilledPlayerState->UniqueId)
		{
			const auto Events = Online::GetEventsInterface();
			const auto Identity = Online::GetIdentityInterface();

			if (Events.IsValid() && Identity.IsValid())
			{							
				const int32 UserIndex = LocalPlayer->GetControllerId();
				TSharedPtr<const FUniqueNetId> UniqueID = Identity->GetUniquePlayerId(UserIndex);
				if (UniqueID.IsValid())
				{				
					AShooterCharacter* ShooterChar = Cast<AShooterCharacter>(GetCharacter());
					AShooterWeaponBase* Weapon = ShooterChar ? ShooterChar->GetWeapon() : NULL;

					FVector Location = ShooterChar ? ShooterChar->GetActorLocation() : FVector::ZeroVector;
					int32 WeaponType = Weapon ? (int32)((AShooterRangedWeapon*)Weapon)->GetAmmoType() : 0;

					FOnlineEventParms Params;
					Params.Add( TEXT( "SectionId" ), FVariantData( (int32)0 ) ); // unused
					Params.Add( TEXT( "GameplayModeId" ), FVariantData( (int32)1 ) ); // @todo determine game mode (ffa v tdm)
					Params.Add( TEXT( "DifficultyLevelId" ), FVariantData( (int32)0 ) ); // unused

					Params.Add( TEXT( "PlayerRoleId" ), FVariantData( (int32)0 ) ); // unused
					Params.Add( TEXT( "PlayerWeaponId" ), FVariantData( (int32)WeaponType ) );
					Params.Add( TEXT( "EnemyRoleId" ), FVariantData( (int32)0 ) ); // unused
					Params.Add( TEXT( "EnemyWeaponId" ), FVariantData( (int32)0 ) ); // untracked
				
					Params.Add( TEXT( "LocationX" ), FVariantData( Location.X ) );
					Params.Add( TEXT( "LocationY" ), FVariantData( Location.Y ) );
					Params.Add( TEXT( "LocationZ" ), FVariantData( Location.Z ) );
										
					Events->TriggerEvent(*UniqueID, TEXT("PlayerDeath"), Params);
				}
			}
		}
	}	
}

void AShooterPlayerController::OnConnectedMessage(class AShooterPlayerState* KillerPlayerState, class AShooterPlayerState* KilledPlayerState, const UDamageType* KillerDamageType)
{
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if (ShooterHUD)
	{
		ShooterHUD->ShowDeathMessage(KillerPlayerState, KilledPlayerState, KillerDamageType);
	}
}

void AShooterPlayerController::UpdateAchievementProgress( const FString& Id, float Percent )
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if(OnlineSub)
		{
			IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
			if (Identity.IsValid())
			{
				FUniqueNetIdRepl UserId = LocalPlayer->GetCachedUniqueNetId();

				if (UserId.IsValid())
				{

					IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();
					if (Achievements.IsValid() && (!WriteObject.IsValid() || WriteObject->WriteState != EOnlineAsyncTaskState::InProgress))
					{
						WriteObject = MakeShareable(new FOnlineAchievementsWrite());
						WriteObject->SetFloatStat(*Id, Percent);

						FOnlineAchievementsWriteRef WriteObjectRef = WriteObject.ToSharedRef();
						Achievements->WriteAchievements(*UserId, WriteObjectRef);
					}
					else
					{
						UE_LOG(LogOnline, Warning, TEXT("No valid achievement interface or another write is in progress."));
					}
				}
				else
				{
					UE_LOG(LogOnline, Warning, TEXT("No valid user id for this controller."));
				}
			}
			else
			{
				UE_LOG(LogOnline, Warning, TEXT("No valid identity interface."));
			}
		}
		else
		{
			UE_LOG(LogOnline, Warning, TEXT("No default online subsystem."));
		}
	}
	else
	{
		UE_LOG(LogOnline, Warning, TEXT("No local player, cannot update achievements."));
	}
}

void AShooterPlayerController::OnToggleInGameMenu()
{
	if( GEngine->GameViewport == nullptr )
	{
		return;
	}

	// this is not ideal, but necessary to prevent both players from pausing at the same time on the same frame
	UWorld* GameWorld = GEngine->GameViewport->GetWorld();

	for(auto It = GameWorld->GetControllerIterator(); It; ++It)
	{
		AShooterPlayerController* Controller = Cast<AShooterPlayerController>(*It);
		if(Controller && Controller->IsPaused())
		{
			return;
		}
	}

	// if no one's paused, pause
	if (ShooterIngameMenu.IsValid())
	{
		ShooterIngameMenu->ToggleGameMenu();
	}
}

void AShooterPlayerController::OnToggleShopMenu()
{
	ShowStoreWidget();
	/*
	if (GEngine->GameViewport == nullptr)
	{
		return;
	}

	// this is not ideal, but necessary to prevent both players from pausing at the same time on the same frame
	UWorld* GameWorld = GEngine->GameViewport->GetWorld();

	for (auto It = GameWorld->GetControllerIterator(); It; ++It)
	{
		AShooterPlayerController* Controller = Cast<AShooterPlayerController>(*It);
		if (Controller && Controller->IsPaused())
		{
			return;
		}
	}

	// if no one's paused, pause
	if (ShooterShopMenu.IsValid())
	{
		ShooterShopMenu->ToggleGameMenu();
	}*/
}
void AShooterPlayerController::OnConditionalCloseScoreboard()
{
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if(ShooterHUD && ( ShooterHUD->IsMatchOver() == false ))
	{
		ShooterHUD->ConditionalCloseScoreboard();
	}
}

void AShooterPlayerController::OnToggleScoreboard()
{
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if(ShooterHUD && ( ShooterHUD->IsMatchOver() == false ))
	{
		ShooterHUD->ToggleScoreboard();
	}
}

void AShooterPlayerController::OnShowScoreboard()
{
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if(ShooterHUD)
	{
		ShooterHUD->ShowScoreboard(true);
	}
}

void AShooterPlayerController::OnHideScoreboard()
{
	AShooterHUD* ShooterHUD = GetShooterHUD();
	// If have a valid match and the match is over - hide the scoreboard
	if( (ShooterHUD != NULL ) && ( ShooterHUD->IsMatchOver() == false ) )
	{
		ShooterHUD->ShowScoreboard(false);
	}
}

bool AShooterPlayerController::IsGameMenuVisible() const
{
	bool Result = false; 
	if (ShooterIngameMenu.IsValid())
	{
		Result = ShooterIngameMenu->GetIsGameMenuUp();
	} 

	return Result;
}

bool AShooterPlayerController::IsGameShopMenuVisible() const
{
	bool Result = false;
	if (ShooterShopMenu.IsValid())
	{
		Result = ShooterShopMenu->GetIsGameMenuUp();
	}

	return Result;
}

void AShooterPlayerController::SetInfiniteAmmo(bool bEnable)
{
	bInfiniteAmmo = bEnable;
}

void AShooterPlayerController::SetInfiniteClip(bool bEnable)
{
	bInfiniteClip = bEnable;
}

void AShooterPlayerController::SetHealthRegen(bool bEnable)
{
	bHealthRegen = bEnable;
}

void AShooterPlayerController::SetGodMode(bool bEnable)
{
	bGodMode = bEnable;
}

void AShooterPlayerController::SetIsVibrationEnabled(bool bEnable)
{
	bIsVibrationEnabled = bEnable;
}

void AShooterPlayerController::ClientGameStarted_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::ClientGameStarted_Implementation()"));
	bAllowGameActions = true;

	// Enable controls mode now the game has started
	SetIgnoreMoveInput(false);

	AShooterHUD* ShooterHUD = GetShooterHUD();
	if (ShooterHUD)
	{
		ShooterHUD->SetMatchState(EShooterMatchState::Playing);
		ShooterHUD->ShowScoreboard(false);
	}
	bGameEndedFrame = false;

	QueryAchievements();

	QueryStats();

	// Send round start event
	const auto Events = Online::GetEventsInterface();
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);

	if(LocalPlayer != nullptr && Events.IsValid())
	{
		auto UniqueId = LocalPlayer->GetPreferredUniqueNetId();

		if (UniqueId.IsValid())
		{
			// Generate a new session id
			Events->SetPlayerSessionId(*UniqueId, FGuid::NewGuid());

			FString MapName = *FPackageName::GetShortName(GetWorld()->PersistentLevel->GetOutermost()->GetName());

			// Fire session start event for all cases
			FOnlineEventParms Params;
			Params.Add( TEXT( "GameplayModeId" ), FVariantData( (int32)1 ) ); // @todo determine game mode (ffa v tdm)
			Params.Add( TEXT( "DifficultyLevelId" ), FVariantData( (int32)0 ) ); // unused
			Params.Add( TEXT( "MapName" ), FVariantData( MapName ) );
			
			Events->TriggerEvent(*UniqueId, TEXT("PlayerSessionStart"), Params);

			// Online matches require the MultiplayerRoundStart event as well
			UShooterGameInstance* SGI = GetWorld() != NULL ? Cast<UShooterGameInstance>(GetWorld()->GetGameInstance()) : NULL;

			if (SGI && (SGI->GetOnlineMode() == EOnlineMode::Online))
			{
				FOnlineEventParms MultiplayerParams;

				// @todo: fill in with real values
				MultiplayerParams.Add( TEXT( "SectionId" ), FVariantData( (int32)0 ) ); // unused
				MultiplayerParams.Add( TEXT( "GameplayModeId" ), FVariantData( (int32)1 ) ); // @todo determine game mode (ffa v tdm)
				MultiplayerParams.Add( TEXT( "MatchTypeId" ), FVariantData( (int32)1 ) ); // @todo abstract the specific meaning of this value across platforms
				MultiplayerParams.Add( TEXT( "DifficultyLevelId" ), FVariantData( (int32)0 ) ); // unused
				
				Events->TriggerEvent(*UniqueId, TEXT("MultiplayerRoundStart"), MultiplayerParams);
			}

			bHasSentStartEvents = true;
		}
	}
}

/** Starts the online game using the session name in the PlayerState */
void AShooterPlayerController::ClientStartOnlineGame_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::ClientStartOnlineGame_Implementation()"));
	if (!IsPrimaryPlayer()){
		return;
	}
	AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
	if (ShooterPlayerState)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid() && (Sessions->GetNamedSession(ShooterPlayerState->SessionName) != nullptr))
			{
				UE_LOG(LogOnline, Log, TEXT("Starting session %s on client"), *ShooterPlayerState->SessionName.ToString() );
				Sessions->StartSession(ShooterPlayerState->SessionName);
			}
		}
	}
	else
	{
		// Keep retrying until player state is replicated
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ClientStartOnlineGame, this, &AShooterPlayerController::ClientStartOnlineGame_Implementation, 0.2f, false);
	}
}

/** Ends the online game using the session name in the PlayerState */
void AShooterPlayerController::ClientEndOnlineGame_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::ClientEndOnlineGame_Implementation()"));
	if (!IsPrimaryPlayer()){
		return;
	}
	AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
	if (ShooterPlayerState)
	{
		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid() && (Sessions->GetNamedSession(ShooterPlayerState->SessionName) != nullptr))
			{
				UE_LOG(LogOnline, Log, TEXT("Ending session %s on client"), *ShooterPlayerState->SessionName.ToString() );
				Sessions->EndSession(ShooterPlayerState->SessionName);
			}
		}
	}
}

void AShooterPlayerController::HandleReturnToMainMenu()
{
	OnHideScoreboard();
	CleanupSessionOnReturnToMenu();
}

void AShooterPlayerController::ClientReturnToMainMenu_Implementation(const FString& InReturnReason)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::ClientReturnToMainMenu_Implementation()"));
	UShooterGameInstance* SGI = GetWorld() != NULL ? Cast<UShooterGameInstance>(GetWorld()->GetGameInstance()) : NULL;

	if ( !ensure( SGI != NULL ) )
	{
		return;
	}

	if ( GetNetMode() == NM_Client )
	{
		const FText ReturnReason	= NSLOCTEXT( "NetworkErrors", "HostQuit", "The host has quit the match." );
		const FText OKButton		= NSLOCTEXT( "DialogButtons", "OKAY", "OK" );

		SGI->ShowMessageThenGotoState( ReturnReason, OKButton, FText::GetEmpty(), ShooterGameInstanceState::MainMenu );
	}
	else
	{
		SGI->GotoState(ShooterGameInstanceState::MainMenu);
	}

	// Clear the flag so we don't do normal end of round stuff next
	bGameEndedFrame = false;
}

/** Ends and/or destroys game session */
void AShooterPlayerController::CleanupSessionOnReturnToMenu()
{
	UShooterGameInstance * SGI = GetWorld() != NULL ? Cast<UShooterGameInstance>( GetWorld()->GetGameInstance() ) : NULL;

	if ( ensure( SGI != NULL ) )
	{
		SGI->CleanupSessionOnReturnToMenu();
	}
}

void AShooterPlayerController::ClientGameEnded_Implementation(class AActor* EndGameFocus, bool bIsWinner)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::ClientGameEnded_Implementation()"));
	Super::ClientGameEnded_Implementation(EndGameFocus, bIsWinner);
	
	// Disable controls now the game has ended
	SetIgnoreMoveInput(true);

	bAllowGameActions = false;

	// Make sure that we still have valid view target
	SetViewTarget(GetPawn());

	AShooterHUD* ShooterHUD = GetShooterHUD();
	if (ShooterHUD)
	{
		ShooterHUD->SetMatchState(bIsWinner ? EShooterMatchState::Won : EShooterMatchState::Lost);
	}

	UpdateSaveFileOnGameEnd(bIsWinner);
	UpdateAchievementsOnGameEnd();
	UpdateLeaderboardsOnGameEnd();

	// Flag that the game has just ended (if it's ended due to host loss we want to wait for ClientReturnToMainMenu_Implementation first, incase we don't want to process)
	bGameEndedFrame = true;
}

void AShooterPlayerController::ClientSendRoundEndEvent_Implementation(bool bIsWinner, int32 ExpendedTimeInSeconds)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::ClientSendRoundEndEvent_Implementation()"));
	const auto Events = Online::GetEventsInterface();
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);

	if(bHasSentStartEvents && LocalPlayer != nullptr && Events.IsValid())
	{	
		auto UniqueId = LocalPlayer->GetPreferredUniqueNetId();

		if (UniqueId.IsValid())
		{
			FString MapName = *FPackageName::GetShortName(GetWorld()->PersistentLevel->GetOutermost()->GetName());
			AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
			int32 PlayerScore = ShooterPlayerState ? ShooterPlayerState->GetScore() : 0;
			
			// Fire session end event for all cases
			FOnlineEventParms Params;
			Params.Add( TEXT( "GameplayModeId" ), FVariantData( (int32)1 ) ); // @todo determine game mode (ffa v tdm)
			Params.Add( TEXT( "DifficultyLevelId" ), FVariantData( (int32)0 ) ); // unused
			Params.Add( TEXT( "ExitStatusId" ), FVariantData( (int32)0 ) ); // unused
			Params.Add( TEXT( "PlayerScore" ), FVariantData( (int32)PlayerScore ) );
			Params.Add( TEXT( "PlayerWon" ), FVariantData( (bool)bIsWinner ) );
			Params.Add( TEXT( "MapName" ), FVariantData( MapName ) );
			Params.Add( TEXT( "MapNameString" ), FVariantData( MapName ) ); // @todo workaround for a bug in backend service, remove when fixed
			
			Events->TriggerEvent(*UniqueId, TEXT("PlayerSessionEnd"), Params);

			// Online matches require the MultiplayerRoundEnd event as well
			UShooterGameInstance* SGI = GetWorld() != NULL ? Cast<UShooterGameInstance>(GetWorld()->GetGameInstance()) : NULL;
			if (SGI && (SGI->GetOnlineMode() == EOnlineMode::Online))
			{
				FOnlineEventParms MultiplayerParams;

				AShooterGameState* const MyGameState = GetWorld() != NULL ? GetWorld()->GetGameState<AShooterGameState>() : NULL;
				if (ensure(MyGameState != nullptr))
				{
					MultiplayerParams.Add( TEXT( "SectionId" ), FVariantData( (int32)0 ) ); // unused
					MultiplayerParams.Add( TEXT( "GameplayModeId" ), FVariantData( (int32)1 ) ); // @todo determine game mode (ffa v tdm)
					MultiplayerParams.Add( TEXT( "MatchTypeId" ), FVariantData( (int32)1 ) ); // @todo abstract the specific meaning of this value across platforms
					MultiplayerParams.Add( TEXT( "DifficultyLevelId" ), FVariantData( (int32)0 ) ); // unused
					MultiplayerParams.Add( TEXT( "TimeInSeconds" ), FVariantData( (float)ExpendedTimeInSeconds ) );
					MultiplayerParams.Add( TEXT( "ExitStatusId" ), FVariantData( (int32)0 ) ); // unused
					
					Events->TriggerEvent(*UniqueId, TEXT("MultiplayerRoundEnd"), MultiplayerParams);
				}
			}
		}

		bHasSentStartEvents = false;
	}
}

void AShooterPlayerController::SetCinematicMode(bool bInCinematicMode, bool bHidePlayer, bool bAffectsHUD, bool bAffectsMovement, bool bAffectsTurning)
{
	Super::SetCinematicMode(bInCinematicMode, bHidePlayer, bAffectsHUD, bAffectsMovement, bAffectsTurning);

	// If we have a pawn we need to determine if we should show/hide the weapon
	AShooterCharacter* MyPawn = Cast<AShooterCharacter>(GetPawn());
	AShooterWeaponBase* MyWeapon = MyPawn ? MyPawn->GetWeapon() : NULL;
	if (MyWeapon)
	{
		if (bInCinematicMode && bHidePlayer)
		{
			MyWeapon->SetActorHiddenInGame(true);
		}
		else if (!bCinematicMode)
		{
			MyWeapon->SetActorHiddenInGame(false);
		}
	}
}

bool AShooterPlayerController::IsMoveInputIgnored() const
{
	if (IsInState(NAME_Spectating))
	{
		return false;
	}
	else
	{
		return Super::IsMoveInputIgnored();
	}
}

bool AShooterPlayerController::IsLookInputIgnored() const
{
	if (IsInState(NAME_Spectating))
	{
		return false;
	}
	else
	{
		return Super::IsLookInputIgnored();
	}
}

void AShooterPlayerController::InitInputSystem()
{
	Super::InitInputSystem();

	UShooterPersistentUser* PersistentUser = GetPersistentUser();
	if(PersistentUser)
	{
		PersistentUser->TellInputAboutKeybindings();
	}
}

void AShooterPlayerController::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	// only to local owner: weapon change requests are locally instigated, other clients don't need it
	DOREPLIFETIME_CONDITION(AShooterPlayerController, bInfiniteAmmo, COND_OwnerOnly );
	DOREPLIFETIME_CONDITION(AShooterPlayerController, bInfiniteClip, COND_OwnerOnly );
}

void AShooterPlayerController::Suicide()
{
	if ( IsInState(NAME_Playing) )
	{
		ServerSuicide();
	}
}

bool AShooterPlayerController::ServerSuicide_Validate()
{
	return true;
}

void AShooterPlayerController::ServerSuicide_Implementation()
{
	if ( (GetPawn() != NULL) && ((GetWorld()->TimeSeconds - GetPawn()->CreationTime > 1) || (GetNetMode() == NM_Standalone)) )
	{
		AShooterCharacter* MyPawn = Cast<AShooterCharacter>(GetPawn());
		if (MyPawn)
		{
			MyPawn->Suicide();
		}
	}
}

bool AShooterPlayerController::HasInfiniteAmmo() const
{
	return bInfiniteAmmo;
}

bool AShooterPlayerController::HasInfiniteClip() const
{
	return bInfiniteClip;
}

bool AShooterPlayerController::HasHealthRegen() const
{
	return bHealthRegen;
}

bool AShooterPlayerController::HasGodMode() const
{
	return bGodMode;
}

bool AShooterPlayerController::IsVibrationEnabled() const
{
	return bIsVibrationEnabled;
}

bool AShooterPlayerController::IsGameInputAllowed() const
{
	return bAllowGameActions && !bCinematicMode;
}

void AShooterPlayerController::ToggleChatWindow()
{
	AShooterHUD* ShooterHUD = Cast<AShooterHUD>(GetHUD());
	if (ShooterHUD)
	{
		ShooterHUD->ToggleChat();
	}
}

void AShooterPlayerController::ClientTeamMessage_Implementation( APlayerState* SenderPlayerState, const FString& S, FName Type, float MsgLifeTime  )
{
	AShooterHUD* ShooterHUD = Cast<AShooterHUD>(GetHUD());
	if (ShooterHUD)
	{
		if( Type == ServerSayString )
		{
			if( SenderPlayerState != PlayerState  )
			{
				ShooterHUD->AddChatLine(FText::FromString(S), false);
			}
		}
	}
}

void AShooterPlayerController::Say( const FString& Msg )
{
	ServerSay(Msg.Left(128));
}

bool AShooterPlayerController::ServerSay_Validate( const FString& Msg )
{
	return true;
}

void AShooterPlayerController::ServerSay_Implementation( const FString& Msg )
{
	GetWorld()->GetAuthGameMode<AShooterGameMode>()->Broadcast(this, Msg, ServerSayString);
}

AShooterHUD* AShooterPlayerController::GetShooterHUD() const
{
	return Cast<AShooterHUD>(GetHUD());
}


UShooterPersistentUser* AShooterPlayerController::GetPersistentUser() const
{
	UShooterLocalPlayer* const ShooterLocalPlayer = Cast<UShooterLocalPlayer>(Player);
	return ShooterLocalPlayer ? ShooterLocalPlayer->GetPersistentUser() : nullptr;
}

bool AShooterPlayerController::SetPause(bool bPause, FCanUnpause CanUnpauseDelegate /*= FCanUnpause()*/)
{
	const bool Result = APlayerController::SetPause(bPause, CanUnpauseDelegate);

	// Update rich presence.
	const auto PresenceInterface = Online::GetPresenceInterface();
	const auto Events = Online::GetEventsInterface();
	const auto LocalPlayer = Cast<ULocalPlayer>(Player);
	FUniqueNetIdRepl UserId = LocalPlayer ? LocalPlayer->GetCachedUniqueNetId() : FUniqueNetIdRepl();

	// Don't send pause events while online since the game doesn't actually pause
	if(GetNetMode() == NM_Standalone && Events.IsValid() && PlayerState->UniqueId.IsValid())
	{
		FOnlineEventParms Params;
		Params.Add( TEXT( "GameplayModeId" ), FVariantData( (int32)1 ) ); // @todo determine game mode (ffa v tdm)
		Params.Add( TEXT( "DifficultyLevelId" ), FVariantData( (int32)0 ) ); // unused
		if(Result && bPause)
		{
			Events->TriggerEvent(*PlayerState->UniqueId, TEXT("PlayerSessionPause"), Params);
		}
		else
		{
			Events->TriggerEvent(*PlayerState->UniqueId, TEXT("PlayerSessionResume"), Params);
		}
	}

	return Result;
}

FVector AShooterPlayerController::GetFocalLocation() const
{
	const AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn());

	// On death we want to use the player's death cam location rather than the location of where the pawn is at the moment
	// This guarantees that the clients see their death cam correctly, as their pawns have delayed destruction.
	if (ShooterCharacter && ShooterCharacter->bIsDying)
	{
		return GetSpawnLocation();
	}

	return Super::GetFocalLocation();
}

void AShooterPlayerController::ShowInGameMenu()
{
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if(ShooterIngameMenu.IsValid() && !ShooterIngameMenu->GetIsGameMenuUp() && ShooterHUD && (ShooterHUD->IsMatchOver() == false))
	{
		ShooterIngameMenu->ToggleGameMenu();
	}
}

void AShooterPlayerController::UpdateAchievementsOnGameEnd()
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	if (LocalPlayer)
	{
		AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
		if (ShooterPlayerState)
		{			
			const UShooterPersistentUser*  PersistentUser = GetPersistentUser();

			if (PersistentUser)
			{						
				const int32 Wins = PersistentUser->GetWins();
				const int32 Losses = PersistentUser->GetLosses();
				const int32 Matches = Wins + Losses;

				const int32 TotalKills = PersistentUser->GetKills();
				const int32 MatchScore = (int32)ShooterPlayerState->GetScore();

				const int32 TotalBulletsFired = PersistentUser->GetBulletsFired();
				const int32 TotalRocketsFired = PersistentUser->GetRocketsFired();
			
				float TotalGameAchievement = 0;
				float CurrentGameAchievement = 0;
			
				///////////////////////////////////////
				// Kill achievements
				if (TotalKills >= 1)
				{
					CurrentGameAchievement += 100.0f;
				}
				TotalGameAchievement += 100;

				{
					float fSomeKillPct = ((float)TotalKills / (float)SomeKillsCount) * 100.0f;
					fSomeKillPct = FMath::RoundToFloat(fSomeKillPct);
					UpdateAchievementProgress(ACH_SOME_KILLS, fSomeKillPct);

					CurrentGameAchievement += FMath::Min(fSomeKillPct, 100.0f);
					TotalGameAchievement += 100;
				}

				{
					float fLotsKillPct = ((float)TotalKills / (float)LotsKillsCount) * 100.0f;
					fLotsKillPct = FMath::RoundToFloat(fLotsKillPct);
					UpdateAchievementProgress(ACH_LOTS_KILLS, fLotsKillPct);

					CurrentGameAchievement += FMath::Min(fLotsKillPct, 100.0f);
					TotalGameAchievement += 100;
				}
				///////////////////////////////////////

				///////////////////////////////////////
				// Match Achievements
				{
					UpdateAchievementProgress(ACH_FINISH_MATCH, 100.0f);

					CurrentGameAchievement += 100;
					TotalGameAchievement += 100;
				}
			
				{
					float fLotsRoundsPct = ((float)Matches / (float)LotsMatchesCount) * 100.0f;
					fLotsRoundsPct = FMath::RoundToFloat(fLotsRoundsPct);
					UpdateAchievementProgress(ACH_LOTS_MATCHES, fLotsRoundsPct);

					CurrentGameAchievement += FMath::Min(fLotsRoundsPct, 100.0f);
					TotalGameAchievement += 100;
				}
				///////////////////////////////////////

				///////////////////////////////////////
				// Win Achievements
				if (Wins >= 1)
				{
					UpdateAchievementProgress(ACH_FIRST_WIN, 100.0f);

					CurrentGameAchievement += 100.0f;
				}
				TotalGameAchievement += 100;

				{			
					float fLotsWinPct = ((float)Wins / (float)LotsWinsCount) * 100.0f;
					fLotsWinPct = FMath::RoundToInt(fLotsWinPct);
					UpdateAchievementProgress(ACH_LOTS_WIN, fLotsWinPct);

					CurrentGameAchievement += FMath::Min(fLotsWinPct, 100.0f);
					TotalGameAchievement += 100;
				}

				{			
					float fManyWinPct = ((float)Wins / (float)ManyWinsCount) * 100.0f;
					fManyWinPct = FMath::RoundToInt(fManyWinPct);
					UpdateAchievementProgress(ACH_MANY_WIN, fManyWinPct);

					CurrentGameAchievement += FMath::Min(fManyWinPct, 100.0f);
					TotalGameAchievement += 100;
				}
				///////////////////////////////////////

				///////////////////////////////////////
				// Ammo Achievements
				{
					float fLotsBulletsPct = ((float)TotalBulletsFired / (float)LotsBulletsCount) * 100.0f;
					fLotsBulletsPct = FMath::RoundToFloat(fLotsBulletsPct);
					UpdateAchievementProgress(ACH_SHOOT_BULLETS, fLotsBulletsPct);

					CurrentGameAchievement += FMath::Min(fLotsBulletsPct, 100.0f);
					TotalGameAchievement += 100;
				}

				{
					float fLotsRocketsPct = ((float)TotalRocketsFired / (float)LotsRocketsCount) * 100.0f;
					fLotsRocketsPct = FMath::RoundToFloat(fLotsRocketsPct);
					UpdateAchievementProgress(ACH_SHOOT_ROCKETS, fLotsRocketsPct);

					CurrentGameAchievement += FMath::Min(fLotsRocketsPct, 100.0f);
					TotalGameAchievement += 100;
				}
				///////////////////////////////////////

				///////////////////////////////////////
				// Score Achievements
				{
					float fGoodScorePct = ((float)MatchScore / (float)GoodScoreCount) * 100.0f;
					fGoodScorePct = FMath::RoundToFloat(fGoodScorePct);
					UpdateAchievementProgress(ACH_GOOD_SCORE, fGoodScorePct);
				}

				{
					float fGreatScorePct = ((float)MatchScore / (float)GreatScoreCount) * 100.0f;
					fGreatScorePct = FMath::RoundToFloat(fGreatScorePct);
					UpdateAchievementProgress(ACH_GREAT_SCORE, fGreatScorePct);
				}
				///////////////////////////////////////

				///////////////////////////////////////
				// Map Play Achievements
				UWorld* World = GetWorld();
				if (World)
				{			
					FString MapName = *FPackageName::GetShortName(World->PersistentLevel->GetOutermost()->GetName());
					if (MapName.Find(TEXT("Highrise")) != -1)
					{
						UpdateAchievementProgress(ACH_PLAY_HIGHRISE, 100.0f);
					}
					else if (MapName.Find(TEXT("Sanctuary")) != -1)
					{
						UpdateAchievementProgress(ACH_PLAY_SANCTUARY, 100.0f);
					}
				}
				///////////////////////////////////////			

				const auto Events = Online::GetEventsInterface();
				const auto Identity = Online::GetIdentityInterface();

				if (Events.IsValid() && Identity.IsValid())
				{							
					const int32 UserIndex = LocalPlayer->GetControllerId();
					TSharedPtr<const FUniqueNetId> UniqueID = Identity->GetUniquePlayerId(UserIndex);
					if (UniqueID.IsValid())
					{				
						FOnlineEventParms Params;

						float fGamePct = (CurrentGameAchievement / TotalGameAchievement) * 100.0f;
						fGamePct = FMath::RoundToFloat(fGamePct);
						Params.Add( TEXT( "CompletionPercent" ), FVariantData( (float)fGamePct ) );
						if (UniqueID.IsValid())
						{				
							Events->TriggerEvent(*UniqueID, TEXT("GameProgress"), Params);
						}
					}
				}
			}
		}
	}
}

void AShooterPlayerController::UpdateLeaderboardsOnGameEnd()
{
	UShooterLocalPlayer* LocalPlayer = Cast<UShooterLocalPlayer>(Player);
	if (LocalPlayer)
	{
		// update leaderboards - note this does not respect existing scores and overwrites them. We would first need to read the leaderboards if we wanted to do that.
		IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
			if (Identity.IsValid())
			{
				TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(LocalPlayer->GetControllerId());
				if (UserId.IsValid())
				{
					IOnlineLeaderboardsPtr Leaderboards = OnlineSub->GetLeaderboardsInterface();
					if (Leaderboards.IsValid())
					{
						AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
						if (ShooterPlayerState)
						{
							FShooterAllTimeMatchResultsWrite ResultsWriteObject;
							int32 MatchWriteData = 1;
							int32 KillsWriteData = ShooterPlayerState->GetKills();
							int32 DeathsWriteData = ShooterPlayerState->GetDeaths();

#if !PLATFORM_XBOXONE
							StatMatchesPlayed = (MatchWriteData += StatMatchesPlayed);
							StatKills = (KillsWriteData += StatKills);
							StatDeaths = (DeathsWriteData += StatDeaths);
#endif

							ResultsWriteObject.SetIntStat(LEADERBOARD_STAT_SCORE, KillsWriteData);
							ResultsWriteObject.SetIntStat(LEADERBOARD_STAT_KILLS, KillsWriteData);
							ResultsWriteObject.SetIntStat(LEADERBOARD_STAT_DEATHS, DeathsWriteData);
							ResultsWriteObject.SetIntStat(LEADERBOARD_STAT_MATCHESPLAYED, MatchWriteData);

							// the call will copy the user id and write object to its own memory
							Leaderboards->WriteLeaderboards(ShooterPlayerState->SessionName, *UserId, ResultsWriteObject);
							Leaderboards->FlushLeaderboards(TEXT("SHOOTERGAME"));
						}
					}
				}
			}
		}
	}
}

void AShooterPlayerController::UpdateSaveFileOnGameEnd(bool bIsWinner)
{
	AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
	if (ShooterPlayerState)
	{
		// update local saved profile
		UShooterPersistentUser* const PersistentUser = GetPersistentUser();
		if (PersistentUser)
		{
			PersistentUser->AddMatchResult(ShooterPlayerState->GetKills(), ShooterPlayerState->GetDeaths(), ShooterPlayerState->GetNumBulletsFired(), ShooterPlayerState->GetNumRocketsFired(), bIsWinner);
			PersistentUser->SaveIfDirty();
		}
	}
}

void AShooterPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	Super::PreClientTravel( PendingURL, TravelType, bIsSeamlessTravel );

	if ( GetWorld() != NULL )
	{
		UShooterGameViewportClient* ShooterViewport = Cast<UShooterGameViewportClient>( GetWorld()->GetGameViewport() );

		if ( ShooterViewport != NULL )
		{
			ShooterViewport->ShowLoadingScreen();
		}
		
		AShooterHUD* ShooterHUD = Cast<AShooterHUD>(GetHUD());
		if (ShooterHUD != nullptr)
		{
			// Passing true to bFocus here ensures that focus is returned to the game viewport.
			ShooterHUD->ShowScoreboard(false, true);
		}
	}
}

bool AShooterPlayerController::UpdateInventoryItem(UShooterItem* NewItem, bool bAdd)
{
	//const TMap<UShooterItem*, FShooterItemData>& InventoryData = GetInventoryDataMap();
	UE_LOG(LogTemp, Warning, TEXT("Controller::UpdateInventoryItem( bAdd = %s)"), bAdd==true?TEXT("true"):TEXT("false"));
	if (!NewItem)
	{
		return false;
	}
	if (bAdd)
	{
		return AddInventoryItem(NewItem, 1, 1, true);
	}
	else
	{
		return RemoveInventoryItem(NewItem, 0);
	}
}

bool AShooterPlayerController::AddInventoryItem(UShooterItem* NewItem, int32 ItemCount, int32 ItemLevel, bool bAutoSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::AddInventoryItem()"));
	bool bChanged = false;
	if (!NewItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddInventoryItem: Failed trying to add null item!"));
		return false;
	}

	if (ItemCount <= 0 || ItemLevel <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddInventoryItem: Failed trying to add item %s with negative count or level!"), *NewItem->GetName());
		return false;
	}

	// Find current item data, which may be empty
	FShooterItemData OldData;
	GetInventoryItemData(NewItem, OldData);
	
	// Find modified data
	FShooterItemData NewData = OldData;
	NewData.UpdateItemData(FShooterItemData(ItemCount, ItemLevel), NewItem->MaxCount, NewItem->MaxLevel);

	if (OldData != NewData)
	{
		// If data changed, need to update storage and call callback
		InventoryData.Add(NewItem, NewData);
		NotifyInventoryItemChanged(NewItem, true);
		bChanged = true;
	}

	if (bAutoSlot)
	{
		// Slot item if required
		bChanged |= FillEmptySlotWithItem(NewItem);
	}
	if (bChanged)
	{
		//if (NewItem->ItemType != UShooterAssetManager::TokenItemType)
		//{
		//	AShooterCharacter* pawn = Cast<AShooterCharacter>(GetCharacter());
		//	pawn->UpdateInventoryActors(NewItem, true);
		//}
		// If anything changed, write to save game
		SaveInventory();
		return true;
	}
	return false;
}

bool AShooterPlayerController::RemoveInventoryItem(UShooterItem* RemovedItem, int32 RemoveCount)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::RemoveInventoryItem()"));
	if (!RemovedItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveInventoryItem: Failed trying to remove null item!"));
		return false;
	}

	// Find current item data, which may be empty
	FShooterItemData NewData;
	GetInventoryItemData(RemovedItem, NewData);

	if (!NewData.IsValid())
	{
		// Wasn't found
		return false;
	}

	// If RemoveCount <= 0, delete all
	if (RemoveCount <= 0)
	{
		NewData.ItemCount = 0;
	}
	else
	{
		NewData.ItemCount -= RemoveCount;
	}

	if (NewData.ItemCount > 0)
	{
		// Update data with new count
		InventoryData.Add(RemovedItem, NewData);
	}
	else
	{
		// Remove item entirely, make sure it is unslotted
		InventoryData.Remove(RemovedItem);

		//if (RemovedItem->ItemType != UShooterAssetManager::TokenItemType)
		//{
		//	AShooterCharacter* pawn = Cast<AShooterCharacter>(GetCharacter());
		//	pawn->UpdateInventoryActors(RemovedItem, false);
		//}
		for (TPair<FShooterItemSlot, UShooterItem*>& Pair : SlottedItems)
		{
			if (Pair.Value == RemovedItem)
			{
				Pair.Value = nullptr;
				NotifySlottedItemChanged(Pair.Key, Pair.Value);
			}
		}

	}

	// If we got this far, there is a change so notify and save
	NotifyInventoryItemChanged(RemovedItem, false);

	SaveInventory();
	return true;
}

TArray<UShooterItem*> AShooterPlayerController::GetInventoryItemList(FPrimaryAssetType ItemType) const
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetInventoryItemList(InventoryData.Num : %d)"), InventoryData.Num());
	TArray<UShooterItem*> Items;
	Items.Reset(10);
	for (const TPair<UShooterItem*, FShooterItemData>& Pair : InventoryData)
	{
		if (Pair.Key->ItemType == ItemType)
		{
			Items.AddUnique(Pair.Key);
		}
	}
	Items.Sort();
	return Items;
}

int32 AShooterPlayerController::GetInventoryItemListCount() const
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetInventoryItemListCount(InventoryData.Num : %d)"), InventoryData.Num());
	TArray<UShooterItem*> ItemList;
	for (const TPair<UShooterItem*, FShooterItemData>& Pair : InventoryData)
	{
		if (Pair.Key)
		{
			ItemList.Add(Pair.Key);
		}
	}
	return ItemList.Num();
}

void AShooterPlayerController::GetInventoryItems(TArray<UShooterItem*>& Items, FPrimaryAssetType ItemType)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetInventoryItems(InventoryData.Num : %d)"), InventoryData.Num());
	for (const TPair<UShooterItem*, FShooterItemData>& Pair : InventoryData)
	{
		if (Pair.Key)
		{
			FPrimaryAssetId AssetId = Pair.Key->GetPrimaryAssetId();

			// Filters based on item type
			if (AssetId.PrimaryAssetType == ItemType || !ItemType.IsValid())
			{
				Items.Add(Pair.Key);
			}
		}
	}
}

bool AShooterPlayerController::SetSlottedItem(FShooterItemSlot ItemSlot, UShooterItem* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::SetSlottedItem(SlottedItems.Num : %d)"), SlottedItems.Num());
	// Iterate entire inventory because we need to remove from old slot
	bool bFound = false;
	for (TPair<FShooterItemSlot, UShooterItem*>& Pair : SlottedItems)
	{
		if (Pair.Key == ItemSlot)
		{
			// Add to new slot
			bFound = true;
			Pair.Value = Item;
			NotifySlottedItemChanged(Pair.Key, Pair.Value);
		}
		else if (Item != nullptr && Pair.Value == Item)
		{
			// If this item was found in another slot, remove it
			Pair.Value = nullptr;
			NotifySlottedItemChanged(Pair.Key, Pair.Value);
		}
	}

	if (bFound)
	{
		SaveInventory();
		return true;
	}

	return false;
}

bool AShooterPlayerController::HasItem(UShooterItem* Item) const
{
	const FShooterItemData* FoundItem = InventoryData.Find(Item);

	if (FoundItem)
	{
		return true;
	}
	return false;
}

int32 AShooterPlayerController::GetInventoryItemCount(UShooterItem* Item) const
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetInventoryItemCount(InventoryData.Num : %d)"), InventoryData.Num());
	const FShooterItemData* FoundItem = InventoryData.Find(Item);

	if (FoundItem)
	{
		return FoundItem->ItemCount;
	}
	return 0;
}

bool AShooterPlayerController::GetInventoryItemData(UShooterItem* Item, FShooterItemData& ItemData) const
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetInventoryItemData(InventoryData.Num : %d)"), InventoryData.Num());
	const FShooterItemData* FoundItem = InventoryData.Find(Item);

	if (FoundItem)
	{
		ItemData = *FoundItem;
		UE_LOG(LogTemp, Warning, TEXT("Controller::GetInventoryItemData(ItemData.ItemCount : %d)"), ItemData.ItemCount);
		return true;
	}
	ItemData = FShooterItemData(0, 0);
	return false;
}

bool AShooterPlayerController::GetInventoryItemSlot(UShooterItem* Item, FShooterItemSlot& ItemSlot) const
{
	if (!Item)
	{
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetInventoryItemSlot( ItemName = %s)"), &Item->ItemName);
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetInventoryItemSlot(SlottedItems.Num : %d)"), SlottedItems.Num());
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("ItemName : %s"), &Item->ItemName));
	//UE_LOG(LogTemp, Warning, TEXT("GameMode::PreLogin(%s)"), *FString::Printf(TEXT("Current NetMode is not DedicatedServer. Or Current OnlineMode is not Online.")));
	const FShooterItemSlot* FoundSlot = SlottedItems.FindKey(Item);
	//UE_LOG(LogTemp, Warning, TEXT("ItemSlotNum = %d)"), FoundSlot->SlotNumber);
	/*
	for (const TPair<FShooterItemSlot, UShooterItem*>& Pair : GetSlottedItemMap())
	{
		UShooterItem* CurItem = Cast<UShooterItem>(Pair.Value);
		if (CurItem == Item)
		{
			ItemSlot = Pair.Key;
			return true;
		}
	}*/

	if (FoundSlot)
	{
		ItemSlot = *FoundSlot;
		return true;
	}
	ItemSlot = FShooterItemSlot();
	return false;
}

UShooterItem* AShooterPlayerController::GetSlottedItem(FShooterItemSlot ItemSlot) const
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetSlottedItem(SlottedItems.Num : %d)"), SlottedItems.Num());
	UShooterItem* const* FoundItem = SlottedItems.Find(ItemSlot);

	if (FoundItem)
	{
		return *FoundItem;
	}
	return nullptr;
}

void AShooterPlayerController::FillEmptySlots()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::FillEmptySlots()"));
	bool bShouldSave = false;
	for (const TPair<UShooterItem*, FShooterItemData>& Pair : InventoryData)
	{
		bShouldSave |= FillEmptySlotWithItem(Pair.Key);
	}

	if (bShouldSave)
	{
		SaveInventory();
	}
}

bool AShooterPlayerController::SaveInventory()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::SaveInventory()"));
	UWorld* World = GetWorld();
	UShooterGameInstance* GameInstance = World ? World->GetGameInstance<UShooterGameInstance>() : nullptr;

	if (!GameInstance)
	{
		return false;
	}

	UShooterSaveGame* CurrentSaveGame = GameInstance->GetCurrentSaveGame();
	if (CurrentSaveGame)
	{
		// Reset cached data in save game before writing to it
		CurrentSaveGame->AssetDataSource.Reset();
		CurrentSaveGame->SlottedItems.Reset();

		for (const TPair<UShooterItem*, FShooterItemData>& ItemPair : InventoryData)
		{
			FPrimaryAssetId AssetId;

			if (ItemPair.Key)
			{
				AssetId = ItemPair.Key->GetPrimaryAssetId();
				CurrentSaveGame->AssetDataSource.Add(AssetId, ItemPair.Value);
			}
		}

		for (const TPair<FShooterItemSlot, UShooterItem*>& SlotPair : SlottedItems)
		{
			FPrimaryAssetId AssetId;

			if (SlotPair.Value)
			{
				AssetId = SlotPair.Value->GetPrimaryAssetId();
			}
			CurrentSaveGame->SlottedItems.Add(SlotPair.Key, AssetId);
		}

		// Now that cache is updated, write to disk
		GameInstance->WriteSaveGame();
		return true;
	}
	return false;
}

bool AShooterPlayerController::LoadInventory()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::LoadInventory()"));
	InventoryData.Reset();
	SlottedItems.Reset();

	// Fill in slots from game instance
	UWorld* World = GetWorld();
	UShooterGameInstance* GameInstance = World ? World->GetGameInstance<UShooterGameInstance>() : nullptr;

	if (!GameInstance)
	{
		return false;
	}

	for (const TPair<FPrimaryAssetType, int32>& Pair : GameInstance->ItemSlotsPerType)
	{
		for (int32 SlotNumber = 0; SlotNumber < Pair.Value; SlotNumber++)
		{
			SlottedItems.Add(FShooterItemSlot(Pair.Key, SlotNumber), nullptr);
		}
	}

	UShooterSaveGame* CurrentSaveGame = GameInstance->GetCurrentSaveGame();
	UShooterAssetManager& AssetManager = UShooterAssetManager::Get();
	if (CurrentSaveGame)
	{
		// Copy from save game into controller data
		bool bFoundAnySlots = false;
		for (const TPair<FPrimaryAssetId, FShooterItemData>& ItemPair : CurrentSaveGame->AssetDataSource)
		{
			UShooterItem* LoadedItem = AssetManager.ForceLoadItem(ItemPair.Key);

			if (LoadedItem != nullptr)
			{
				InventoryData.Add(LoadedItem, ItemPair.Value);
			}
		}

		for (const TPair<FShooterItemSlot, FPrimaryAssetId>& SlotPair : CurrentSaveGame->SlottedItems)
		{
			if (SlotPair.Value.IsValid())
			{
				UShooterItem* LoadedItem = AssetManager.ForceLoadItem(SlotPair.Value);
				if (GameInstance->IsValidItemSlot(SlotPair.Key) && LoadedItem)
				{
					SlottedItems.Add(SlotPair.Key, LoadedItem);
					bFoundAnySlots = true;
				}
			}
		}

		if (!bFoundAnySlots)
		{
			// Auto slot items as no slots were saved
			FillEmptySlots();
		}

		NotifyInventoryLoaded();

		return true;
	}

	// Load failed but we reset inventory, so need to notify UI
	NotifyInventoryLoaded();

	return false;
}

bool AShooterPlayerController::FillEmptySlotWithItem(UShooterItem* NewItem)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::FillEmptySlotWithItem()"));
	// Look for an empty item slot to fill with this item
	FPrimaryAssetType NewItemType = NewItem->GetPrimaryAssetId().PrimaryAssetType;
	FShooterItemSlot EmptySlot;
	for (TPair<FShooterItemSlot, UShooterItem*>& Pair : SlottedItems)
	{
		if (Pair.Key.ItemType == NewItemType)
		{
			if (Pair.Value == NewItem)
			{
				// Item is already slotted
				return false;
			}
			else if (Pair.Value == nullptr && (!EmptySlot.IsValid() || EmptySlot.SlotNumber > Pair.Key.SlotNumber))
			{
				// We found an empty slot worth filling
				EmptySlot = Pair.Key;
			}
		}
	}

	if (EmptySlot.IsValid())
	{
		SlottedItems[EmptySlot] = NewItem;
		NotifySlottedItemChanged(EmptySlot, NewItem);
		//if (NewItem->ItemType == UShooterAssetManager::WeaponItemType)
		//{
			//AShooterCharacter* pawn = Cast<AShooterCharacter>(GetCharacter());
			//SetSlottedItem(EmptySlot, NewItem);
			//pawn->SpawnInventoryActor(NewItem);
			//pawn->UpdateInventoryActors(NewItem, true);
		//}
		return true;
	}
	return false;
}


void AShooterPlayerController::OnInventoryItemChange_Implementation(UShooterItem* item, bool bAdded)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::OnInventoryItemChange_Implementation()"));
	UE_LOG(LogTemp, Warning, TEXT("Controller::OnInventoryItemChange_Implementation(%s)"), *FString::Printf(TEXT("bAdded = %s"), bAdded == true ? TEXT("true") : TEXT("false")));
	if (item->ItemType == UShooterAssetManager::TokenItemType)
	{
		int32 SoulsCount = GetInventoryItemCount(item);
		OnInventorySoulsChangedNative.Broadcast(SoulsCount);
	}
	if (bAdded)
	{
		AShooterCharacter* pawn = Cast<AShooterCharacter>(GetCharacter());
		if (item->ItemType == UShooterAssetManager::WeaponItemType)
		{
			pawn->UpdateInventoryActors(item, true);
			//pawn->SpawnPropsActors();
		}
	}
	else
	{
		RemoveInventoryItem(item, 1);
	}
}

void AShooterPlayerController::OnItemSlotChanged_Implementation(FShooterItemSlot ItemSlot, UShooterItem* item)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::OnItemSlotChanged_Implementation()"));
	//UE_LOG(LogTemp, Warning, TEXT("Controller::OnInventoryItemChange(%s)"), *FString::Printf(TEXT("bAdded = %s"), bAdded == true ? TEXT("true") : TEXT("false")));
	if (item)
	{
		AShooterCharacter* pawn = Cast<AShooterCharacter>(GetCharacter());
		if (item->ItemType != UShooterAssetManager::TokenItemType)
		{
			//pawn->UpdateInventoryActors(item, true);
		}
	}
	else
	{
		//RemoveInventoryItem(item, 1);
	}
}

void AShooterPlayerController::NotifyInventoryItemChanged(UShooterItem* Item, bool bAdded)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::NotifyInventoryItemChanged()"));
	// Notify native before blueprint
	OnInventoryItemChangedNative.Broadcast(Item, bAdded);
	OnInventoryItemChanged.Broadcast(Item, bAdded);
}

void AShooterPlayerController::NotifySlottedItemChanged(FShooterItemSlot ItemSlot, UShooterItem* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::NotifySlottedItemChanged()"));
	// Notify native before blueprint
	OnSlottedItemChangedNative.Broadcast(ItemSlot, Item);
	OnSlottedItemChanged.Broadcast(ItemSlot, Item);
}

void AShooterPlayerController::NotifyInventoryLoaded()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::NotifyInventoryLoaded()"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Inventory Loaded !"));
	// Notify native before blueprint
	OnInventoryLoadedNative.Broadcast();
	OnInventoryLoaded.Broadcast();
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AShooterPlayerController::Possess(APawn* NewPawn)
{
	Super::Possess(NewPawn);

	// Notify blueprint about the possession, only if it is valid
	// In a network game this would need to be replicated
	if (NewPawn)
	{
		ReceivePossess(NewPawn);
	}
}

void AShooterPlayerController::UnPossess()
{
	APawn* PreviousPawn = GetPawn();

	if (PreviousPawn)
	{
		// Only call if we had one before		
		ReceiveUnPossess(PreviousPawn);
	}

	Super::UnPossess();
}

/************************* Store UI widgets *****************************/

void AShooterPlayerController::ShowStoreWidget()
{	
	if (StoreWidgetClass && !bStoreVisible)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("CreateWidgetInstance()"));
		StoreWidgetIns = CreateWidget<UUserWidget>(this, StoreWidgetClass);
		check(StoreWidgetIns);

		//StoreWidgetIns->

		 if (StoreWidgetIns)
		 {
			 //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("StoreWidgetInstance"));
			 bShowMouseCursor = true;
			 bStoreVisible = true;
			 GEngine->GameViewport->AddViewportWidgetContent(StoreWidgetIns->TakeWidget(), 10);
		 }
	}
	else 
	{
		CloseStoreWidget();
	}
	//UWorld* World = GetWorld();
	//UShooterGameInstance* GameInstance = World ? World->GetGameInstance<UShooterGameInstance>() : nullptr;
}


void AShooterPlayerController::CloseStoreWidget()
{
	if (StoreWidgetIns && bStoreVisible)
	{
		bShowMouseCursor = false;
		bStoreVisible = false;
		GEngine->GameViewport->RemoveViewportWidgetContent(StoreWidgetIns->TakeWidget());
	}
}


bool AShooterPlayerController::SellItem(UShooterItem* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::SellItem()"));
	if (Item)
	{
		int32 Price = GetInventoryItemCount(Item);
		AddSouls(Price);
		return UpdateInventoryItem(Item, false);
	}
	return false;
}

void AShooterPlayerController::AddSouls(int32 Price)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::AddSouls()"));
	AddInventoryItem(SoulsItem, Price);
}

bool AShooterPlayerController::CanPurchaseItem(UShooterItem* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::CanPurchaseItem()"));
	if (!HasItem(Item) && Item->Price >= GetInventoryItemCount(SoulsItem))
	{
		return true;
	}
	return false;
}

bool AShooterPlayerController::PurchaseItem(UShooterItem* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::PurchaseItem()"));
	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_Authority"));
		break;
	case ENetRole::ROLE_AutonomousProxy:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_AutonomousProxy"));
		break;
	case ENetRole::ROLE_SimulatedProxy:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_SimulatedProxy"));
		break;
	default:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_None"));
		break;
	}
	if (Item && CanPurchaseItem(Item))
	{
		if (HasAuthority())
		{
			return UpdateInventoryItem(Item, true);
		}
		else 
		{
			ServerPurchaseItem(Item);
			return true;
		}
	}
	return false;
}

bool AShooterPlayerController::ServerPurchaseItem_Validate(UShooterItem* Item)
{
	return true;
}

void AShooterPlayerController::ServerPurchaseItem_Implementation(UShooterItem* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::ServerPurchaseItem_Implementation()"));
	PurchaseItem(Item);
}

void AShooterPlayerController::FindImageComponents()
{

}

void AShooterPlayerController::LoadAssetsDynamic()
{

}
