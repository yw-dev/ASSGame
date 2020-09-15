// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "UObject/PrimaryAssetId.h"
#include "Brushes/SlateDynamicImageBrush.h"
#include "ShooterTypes.generated.h"

#pragma once

namespace EShooterMatchState
{
	enum Type
	{
		Warmup,
		Playing,
		Won,
		Lost,
	};
}

namespace EShooterCrosshairDirection
{
	enum Type
	{
		Left=0,
		Right=1,
		Top=2,
		Bottom=3,
		Center=4
	};
}

namespace EShooterHudPosition
{
	enum Type
	{
		Left=0,
		FrontLeft=1,
		Front=2,
		FrontRight=3,
		Right=4,
		BackRight=5,
		Back=6,
		BackLeft=7,
	};
}

UENUM(BlueprintType)
enum class EShooterWeaponType : uint8
{
	None UMETA(DisplayName = "无"),
	Wand UMETA(DisplayName = "法杖"),
	Sword UMETA(DisplayName = "剑"),
	Axe UMETA(DisplayName = "斧"),
	Gun UMETA(DisplayName = "枪械")
};


UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EBullet UMETA(DisplayName = "子弹"),
	ERocket UMETA(DisplayName = "火箭弹"),
	EMax,
};

/*
UENUM(BlueprintType)
enum class EPawnState : uint8
{
	Idle UMETA(DisplayName = "闲置"),
	Firing UMETA(DisplayName = "发起攻击"),
	Reloading UMETA(DisplayName = "装填弹药"),
	Equipping UMETA(DisplayName = "切换武器")
};
*/

/** Defines categories for Game sources. */
UENUM()
enum class EShooterSourceCategory : uint8
{
	Token UMETA(DisplayName = "货币"),
	Skill UMETA(DisplayName = "技能"),
	Potion UMETA(DisplayName = "药品"),
	Equipment UMETA(DisplayName = "装备"),
	Unknown
};


/** keep in sync with ShooterImpactEffect */
UENUM()
namespace EShooterPhysMaterialType
{
	enum Type
	{
		Unknown,
		Concrete,
		Dirt,
		Water,
		Metal,
		Wood,
		Grass,
		Glass,
		Flesh,
	};
}

namespace EShooterDialogType
{
	enum Type
	{
		None,
		Generic,
		ControllerDisconnected
	};
}

namespace EShooterMenuItemType
{
	enum Type
	{
		Root,
		Standard,
		MultiChoice,
		CustomWidget,
	};
}

namespace EShooterCustomWidget
{
	enum Type
	{
		None,
		Options,
		Save,
		Server,
		Main,
	};
}

#define SHOOTER_SURFACE_Default		SurfaceType_Default
#define SHOOTER_SURFACE_Concrete	SurfaceType1
#define SHOOTER_SURFACE_Dirt		SurfaceType2
#define SHOOTER_SURFACE_Water		SurfaceType3
#define SHOOTER_SURFACE_Metal		SurfaceType4
#define SHOOTER_SURFACE_Wood		SurfaceType5
#define SHOOTER_SURFACE_Grass		SurfaceType6
#define SHOOTER_SURFACE_Glass		SurfaceType7
#define SHOOTER_SURFACE_Flesh		SurfaceType8


struct FShooterImageBrush : public FSlateDynamicImageBrush, public FGCObject
{
	FShooterImageBrush(const FName InTextureName, const FVector2D& InImageSize)
		: FSlateDynamicImageBrush(InTextureName, InImageSize)
	{
		SetResourceObject(LoadObject<UObject>(NULL, *InTextureName.ToString()));
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector)
	{
		FSlateBrush::AddReferencedObjects(Collector);
	}
};


USTRUCT(BlueprintType)
struct FPlayerAnim
{
	GENERATED_USTRUCT_BODY()

	/** animation played on pawn (1st person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Pawn1P;

	/** animation played on pawn (3rd person view) */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* Pawn3P;

	FPlayerAnim()
		: Pawn1P(nullptr)
		, Pawn3P(nullptr)
	{

	}

};

USTRUCT(BlueprintType)
struct FDecalData
{
	GENERATED_USTRUCT_BODY()

	/** material */
	UPROPERTY(EditDefaultsOnly, Category=Decal)
	UMaterial* DecalMaterial;

