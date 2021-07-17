// Fill out your copyright notice in the Description page of Project Settings.


#include "CS_ClimbSystem.h"
#include "CS_Player.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Animation/AnimMontage.h"


// Sets default values for this component's properties
UCS_ClimbSystem::UCS_ClimbSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

    bIsClimbingLedge = false;
    bIsHanging = false;

    bForwardHit = false;
    bHeightHit = false;
    bLeftHit = false;
    bRightHit = false;

    bCanMoveLeft = true;
    bCanMoveRight = true;

    bCanJumpLeft = false;
    bCanJumpRight = false;

    bCanTurnLeft = false;
    bCanTurnRight = false;
    bIsTurning = false;
    
	// ...
}


// Called when the game starts
void UCS_ClimbSystem::BeginPlay()
{
	Super::BeginPlay();

    playerClass = Cast<ACS_Player>(OwnerActor);
    if(playerClass == nullptr)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("UCS_ClimbSystem playerClass is Nullptr"));
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
    
    if (bIsHanging)
    {
        SideMoveTrace(playerClass->LeftArrow, bCanMoveLeft, bCanJumpLeft, bCanTurnLeft);
        SideMoveTrace(playerClass->RightArrow, bCanMoveRight, bCanJumpRight, bCanTurnRight);
        JumpUpTrace();

        SideMovement();

        if (!bCanMoveLeft)
        {
            EdgeTrace(playerClass->LeftEdge, bCanMoveLeft, bCanJumpLeft);
            TurnTrace(playerClass->LeftArrow, bCanMoveLeft, bCanTurnLeft);
        }
        if (!bCanMoveRight)
        {
            EdgeTrace(playerClass->RightEdge, bCanMoveRight, bCanJumpRight);
            TurnTrace(playerClass->RightArrow, bCanMoveRight, bCanTurnRight);
        }

        if(bCanMoveLeft || bCanMoveRight)
            ClimbTurn();
    }
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
        DrawDebugType.GetValue(),                      // �������Ӵ� Ʈ���̽�
        outHit,                             // ��Ʈ�� ���
        true                                // self ����
    );

    if (isHit)
    {
        bForwardHit = true;

        // ���� Ʈ���̽��� ��Ʈ�� ��ġ�� ����
        WallLocation = outHit.Location;
        // ���� Ʈ���̽��� ��Ʈ�� ������Ʈ�� ���� ���͸� ����
        WallNormal = outHit.Normal;

        WallZLocation = outHit.GetActor()->GetActorLocation().Z;

        FBox mesh = outHit.GetActor()->GetComponentsBoundingBox();
        WallZSize = mesh.GetSize().Z;

        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("WallZLocation: %f     WallZSize: %f"), WallZLocation, WallZSize));
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
        OwnerActor->GetActorLocation().Z + 120.f
    } + OwnerActor->GetActorForwardVector() * 40.f;

    // Ʈ���̽� �� ��ġ
    traceEnd = traceStart + FVector
    { 
        OwnerActor->GetActorForwardVector().X, 
        OwnerActor->GetActorForwardVector().Y, 
        OwnerActor->GetActorForwardVector().Z - 120.f
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
        DrawDebugType.GetValue(),           // �������Ӵ� Ʈ���̽�
        outHit,                             // ��Ʈ�� ���
        true                                // self ����
    );

    if (isHit)
    {
        bHeightHit = true;;

        // ���� Ʈ���̽��� ��Ʈ�� ��ġ�� ����
        HeightLocation = outHit.Location;
        
        // Pelvis ������ ������ ������ �� �ִ� ������ �ִ���?
        // ���� �ö󼭴� �� �ƴҶ�
        // ���߿� ���ִ� �����϶��� ����
        if (IsHipToLedge() && !bIsClimbingLedge && playerClass->playerAnim->bIsInAir)
            GrabLedge();    // ���� ��� ����
    }
    else
        bHeightHit = false;
}

