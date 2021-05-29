// Fill out your copyright notice in the Description page of Project Settings.


#include "CS_ClimbSystem.h"
#include "CS_Player.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values for this component's properties
UCS_ClimbSystem::UCS_ClimbSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

    bIsClimbingLedge = false;
	// ...
}


// Called when the game starts
void UCS_ClimbSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UCS_ClimbSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateClimbSystem();

	// ...
}

void UCS_ClimbSystem::UpdateClimbSystem()
{
    ForwardTrace();
    HeightTrace();
    
}

void UCS_ClimbSystem::ForwardTrace()
{
    FVector traceStart;
    FVector traceEnd;
    float Radius;
    ETraceTypeQuery TraceType;
    TArray<AActor*, FDefaultAllocator> ignoreActors;
    FHitResult outHit;

    // 액터 정면으로 쏘는 트레이스
    // 트레이스 시작 위치
    traceStart = OwnerActor->GetActorLocation();

    // 트레이스 끝 위치
    traceEnd = traceStart + FVector
    { 
        OwnerActor->GetActorForwardVector().X * 150.f, 
        OwnerActor->GetActorForwardVector().Y * 150.f, 
        OwnerActor->GetActorForwardVector().Z 
    };

    Radius = ForwardTraceRadius;
    TraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel4); // LedgeTrace

    bool isHit = UKismetSystemLibrary::SphereTraceSingle
    (
        GetWorld(),
        traceStart,                         // 트레이스 시작 위치
        traceEnd,                           // 트레이스 끝 위치
        Radius,                             // 스피어 반지름
        TraceType,                          // 트레이스에 사용될 채널
        false,                              // 트레이스 복잡성, 단순 트레이스로 설정
        ignoreActors,                       // 비워둠
        EDrawDebugTrace::ForOneFrame,       // 한프레임당 트레이스
        outHit,                             // 히트된 결과
        true                                // self 무시
    );

    if (isHit)
    {
        // 정면 트레이스에 히트된 위치를 저장
        WallLocation = outHit.Location;
        // 정면 트레이스에 히트된 오브젝트의 법선 벡터를 저장
        WallNormal = outHit.Normal;

        bForwardHit = true;
    }
    else
        bForwardHit = false;
}

void UCS_ClimbSystem::HeightTrace()
{
    FVector traceStart;
    FVector traceEnd;
    float Radius;
    ETraceTypeQuery TraceType;
    TArray<AActor*, FDefaultAllocator> ignoreActors;
    FHitResult outHit;

    // 위에서 아래로 쏘는 트레이스
    // 트레이스 시작 위치
    traceStart = FVector
    { 
        OwnerActor->GetActorLocation().X, 
        OwnerActor->GetActorLocation().Y, 
        OwnerActor->GetActorLocation().Z + 500.f
    } + OwnerActor->GetActorForwardVector() * 70.f;

    // 트레이스 끝 위치
    traceEnd = traceStart + FVector
    { 
        OwnerActor->GetActorForwardVector().X, 
        OwnerActor->GetActorForwardVector().Y, 
        OwnerActor->GetActorForwardVector().Z - 500.f
    };

    Radius = HeightTraceRadius;
    TraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel4); // LedgeTraceChannel

    bool isHit = UKismetSystemLibrary::SphereTraceSingle
    (
        GetWorld(),
        traceStart,                         // 트레이스 시작 위치
        traceEnd,                           // 트레이스 끝 위치
        Radius,                             // 스피어 반지름
        TraceType,                          // 트레이스에 사용될 채널
        false,                              // 트레이스 복잡성, 단순 트레이스로 설정
        ignoreActors,                       // 비워둠
        EDrawDebugTrace::ForOneFrame,       // 한프레임당 트레이스
        outHit,                             // 히트된 결과
        true                                // self 무시
    );

    if (isHit)
    {
        // 상하 트레이스에 히트된 위치를 저장
        HeightLocation = outHit.Location;

        bHeightHit = true;

        // 기준 소켓이 지정한 범위 내에 있고, bIsClimbingLedge가 false인 경우, 매달리기 실행한다.
        if (IsHipToLedge() && !bIsClimbingLedge)
        {
            GrabLedge();
        }
    }
    else
        bHeightHit = false;
}

