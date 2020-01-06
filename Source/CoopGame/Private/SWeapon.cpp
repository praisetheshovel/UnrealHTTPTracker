// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "HAL/IConsoleManager.h"
#include "GameFramework/Pawn.h"
#include "Templates/Casts.h"
#include "GameFramework/PlayerController.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "Templates/SharedPointer.h"
#include "Misc/Char.h"
#include "Runtime/PerfCounters/Public/PerfCountersModule.h"
#include "Runtime/Json/Public/Serialization/JsonWriter.h"
#include "Runtime/Json/Public/Policies/JsonPrintPolicy.h"
#include "Misc/FileHelper.h"
#include "Misc/DateTime.h"
#include "CoopGame.h"


static int32 DebugWeaponDrawingRifle = 0;
FAutoConsoleVariableRef CVARDebeugWeaponDrawingRifle(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawingRifle,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleFlashSocket";
	TracerTargetName = "BeamEnd";


}

void ASWeapon::Fire()
{
	// Trace the world, from pawn eyes to target location

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();
		FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this); // Ignoring weapon
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		// Particle "Target" parameter
		FVector TracerEndPoint = TraceEnd;

		FHitResult Hit;
		FHitJST HitJST;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			// Recording Bone hit on skeletal mesh
			FName RecordBone = Hit.BoneName;
			float RecordDistance = Hit.Distance;

			HitJST.Time = HitJST.Time.Now();
			HitJST.ShotBoneHit = RecordBone.ToString();
			HitJST.ShotDistance = RecordDistance;

			// UE_LOG(LogTemp, Warning, TEXT("%s"), *HitBone.ToString());

			// TODO:: Use FJsonObjectConverter to serialize our FHitJST struct to a JSON format
			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
			// JsonObject->SetStringField("Name", "Super Sword");
			// JsonObject->SetNumberField("Damage", 15);
			// JsonObject->SetNumberField("Weight", 3);
			JsonObject->SetStringField("Time",(HitJST.Time).ToString());
			JsonObject->SetStringField("Impact", HitJST.ShotBoneHit);
			JsonObject->SetNumberField("Distance", HitJST.ShotDistance);

			FString OutputString;
			//TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
			TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
			FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

			//UE_LOG(LogTemp, Warning, TEXT("%s"), *OutputString);

			OutputString += TEXT(",\n");
			// TODO:: Output JSON information to a log file in "LogJST" folder in /Content
			FFileHelper::SaveStringToFile(OutputString, *(FPaths::ProjectLogDir() + "\\" + "LogJST.json"), FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);

			

			// Blocking hit, process damage

			AActor* HitActor = Hit.GetActor();

			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			UParticleSystem* SelectedEffect = nullptr;

			switch (SurfaceType)
			{
			case SURFACE_FLESHDEFAULT:
			case SURFACE_FLESHVULNERABLE:
				SelectedEffect = FleshImpactEffect;
				break;
			default:
				SelectedEffect = DefaultImpactEffect;
				break;
			}

			if (SelectedEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			TracerEndPoint = Hit.ImpactPoint;
		}

		if (DebugWeaponDrawingRifle > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0.f, 1.0f);
		}

		PlayFireEffects(TracerEndPoint);
	}
}

void ASWeapon::PlayFireEffects(FVector TracerEndPoint)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
		}
	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}

}

