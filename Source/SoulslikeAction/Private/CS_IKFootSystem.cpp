// Fill out your copyright notice in the Description page of Project Settings.


#include "CS_IKFootSystem.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UCS_IKFootSystem::UCS_IKFootSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCS_IKFootSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UCS_IKFootSystem::IKProcessing()
{
	// Hip displacement
	RFootDistance = IKHipTrace(RFootSocketName);
	LFootDistance = IKHipTrace(LFootSocketName);

	HipOffset = HipDisplacement(RFootDistance >= LFootDistance ? RFootDistance : LFootDistance);

	// Foot displacement
	RFootDistance = IKFootTrace(RFootSocketName, RNormal);
	LFootDistance = IKFootTrace(LFootSocketName, LNormal);
	RFootAlpha = FootDisplacement(RFootDistance, RFootAlpha);
	LFootAlpha = FootDisplacement(LFootDistance, LFootAlpha);

	RFootRotation = FootRotate(RFootRotation, RNormal);
	LFootRotation = FootRotate(LFootRotation, LNormal);
}

float UCS_IKFootSystem::IKFootTrace(FName SocketName, FVector& Normal)
{
	FVector SocketLocation = Cast<ACharacter>(OwnerActor)->GetMesh()->GetSocketLocation(SocketName);
	FVector SocketRoot = Cast<ACharacter>(OwnerActor)->GetMesh()->GetSocketLocation(RootSocketName);

	FVector traceStart = FVector(SocketLocation.X, SocketLocation.Y, SocketRoot.Z + TraceDistance);
	FVector traceEnd = FVector(SocketLocation.X, SocketLocation.Y, SocketRoot.Z - TraceDistance);

	ETraceTypeQuery TraceType;
	TraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);

	TArray<AActor*, FDefaultAllocator> ignoreActors;
	FHitResult outHit;

	bool isHit = UKismetSystemLibrary::LineTraceSingle
	(
		GetWorld(),
		traceStart,
		traceEnd,
		TraceType,
		false,
		ignoreActors,
		EDrawDebugTrace::ForOneFrame,
		outHit,
		true
	);

	if (isHit)
	{
		Normal = outHit.Normal;
		return outHit.Distance;
	}
	else
	{
		Normal = FVector::ZeroVector;
		return 0.f;
	}
}

float UCS_IKFootSystem::IKHipTrace(FName SocketName)
{
	FVector WorldLocation = OwnerActor->GetActorLocation() - FVector(0.f, 0.f, CapsuleHalfHeight);
	FVector SocketLocation = Cast<ACharacter>(OwnerActor)->GetMesh()->GetSocketLocation(SocketName);

	FVector traceStart = FVector(SocketLocation.X, SocketLocation.Y, WorldLocation.Z + CapsuleHalfHeight);
	FVector traceEnd = FVector(SocketLocation.X, SocketLocation.Y, WorldLocation.Z - CapsuleHalfHeight + 35.f);

	ETraceTypeQuery TraceType;
	TraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);

	TArray<AActor*, FDefaultAllocator> ignoreActors;
	FHitResult outHit;

	bool isHit = UKismetSystemLibrary::LineTraceSingle
	(
		GetWorld(),
		traceStart,
		traceEnd,
		TraceType,
		false,
		ignoreActors,
		EDrawDebugTrace::ForOneFrame,
		outHit,
		true
	);

	if (isHit)
	{
		return outHit.Distance;
	}
	else
		return 0.f;
}

float UCS_IKFootSystem::HipDisplacement(float distanceToGround)
{
	float displacement;
	if (distanceToGround != 0.f)
		displacement = CapsuleHalfHeight - distanceToGround;
	else
		displacement = 0.f;

	return UKismetMathLibrary::FInterpTo(HipOffset, displacement, GetWorld()->GetDeltaSeconds(), InterpSpeed);
}

float UCS_IKFootSystem::FootDisplacement(float distanceToGround, float footAlpha)
{
	float displacement; 
	if (distanceToGround != 0.f)
		displacement = (TraceDistance - distanceToGround) / AlphaDivValue;
	else
		displacement = 0.f;
	
	return UKismetMathLibrary::FInterpTo(footAlpha, displacement, GetWorld()->GetDeltaSeconds(), InterpSpeed);
}

FRotator UCS_IKFootSystem::FootRotate(FRotator FootRotation, FVector Normal)
{
	float X = UKismetMathLibrary::DegAtan2(Normal.Y, Normal.Z);
	float Y = UKismetMathLibrary::DegAtan2(Normal.X, Normal.Z);

	FRotator desiredRotation = UKismetMathLibrary::MakeRotator(X, -Y, 0.f);
	return UKismetMathLibrary::RInterpTo(FootRotation, desiredRotation, GetWorld()->GetDeltaSeconds(), InterpSpeed);
}