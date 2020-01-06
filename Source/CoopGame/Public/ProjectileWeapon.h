// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "ProjectileWeapon_Grenade.h"
#include "ProjectileWeapon.generated.h"


/**
 * 
 */
UCLASS()
class COOPGAME_API AProjectileWeapon : public ASWeapon
{
	GENERATED_BODY()

protected:

	virtual void Fire() override;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AProjectileWeapon_Grenade> ProjectileClass;
};