void UCS_ClimbSystem::SideMoveTrace(UArrowComponent* Arrow, bool& bCanMove, bool& bCanJump, bool& bCanTurn)
{
    FVector traceStart;
    FVector traceEnd;
    float Radius;
    ETraceTypeQuery TraceType;
    TArray<AActor*, FDefaultAllocator> ignoreActors;
    FHitResult outHit;

    traceStart = Arrow->GetComponentLocation();

    // Ʈ���̽� �� ��ġ
    traceEnd = traceStart;

    Radius = SideTraceRadius;
    TraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel4); // LedgeTraceChannel

    bool isHit = UKismetSystemLibrary::CapsuleTraceSingle
    (
        GetWorld(),
        traceStart,
        traceEnd,
        Radius,
        TraceCapsuleHalfHeight,
        TraceType,                          // Ʈ���̽��� ���� ä��
        false,                              // Ʈ���̽� ���⼺, �ܼ� Ʈ���̽��� ����
        ignoreActors,                       // �����
        DrawDebugType.GetValue(),       // �������Ӵ� Ʈ���̽�
        outHit,                             // ��Ʈ�� ���
        true                                // self ����
    );

    if (isHit)
    {
        bCanMove = true;
        bCanJump = false;
        bCanTurn = false;
    }
    else
        bCanMove = false;
}

void UCS_ClimbSystem::EdgeTrace(UArrowComponent* Arrow, bool bCanMove, bool& bCanJump)
{
    FVector traceStart;
    FVector traceEnd;
    float Radius;
    ETraceTypeQuery TraceType;
    TArray<AActor*, FDefaultAllocator> ignoreActors;
    FHitResult outHit;

    traceStart = Arrow->GetComponentLocation();

    // Ʈ���̽� �� ��ġ
    traceEnd = traceStart;

    Radius = EdgeTraceRadius;
    TraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel4); // LedgeTraceChannel

    bool isHit = UKismetSystemLibrary::CapsuleTraceSingle
    (
        GetWorld(),
        traceStart,
        traceEnd,
        Radius,
        TraceCapsuleHalfHeight,
        TraceType,                          // Ʈ���̽��� ���� ä��
        false,                              // Ʈ���̽� ���⼺, �ܼ� Ʈ���̽��� ����
        ignoreActors,                       // �����
        DrawDebugType.GetValue(),       // �������Ӵ� Ʈ���̽�
        outHit,                             // ��Ʈ�� ���
        true                                // self ����
    );

    if (isHit)
    {
        if (!bCanMove)
            bCanJump = true;
        else
            bCanJump = false;
    }
    else
        bCanJump = false;
}

void UCS_ClimbSystem::TurnTrace(UArrowComponent* Arrow, bool bCanMove, bool& bCanTurn)
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
    (
        Arrow->GetComponentLocation().X,
        Arrow->GetComponentLocation().Y,
        Arrow->GetComponentLocation().Z + 60.f
    );

    // Ʈ���̽� �� ��ġ
    traceEnd = traceStart + Arrow->GetForwardVector() * 70.f;

    Radius = TurnTraceRadius;
    TraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility); // LedgeTraceChannel

    bool isHit = UKismetSystemLibrary::SphereTraceSingle
    (
        GetWorld(),
        traceStart,                         // Ʈ���̽� ���� ��ġ
        traceEnd,                           // Ʈ���̽� �� ��ġ
        Radius,                             // ���Ǿ� ������
        TraceType,                          // Ʈ���̽��� ���� ä��
        false,                              // Ʈ���̽� ���⼺, �ܼ� Ʈ���̽��� ����
        ignoreActors,                       // �����
        DrawDebugType.GetValue(),       // �������Ӵ� Ʈ���̽�
        outHit,                             // ��Ʈ�� ���
        true                                // self ����
    );

    if (isHit)
    {
        bCanTurn = false;
    }
    else
    {
        if (!bCanMove)
            bCanTurn = true;
        else
            bCanTurn = false;
    }
}

