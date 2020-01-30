// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HTTPService.generated.h"

class FHttpModule;
class IHttpRequest;
class IHttpResponse;
class ACustomPlayerState;

typedef TSharedPtr< IHttpResponse, ESPMode::ThreadSafe > FHttpResponsePtr;
typedef TSharedPtr< IHttpRequest > FHttpRequestPtr;

struct FRequest_Login;
struct FNestedUser;
struct FResponse_Login;
struct FMatch_Session;

class UUserWidget;
class ASCharacter;

/* PLACE INTO ANOTHER FILE
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
};

USTRUCT()
struct FResponse_Login {
	GENERATED_USTRUCT_BODY()

	UPROPERTY() FString token;

	UPROPERTY() FNestedUser user;

	FResponse_Login() {}
};

*/
USTRUCT()
struct FResponse_Stats_Matches_Shot {
	GENERATED_BODY()

	UPROPERTY() FString time;
	UPROPERTY() FString _id;
	UPROPERTY() FString impact;
	UPROPERTY() FString distance;

	FResponse_Stats_Matches_Shot() {}
};

USTRUCT()
struct FResponse_Stats_Matches {
	GENERATED_BODY()

	UPROPERTY() TArray<FResponse_Stats_Matches_Shot> shots;

	FResponse_Stats_Matches() {}
};

USTRUCT()
struct FResponse_Stats {
	GENERATED_BODY()

	UPROPERTY() FString _id;

	UPROPERTY() FString user;

	UPROPERTY() TArray<FResponse_Stats_Matches> matches;

	FResponse_Stats() {}
};


UCLASS(Blueprintable, hideCategories = (Rendering, Replication, Input, Actor, "Actor Tick"))
class COOPGAME_API AHTTPService : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHTTPService();

protected:

	FHttpModule* Http;
	FString ApiBaseUrl;

	FString AuthorizationHeader = TEXT("x-auth-token");
	void SetAuthorizationHash(FString Hash, TSharedRef<IHttpRequest>& Request);

	TSharedRef<IHttpRequest> RequestWithRoute(FString Subroute);
	void SetRequestHeaders(TSharedRef<IHttpRequest>& Request);

	TSharedRef<IHttpRequest> GetRequest(FString Subroute);
	TSharedRef<IHttpRequest> PostRequest(FString Subroute, FString ContentJsonString);
	TSharedRef<IHttpRequest> PutRequest(FString Subroute, FString ContentJsonString);
	void Send(TSharedRef<IHttpRequest>& Request);

	bool ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful);

	template <typename StructType>
	void GetJsonStringFromStruct(StructType FilledStruct, FString& StringOutput);
	template <typename StructType>
	void GetStructFromJsonString(FHttpResponsePtr Response, StructType& StructOutput);

	ASCharacter* LocalCharacter;

public:
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	void Login(FRequest_Login LoginCredentials);
	void LoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Login overload w/ player state
	void Login(FRequest_Login LoginCredentials, ACustomPlayerState* PlayerState);
	void LoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, ACustomPlayerState* PlayerState);

	void GetStatsMe(FString Hash);
	void GetStatsMeResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UUserWidget* LoginWidget;

	UFUNCTION()
	void OnLoginClicked();
};
