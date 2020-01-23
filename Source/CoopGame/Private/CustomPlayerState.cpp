// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/CustomPlayerState.h"

void ACustomPlayerState::OnLoginSuccess(FResponse_Login LoginResponse)
{
	PlayerConnectionInfo = LoginResponse;
	UE_LOG(LogTemp, Warning, TEXT("Called: OnLoginSuccess function in CustomPlayerState"));
}
