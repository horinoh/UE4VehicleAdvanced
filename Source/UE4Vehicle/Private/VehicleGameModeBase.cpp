// Fill out your copyright notice in the Description page of Project Settings.

#include "VehicleGameModeBase.h"

#include "VehiclePawn.h"

AVehicleGameModeBase::AVehicleGameModeBase()
{
	DefaultPawnClass = AVehiclePawn::StaticClass();
}
