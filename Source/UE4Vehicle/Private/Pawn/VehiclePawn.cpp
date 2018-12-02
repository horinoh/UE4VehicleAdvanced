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

	const auto SkelMeshComp = GetMesh();
	if (nullptr != SkelMeshComp)
	{
		//!< メッシュ
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("SkeletalMesh'/Game/VehicleAdv/Vehicle/Vehicle_SkelMesh.Vehicle_SkelMesh'"));
		if (CarMesh.Succeeded())
		{
			SkelMeshComp->SetSkeletalMesh(CarMesh.Object);
		}

		//!< アニメーションBP
		static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Vehicle/Animation/ABP_Vehicle"));
		SkelMeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		SkelMeshComp->SetAnimInstanceClass(AnimBPClass.Class);

		//!< 物理マテリアル #MY_TODO ここでコールしてはダメ？
		//static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> SlipperyMat(TEXT("PhysicalMaterial'/Game/VehicleAdv/PhysicsMaterials/Slippery.Slippery'"));
		//if (SlipperyMat.Succeeded())
		//{
		//	SkelMeshComp->SetPhysMaterialOverride(SlipperyMat.Object);
		//}

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
			static ConstructorHelpers::FObjectFinder<USoundCue> SoundCue(TEXT("SoundCue'/Game/VehicleAdv/Sound/Engine_Loop_Cue.Engine_Loop_Cue'"));
			if (SoundCue.Succeeded())
			{
				AudioComp->SetSound(SoundCue.Object);
			}
		}
	}

	const auto MovementComp = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovementComponent());
	check(4 == MovementComp->WheelSetups.Num());

	MovementComp->WheelSetups[0].WheelClass = UVehicleWheelFront::StaticClass();
	MovementComp->WheelSetups[0].BoneName = FName("PhysWheel_FL");
	MovementComp->WheelSetups[0].AdditionalOffset = FVector(0.0f, -8.0f, 0.0f);

	MovementComp->WheelSetups[1].WheelClass = UVehicleWheelFront::StaticClass();
	MovementComp->WheelSetups[1].BoneName = FName("PhysWheel_FR");
	MovementComp->WheelSetups[1].AdditionalOffset = FVector(0.0f, 8.0f, 0.0f);

	MovementComp->WheelSetups[2].WheelClass = UVehicleWheelRear::StaticClass();
	MovementComp->WheelSetups[2].BoneName = FName("PhysWheel_BL");
	MovementComp->WheelSetups[2].AdditionalOffset = FVector(0.0f, -8.0f, 0.0f);

	MovementComp->WheelSetups[3].WheelClass = UVehicleWheelRear::StaticClass();
	MovementComp->WheelSetups[3].BoneName = FName("PhysWheel_BR");
	MovementComp->WheelSetups[3].AdditionalOffset = FVector(0.0f, 8.0f, 0.0f);

	//!< タイヤ負荷
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
#if 1
	MovementComp->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;
	// Drive the front wheels a little more than the rear
	MovementComp->DifferentialSetup.FrontRearSplit = 0.65;
#else
	MovementComp->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_RearDrive;
#endif

	//!< トランスミッション
	MovementComp->TransmissionSetup.bUseGearAutoBox = false;
	MovementComp->TransmissionSetup.GearSwitchTime = 0.15f;
	MovementComp->TransmissionSetup.GearAutoBoxLatency = 1.0f;

	//!< 重心の調整
	const auto UpdatedPrimitive = Cast<UPrimitiveComponent>(MovementComp->UpdatedComponent);
	if (UpdatedPrimitive)
	{
		//!< Center Of Mass Nudge
		UpdatedPrimitive->BodyInstance.COMNudge = FVector(8.0f, 0.0f, 0.0f);
	}

	//!< 慣性 質量の分散度合い
	MovementComp->InertiaTensorScale = FVector(1.0f, 1.333f, 1.2f);
}

