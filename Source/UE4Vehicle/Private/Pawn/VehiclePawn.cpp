// Fill out your copyright notice in the Description page of Project Settings.

#include "VehiclePawn.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundCue.h"

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

	//!< #TODO アセット
#if 0
	const auto SkelMeshComp = GetMesh();
	if (nullptr != SkelMeshComp)
	{
		//!< メッシュ
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/VehicleAdv/Vehicle/Vehicle_SkelMesh.Vehicle_SkelMesh"));
		if (CarMesh.Succeeded())
		{
			SkelMeshComp->SetSkeletalMesh(CarMesh.Object);
		}

		//!< アニメーションBP
		static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/VehicleAdv/Vehicle/VehicleAnimationBlueprint"));
		SkelMeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		SkelMeshComp->SetAnimInstanceClass(AnimBPClass.Class);

		//!< 物理マテリアル
		static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> SlipperyMat(TEXT("/Game/VehicleAdv/PhysicsMaterials/Slippery.Slippery"));
		if (SlipperyMat.Succeeded())
		{
			SkelMeshComp->SetPhysMaterialOverride(SlipperyMat.Object);
		}

		//!< 車内カメラ
		InternalCameraSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("InternalCameraSceneComp"));
		if (nullptr != InternalCameraSceneComp)
		{
			InternalCameraSceneComp->SetRelativeLocation(FVector(-34.0f, -10.0f, 50.0f));
			InternalCameraSceneComp->SetupAttachment(SkelMeshComp);

			InternalCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("InternalCameraComp"));
			if (nullptr != InternalCameraComp)
			{
				InternalCameraComp->bUsePawnControlRotation = false;
				InternalCameraComp->FieldOfView = 90.0f;
				InternalCameraComp->SetupAttachment(InternalCameraSceneComp);
			}
		}

		//!< エンジン音
		AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
		if (nullptr != AudioComp)
		{
			AudioComp->SetupAttachment(SkelMeshComp);
			static ConstructorHelpers::FObjectFinder<USoundCue> SoundCue(TEXT("/Game/VehicleAdv/Sound/Engine_Loop_Cue.Engine_Loop_Cue"));
			if (SoundCue.Succeeded())
			{
				AudioComp->SetSound(SoundCue.Object);
			}
		}
	}
#endif

	const auto MovementComp = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovementComponent());
	if(nullptr != MovementComp)
	{
		check(4 == MovementComp->WheelSetups.Num());

		MovementComp->WheelSetups[0].WheelClass = UVehicleWheelFront::StaticClass();
		//!< #TODO
		//MovementComp->WheelSetups[0].BoneName = FName("PhysWheel_FL");
		MovementComp->WheelSetups[0].AdditionalOffset = FVector(0.0f, -8.0f, 0.0f);

		MovementComp->WheelSetups[1].WheelClass = UVehicleWheelFront::StaticClass();
		//MovementComp->WheelSetups[1].BoneName = FName("PhysWheel_FR");
		MovementComp->WheelSetups[1].AdditionalOffset = FVector(0.0f, 8.0f, 0.0f);

		MovementComp->WheelSetups[2].WheelClass = UVehicleWheelRear::StaticClass();
		//MovementComp->WheelSetups[2].BoneName = FName("PhysWheel_BL");
		MovementComp->WheelSetups[2].AdditionalOffset = FVector(0.0f, -8.0f, 0.0f);

		MovementComp->WheelSetups[3].WheelClass = UVehicleWheelRear::StaticClass();
		//MovementComp->WheelSetups[3].BoneName = FName("PhysWheel_BR");
		MovementComp->WheelSetups[3].AdditionalOffset = FVector(0.0f, 8.0f, 0.0f);

		// Adjust the tire loading
		MovementComp->MinNormalizedTireLoad = 0.0f;
		MovementComp->MinNormalizedTireLoadFiltered = 0.2f;
		MovementComp->MaxNormalizedTireLoad = 2.0f;
		MovementComp->MaxNormalizedTireLoadFiltered = 2.0f;

		//!< エンジントルク
		MovementComp->MaxEngineRPM = 5700.0f;
		const auto EngineCurve = MovementComp->EngineSetup.TorqueCurve.GetRichCurve();
		if (nullptr != EngineCurve)
		{
			EngineCurve->Reset();
			EngineCurve->AddKey(0.0f, 400.0f);
			EngineCurve->AddKey(1890.0f, 500.0f);
			EngineCurve->AddKey(5730.0f, 400.0f);
		}

		//!< ステアリング 
		const auto SteerCurve = MovementComp->SteeringCurve.GetRichCurve();
		if (nullptr != SteerCurve)
		{
			SteerCurve->Reset();
			SteerCurve->AddKey(0.0f, 1.0f);
			SteerCurve->AddKey(40.0f, 0.7f);
			SteerCurve->AddKey(120.0f, 0.6f);
		}

		//!< 駆動輪
		MovementComp->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;
		//MovementComp->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_RearDrive;
		// Drive the front wheels a little more than the rear
		MovementComp->DifferentialSetup.FrontRearSplit = 0.65;

		//!< トランスミッション
		MovementComp->TransmissionSetup.bUseGearAutoBox = true;
		MovementComp->TransmissionSetup.GearSwitchTime = 0.15f;
		MovementComp->TransmissionSetup.GearAutoBoxLatency = 1.0f;

		// Physics settings
		// Adjust the center of mass - the buggy is quite low
		UPrimitiveComponent* UpdatedPrimitive = Cast<UPrimitiveComponent>(MovementComp->UpdatedComponent);
		if (UpdatedPrimitive)
		{
			UpdatedPrimitive->BodyInstance.COMNudge = FVector(8.0f, 0.0f, 0.0f);
		}

		// Set the inertia scale. This controls how the mass of the vehicle is distributed.
		MovementComp->InertiaTensorScale = FVector(1.0f, 1.333f, 1.2f);
	}

	//!< カメラの切り替え
	if (nullptr != CameraComp && nullptr != InternalCameraComp)
	{
#if 1
		CameraComp->Activate();
		InternalCameraComp->Deactivate();
#else
		CameraComp->Deactivate();
		InternalCameraComp->Activate();
#endif
	}
}

void AVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (nullptr != PlayerInputComponent)
	{
		PlayerInputComponent->BindAxis("MoveForward", this, &AVehiclePawn::MoveForward);
		PlayerInputComponent->BindAxis("MoveRight", this, &AVehiclePawn::MoveRight);
		//!< #TODO
		//PlayerInputComponent->BindAxis("LookUp");
		//PlayerInputComponent->BindAxis("LookRight");

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