void UCS_ClimbSystem::JumpUpTrace()
{
    FVector traceStart;
    FVector traceEnd;
    float Radius;
    ETraceTypeQuery TraceType;
    TArray<AActor*, FDefaultAllocator> ignoreActors;
    FHitResult outHit;

    // ������ �Ʒ��� ��� Ʈ���̽�
    // Ʈ���̽� ���� ��ġ
    traceStart = playerClass->UpArrow->GetComponentLocation();

    // Ʈ���̽� �� ��ġ
    traceEnd = traceStart;

    Radius = UpTraceRadius;
    TraceType = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel4); // LedgeTraceChannel

    bool isHit = UKismetSystemLibrary::CapsuleTraceSingle
    (
        GetWorld(),
        traceStart,
        traceEnd,
        Radius,
        100.f,
        TraceType,                          // Ʈ���̽��� ���� ä��
        false,                              // Ʈ���̽� ���⼺, �ܼ� Ʈ���̽��� ����
        ignoreActors,                       // �����
        DrawDebugType.GetValue(),       // �������Ӵ� Ʈ���̽�
        outHit,                             // ��Ʈ�� ���
        true                                // self ����
    );

    if (isHit)
    {
        bCanJumpUp = true;
    }
    else
    {
        bCanJumpUp = false;
    }
}

bool UCS_ClimbSystem::IsHipToLedge()
{
    FVector SocketLocation = Cast<ACharacter>(OwnerActor)->GetMesh()->GetSocketLocation("Socket_Pelvis");

    float ZValue = SocketLocation.Z - HeightLocation.Z;
    return UKismetMathLibrary::InRange_FloatFloat(ZValue, HipToLedgeMin, HipToLedgeMax);
}

void UCS_ClimbSystem::GrabLedge()
{
    if (playerClass != nullptr)
    {
        // �̵� ��带 Flying���� �����Ѵ�.
        playerClass->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
        playerClass->SetActorRotation(FRotator(0.f, playerClass->GetActorRotation().Yaw, 0.f));
        // �Ŵ޸� ���� true
        playerClass->playerAnim->bIsHanging = true;
        bIsHanging = true;
        playerClass->SetWeaponLocation("Socket_ClimbWeapon");

        // ����ƾ��ϴ� ������ ��ġ�� ȸ������ �Ҵ��Ѵ�.
        TargetRelativeLocation = MoveToLocation();
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
            0.2f,                               // ���� �ð�
            false,                              // true�̸� ȸ���� �׻� �ִ� ��θ� ���
            EMoveComponentAction::Move,         // �ʿ��� ������ �ൿ
            LatentInfo                          // The latent action
        );
    }
}

void UCS_ClimbSystem::ExitLedge()
{
    // �Ŵ޸� ���¿����� ����.
    if (playerClass != nullptr && bIsHanging && !bIsClimbingLedge)
    {
        playerClass->playerAnim->StopAllMontages(0.25f);

        playerClass->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
        playerClass->playerAnim->bIsHanging = false;
        bIsHanging = false;
        bIsTurnBack = false;
        bIsTurning = false;
        bIsJumpingUp = false;
        playerClass->SetWeaponLocation("Socket_twinblade");

        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("ExitLedge")));
    }
}

FVector UCS_ClimbSystem::MoveToLocation()
{
    FVector NewWallNormal = WallNormal * FVector(22.f,22.f,0.f);
    float MakeXValue = NewWallNormal.X + WallLocation.X;
    float MakeYValue = NewWallNormal.Y + WallLocation.Y;
    float MakeZValue = WallZLocation + (WallZSize * 0.5f) - 100.f;
    FVector DesiredLocation = FVector(MakeXValue, MakeYValue, MakeZValue);
    return DesiredLocation;
}

void UCS_ClimbSystem::GrabLedgeMoveFinished()
{
    playerClass->GetCharacterMovement()->StopMovementImmediately();
    playerClass->SetActorRotation(FRotator(0.f, playerClass->GetActorRotation().Yaw, 0.f));
}

void UCS_ClimbSystem::ClimbLedge()
{
    // �Ŵ޸� ���¿����� ����, �ö󰡴� �� �ߺ� ���� ���� bIsClimbingLedge = true
    if (bIsHanging && !bIsClimbingLedge && !bCanJumpUp && !bIsTurnBack)
    {
        bIsClimbingLedge = true;
        playerClass->playerAnim->bIsClimbingUp = true;
        playerClass->playerAnim->PlayMontage(playerClass->playerAnim->Climb_Montage);
    }
}

