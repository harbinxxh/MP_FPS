// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/CombatComponent.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::Initiate_CycleWeapon()
{
}

void UCombatComponent::Initiate_FireWeapon_Pressed()
{
}

void UCombatComponent::Initiate_FireWeapon_Released()
{
}

void UCombatComponent::Initiate_ReloadWeapon()
{
}

void UCombatComponent::Initiate_Aim_Pressed()
{
}

void UCombatComponent::Initiate_Aim_Released()
{
}

