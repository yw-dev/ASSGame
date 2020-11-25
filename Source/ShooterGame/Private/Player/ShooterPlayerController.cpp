// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ShooterGame.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "ShooterTypes.h"
#include "ShooterAssetManager.h"
#include "ShooterBlueprintLibrary.h"
#include "Items/ShooterItem.h"
#include "Items/ShooterCategoryItem.h"
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
//static const int32 PlayerAllSlotCount = 13;
//static const int32 PlayerAbilitySlotCount = 5;
//static const int32 PlayerWeaponSlotCount = 2;
//static const int32 PlayerInventorySlotCount = 6;


AShooterPlayerController::AShooterPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AShooterPlayerCameraManager::StaticClass();
	CheatClass = UShooterCheatManager::StaticClass();
	bAllowGameActions = true;
	bGameEndedFrame = false;
	LastDeathLocation = FVector::ZeroVector;

	SlottedItems.Reserve(PLAYER_ALL_SLOT_COUNT);
	AbilitySlot.Reserve(PLAYER_ABILITY_SLOT_COUNT);
	WeaponSlot.Reserve(PLAYER_WEAPON_SLOT_COUNT);
	InventorySlot.Reserve(PLAYER_INVENTORY_SLOT_COUNT);

	ServerSayString = TEXT("Say");
	ShooterFriendUpdateTimer = 0.0f;
	bHasSentStartEvents = false;

	bStoreVisible = false;
	CurrentRespawnDelay = 0.f;
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
	InputComponent->BindAction("ToggleStoreboard", IE_Pressed, this, &AShooterPlayerController::OnToggleStoreboard);
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
	UE_LOG(LogTemp, Warning, TEXT("PlayerController::PostInitializeComponents()"));
	Super::PostInitializeComponents();
	FShooterStyle::Initialize();
	ShooterFriendUpdateTimer = 0;
	//LoadInventory();
	//OnInventoryItemChanged.AddDynamic(this, &AShooterPlayerController::OnInventoryItemChange);
	//OnSlottedItemChanged.AddDynamic(this, &AShooterPlayerController::OnItemSlotChanged);
	OnInventoryItemChangedNative.AddUObject(this, &AShooterPlayerController::OnInventoryItemChange);
	//OnSlottedItemChangedNative.AddUObject(this, &AShooterPlayerController::OnItemSlotChanged);
	InitializPlayerSlots();
}

