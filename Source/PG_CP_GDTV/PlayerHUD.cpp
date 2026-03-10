// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

void UPlayerHUD::SetHP(int NewHP)
{
	if (!HPText) return; //Added with CLaude help
	FString Str = FString::Printf(TEXT("HP:%d"), NewHP);
	HPText->SetText(FText::FromString(Str));

}
void UPlayerHUD::SetDiamonds(int Amount)
{
	if (!DiamondText) return;  // was wrongly using HPText
	FString Str = FString::Printf(TEXT("Diamonds:%d"), Amount);
	DiamondText->SetText(FText::FromString(Str));

}
void UPlayerHUD::SetLevel(int Index)
{
	if (!LevelText) return;  // was wrongly using HPText
	FString Str = FString::Printf(TEXT("Level:%d"), Index);
	LevelText->SetText(FText::FromString(Str));

}

