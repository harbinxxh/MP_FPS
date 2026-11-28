// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Elims/SpecialElim.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetLayoutLibrary.h"


void USpecialElim::InitializeWidget(const FString& ElimMessage, UTexture2D* ElimTexture)
{
	if (IsValid(Text_ElimMessage))
	{
		Text_ElimMessage->SetText(FText::FromString(ElimMessage));
	}
	if (IsValid(Image_ElimType))
	{
		Image_ElimType->SetBrushFromTexture(ElimTexture);
	}
}

void USpecialElim::CenterWidget(UUserWidget* Widget, float VerticalRatio)
{
	if (!IsValid(Widget)) return;

	// 获取视口大小
	FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(Widget);
	const float VerticalFraction = VerticalRatio == 0.f ? 1.f : VerticalRatio * 2.f;
	FVector2D CenterPosition(ViewportSize.X / 2.f, VerticalFraction * ViewportSize.Y / 2.f);
	Widget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f)); // 将控件中心对齐到视口中心
	Widget->SetPositionInViewport(CenterPosition, true);
}
