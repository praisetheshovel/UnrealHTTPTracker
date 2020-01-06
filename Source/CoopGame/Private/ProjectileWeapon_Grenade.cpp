// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon_Grenade.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "HAL/IConsoleManager.h"

static int32 DebugWeaponDrawingGrenadeLauncher = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawingGrenadeLauncher(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawingGrenadeLauncher,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);

// Sets default values
AProjectileWeapon_Grenade::AProjectileWeapon_Grenade()
{
	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
	RootComponent = SphereMesh;

	ProjectileMoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMoveComp"));
}

// Called when the game starts or when spawned
void AProjectileWeapon_Grenade::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectileWeapon_Grenade::Detonate()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GrenadeExplosion, GetActorLocation());
	UGameplayStatics::ApplyRadialDamage(this, GrenadeBaseDamage, GetActorLocation(), GrenadeDamageRadius, GrenadeDamageTypeClass, GrenadeIgnoreActors, this, this->GetInstigatorController());
	if (DebugWeaponDrawingGrenadeLauncher > 0)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), GrenadeDamageRadius, 12, FColor::Yellow, false, 1.0f, 0, 2.0f);
	}
	Destroy();
}
