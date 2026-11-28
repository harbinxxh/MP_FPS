// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpecialElim.generated.h"

class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class MP_FPS_API USpecialElim : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ElimMessage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_ElimType;

	void InitializeWidget(const FString& ElimMessage, UTexture2D* ElimTexture);

	// 控件在屏幕居中的函数，这样就能在任意位置创建这个控件并添加到视口
	UFUNCTION(BlueprintCallable)
	static void CenterWidget(UUserWidget* Widget, float VerticalRatio = 0.f);
};
