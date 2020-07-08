// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IShooterSource.h"

class IShooterSourceProvider
{
public:
	DECLARE_DELEGATE(FOnShooterSourcesChanged);

public:
	/** Gets the available content sources. */
	virtual const TArray<TSharedRef<IShooterSource>> GetShooterSources() = 0;

	/** Sets the delegate which will be executed when the avaialble content sources change */
	virtual void SetShooterSourcesChanged(FOnShooterSourcesChanged OnShooterSourcesChangedIn) = 0;

	virtual ~IShooterSourceProvider() { }
	
};
