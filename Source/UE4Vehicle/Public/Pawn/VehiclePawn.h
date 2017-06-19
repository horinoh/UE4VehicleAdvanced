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

	//!< APawn
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void OnHandbrakePressed();
	void OnHandbrakeReleased();

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
