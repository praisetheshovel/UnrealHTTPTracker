// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class AHTTPService;

USTRUCT()
struct FRequest_Login {
	GENERATED_USTRUCT_BODY()

	UPROPERTY() FString email;
	UPROPERTY() FString password;

	FRequest_Login() {}
};


USTRUCT()
struct FNestedUser {
	GENERATED_USTRUCT_BODY()

	UPROPERTY() FString id;
	UPROPERTY() FString name;
	UPROPERTY() FString avatar;

	FNestedUser() {}
};

USTRUCT()
struct FResponse_Login {
	GENERATED_USTRUCT_BODY()

	UPROPERTY() FString token;
	UPROPERTY() FNestedUser user;

	FResponse_Login() {}
};

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Movement & Crouch
	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();

	// Third-Person Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	// Zoom
	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	float DefaultFOV;

	void BeginZoom();

	void EndZoom();

	// Fire Weapon
	ASWeapon* CurrentWeapon;

	void Fire();

	// Begin Play
	UPROPERTY(EditDefaultsOnly, Category = "Player")
		TSubclassOf<ASWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
		FName WeaponAttachSocketName;

	AHTTPService* MyHTTPService;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Overriding viewpoint to be in place of third-person camera instead of actor eyes
	virtual FVector GetPawnViewLocation() const override;
};
