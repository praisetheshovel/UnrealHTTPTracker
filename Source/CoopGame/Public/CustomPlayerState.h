// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "CustomPlayerState.generated.h"

USTRUCT()
struct FNestedUser {
	GENERATED_BODY()

	UPROPERTY() FString id;
	UPROPERTY() FString name;
	UPROPERTY() FString avatar;

	FNestedUser() {}
};

USTRUCT()
struct FResponse_Login {
	GENERATED_BODY()

	UPROPERTY() FString token;
	UPROPERTY() FNestedUser user;

	FResponse_Login() {}
};

/**
 * 
 */


UCLASS()
class COOPGAME_API ACustomPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	FResponse_Login PlayerConnectionInfo;

	void OnLoginSuccess(FResponse_Login LoginResponse);

};
