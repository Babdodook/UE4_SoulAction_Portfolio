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

    // ���� �������� ��� Ʈ���̽�
    // Ʈ���̽� ���� ��ġ
    traceStart = OwnerActor->GetActorLocation();

    // Ʈ���̽� �� ��ġ
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
        traceStart,                         // Ʈ���̽� ���� ��ġ
        traceEnd,                           // Ʈ���̽� �� ��ġ
        Radius,                             // ���Ǿ� ������
        TraceType,                          // Ʈ���̽��� ���� ä��
        false,                              // Ʈ���̽� ���⼺, �ܼ� Ʈ���̽��� ����
        ignoreActors,                       // �����
        EDrawDebugTrace::ForOneFrame,       // �������Ӵ� Ʈ���̽�
        outHit,                             // ��Ʈ�� ���
        true                                // self ����
    );

    if (isHit)
    {
        // ���� Ʈ���̽��� ��Ʈ�� ��ġ�� ����
        WallLocation = outHit.Location;
        // ���� Ʈ���̽��� ��Ʈ�� ������Ʈ�� ���� ���͸� ����
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

    // ������ �Ʒ��� ��� Ʈ���̽�
    // Ʈ���̽� ���� ��ġ
    traceStart = FVector
    { 
        OwnerActor->GetActorLocation().X, 
        OwnerActor->GetActorLocation().Y, 
        OwnerActor->GetActorLocation().Z + 500.f
    } + OwnerActor->GetActorForwardVector() * 70.f;

    // Ʈ���̽� �� ��ġ
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
        traceStart,                         // Ʈ���̽� ���� ��ġ
        traceEnd,                           // Ʈ���̽� �� ��ġ
        Radius,                             // ���Ǿ� ������
        TraceType,                          // Ʈ���̽��� ���� ä��
        false,                              // Ʈ���̽� ���⼺, �ܼ� Ʈ���̽��� ����
        ignoreActors,                       // �����
        EDrawDebugTrace::ForOneFrame,       // �������Ӵ� Ʈ���̽�
        outHit,                             // ��Ʈ�� ���
        true                                // self ����
    );

    if (isHit)
    {
        // ���� Ʈ���̽��� ��Ʈ�� ��ġ�� ����
        HeightLocation = outHit.Location;

        bHeightHit = true;

        // ���� ������ ������ ���� ���� �ְ�, bIsClimbingLedge�� false�� ���, �Ŵ޸��� �����Ѵ�.
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
        // bIsHanging ���� true�� �����, �̵� ��带 Flying���� �����Ѵ�.
        // bIsClimbingLedge = true;
        playerClass->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
        playerClass->playerAnim->bIsHanging = true;
        // �Ŵ޸� ���� true
        bIsHanging = true;

        FVector TargetRelativeLocation = MoveToLocation();

        FRotator TargetRelativeRotation;
        FRotator NewRotWallNormal = UKismetMathLibrary::MakeRotFromX(WallNormal);
        TargetRelativeRotation = UKismetMathLibrary::MakeRotator(NewRotWallNormal.Roll, NewRotWallNormal.Pitch, NewRotWallNormal.Yaw - 180.f);

        FLatentActionInfo LatentInfo;
        LatentInfo.CallbackTarget = this;
        LatentInfo.ExecutionFunction = FName("GrabLedgeMoveFinished");  // Move�� ������ �ݹ��Ѵ�.
        LatentInfo.Linkage = 0;
        LatentInfo.UUID = 0;

        // ���� Grab�ϰ� �ִ� ������Ʈ�� �������� �����Ͽ� �����δ�.
        UKismetSystemLibrary::MoveComponentTo
        (
            playerClass->GetCapsuleComponent(), // ������ ���
            TargetRelativeLocation,             // ��� Ÿ�� ��ġ
            TargetRelativeRotation,             // ��� Ÿ�� ȸ��
            false,                              // true�̸� ���� �߿� ��ȭ (��, õõ�� ����)
            false,                              // true �� ��� �����ϴ� ���� õõ�� ���� (��, õõ�� ����)
            0.13f,                              // ���� �ð�
            false,                              // true�̸� ȸ���� �׻� �ִ� ��θ� ���
            EMoveComponentAction::Move,         // �ʿ��� ������ �ൿ
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