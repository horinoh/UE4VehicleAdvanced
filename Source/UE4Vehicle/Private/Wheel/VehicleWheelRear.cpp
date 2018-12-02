// Fill out your copyright notice in the Description page of Project Settings.

#include "VehicleWheelRear.h"

#include "UObject/ConstructorHelpers.h"
#include "TireConfig.h"

UVehicleWheelRear::UVehicleWheelRear()
{
	ShapeRadius = 18.0f;
	ShapeWidth = 15.0f;
	bAffectedByHandbrake = true;
	SteerAngle = 0.0f;

	SuspensionForceOffset = -4.0f;
	SuspensionMaxRaise = 8.0f;
	SuspensionMaxDrop = 12.0f;
	SuspensionNaturalFrequency = 9.0f;
	SuspensionDampingRatio = 1.05f;

	static ConstructorHelpers::FObjectFinder<UTireConfig> TC(TEXT("TireConfig'/Game/Vehicle/Wheel/TC_Rear.TC_Rear'"));
	if (TC.Succeeded())
	{
		TireConfig = TC.Object;
	}
}