	/** quad size (width & height) */
	UPROPERTY(EditDefaultsOnly, Category=Decal)
	float DecalSize;

	/** lifespan */
	UPROPERTY(EditDefaultsOnly, Category=Decal)
	float LifeSpan;
	
	/** defaults */
	FDecalData()
		: DecalSize(256.f)
		, LifeSpan(10.f)
	{
	}

};

/** replicated information on a hit we've taken */
USTRUCT(BlueprintType)
struct FTakeHitInfo
{
	GENERATED_USTRUCT_BODY()

	/** The amount of damage actually applied */
	UPROPERTY()
	float ActualDamage;

	/** The damage type we were hit with. */
	UPROPERTY()
	UClass* DamageTypeClass;

	/** Who hit us */
	UPROPERTY()
	TWeakObjectPtr<class AShooterCharacter> PawnInstigator;

	/** Who actually caused the damage */
	UPROPERTY()
	TWeakObjectPtr<class AActor> DamageCauser;

	/** Specifies which DamageEvent below describes the damage received. */
	UPROPERTY()
	int32 DamageEventClassID;

	/** Rather this was a kill */
	UPROPERTY()
	uint32 bKilled:1;

private:

	/** A rolling counter used to ensure the struct is dirty and will replicate. */
	UPROPERTY()
	uint8 EnsureReplicationByte;

	/** Describes general damage. */
	UPROPERTY()
	FDamageEvent GeneralDamageEvent;

	/** Describes point damage, if that is what was received. */
	UPROPERTY()
	FPointDamageEvent PointDamageEvent;

	/** Describes radial damage, if that is what was received. */
	UPROPERTY()
	FRadialDamageEvent RadialDamageEvent;

public:
	FTakeHitInfo();

	FDamageEvent& GetDamageEvent();
	void SetDamageEvent(const FDamageEvent& DamageEvent);
	void EnsureReplication();
};

USTRUCT()
struct FShooterGameAsset
{
	GENERATED_BODY()

	/*
	FShooterGameAsset()
	{ }

	FShooterGameAsset(const FPrimaryAssetId& InAssetId, const TArray<class UShooterItem*>& InAssetItems)
		: AssetCategory(InAssetId)
		, AssetItems(InAssetItems)
	{ }*/

	/** This identifies an object as a "primary" asset that can be searched for by the AssetManager and used in various tools */
	UPROPERTY()
	FPrimaryAssetId AssetCategory;

	/** This identifies an object as a "primary" asset that can be searched for by the AssetManager and used in various tools */
	UPROPERTY()
	TArray<class UShooterItem*> AssetItems;

public:

	void Init(FPrimaryAssetId InAssetId, TArray<class UShooterItem*> InAssetItems) // Assign only the vars we wish to replicate
	{
		AssetCategory = InAssetId;
		AssetItems = InAssetItems;
	}

	void SetAssetID(const FPrimaryAssetId& InAssetCategory) const;

	FPrimaryAssetId GetAssetID() const { return AssetCategory; }

	void SetAssetItems(const TArray<class UShooterItem*>& InAssetItems) const;

	TArray<class UShooterItem*> GetAllAssetItems() const { return AssetItems; };

	TArray<class UShooterItem*> GetAssetItemsByID() const;

	/** Equality operators */
	bool operator==(const FShooterGameAsset& Other) const
	{
		return AssetCategory.IsValid() && Other.AssetCategory.IsValid() && AssetCategory == Other.AssetCategory;
	}
	bool operator!=(const FShooterGameAsset& Other) const
	{
		return !(*this == Other);
	}

	/** Implemented so it can be used in Maps/Sets */
	friend inline uint32 GetTypeHash(const FShooterGameAsset& Key)
	{
		uint32 Hash = 0;

		Hash = HashCombine(Hash, GetTypeHash(Key.AssetCategory.PrimaryAssetType));
		Hash = HashCombine(Hash, GetTypeHash(Key.AssetCategory.PrimaryAssetName));
		return Hash;
	}

	/** Returns true if slot is valid */
	bool IsValid() const
	{
		return AssetCategory.IsValid();
	}
};

class UShooterItem;

