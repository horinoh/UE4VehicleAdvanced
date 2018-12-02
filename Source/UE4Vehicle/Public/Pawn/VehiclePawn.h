// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "VehiclePawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USceneComponent;
class UAudioComponent;

/**
 * 
 */
UCLASS()
class UE4VEHICLE_API AVehiclePawn : public AWheeledVehicle
{
	GENERATED_BODY()
	
public:
	AVehiclePawn();

protected:
	//!< AActor
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	//!< APawn
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void OnHandbrakePressed();
	void OnHandbrakeReleased();
	void OnSwitchCamera();
	void OnTargetGear(const int32 GearNum);
	void OnGear1() { OnTargetGear(1); }
	void OnGear2() { OnTargetGear(2); }
	void OnGear3() { OnTargetGear(3); }
	void OnGear4() { OnTargetGear(4); }
	void OnGear5() { OnTargetGear(5); }
	void OnGear6() { OnTargetGear(6); }
	void OnGear7() { OnTargetGear(7); }
	void OnGearR() { OnTargetGear(-1); }
	void OnGearUp();
	void OnGearDown();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	USpringArmComponent* SpringArmComp = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* CameraComp = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USceneComponent* InternalCameraSceneComp = nullptr;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* InternalCameraComp = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* AudioComp = nullptr;
};