void AVehiclePawn::BeginPlay()
{
	Super::BeginPlay();

	if (nullptr != AudioComp)
	{
		AudioComp->Play();
	}
}

void AVehiclePawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const auto MovementComp = GetVehicleMovementComponent();
	if (nullptr != MovementComp && nullptr != AudioComp)
	{
		const auto AudioScale = 2500.0f * MovementComp->GetEngineRotationSpeed() / MovementComp->GetEngineMaxRotationSpeed();
		AudioComp->SetFloatParameter(TEXT("RPM"), AudioScale);
	}

	if (!MovementComp->GetUseAutoGears())
	{
		if (MovementComp->GetTargetGear() != MovementComp->GetCurrentGear())
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.16, FColor::White, FString::FromInt(MovementComp->GetCurrentGear()) + TEXT(" -> ") + FString::FromInt(MovementComp->GetTargetGear()));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.16, FColor::Green, FString::FromInt(MovementComp->GetCurrentGear()));
		}
	}
}

void AVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (nullptr != PlayerInputComponent)
	{
		PlayerInputComponent->BindAxis("MoveForward", this, &AVehiclePawn::MoveForward);
		PlayerInputComponent->BindAxis("MoveRight", this, &AVehiclePawn::MoveRight);
		//!< #MY_TODO 効果なし
		//PlayerInputComponent->BindAxis("LookUp");
		//PlayerInputComponent->BindAxis("LookRight");

		PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &AVehiclePawn::OnHandbrakePressed);
		PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &AVehiclePawn::OnHandbrakeReleased);

		PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &AVehiclePawn::OnSwitchCamera);

		PlayerInputComponent->BindAction("Gear1", IE_Pressed, this, &AVehiclePawn::OnGear1);
		PlayerInputComponent->BindAction("Gear2", IE_Pressed, this, &AVehiclePawn::OnGear2);
		PlayerInputComponent->BindAction("Gear3", IE_Pressed, this, &AVehiclePawn::OnGear3);
		PlayerInputComponent->BindAction("Gear4", IE_Pressed, this, &AVehiclePawn::OnGear4);
		PlayerInputComponent->BindAction("Gear5", IE_Pressed, this, &AVehiclePawn::OnGear5);
		PlayerInputComponent->BindAction("Gear6", IE_Pressed, this, &AVehiclePawn::OnGear6);
		PlayerInputComponent->BindAction("Gear7", IE_Pressed, this, &AVehiclePawn::OnGear7);
		PlayerInputComponent->BindAction("GearR", IE_Pressed, this, &AVehiclePawn::OnGearR);
		PlayerInputComponent->BindAction("GearUp", IE_Pressed, this, &AVehiclePawn::OnGearUp);
		PlayerInputComponent->BindAction("GearDown", IE_Pressed, this, &AVehiclePawn::OnGearDown);
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

void AVehiclePawn::OnSwitchCamera()
{
	if (nullptr != CameraComp && nullptr != InternalCameraComp)
	{
		if (CameraComp->IsActive())
		{
			CameraComp->Deactivate();
			InternalCameraComp->Activate();
		}
		else
		{
			CameraComp->Activate();
			InternalCameraComp->Deactivate();
		}
	}
}

void AVehiclePawn::OnTargetGear(const int32 GearNum)
{
	const auto MovementComp = GetVehicleMovementComponent();
	if (nullptr != MovementComp && !MovementComp->GetUseAutoGears())
	{
		MovementComp->SetTargetGear(GearNum, true);
	}
}
void AVehiclePawn::OnGearUp()
{
	const auto MovementComp = GetVehicleMovementComponent();
	if (nullptr != MovementComp && !MovementComp->GetUseAutoGears())
	{
		MovementComp->SetGearUp(true);
	}
}
void AVehiclePawn::OnGearDown()
{
	const auto MovementComp = GetVehicleMovementComponent();
	if (nullptr != MovementComp && !MovementComp->GetUseAutoGears())
	{
		MovementComp->SetGearDown(true);
	}
}