void AShooterPlayerController::InitializPlayerSlots()
{
	for (int32 i = 0; i < PLAYER_ABILITY_SLOT_COUNT; i++)
	{
		FShooterItemSlot ItemSlot = FShooterItemSlot(UShooterAssetManager::SkillItemType, i);
		SlottedItems.Emplace(ItemSlot, nullptr);
	}
	for (int32 i = 0; i < PLAYER_WEAPON_SLOT_COUNT; i++)
	{
		FShooterItemSlot ItemSlot = FShooterItemSlot(UShooterAssetManager::WeaponItemType, i);
		SlottedItems.Emplace(ItemSlot, nullptr);
	}
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
	if (CurrentRespawnDelay)
	{
		CurrentRespawnDelay -= DeltaTime;
	}
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
	UE_LOG(LogTemp, Warning, TEXT("PlayerController::BeginDestroy()"));
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
	UE_LOG(LogTemp, Warning, TEXT("PlayerController::SetPlayer()"));
	Super::SetPlayer( InPlayer );

	if (ULocalPlayer* const LocalPlayer = Cast<ULocalPlayer>(Player))
	{
		//Build menu only after game is initialized
		ShooterIngameMenu = MakeShareable(new FShooterIngameMenu());
		ShooterIngameMenu->Construct(Cast<ULocalPlayer>(Player));
		ShooterShopMenu = MakeShareable(new FShooterShopMenu());
		ShooterShopMenu->Construct(Cast<ULocalPlayer>(Player));
		
		//ShowPlayerDashboard();
		//ShowPlayerTarget();
		//ShowTeamBar();

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
	UE_LOG(LogTemp, Warning, TEXT("PlayerController::OnLeaderboardReadComplete()"));
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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Controller::UnFreeze(CurrentDelay=%f, MinDelay=%f)"), GetCurrentRespawnDelay(), GetMinRespawnDelay()));
	if (CurrentRespawnDelay > 0)
	{
		CurrentRespawnDelay--;
	}
	else
	{
		ServerRestartPlayer();
	}
}

void AShooterPlayerController::BeginInactiveState()
{
	CurrentRespawnDelay = GetMinRespawnDelay();
	Super::BeginInactiveState();
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

/*
void AShooterPlayerController::OnInventoryMessage(class AShooterPlayerState* OwnerPlayerState, const UShooterItem* Item, bool bAdd)
{
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if (ShooterHUD)
	{
		ShooterHUD->RefreshInventoryByMessage(OwnerPlayerState, Item, bAdd);
	}
}*/

void AShooterPlayerController::OnCoinsIncomeMessage(class AShooterPlayerState* KillerPlayerState, class AShooterPlayerState* KilledPlayerState, const UDamageType* KillerDamageType)
{
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if (ShooterHUD)
	{
		ShooterHUD->RefreshInventoryWidget(KillerPlayerState, KilledPlayerState, KillerDamageType);
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

void AShooterPlayerController::SetRespawnDelay(float InDelay)
{
	CurrentRespawnDelay = InDelay;
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
		//ShooterHUD->ShowPlayerDashboard();
		//ShooterHUD->ShowTeambar();
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

void AShooterPlayerController::ClientReceivePurchaseFailure_Implementation(const FText& Message, class UShooterItem* NewItem)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::ClientReceivePurchaseFailer_Implementation()"));
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if (ShooterHUD)
	{
		ShooterHUD->ShowPurchaseFailureMessage(Message, NewItem);
	}
}

void AShooterPlayerController::ClientReceivePurchaseEvent_Implementation(const FShooterItemSlot& NewItemSlot, const FShooterItemData& NewItemData, class UShooterItem* NewItem, bool bAdd)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::ClientReceivePurchaseEvent_Implementation()"));
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if (ShooterHUD)
	{
		ShooterHUD->RefreshInventoryByMessage(NewItemSlot, NewItemData, NewItem, bAdd);
	}
}

void AShooterPlayerController::ClientReceivePlayerStateChangeEvent_Implementation(float InHealth, float InMaxHealth, float InRestoreHealth)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::ClientReceivePlayerStateChangeEvent_Implementation()"));
	AShooterHUD* ShooterHUD = GetShooterHUD();
	if (ShooterHUD)
	{
		ShooterHUD->RefreshAbilityWidget(InHealth, InMaxHealth, InRestoreHealth);
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

	DOREPLIFETIME(AShooterPlayerController, bHealthRegen);
	DOREPLIFETIME(AShooterPlayerController, bGodMode);
	DOREPLIFETIME(AShooterPlayerController, CurrentRespawnDelay);

}
/*
void AShooterPlayerController::OnRep_PurchaseItem(UShooterItem* NewPurchaseItem)
{
	SetCurrentPurchaseItem(NewPurchaseItem);
}

void AShooterPlayerController::SetCurrentPurchaseItem(UShooterItem* NewPurchaseItem)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::SetCurrentPurchaseItem()"));
	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_Authority"));
		UE_LOG(LogTemp, Warning, TEXT("SetCurrentPurchaseItem On  ROLE_Authority"));
		break;
	case ENetRole::ROLE_AutonomousProxy:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_AutonomousProxy"));
		UE_LOG(LogTemp, Warning, TEXT("SetCurrentPurchaseItem On  ROLE_AutonomousProxy"));
		break;
	case ENetRole::ROLE_SimulatedProxy:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_SimulatedProxy"));
		UE_LOG(LogTemp, Warning, TEXT("SetCurrentPurchaseItem On  ROLE_SimulatedProxy"));
		break;
	default:
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("On  ROLE_None"));
		UE_LOG(LogTemp, Warning, TEXT("SetCurrentPurchaseItem On  ROLE_None"));
		break;
	}
	CurrentPurchaseItem = NewPurchaseItem;

	UpdateInventoryItem(CurrentPurchaseItem, true);
	//if (IsLocalController())
	//{
	//	NotifyWeaponSlotChanged(CurrentPurchaseItem, true);
	//}
}*/

bool AShooterPlayerController::CanPurchaseItem(UShooterItem* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::CanPurchaseItem()"));
	/*
	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		UE_LOG(LogOnline, Warning, TEXT("On  ROLE_Authority."));
		break;
	case ENetRole::ROLE_AutonomousProxy:
		UE_LOG(LogTemp, Warning, TEXT("On  ROLE_AutonomousProxy."));
		break;
	}*/
	if (IsSlotSpaceEnough(Item))
	{
		return true;
	}
	return false;
}

bool AShooterPlayerController::HasItem(UShooterItem* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::CanPurchaseItem()"));
	/*
	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		UE_LOG(LogOnline, Warning, TEXT("On  ROLE_Authority."));
		break;
	case ENetRole::ROLE_AutonomousProxy:
		UE_LOG(LogTemp, Warning, TEXT("On  ROLE_AutonomousProxy."));
		break;
	}*/
	const FShooterItemSlot* FoundItem = SlottedItems.FindKey(Item);

	if (FoundItem)
	{
		return true;
	}
	return false;
}

bool AShooterPlayerController::IsCoinsEnough(UShooterItem* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::IsCoinsEnough()"));
	/*
	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		UE_LOG(LogOnline, Warning, TEXT("On  ROLE_Authority."));
		break;
	case ENetRole::ROLE_AutonomousProxy:
		UE_LOG(LogTemp, Warning, TEXT("On  ROLE_AutonomousProxy."));
		break;
	}*/
	if (Item->Price >= GetInventoryItemCount(SoulsItem))
	{
		return true;
	}
	const FText ReturnReason = NSLOCTEXT("PurchaseFailures", "Coins", "Coins Not Enough.");
	ClientReceivePurchaseFailure(ReturnReason, Item);
	return false;
}

bool AShooterPlayerController::IsSlotSpaceEnough(UShooterItem* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::IsCoinsEnough()"));
	/*
	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		UE_LOG(LogOnline, Warning, TEXT("On  ROLE_Authority."));
		break;
	case ENetRole::ROLE_AutonomousProxy:
		UE_LOG(LogTemp, Warning, TEXT("On  ROLE_AutonomousProxy."));
		break;
	}*/
	if (HasItem(Item) == false || IsPluralItems(Item) == true)
	{
		if (Item->ItemType == UShooterAssetManager::SkillItemType && AbilitySlot.Num() < PLAYER_ABILITY_SLOT_COUNT)
		{
			return true;
		}
		else if (Item->ItemType == UShooterAssetManager::WeaponItemType && WeaponSlot.Num() < PLAYER_WEAPON_SLOT_COUNT)
		{
			return true;
		}
		else if(Item->ItemType != UShooterAssetManager::WeaponItemType && Item->ItemType != UShooterAssetManager::SkillItemType && InventorySlot.Num() < PLAYER_INVENTORY_SLOT_COUNT || Item->ItemType == UShooterAssetManager::PotionItemType)
		{
			return true;
		}
		const FText ReturnReason = NSLOCTEXT("PurchaseFailures", "SlotSpace", "玩家可用插槽数量不足.");
		ClientReceivePurchaseFailure(ReturnReason, Item);
		return false;
	}
	const FText ReturnReason = NSLOCTEXT("PurchaseFailures", "SlotSpace", "非药品类道具不可重复购买.");
	ClientReceivePurchaseFailure(ReturnReason, Item);
	return false;
}

bool AShooterPlayerController::IsPluralItems(UShooterItem* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::IsPluralItems()"));
	/*
	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		UE_LOG(LogOnline, Warning, TEXT("On  ROLE_Authority."));
		break;
	case ENetRole::ROLE_AutonomousProxy:
		UE_LOG(LogTemp, Warning, TEXT("On  ROLE_AutonomousProxy."));
		break;
	}*/
	if (Item->ItemType == UShooterAssetManager::PotionItemType)
	{
		return true;
	}
	return false;
}

void AShooterPlayerController::PurchaseItem(UShooterItem* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::PurchaseItem()"));
	/*
	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		UE_LOG(LogOnline, Warning, TEXT("On  ROLE_Authority."));
		break;
	case ENetRole::ROLE_AutonomousProxy:
		UE_LOG(LogTemp, Warning, TEXT("On  ROLE_AutonomousProxy."));
		break;
	}*/
	if (Item && IsInState(NAME_Playing))
	{
		if (HasAuthority())
		{
			if (CanPurchaseItem(Item)) 
			{
				//SetCurrentPurchaseItem(Item);
				UpdateInventoryItem(Item, true);
			}
		}
		else
		{
			ServerPurchase(Item);
		}
		//if (GetNetMode() == NM_Client && Role != ROLE_Authority)
		//{
			//AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
			//ShooterPlayerState->InformAboutInventory(ShooterPlayerState, NewItem, bAdd);
		//}
	}
}

bool AShooterPlayerController::ServerPurchase_Validate(UShooterItem* Item)
{
	return true;
}

void AShooterPlayerController::ServerPurchase_Implementation(UShooterItem* Item)
{
	PurchaseItem(Item);
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

float AShooterPlayerController::GetCurrentRespawnDelay() const
{
	return CurrentRespawnDelay;
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

void AShooterPlayerController::NotifyHPChanged(float InHealth, float InMaxHealth, float InRestoreHealth)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::NotifyHPChanged()"));
	//if (PlayerDashboard)
	//{
	//	PlayerDashboard->GetAbilityWidget()->HPChangedDelegate.Broadcast(InHealth, InMaxHealth, InRestoreHealth);
	//}
}

void AShooterPlayerController::NotifyMPChanged(float InMana, float InMaxMana, float InRestoreMana)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::NotifyMPChanged()"));
	/*if (PlayerDashboard)
	{
		PlayerDashboard->GetAbilityWidget()->MPChangedDelegate.Broadcast(InMana, InMaxMana, InRestoreMana);
	}*/
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

void AShooterPlayerController::UpdateInventoryItem(UShooterItem* NewItem, bool bAdd)
{
	//const TMap<UShooterItem*, FShooterItemData>& InventorySlot = GetInventoryDataMap();
	UE_LOG(LogTemp, Warning, TEXT("Controller::UpdateInventoryItem( bAdd = %s)"), bAdd==true?TEXT("true"):TEXT("false"));
	if (!NewItem)
	{
		return;
	}
	if (NewItem->ItemType == UShooterAssetManager::TokenItemType)
	{
		int32 SoulsCount = GetInventoryItemCount(NewItem);
		OnInventorySoulsChangedNative.Broadcast(SoulsCount);
	}
	if (bAdd)
	{
		AddInventoryItem(NewItem, 1, 1, true);

		AShooterCharacter* AC = Cast<AShooterCharacter>(GetCharacter());
		if (NewItem->ItemType == UShooterAssetManager::WeaponItemType)
		{
			AC->UpdateInventoryActors(NewItem, true);
			//pawn->SpawnPropsActors();
		}
	}
	else
	{
		RemoveInventoryItem(NewItem, 0);
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
		AddItemToSlottedItems(NewData, NewItem, ItemCount);
		NotifyInventoryItemChanged(NewItem, true);
		bChanged = true;
	}

	if (bAutoSlot)
	{
		// Slot item if required
		//bChanged |= FillEmptySlotWithItem(NewItem);
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
	//RemoveItemFromSlottedItems(NewData, RemovedItem);
	// If we got this far, there is a change so notify and save
	NotifyInventoryItemChanged(RemovedItem, false);

	SaveInventory();
	return true;
}

void AShooterPlayerController::AddItemToSlottedItems(FShooterItemData NewData, UShooterItem* NewItem, int32 ItemCount)
{
	FShooterItemSlot CurrentSlot;
	AShooterPlayerState* ShooterPlayerState = Cast<AShooterPlayerState>(PlayerState);
	if (NewItem->ItemType == UShooterAssetManager::WeaponItemType)
	{
		WeaponSlot.Add(NewItem, NewData);
		//ShooterPlayerState->AddWeaponItems(NewData, NewItem);
		CurrentSlot = FShooterItemSlot(NewItem->ItemType, WeaponSlot.Num() - 1);
		UE_LOG(LogTemp, Warning, TEXT("Controller::AddItemToSlottedItems(WeaponSlot.Num = %d)"), WeaponSlot.Num());
		SlottedItems[CurrentSlot] = NewItem;
	}
	else if (NewItem->ItemType == UShooterAssetManager::SkillItemType)
	{
		AbilitySlot.Add(NewItem, NewData);
		CurrentSlot = FShooterItemSlot(NewItem->ItemType, AbilitySlot.Num() - 1);
		UE_LOG(LogTemp, Warning, TEXT("Controller::AddItemToSlottedItems(AbilitySlot.Num = %d)"), AbilitySlot.Num());
		SlottedItems[CurrentSlot] = NewItem;
	}
	else
	{
		// If data changed, need to update storage and call callback
		InventorySlot.Add(NewItem, NewData);
		int32 Count = InventorySlot.Num() - 1;
		CurrentSlot = FShooterItemSlot(NewItem->ItemType, Count);
		UE_LOG(LogTemp, Warning, TEXT("Controller::AddItemToSlottedItems(InventorySlot.Num = %d)"), InventorySlot.Num());
		SlottedItems.Add(CurrentSlot, NewItem);
	}
	ClientReceivePurchaseEvent(CurrentSlot, NewData, NewItem, true);
}

void AShooterPlayerController::RemoveItemFromSlottedItems(FShooterItemData NewData, UShooterItem* RemovedItem)
{
	if (NewData.ItemCount > 0)
	{
		// Update data with new count
		InventorySlot.Add(RemovedItem, NewData);
		if (RemovedItem->ItemType == UShooterAssetManager::WeaponItemType)
		{
			WeaponSlot.Add(RemovedItem, NewData);
			UE_LOG(LogTemp, Warning, TEXT("Controller::RemoveItemFromSlottedItems(WeaponSlot.Num = %d)"), WeaponSlot.Num());
		}
		else if (RemovedItem->ItemType == UShooterAssetManager::SkillItemType)
		{
			AbilitySlot.Add(RemovedItem, NewData);
			UE_LOG(LogTemp, Warning, TEXT("Controller::RemoveItemFromSlottedItems(AbilitySlot.Num = %d)"), AbilitySlot.Num());
		}
		else
		{
			// If data changed, need to update storage and call callback
			InventorySlot.Add(RemovedItem, NewData);
			UE_LOG(LogTemp, Warning, TEXT("Controller::RemoveItemFromSlottedItems(InventorySlot.Num = %d)"), InventorySlot.Num());
		}
	}
	else
	{
		// Remove item entirely, make sure it is unslotted
		if (RemovedItem->ItemType == UShooterAssetManager::WeaponItemType)
		{
			WeaponSlot.Remove(RemovedItem);
			UE_LOG(LogTemp, Warning, TEXT("Controller::RemoveItemFromSlottedItems(WeaponSlot.Num = %d)"), WeaponSlot.Num());
		}
		else if (RemovedItem->ItemType == UShooterAssetManager::SkillItemType)
		{
			AbilitySlot.Remove(RemovedItem);
			UE_LOG(LogTemp, Warning, TEXT("Controller::RemoveItemFromSlottedItems(AbilitySlot.Num = %d)"), AbilitySlot.Num());
		}
		else
		{
			// If data changed, need to update storage and call callback
			InventorySlot.Remove(RemovedItem);
			UE_LOG(LogTemp, Warning, TEXT("Controller::RemoveItemFromSlottedItems(InventorySlot.Num = %d)"), InventorySlot.Num());
		}
		for (TPair<FShooterItemSlot, UShooterItem*>& Pair : SlottedItems)
		{
			if (Pair.Value == RemovedItem)
			{
				Pair.Value = nullptr;
				NotifySlottedItemChanged(Pair.Key, Pair.Value);
				ClientReceivePurchaseEvent(Pair.Key, NewData, RemovedItem, false);
			}
		}
	}
}

TArray<UShooterItem*> AShooterPlayerController::GetInventoryItemList(FPrimaryAssetType ItemType) const
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetInventoryItemList(InventorySlot.Num : %d)"), InventorySlot.Num());
	TArray<UShooterItem*> Items;
	Items.Reset(10);
	for (const TPair<UShooterItem*, FShooterItemData>& Pair : InventorySlot)
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
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetInventoryItemListCount(InventorySlot.Num : %d)"), InventorySlot.Num());
	TArray<UShooterItem*> ItemList;
	for (const TPair<UShooterItem*, FShooterItemData>& Pair : InventorySlot)
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
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetInventoryItems(InventorySlot.Num : %d)"), InventorySlot.Num());
	for (const TPair<UShooterItem*, FShooterItemData>& Pair : InventorySlot)
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

int32 AShooterPlayerController::GetInventoryItemCount(UShooterItem* Item) const
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetInventoryItemCount(InventorySlot.Num : %d)"), InventorySlot.Num());
	const FShooterItemData* FoundItem = InventorySlot.Find(Item);

	if (FoundItem)
	{
		return FoundItem->ItemCount;
	}
	return 0;
}

bool AShooterPlayerController::GetInventoryItemData(UShooterItem* Item, FShooterItemData& ItemData) const
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetInventoryItemData(InventorySlot.Num : %d)"), InventorySlot.Num());
	if (InventorySlot.Num()>0)
	{
		const FShooterItemData* FoundItem = InventorySlot.Find(Item);

		if (FoundItem)
		{
			ItemData = *FoundItem;
			UE_LOG(LogTemp, Warning, TEXT("Controller::GetInventoryItemData(ItemData.ItemCount : %d)"), ItemData.ItemCount);
			return true;
		}
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
	for (const TPair<UShooterItem*, FShooterItemData>& Pair : InventorySlot)
	{
		bShouldSave |= FillEmptySlotWithItem(Pair.Key);
	}

	if (bShouldSave)
	{
		SaveInventory();
	}
}

/*
const TArray<TSharedPtr<FShooterAssetEntry>>* AShooterPlayerController::GetStoreCategory()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetStoreCategory()"));
	TArray<UShooterItem*> FindItems;
	Categories.Reset();
	FShooterAssetEntry AssetEntry;
	FPrimaryAssetId AssetId = FPrimaryAssetId(UShooterAssetManager::CategoryItemType, UShooterAssetManager::CategoryItemType.GetName());
	FindStoreAssetsByID(AssetId, FindItems);
	if (FindItems.Num()>0)
	{	
		for (UShooterItem* Item : FindItems)
		{
			//TSharedPtr<FSlateBrush*> IconBrush = MakeShareable(new FShooterImageBrush(Item->IconPath, FVector2D(64, 64)));
			UE_LOG(LogTemp, Warning, TEXT("Controller::GetStoreCategory(Item.Name = %s)"), &Item->ItemName.ToString());
			AssetEntry.Init(AssetId, Item->ItemName, Item->ItemName, Item->IconPath, nullptr, FShooterImageBrush(Item->IconPath, FVector2D(64, 64)), FShooterImageBrush(Item->IconPath, FVector2D(64, 64)));
			
			//UE_LOG(LogTemp, Warning, TEXT("Controller::GetStoreCategory(AssetEntry.Name = %s)"), AssetEntry.Name.ToString());
			//Categories.Add(MakeShareable(AssetEntry));
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetStoreCategory(Categories.Num = %d)"), Categories.Num());
	return &Categories;
}
*/
bool AShooterPlayerController::FindStoreAssetsByID(FPrimaryAssetId InAssetId, TArray<UShooterItem*>& OutItems)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::FindStoreAssetsByID()"));

	// Fill in slots from game instance
	UWorld* World = GetWorld();
	UShooterGameInstance* GameInstance = World ? World->GetGameInstance<UShooterGameInstance>() : nullptr;

	if (!GameInstance)
	{
		return false;
	}

	UShooterSaveGame* CurrentSaveGame = GameInstance->GetCurrentSaveGame();
	UShooterAssetManager& AssetManager = UShooterAssetManager::Get();
	if (CurrentSaveGame)
	{
		// Copy from save game into controller data
		for (auto AssetEntry : CurrentSaveGame->AssetSources)
		{
			if (AssetEntry.AssetCategory.PrimaryAssetType == InAssetId.PrimaryAssetType)
			{
				TArray<class UShooterItem*> LoadedItems = AssetEntry.AssetItems;
				for (auto Item : LoadedItems)
				{
					UShooterItem* LoadedItem = AssetManager.ForceLoadItem(Item->GetPrimaryAssetId());

					if (LoadedItem != nullptr)
					{
						OutItems.Add(LoadedItem);
					}
				}
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("Controller::FindStoreAssetsByID(OutItems.Num = %d)"), OutItems.Num());
		return true;
	}

	return false;
}

/*
bool AShooterPlayerController::LoadStoreItemSource()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::LoadStoreItemSource()"));

	// Fill in slots from game instance
	UWorld* World = GetWorld();
	UShooterGameInstance* GameInstance = World ? World->GetGameInstance<UShooterGameInstance>() : nullptr;

	if (!GameInstance)
	{
		return false;
	}

	UShooterSaveGame* CurrentSaveGame = GameInstance->GetCurrentSaveGame();
	UShooterAssetManager& AssetManager = UShooterAssetManager::Get();
	if (CurrentSaveGame)
	{
		// Copy from save game into controller data
	/*	for (auto AssetEntry : CurrentSaveGame->AssetSources)
		{
			if (AssetEntry.AssetCategory.PrimaryAssetType == InAssetId.PrimaryAssetType)
			{
				TArray<class UShooterItem*> LoadedItems = AssetEntry.AssetItems;
				for (auto Item : LoadedItems)
				{
					UShooterItem* LoadedItem = AssetManager.ForceLoadItem(Item->GetPrimaryAssetId());

					if (LoadedItem != nullptr)
					{
						OutItems.Add(LoadedItem);
					}
				}
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("Controller::FindStoreAssetsByID(OutItems.Num = %d)"), OutItems.Num());
		return true;
		
	}

	return false;
}*/

bool AShooterPlayerController::SaveStore()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::SaveStore()"));

	return true;
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

		for (const TPair<UShooterItem*, FShooterItemData>& ItemPair : InventorySlot)
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
	//InventorySlot.Reset();
	//SlottedItems.Reset();

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
				InventorySlot.Add(LoadedItem, ItemPair.Value);
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
		FShooterItemSlot itemSlot = Pair.Key;
		if (itemSlot.ItemType == NewItemType)
		{
			if (Pair.Value == NewItem)
			{
				// Item is already slotted
				return false;
			}
			else if (Pair.Value == nullptr && (!EmptySlot.IsValid() || EmptySlot.SlotNumber > itemSlot.SlotNumber))
			{
				// We found an empty slot worth filling
				EmptySlot = itemSlot;
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

void AShooterPlayerController::OnInventoryItemChange(UShooterItem* item, bool bAdded)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::OnInventoryItemChange_Implementation(%s)"), *FString::Printf(TEXT("bAdded = %s"), bAdded == true ? TEXT("true") : TEXT("false")));
	//if (CurrentPurchaseItem)
	//{
		//UpdateInventoryItem(CurrentPurchaseItem, bAdded);
	//}
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
	UE_LOG(LogTemp, Warning, TEXT("Controller::Possess()"));
	Super::Possess(NewPawn);

	// Notify blueprint about the possession, only if it is valid
	// In a network game this would need to be replicated
	if (NewPawn)
	{
		ReceivePossess(NewPawn);
	}

}

void AShooterPlayerController::ReceivePossess_Implementation(APawn* NewPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::ReceivePossess_Implementation()"));

	//ShowPlayerDashboard();
}

void AShooterPlayerController::UnPossess()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::UnPossess()"));
	APawn* PreviousPawn = GetPawn();

	if (PreviousPawn)
	{
		// Only call if we had one before		
		ReceiveUnPossess(PreviousPawn);
	}

	Super::UnPossess();
}

void AShooterPlayerController::ReceiveUnPossess_Implementation(APawn* NewPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::ReceiveUnPossess_Implementation()"));

}

/************************* HUD UI widgets *****************************/
/*
UShooterPlayerView* AShooterPlayerController::GetPlayerDashboard() const
{
	//check(PlayerDashboardContainer);
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetPlayerDashboard()"));
	if (PlayerDashboard)
	{
		return PlayerDashboard;
	}
	return nullptr;
}

UShooterTargetPlayer* AShooterPlayerController::GetPlayerTarget() const
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetPlayerTarget()"));
	if (PlayerTarget)
	{
		return PlayerTarget;
	}
	return nullptr;
}

UShooterTeamBar* AShooterPlayerController::GetTeamBar() const
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::GetTeamBar()"));
	if (TeamBar)
	{
		return TeamBar;
	}
	return nullptr;
}

void AShooterPlayerController::ShowPlayerDashboard()
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::ShowPlayerDashboard()"));
	
	if (!PlayerBoardWidgetClass)
	{
		return;
	}

	PlayerDashboard = CreateWidget<UShooterPlayerView>(GetWorld(), PlayerBoardWidgetClass);
	PlayerDashboard->SetOwningPlayer(this);

	//PlayerDashboard->GetInventoryWidget()->WeaponSlotChangedDelegate.AddDynamic(this, &AShooterPlayerController::OnInventoryItemChange);

	PlayerDashboard->InitWidget();

	if (PlayerDashboard != nullptr)
	{
		SAssignNew(PlayerboardOverlay, SOverlay)
			+ SOverlay::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.VAlign(EVerticalAlignment::VAlign_Bottom)
			[
				SNew(SBox)
				.HeightOverride(230)
				.WidthOverride(768)
				[
					PlayerDashboard->TakeWidget()
				]
			];
	}
	if (PlayerboardOverlay)
	{
		GEngine->GameViewport->AddViewportWidgetForPlayer(GetLocalPlayer(), PlayerboardOverlay.ToSharedRef(), 1);
		//GEngine->GameViewport->AddViewportWidgetContent(PlayerboardOverlay.ToSharedRef(), 1);
	}
}

void AShooterPlayerController::ShowTeamBar()
{
	if (!TeamBarWidgetClass)
	{
		return;
	}

	TeamBar = CreateWidget<UShooterTeamBar>(this, TeamBarWidgetClass);

	//check(TeamBar);

	SAssignNew(TeamBarOverlay, SOverlay)
	+ SOverlay::Slot()
	.HAlign(EHorizontalAlignment::HAlign_Center)
	.VAlign(EVerticalAlignment::VAlign_Top)
	[
		SNew(SBox)
		.WidthOverride(820)
		.HeightOverride(80)
		[
			TeamBar->TakeWidget()
		]
	];

	if (TeamBarOverlay)
	{
		GEngine->GameViewport->AddViewportWidgetContent(TeamBarOverlay.ToSharedRef(), 1);
	}
}

void AShooterPlayerController::ShowPlayerTarget()
{
	if (!PlayerTargetWidgetClass)
	{
		return;
	}

	PlayerTarget = CreateWidget<UShooterTargetPlayer>(this, PlayerTargetWidgetClass);

	//check(PlayerTarget);

	SAssignNew(PlayerTargetOverlay, SOverlay)
	+ SOverlay::Slot()
	.HAlign(EHorizontalAlignment::HAlign_Left)
	.VAlign(EVerticalAlignment::VAlign_Center)
	[
		SNew(SBox)
		.WidthOverride(200)
		.HeightOverride(230)
		.Padding(10)
		[
			PlayerTarget->TakeWidget()
		]
	];

	if (PlayerTargetOverlay)
	{
		GEngine->GameViewport->AddViewportWidgetContent(PlayerTargetOverlay.ToSharedRef(), 1);
	}
}*/

void AShooterPlayerController::OnToggleStoreboard()
{
	if (StoreWidgetClass && !bStoreVisible)
	{
		TArray<UShooterItem*> FindItems;
		TArray<UShooterItem*> ContentItems;
		FPrimaryAssetId AssetId = FPrimaryAssetId(UShooterAssetManager::CategoryItemType, UShooterAssetManager::CategoryItemType.GetName());
		FindStoreAssetsByID(AssetId, FindItems);
		UShooterCategoryItem* CategoryItem = Cast<UShooterCategoryItem>(FindItems[0]);
		FPrimaryAssetId CategoryId = FPrimaryAssetId(CategoryItem->Category.Type, CategoryItem->Category.Type.GetName());
		FindStoreAssetsByID(CategoryId, ContentItems);

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("CreateWidgetInstance()"));
		const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
		FVector2D ViewMargin = ViewportSize / 8;

		StoreBoard = CreateWidget<UShooterStore>(this, StoreWidgetClass);

		check(StoreBoard);
		StoreBoard->SetCategoryItem(FindItems);
		StoreBoard->SetCurrentCategory(FindItems[0]);
		StoreBoard->SetContentItem(ContentItems);
		StoreBoard->SetCurrentContent(ContentItems[0]);
		StoreBoard->SetSelectedContentCategory(FindItems[0]);

		StoreboardContainer = SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			.VAlign(EVerticalAlignment::VAlign_Fill)
			.Padding(ViewMargin)
			[
				StoreBoard->TakeWidget()
			];


		if (StoreboardContainer)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("StoreWidgetInstance"));
			bShowMouseCursor = true;
			bStoreVisible = true;
			GEngine->GameViewport->AddViewportWidgetContent(StoreboardContainer.ToSharedRef(), 10);
		}
	}
	else
	{
		CloseStoreBoard();
	}
}

void AShooterPlayerController::CloseStoreBoard()
{
	check(StoreBoard);
	if (bStoreVisible)
	{
		StoreBoard->DeSelectedAll();
		bShowMouseCursor = false;
		bStoreVisible = false;
		GEngine->GameViewport->RemoveViewportWidgetContent(StoreboardContainer.ToSharedRef());
	}
}


void AShooterPlayerController::SellItem(UShooterItem* Item)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::SellItem()"));
	if (Item)
	{
		int32 Price = GetInventoryItemCount(Item);
		AddSouls(Price);
		UpdateInventoryItem(Item, false);
	}
}

void AShooterPlayerController::AddSouls(int32 Price)
{
	UE_LOG(LogTemp, Warning, TEXT("Controller::AddSouls()"));
	AddInventoryItem(SoulsItem, Price);
}

void AShooterPlayerController::FindImageComponents()
{

}

void AShooterPlayerController::LoadAssetsDynamic()
{

}