/** Struct representing a slot for an item, shown in the UI */
USTRUCT(BlueprintType)
struct SHOOTERGAME_API FShooterItemSlot
{
	GENERATED_BODY()

	/** Constructor, -1 means an invalid slot */
	FShooterItemSlot()
		: SlotNumber(-1)
	{}

	FShooterItemSlot(const FPrimaryAssetType& InItemType, int32 InSlotNumber)
		: ItemType(InItemType)
		, SlotNumber(InSlotNumber)
	{}

	/** The type of items that can go in this slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FPrimaryAssetType ItemType;

	/** The number of this slot, 0 indexed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 SlotNumber;

	/** Equality operators */
	bool operator==(const FShooterItemSlot& Other) const
	{
		return ItemType == Other.ItemType && SlotNumber == Other.SlotNumber;
	}
	bool operator!=(const FShooterItemSlot& Other) const
	{
		return !(*this == Other);
	}

	/** Implemented so it can be used in Maps/Sets */
	friend inline uint32 GetTypeHash(const FShooterItemSlot& Key)
	{
		uint32 Hash = 0;

		Hash = HashCombine(Hash, GetTypeHash(Key.ItemType));
		Hash = HashCombine(Hash, (uint32)Key.SlotNumber);
		return Hash;
	}

	/** Returns true if slot is valid */
	bool IsValid() const
	{
		return ItemType.IsValid() && SlotNumber >= 0;
	}
};

/** Extra information about a URPGItem that is in a player's inventory */
USTRUCT(BlueprintType)
struct SHOOTERGAME_API FShooterItemData
{
	GENERATED_BODY()

		/** Constructor, default to count/level 1 so declaring them in blueprints gives you the expected behavior */
	FShooterItemData()
		: ItemCount(1)
		, ItemLevel(1)
	{}

	FShooterItemData(int32 InItemCount, int32 InItemLevel)
		: ItemCount(InItemCount)
		, ItemLevel(InItemLevel)
	{}

	/** The number of instances of this item in the inventory, can never be below 1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 ItemCount;

	/** The level of this item. This level is shared for all instances, can never be below 1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 ItemLevel;

	/** Equality operators */
	bool operator==(const FShooterItemData& Other) const
	{
		return ItemCount == Other.ItemCount && ItemLevel == Other.ItemLevel;
	}
	bool operator!=(const FShooterItemData& Other) const
	{
		return !(*this == Other);
	}

	/** Returns true if count is greater than 0 */
	bool IsValid() const
	{
		return ItemCount > 0;
	}

	/** Append an item data, this adds the count and overrides everything else */
	void UpdateItemData(const FShooterItemData& Other, int32 MaxCount, int32 MaxLevel)
	{
		if (MaxCount <= 0)
		{
			MaxCount = MAX_int32;
		}

		if (MaxLevel <= 0)
		{
			MaxLevel = MAX_int32;
		}

		ItemCount = FMath::Clamp(ItemCount + Other.ItemCount, 1, MaxCount);
		ItemLevel = FMath::Clamp(Other.ItemLevel, 1, MaxLevel);
	}
};

/** Delegate called when the entire inventory has been loaded, all items may have been replaced */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryLoaded);
DECLARE_MULTICAST_DELEGATE(FOnInventoryLoadedNative);

/** Delegate called when an inventory Slot changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChanged, UShooterItem*, Item, bool, bAdded);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChangedNative, UShooterItem*, bool);

/** Delegate called when the contents of an inventory slot change */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlottedItemChanged, FShooterItemSlot, ItemSlot, UShooterItem*, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSlottedItemChangedNative, FShooterItemSlot, UShooterItem*);

/** Delegate called when inventory souls changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventorySoulsChanged, int32, NewSoulsValue);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInventorySoulsChangedNative, int32);

/** Delegate called when an inventory item changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStoreCategoryChanged, UShooterItem*, Item, bool, bSelected);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStoreCategoryChangedNative, UShooterItem*, bool);

/** Delegate called when an inventory item changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStoreContentChanged, UShooterItem*, Item, bool, bSelected);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStoreContentChangedNative, UShooterItem*, bool);

/** Delegate called when an inventory item changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStoreItemPurchase, UShooterItem*, Item, bool, bPurchase);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStoreItemPurchaseNative, UShooterItem*, bool);

