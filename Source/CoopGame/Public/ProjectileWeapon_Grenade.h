// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileWeapon_Grenade.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class UDamageType;

UCLASS()
class COOPGAME_API AProjectileWeapon_Grenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileWeapon_Grenade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent * SphereMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMoveComp;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
		void Detonate();

	// Effects
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	UParticleSystem* GrenadeExplosion;

	// Damage
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		float GrenadeBaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		float GrenadeDamageRadius;
	
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<UDamageType> GrenadeDamageTypeClass;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TArray<AActor*> GrenadeIgnoreActors;
	


public:	

};

