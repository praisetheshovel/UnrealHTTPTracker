// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HTTPService.generated.h"

class FHttpModule;
class IHttpRequest;
class IHttpResponse;

typedef TSharedPtr< IHttpResponse, ESPMode::ThreadSafe > FHttpResponsePtr;
typedef TSharedPtr< IHttpRequest > FHttpRequestPtr;

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

USTRUCT()
struct FResponse_Stats_Shot {
	GENERATED_USTRUCT_BODY()

	UPROPERTY() FString Time;
	UPROPERTY() FString _id;
	UPROPERTY() FString Impact;
	UPROPERTY() FString Distance;

};

USTRUCT()
struct FResponse_Stats {
	GENERATED_USTRUCT_BODY()

	UPROPERTY() FString _id;

	UPROPERTY() FString user;

	UPROPERTY() TArray<FResponse_Stats_Shot> shots;

	FResponse_Stats() {}
};

USTRUCT()
struct FSend_Shot {
	GENERATED_USTRUCT_BODY()

	UPROPERTY() FString Time;
	UPROPERTY() FString Impact;
	UPROPERTY() FString Distance;

	FSend_Shot() {}
};



UCLASS()
class COOPGAME_API AHTTPService : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHTTPService();

protected:

	FHttpModule* Http;
	FString ApiBaseUrl = "http://localhost:5000/api/";

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

	FString TempTokenHolder;

public:
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	void Login(FRequest_Login LoginCredentials);
	void LoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void GetStatsMe();
	void GetStatsMeResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void PutShotStats(FSend_Shot ShotBody);
	void PutShotStats_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

};
