// Fill out your copyright notice in the Description page of Project Settings.

#include "VehiclePawn.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "WheeledVehicleMovementComponent4W.h"

#include "VehicleWheelFront.h"
#include "VehicleWheelRear.h"

AVehiclePawn::AVehiclePawn()
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	if (nullptr != SpringArmComp)
	{
		SpringArmComp->SetupAttachment(RootComponent);
		//SpringArmComp->SetRelativeLocationAndRotation(FVector(0.0f, 100.0f, 50.0f), FRotator(-20.0f, 0.0f, 0.0f));
		SpringArmComp->SetRelativeLocation(FVector(0.0f, 0.0f, 34.0f));
		SpringArmComp->SetWorldRotation(FRotator(-20.0f, 0.0f, 0.0f));
		SpringArmComp->TargetArmLength = 125.0f;
		SpringArmComp->bEnableCameraLag = false;
		SpringArmComp->bEnableCameraRotationLag = false;
		SpringArmComp->bInheritPitch = true;
		SpringArmComp->bInheritYaw = true;
		SpringArmComp->bInheritRoll = true;

		CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
		if (nullptr != CameraComp)
		{
			CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
			CameraComp->SetRelativeLocation(FVector(-125.0f, 0.0f, 0.0f));
			CameraComp->SetRelativeRotation(FRotator(10.0f, 0.0f, 0.0f));
			CameraComp->FieldOfView = 90.0f;
			CameraComp->bUsePawnControlRotation = false;
		}
	}

	{
		const auto MovementComp = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovementComponent());
		check(4 == MovementComp->WheelSetups.Num());

		MovementComp->WheelSetups[0].WheelClass = UVehicleWheelFront::StaticClass();
		//MovementComp->WheelSetups[0].BoneName = FName("PhysWheel_FL");
		//MovementComp->WheelSetups[0].AdditionalOffset = FVector(0.0f, -8.0f, 0.0f);

		MovementComp->WheelSetups[1].WheelClass = UVehicleWheelFront::StaticClass();
		//MovementComp->WheelSetups[1].BoneName = FName("PhysWheel_FR");
		//MovementComp->WheelSetups[1].AdditionalOffset = FVector(0.0f, 8.0f, 0.0f);

		MovementComp->WheelSetups[2].WheelClass = UVehicleWheelRear::StaticClass();
		//MovementComp->WheelSetups[2].BoneName = FName("PhysWheel_BL");
		//MovementComp->WheelSetups[2].AdditionalOffset = FVector(0.0f, -8.0f, 0.0f);

		MovementComp->WheelSetups[3].WheelClass = UVehicleWheelRear::StaticClass();
		//MovementComp->WheelSetups[3].BoneName = FName("PhysWheel_BR");
		//MovementComp->WheelSetups[3].AdditionalOffset = FVector(0.0f, 8.0f, 0.0f);


		// Adjust the tire loading
		MovementComp->MinNormalizedTireLoad = 0.0f;
		MovementComp->MinNormalizedTireLoadFiltered = 0.2f;
		MovementComp->MaxNormalizedTireLoad = 2.0f;
		MovementComp->MaxNormalizedTireLoadFiltered = 2.0f;

		// Engine 
		// Torque setup
		MovementComp->MaxEngineRPM = 5700.0f;
		const auto EngineCurve = MovementComp->EngineSetup.TorqueCurve.GetRichCurve();
		if (nullptr != EngineCurve)
		{
			EngineCurve->Reset();
			EngineCurve->AddKey(0.0f, 400.0f);
			EngineCurve->AddKey(1890.0f, 500.0f);
			EngineCurve->AddKey(5730.0f, 400.0f);
		}

		// Adjust the steering 
		const auto SteerCurve = MovementComp->SteeringCurve.GetRichCurve();
		if (nullptr != SteerCurve)
		{
			SteerCurve->Reset();
			SteerCurve->AddKey(0.0f, 1.0f);
			SteerCurve->AddKey(40.0f, 0.7f);
			SteerCurve->AddKey(120.0f, 0.6f);
		}

		// Transmission	
		// We want 4wd
		MovementComp->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;
		// Drive the front wheels a little more than the rear
		MovementComp->DifferentialSetup.FrontRearSplit = 0.65;

		// Automatic gearbox
		MovementComp->TransmissionSetup.bUseGearAutoBox = true;
		MovementComp->TransmissionSetup.GearSwitchTime = 0.15f;
		MovementComp->TransmissionSetup.GearAutoBoxLatency = 1.0f;
	}
}

void AVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (nullptr != PlayerInputComponent)
	{
		PlayerInputComponent->BindAxis("MoveForward", this, &AVehiclePawn::MoveForward);
		PlayerInputComponent->BindAxis("MoveRight", this, &AVehiclePawn::MoveRight);
		//PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::AddControllerYawInput);
		//PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::AddControllerPitchInput);

		PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &AVehiclePawn::OnHandbrakePressed);
		PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &AVehiclePawn::OnHandbrakeReleased);
	}
}

void AVehiclePawn::MoveForward(float Value)
{
	if (0.0f != Value)
	{
		const auto MovementComp = GetVehicleMovementComponent();
		if (nullptr != MovementComp)
		{
			MovementComp->SetThrottleInput(Value);
		}
	}
}
void AVehiclePawn::MoveRight(float Value)
{
	if (0.0f != Value)
	{
		const auto MovementComp = GetVehicleMovementComponent();
		if (nullptr != MovementComp)
		{
			MovementComp->SetSteeringInput(Value);
		}
	}
}
void AVehiclePawn::OnHandbrakePressed()
{
	const auto MovementComp = GetVehicleMovementComponent();
	if (nullptr != MovementComp)
	{
		MovementComp->SetHandbrakeInput(true);
	}
}
void AVehiclePawn::OnHandbrakeReleased()
{
	const auto MovementComp = GetVehicleMovementComponent();
	if (nullptr != MovementComp)
	{
		MovementComp->SetHandbrakeInput(false);
	}
}
