// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/HTTPService.h"

#include "Runtime/Online/HTTP/Public/Interfaces/IHttpResponse.h"	// IHttpRequest
#include "Runtime/Online/HTTP/Public/HttpModule.h"					// CreateRequest
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpRequest.h"		// SetHeader
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"		// FJsonObjectConverter
#include "Runtime/Json/Public/Dom/JsonObject.h"						// GetObjectField

// Sets default values
AHTTPService::AHTTPService()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AHTTPService::BeginPlay()
{
	Super::BeginPlay();
	Http = &FHttpModule::Get();

	// You can uncomment this out for testing.
	
	FRequest_Login LoginCredentials;
	LoginCredentials.email = TEXT("onionknight@gmail.com");
	LoginCredentials.password = TEXT("123123");
	// UE_LOG(LogTemp, Warning, TEXT("email: %s"), *(LoginCredentials.email));
	
	Login(LoginCredentials);
}

TSharedRef<IHttpRequest> AHTTPService::RequestWithRoute(FString Subroute) {
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(ApiBaseUrl + Subroute);
	SetRequestHeaders(Request);
	return Request;
}

void AHTTPService::SetRequestHeaders(TSharedRef<IHttpRequest>& Request) {
	Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
}

TSharedRef<IHttpRequest> AHTTPService::GetRequest(FString Subroute) {
	TSharedRef<IHttpRequest> Request = RequestWithRoute(Subroute);
	Request->SetVerb("GET");
	return Request;
}

TSharedRef<IHttpRequest> AHTTPService::PostRequest(FString Subroute, FString ContentJsonString) {
	TSharedRef<IHttpRequest> Request = RequestWithRoute(Subroute);
	Request->SetVerb("POST");
	Request->SetContentAsString(ContentJsonString);
	return Request;
}

TSharedRef<IHttpRequest> AHTTPService::PutRequest(FString Subroute, FString ContentJsonString)
{
	TSharedRef<IHttpRequest> Request = RequestWithRoute(Subroute);
	Request->SetVerb("PUT");
	Request->SetContentAsString(ContentJsonString);
	return Request;
}

void AHTTPService::Send(TSharedRef<IHttpRequest>& Request) {
	Request->ProcessRequest();
}

bool AHTTPService::ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!bWasSuccessful || !Response.IsValid()) return false;
	if (EHttpResponseCodes::IsOk(Response->GetResponseCode())) return true;
	else {
		UE_LOG(LogTemp, Warning, TEXT("Http Response returned error code: %d"), Response->GetResponseCode());
		return false;
	}
}

void AHTTPService::SetAuthorizationHash(FString Hash, TSharedRef<IHttpRequest>& Request) {
	Request->SetHeader(AuthorizationHeader, Hash);
}

/**************************************************************************************************************************/



template <typename StructType>
void AHTTPService::GetJsonStringFromStruct(StructType FilledStruct, FString& StringOutput) {
	FJsonObjectConverter::UStructToJsonObjectString(StructType::StaticStruct(), &FilledStruct, StringOutput, 0, 0);
}

template <typename StructType>
void AHTTPService::GetStructFromJsonString(FHttpResponsePtr Response, StructType& StructOutput) {
	StructType StructData;
	FString JsonString = Response->GetContentAsString();
	//UE_LOG(LogTemp, Warning, TEXT("Response is: %s"), *JsonString);
	FJsonObjectConverter::JsonObjectStringToUStruct<StructType>(JsonString, &StructOutput, 0, 0);
}



/**************************************************************************************************************************/

void AHTTPService::Login(FRequest_Login LoginCredentials) {
	FString ContentJsonString;
	GetJsonStringFromStruct<FRequest_Login>(LoginCredentials, ContentJsonString);

	TSharedRef<IHttpRequest> Request = PostRequest("auth", ContentJsonString);
	Request->OnProcessRequestComplete().BindUObject(this, &AHTTPService::LoginResponse);
	Send(Request);
}

void AHTTPService::LoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!ResponseIsValid(Response, bWasSuccessful)) return;

	FResponse_Login LoginResponse;
	GetStructFromJsonString<FResponse_Login>(Response, LoginResponse);

	UE_LOG(LogTemp, Warning, TEXT("Token: %s"), *LoginResponse.token);
	UE_LOG(LogTemp, Warning, TEXT("ID: %s"), *LoginResponse.user.id);
	UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *LoginResponse.user.name);
	UE_LOG(LogTemp, Warning, TEXT("Avatar: %s"), *LoginResponse.user.avatar);

	// Login success, save our hash (JWT token)
	TempTokenHolder = LoginResponse.token;
	UE_LOG(LogTemp, Warning, TEXT("TempTokenHolder: %s"), *TempTokenHolder);

	//	** AUTH TEST ROUTE -- ONLY TO BE USED TO TEST ROUOTES QUICKLY** 
	//	TODO: IMPLEMENT PLAYERSTATE TO HOLD JWT TOKEN**

	FSend_Shot TestShot;
	TestShot.Time = "TestTime";
	TestShot.Impact = "TestImpact";
	TestShot.Distance = "TestDistance";

	PutShotStats(TestShot);
}

void AHTTPService::GetStatsMe() {
	TSharedRef<IHttpRequest> Request = GetRequest("stats/me");
	SetAuthorizationHash(TempTokenHolder, Request);
	Request->OnProcessRequestComplete().BindUObject(this, &AHTTPService::GetStatsMeResponse);
	Send(Request);
}

void AHTTPService::GetStatsMeResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!ResponseIsValid(Response, bWasSuccessful)) return;
	FResponse_Stats StatsResponse;
	GetStructFromJsonString<FResponse_Stats>(Response, StatsResponse);
	UE_LOG(LogTemp, Warning, TEXT("StatsDocumentID: %s"), *StatsResponse._id);
	UE_LOG(LogTemp, Warning, TEXT("StatsUserID: %s"), *StatsResponse.user);
	for (auto shot : StatsResponse.shots) {
		UE_LOG(LogTemp, Warning, TEXT("Time: %s"), *shot.Time);
		UE_LOG(LogTemp, Warning, TEXT("_id: %s"), *shot._id);
		UE_LOG(LogTemp, Warning, TEXT("Impact: %s"), *shot.Impact);
		UE_LOG(LogTemp, Warning, TEXT("Distance: %s"), *shot.Distance);
	}
}

void AHTTPService::PutShotStats(FSend_Shot ShotBody)
{
	FString ContentJsonString;
	GetJsonStringFromStruct<FSend_Shot>(ShotBody, ContentJsonString);

	UE_LOG(LogTemp, Warning, TEXT("PutShotStats.ContentJsonString: %s"), *ContentJsonString);

	TSharedRef<IHttpRequest> Request = PutRequest("stats//matchidentifier", ContentJsonString);
	SetAuthorizationHash(TempTokenHolder, Request);
	Request->OnProcessRequestComplete().BindUObject(this, &AHTTPService::GetStatsMeResponse);
	Send(Request);
}

// Called every frame
void AHTTPService::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