bool UCS_ClimbSystem::IsHipToLedge()
{
    FVector SocketLocation = Cast<ACharacter>(OwnerActor)->GetMesh()->GetSocketLocation("Socket_Pelvis");

    float ZValue = SocketLocation.Z - HeightLocation.Z;
    return UKismetMathLibrary::InRange_FloatFloat(ZValue, HipToLedgeMin, HipToLedgeMax);
}

void UCS_ClimbSystem::GrabLedge()
{
    ACS_Player* playerClass = Cast<ACS_Player>(OwnerActor);
    if (playerClass != nullptr)
    {
        // bIsHanging 상태 true로 만들고, 이동 모드를 Flying으로 변경한다.
        // bIsClimbingLedge = true;
        playerClass->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
        playerClass->playerAnim->bIsHanging = true;
        // 매달린 상태 true
        bIsHanging = true;

        FVector TargetRelativeLocation = MoveToLocation();

        FRotator TargetRelativeRotation;
        FRotator NewRotWallNormal = UKismetMathLibrary::MakeRotFromX(WallNormal);
        TargetRelativeRotation = UKismetMathLibrary::MakeRotator(NewRotWallNormal.Roll, NewRotWallNormal.Pitch, NewRotWallNormal.Yaw - 180.f);

        FLatentActionInfo LatentInfo;
        LatentInfo.CallbackTarget = this;
        LatentInfo.ExecutionFunction = FName("GrabLedgeMoveFinished");  // Move가 끝날시 콜백한다.
        LatentInfo.Linkage = 0;
        LatentInfo.UUID = 0;

        // 현재 Grab하고 있는 오브젝트를 기준으로 보간하여 움직인다.
        UKismetSystemLibrary::MoveComponentTo
        (
            playerClass->GetCapsuleComponent(), // 보간할 대상
            TargetRelativeLocation,             // 상대 타겟 위치
            TargetRelativeRotation,             // 상대 타겟 회전
            false,                              // true이면 보간 중에 완화 (즉, 천천히 종료)
            false,                              // true 인 경우 보간하는 동안 천천히 시작 (즉, 천천히 시작)
            0.13f,                              // 보간 시간
            false,                              // true이면 회전에 항상 최단 경로를 사용
            EMoveComponentAction::Move,         // 필요한 움직임 행동
            LatentInfo                          // The latent action
        );

    }
}

void UCS_ClimbSystem::ExitLedge()
{
    ACS_Player* playerClass = Cast<ACS_Player>(OwnerActor);
    if (playerClass != nullptr)
    {
        playerClass->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
        playerClass->playerAnim->bIsHanging = false;
        playerClass->playerAnim->bClimbJump = false;
        bIsHanging = false;
    }
}

FVector UCS_ClimbSystem::MoveToLocation()
{
    FVector NewWallNormal = WallNormal * FVector{ 22.f,22.f,0.f };
    float MakeXValue = NewWallNormal.X + WallLocation.X;
    float MakeYValue = NewWallNormal.Y + WallLocation.Y;
    float MakeZValue = HeightLocation.Z - 120.f;
    FVector DesiredLocation = FVector{ MakeXValue, MakeYValue, MakeZValue };
    return DesiredLocation;
}

void UCS_ClimbSystem::GrabLedgeMoveFinished()
{
    ACS_Player* playerClass = Cast<ACS_Player>(OwnerActor);
    playerClass->GetCharacterMovement()->StopMovementImmediately();
}

bool UCS_ClimbSystem::IsCanJump()
{
    if (bForwardHit && bHeightHit)
        return true;
    else
        return false;
}

void UCS_ClimbSystem::ClimbLedge()
{
    ACS_Player* playerClass = Cast<ACS_Player>(OwnerActor);
    if (playerClass != nullptr && !bIsClimbingLedge)
    {
        bIsClimbingLedge = true;
        playerClass->playerAnim->bIsClimbingUp = true;
        playerClass->playerAnim->StopAllMontages(0.25f);
        playerClass->playerAnim->PlayMontage(playerClass->playerAnim->Climb_Montage);
    }
}

void UCS_ClimbSystem::ClimbingEnd()
{
    ACS_Player* playerClass = Cast<ACS_Player>(OwnerActor);
    if (playerClass != nullptr)
    {
        bIsClimbingLedge = false;

        playerClass->playerAnim->PlayMontage(playerClass->playerAnim->ClimbUp_Montage);
        playerClass->playerAnim->bIsClimbingUp = false;
    }
}