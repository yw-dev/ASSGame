// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "ShooterSourceModel.h"
#include "IImageWrapper.h"
#include "IShooterSource.h"
#include "IImageWrapperModule.h"
#include "Internationalization/Culture.h"
#include "ShooterCategoryViewModel.h"
#include "Modules/ModuleManager.h"



#define LOCTEXT_NAMESPACE "ShooterSourceViewModel"

const FString DefaultLanguageCode = "en";
uint32 FShooterSourceModel::ImageID = 0;


FShooterSourceModel::FShooterSourceModel(TSharedPtr<IShooterSource> ContentSourceIn)
{
	ContentSource = ContentSourceIn;
	SetupBrushes();
	Category = FShooterCategoryViewModel(ContentSource->GetCategory());
}

TSharedPtr<IShooterSource> FShooterSourceModel::GetContentSource()
{
	return ContentSource;
}

FText FShooterSourceModel::GetName()
{
	FString CurrentLanguage = FInternationalization::Get().GetCurrentCulture()->GetTwoLetterISOLanguageName();
	if (NameText.GetTwoLetterLanguage() != CurrentLanguage)
	{
		NameText = ChooseLocalizedText(ContentSource->GetLocalizedNames(), CurrentLanguage);
	}
	return NameText.GetText();
}

FText FShooterSourceModel::GetDescription()
{
	FString CurrentLanguage = FInternationalization::Get().GetCurrentCulture()->GetTwoLetterISOLanguageName();
	if (DescriptionText.GetTwoLetterLanguage() != CurrentLanguage)
	{
		DescriptionText = ChooseLocalizedText(ContentSource->GetLocalizedDescriptions(), CurrentLanguage);
	}
	return DescriptionText.GetText();
}

FText FShooterSourceModel::GetAssetTypes()
{
	FString CurrentLanguage = FInternationalization::Get().GetCurrentCulture()->GetTwoLetterISOLanguageName();
	if (AssetTypeText.GetTwoLetterLanguage() != CurrentLanguage)
	{
		AssetTypeText = ChooseLocalizedText(ContentSource->GetLocalizedAssetTypes(), CurrentLanguage);
	}
	return AssetTypeText.GetText();

}

FString FShooterSourceModel::GetClassTypes()
{
	return ContentSource->GetClassTypesUsed();
}

FShooterCategoryViewModel FShooterSourceModel::GetCategory()
{
	return Category;
}

TSharedPtr<FSlateBrush> FShooterSourceModel::GetIconBrush()
{
	return IconBrush;
}

TArray<TSharedPtr<FSlateBrush>>* FShooterSourceModel::GetScreenshotBrushes()
{
	return &ScreenshotBrushes;
}

void FShooterSourceModel::SetupBrushes()
{
	if (ContentSource->GetIconData().IsValid())
	{
		FString IconBrushName = GetName().ToString() + "_" + ContentSource->GetIconData()->GetName();
		IconBrush = CreateBrushFromRawData(IconBrushName, *ContentSource->GetIconData()->GetData());
	}

	for (TSharedPtr<FImageData> ScreenshotData : ContentSource->GetScreenshotData())
	{
		if (ScreenshotData.IsValid() == true)
		{
			FString ScreenshotBrushName = GetName().ToString() + "_" + ScreenshotData->GetName();
			ScreenshotBrushes.Add(CreateBrushFromRawData(ScreenshotBrushName, *ScreenshotData->GetData()));
		}
	}
}

TSharedPtr<FSlateDynamicImageBrush> FShooterSourceModel::CreateBrushFromRawData(FString ResourceNamePrefix, const TArray< uint8 >& RawData) const
{
	TSharedPtr< FSlateDynamicImageBrush > Brush;

	uint32 BytesPerPixel = 4;
	int32 Width = 0;
	int32 Height = 0;

	bool bSucceeded = false;
	TArray<uint8> DecodedImage;
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	if (ImageWrapper.IsValid() && (RawData.Num() > 0) && ImageWrapper->SetCompressed(RawData.GetData(), RawData.Num()))
	{
		Width = ImageWrapper->GetWidth();
		Height = ImageWrapper->GetHeight();

		const TArray<uint8>* RawImageData = NULL;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawImageData))
		{
			DecodedImage.AddUninitialized(Width * Height * BytesPerPixel);
			DecodedImage = *RawImageData;
			bSucceeded = true;
		}
	}

	if (bSucceeded)
	{
		FString UniqueResourceName = ResourceNamePrefix + "_" + FString::FromInt(ImageID++);
		Brush = FSlateDynamicImageBrush::CreateWithImageData(FName(*UniqueResourceName), FVector2D(ImageWrapper->GetWidth(), ImageWrapper->GetHeight()), DecodedImage);
	}

	return Brush;
}

FLocalizedText FShooterSourceModel::ChooseLocalizedText(TArray<FLocalizedText> Choices, FString LanguageCode)
{
	FLocalizedText Default;
	for (const FLocalizedText& Choice : Choices)
	{
		if (Choice.GetTwoLetterLanguage() == LanguageCode)
		{
			return Choice;
			break;
		}
		else if (Choice.GetTwoLetterLanguage() == DefaultLanguageCode)
		{
			Default = Choice;
		}
	}
	return Default;
}

#undef LOCTEXT_NAMESPACE