void UCS_ClimbSystem::ClimbUpEnd()
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("ClimbUpEnd")));
    playerClass->SetActorRotation(FRotator(0.f, playerClass->GetActorRotation().Yaw, 0.f));
    bIsClimbingLedge = false;
    ExitLedge();
}

void UCS_ClimbSystem::SideMovement()
{
    if (bIsHanging && !bIsClimbingLedge && !bIsTurnBack)
    {
        // ���������� ������ �� ���µ� ���������� �����̷� �ϴ°��,
        // �������� ������ �� ���� �������� �����̷� �ϴ°�� return �Ѵ�.
        if ((!bCanMoveLeft && playerClass->InputHorizontal < 0.f) ||
            (!bCanMoveRight && playerClass->InputHorizontal > 0.f)) return;

        FVector DesiredLocation =
            playerClass->GetActorLocation() +
            (playerClass->GetActorRightVector() * playerClass->InputHorizontal * SideMoveSpeed);

        FVector NewLocation = UKismetMathLibrary::VInterpTo(playerClass->GetActorLocation(), DesiredLocation, GetWorld()->GetDeltaSeconds(), 5.f);

        playerClass->SetActorLocation(NewLocation);
    }
}

void UCS_ClimbSystem::ClimbJumpSide()
{
    if (bIsHanging && !bIsClimbingLedge && playerClass->InputHorizontal != 0.f && !bIsTurnBack)
    {
        UAnimMontage* JumpMontage = nullptr;
        if (bCanJumpLeft && playerClass->InputHorizontal < 0.f)
            JumpMontage = playerClass->playerAnim->ClimbJumpLeft_Montage;
        else if (bCanJumpRight && playerClass->InputHorizontal > 0.f)
            JumpMontage = playerClass->playerAnim->ClimbJumpRight_Montage;
        else
            return;
        
        playerClass->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
        playerClass->playerAnim->PlayMontage(JumpMontage);
    }
}

void UCS_ClimbSystem::ClimbJumpSideEnd()
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("ClimbJumpSideEnd")));
    playerClass->GetCharacterMovement()->StopMovementImmediately();
    GrabLedge();
}

void UCS_ClimbSystem::ClimbTurn()
{
    if (bIsHanging && !bIsClimbingLedge && !bIsTurning && playerClass->bPressShift && !bIsJumpingUp && !bIsTurnBack)
    {
        UAnimMontage* TurnMontage = nullptr;

        if (bCanTurnLeft && playerClass->InputHorizontal < 0.f)
        {
            TurnMontage = playerClass->playerAnim->ClimbTurnLeft_Montage;
        }
        else if (bCanTurnRight && playerClass->InputHorizontal > 0.f)
        {
            TurnMontage = playerClass->playerAnim->ClimbTurnRight_Montage;
        }
        else
            return;

        bIsTurning = true;

        playerClass->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
        playerClass->playerAnim->PlayMontage(TurnMontage);
    }
}

void UCS_ClimbSystem::ClimbTurnEnd()
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("ClimbTurnEnd")));
    bIsTurning = false;
    playerClass->GetCharacterMovement()->StopMovementImmediately();
    GrabLedge();
}

void UCS_ClimbSystem::ClimbJumpUp()
{
    if (bIsHanging && !bIsClimbingLedge && bCanJumpUp && !bIsTurning && !bIsJumpingUp && !bIsTurnBack)
    {
        if (playerClass->InputHorizontal != 0.f)
            return;

        bIsJumpingUp = true;

        playerClass->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
        playerClass->playerAnim->PlayMontage(playerClass->playerAnim->ClimbJumpUp_Montage);
    }
}

void UCS_ClimbSystem::ClimbJumpUpEnd()
{
    bIsJumpingUp = false;
    playerClass->GetCharacterMovement()->StopMovementImmediately();
    GrabLedge();
}