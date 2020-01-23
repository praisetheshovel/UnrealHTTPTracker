// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class AHTTPService;
class ACustomPlayerState;

USTRUCT()
struct FRequest_Login {
	GENERATED_BODY()

	UPROPERTY() FString email;
	UPROPERTY() FString password;

	FRequest_Login() {}
};

USTRUCT()
struct FMatch_Session_Shot {
	GENERATED_BODY()

		UPROPERTY() FString impact;
	UPROPERTY() float distance;
	UPROPERTY() FDateTime time;

	FMatch_Session_Shot() {}
};

USTRUCT()
struct FMatch_Session {
	GENERATED_BODY()

		UPROPERTY() FString matchsessionid;
	UPROPERTY() TArray<FMatch_Session_Shot> shots;

	FMatch_Session() {}
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
	ACustomPlayerState* CustomPlayerState;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Overriding viewpoint to be in place of third-person camera instead of actor eyes
	virtual FVector GetPawnViewLocation() const override;

	FMatch_Session PlayerMatchSessionStats;
};
