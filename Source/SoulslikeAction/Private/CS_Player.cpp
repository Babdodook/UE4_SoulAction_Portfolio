// Fill out your copyright notice in the Description page of Project Settings.

#include "CS_Player.h"
#include "CS_Enemy.h"
#include "CS_Weapon.h"
#include "CS_TargetingSystem.h"
#include "EnumState.h"
#include "LJSMathHelpers.h"
#include "CS_ClimbSystem.h"
#include "CS_IKFootSystem.h"

#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Engine/DataTable.h"


// Sets default values
ACS_Player::ACS_Player()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// SpringArm Initialize
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;		// 컨트롤러 입력에 따른 회전 사용
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 5.0f;
	SpringArm->CameraLagMaxDistance = 100.f; 

	// FollowCamera Initialize
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match
	// the controller orientation
	FollowCamera->bUsePawnControlRotation = false;

	SecondCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("SecondCamera"));
	SecondCamera->SetupAttachment(GetRootComponent());

	RefCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("RefCamera"));
	RefCamera->SetupAttachment(GetRootComponent());

	// TargetSystemComponent Initialize
	TargetSystemComponent = CreateDefaultSubobject<UCS_TargetingSystem>(TEXT("TargetSystemComponent"));

	// ClimbSystemComponent Initialize
	ClimbSystemComponent = CreateDefaultSubobject<UCS_ClimbSystem>(TEXT("ClimbSystemComponent"));
	ClimbSystemComponent->SetOwnerActor(this);

	IKFootSystemComponent = CreateDefaultSubobject<UCS_IKFootSystem>(TEXT("IKFootSystemComponent"));
	IKFootSystemComponent->SetOwnerActor(this);
	IKFootSystemComponent->SetCapsuleHalfHeight(GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	LeftArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftArrow"));
	LeftArrow->SetupAttachment(GetRootComponent());

	RightArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightArrow"));
	RightArrow->SetupAttachment(GetRootComponent());

	LeftEdge = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftEdge"));
	LeftEdge->SetupAttachment(GetRootComponent());

	RightEdge = CreateDefaultSubobject<UArrowComponent>(TEXT("RightEdge"));
	RightEdge->SetupAttachment(GetRootComponent());

	UpArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("UpArrow"));
	UpArrow->SetupAttachment(GetRootComponent());

	// Don't Rotate when the controller rotates
	// Let that just affect the camera
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;			// 입력값에따라 회전시킴
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); // 회전속도 설정, y값이 올라갈수록 회전 속도 올라감
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->GravityScale = 1.3f;
	GetCharacterMovement()->AirControl = 0.5f;
	SetRootRotation(false);

	// Set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	EvadeDirection = FVector(0.f);
	
	// 플레이어 정보 초기화
	MaxHealth = 100.f;
	Health = MaxHealth;
	HealthCover = Health;
	MaxStamina = 350.f;
	Stamina = MaxStamina;

	MoveSpeedMultiplier = 80.f;
	OriginMoveSpeed = MoveSpeedMultiplier;
	LerpMoveSpeed = 0;
	OriginCameraLagSpeed = SpringArm->CameraLagSpeed;
	
	bIsRolling = false;
	bOnAttack = false;
	bCanApplyDmg = false;
	bIsCanSprint = true;
	bCanAttack = true;
	bMoveable = true;
	bIsInputAttackKey = false;
	bPlayNextAttackAuto = false;

	InputVertical = 0;
	InputHorizontal = 0;

	WalkSpeed = 100.f;
	RunSpeed = 400.f;
	SprintSpeed = 500.f;

	bIsStaminaTimerEnd = true;

	BossEnemy = nullptr;

	LightAttackNum = 0;
	HeavyAttackNum = 0;

	DT_DamageType = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/DataTable/PlayerDamageType.PlayerDamageType'"));

	if (DT_DamageType == nullptr)
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("DT_DamageType == nullptr")));
	else
	{
		RowNames = DT_DamageType->GetRowNames();

		for (int i = 0; i < RowNames.Num(); i++)
		{
			FDamageTypeStruct DamageTypeData = *(DT_DamageType->FindRow<FDamageTypeStruct>(RowNames[i], RowNames[i].ToString()));

			DamageTypeAry.Add(DamageTypeData);
		}
	}
}

// Called when the game starts or when spawned
void ACS_Player::BeginPlay()
{
	Super::BeginPlay();

	playerAnim = Cast<UCS_PlayerAnim>(GetMesh()->GetAnimInstance());

	AttachWeapon();

	bInvincibility = false;

	Mat_Origin = GetMesh()->GetMaterial(0)->GetMaterial();
}

// Called every frame
void ACS_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move();
	TargetSystemComponent->RotateToTarget();

	UpdateStats(DeltaTime);
	
	//if (!playerAnim->bIsInAir && !ClimbSystemComponent->bIsHanging)
		//IKFootSystemComponent->IKProcessing();

}

// Called to bind functionality to input
void ACS_Player::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// 마우스 왼쪽 클릭 바인드
	PlayerInputComponent->BindAction("LeftMouseBtn", IE_Pressed, this, &ACS_Player::InputLMB);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &ACS_Player::HeavyAttack);
	// 특수 공격 바인드
	PlayerInputComponent->BindAction("SAttack", IE_Pressed, this, &ACS_Player::SpecialAttack);
	// 마우스 오른쪽 클릭 바인드
	PlayerInputComponent->BindAction("RightMouseBtn", IE_Pressed, this, &ACS_Player::InputRMB);
	PlayerInputComponent->BindAction("RightMouseBtn", IE_Repeat, this, &ACS_Player::DefenseLoop);
	PlayerInputComponent->BindAction("RightMouseBtn", IE_Released, this, &ACS_Player::EndDefense);

	// 스페이스바 바인드
	PlayerInputComponent->BindAction("SpaceBar", IE_Pressed, this, &ACS_Player::InputSpaceBar);

	// 쉬프트 바인드
	PlayerInputComponent->BindAction("Shift", IE_Pressed, this, &ACS_Player::InputShift);
	PlayerInputComponent->BindAction("Shift", IE_Released, this, &ACS_Player::ReleaseShift);

	// R 바인드
	PlayerInputComponent->BindAction("R", IE_Pressed, this, &ACS_Player::InputR);

	// Tab 바인드
	PlayerInputComponent->BindAction("Tab", IE_Pressed, this, &ACS_Player::InputTab);

	// 이동 바인드
	PlayerInputComponent->BindAxis("MoveForward", this, &ACS_Player::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACS_Player::MoveRight);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACS_Player::InputQ);

	// 카메라 회전 바인드
	PlayerInputComponent->BindAxis("Turn", this, &ACS_Player::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up", this, &ACS_Player::LookUpAtRate);

	// Climb 바인드
	PlayerInputComponent->BindAction("ClimbUp", IE_Pressed, this, &ACS_Player::ClimbUp);
	PlayerInputComponent->BindAction("ClimbTurnBack", IE_Pressed, this, &ACS_Player::ClimbTurnBack);
	PlayerInputComponent->BindAction("ClimbTurnBack", IE_Released, this, &ACS_Player::ClimbTurnBackEnd);
	PlayerInputComponent->BindAction("ClimbTurn", IE_Pressed, this, &ACS_Player::ClimbTurn);

	// 마우스 휠 업, 다운
	PlayerInputComponent->BindAction("MWheelUp", IE_Pressed, this, &ACS_Player::MWheelUp);
	PlayerInputComponent->BindAction("MWheelDown", IE_Pressed, this, &ACS_Player::MWheelDown);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &ACS_Player::Walk);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &ACS_Player::WalkEnd);

	PlayerInputComponent->BindAction("Test", IE_Pressed, this, &ACS_Player::InputTestBtn);
}

void ACS_Player::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && !bIsRolling)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		MoveDirection += ForwardDirection * Value;
	}

	InputVertical = Value;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Forward %f"), Value));
}

void ACS_Player::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) && !bIsRolling)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		MoveDirection += RightDirection * Value;
	}

	InputHorizontal = Value;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Right %f"), Value));
}

void ACS_Player::Move()
{
	if (MoveDirection.IsZero() || !bMoveable || ClimbSystemComponent->bIsHanging || ClimbSystemComponent->bIsClimbingLedge)
	{
		LerpMoveSpeed = 0;
		return;
	}
	
	// 달리기가 true일 경우 선형보간으로 스피드를 올린다.
	// 스프링암이 액터를 따라가는 속도도 선형보간으로 늦춘다.
	if (bIsWalk)
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	else if (playerAnim->bIsSprint && !bOnDefense /* MoveSpeedMultiplier <= OriginMoveSpeed + 30.f*/)
	{
		GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp<float, float>(GetCharacterMovement()->MaxWalkSpeed, SprintSpeed, 4 * GetWorld()->GetDeltaSeconds());
		//MoveSpeedMultiplier = FMath::Lerp<float, float>(MoveSpeedMultiplier, OriginMoveSpeed + 30.f, 4 * GetWorld()->GetDeltaSeconds());
		SpringArm->CameraLagSpeed = FMath::Lerp<float, float>(SpringArm->CameraLagSpeed, OriginCameraLagSpeed - 1.f, 3 * GetWorld()->GetDeltaSeconds());
	}
	else if (!playerAnim->bIsSprint && !bOnDefense)
	{
		GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp<float, float>(GetCharacterMovement()->MaxWalkSpeed, RunSpeed, 4 * GetWorld()->GetDeltaSeconds());
		SpringArm->CameraLagSpeed = FMath::Lerp<float, float>(SpringArm->CameraLagSpeed, OriginCameraLagSpeed, 3 * GetWorld()->GetDeltaSeconds());;
	}
	else if(bOnDefense)
	{
		GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp<float, float>(GetCharacterMovement()->MaxWalkSpeed, WalkSpeed, 4 * GetWorld()->GetDeltaSeconds());
		//MoveSpeedMultiplier = OriginMoveSpeed;
		SpringArm->CameraLagSpeed = FMath::Lerp<float, float>(SpringArm->CameraLagSpeed, OriginCameraLagSpeed, 3 * GetWorld()->GetDeltaSeconds());;
	}

	MoveDirection.Normalize();
	
	AddMovementInput(MoveDirection, 1);
	MoveDirection.Set(0.f, 0.f, 0.f);
}

void ACS_Player::Walk()
{
	bIsWalk = true;
}

void ACS_Player::WalkEnd()
{
	bIsWalk = false;
}

void ACS_Player::TurnAtRate(float Rate)
{
	if (TargetSystemComponent->GetIsLockOn() || bOnExecute) return;

	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACS_Player::LookUpAtRate(float Rate)
{
	if (TargetSystemComponent->GetIsLockOn() || bOnExecute) return;

	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACS_Player::Dodge()
{
	if (Controller != NULL && !bIsRolling && !bOnAttack && !bOnHit && !playerAnim->bIsInAir && !playerAnim->bIsHanging)
	{
		// 스태미나가 충분하지 않다면 Dodge 실패
		if (Stamina - ((MaxStamina / 100) * 10.f) < 0.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not enough Stamina"));
			return;
		}
		else
		{
			StaminaTimerActive();
			DecrementStamina((MaxStamina / 100) * 15.f);
		}
				
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		FVector FDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		FVector RDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		EvadeDirection = FDirection * InputVertical + RDirection * InputHorizontal;

		NotifyMoveVertical = InputVertical;
		NotifyMoveHorizontal = InputHorizontal;

		if (EvadeDirection.IsZero())
		{
			EvadeDirection = GetActorForwardVector();
		}
		EvadeDirection.Normalize();

		if (playerAnim != nullptr)
		{
			// 공중 아닐때 실행
			if (!playerAnim->bIsInAir)
			{
				// 캐릭터를 마지막으로 입력받은 벡터로 회전시킨다
				SetActorRotation(FRotator(0.f, ULJSMathHelpers::GetAngleToDirection(FVector::ForwardVector, EvadeDirection), 0.f));
				
				playerAnim->PlayMontage(currentMontage);

				bOnDefense = false;
				bIsRolling = true;
				playerAnim->bIsRolling = true;
				playerAnim->bIsSprint = false;
			}
		}
	}
}

void ACS_Player::Attack()
{
	// 선입력 구간이 활성화 일때만 체크한다.
	if (bIsInputAttackKey)
	{
		// 다음 공격을 자동으로 실행한다.
		bPlayLightAttackAuto = true;
		bIsInputAttackKey = false;
	}

	if (Controller != NULL && !bOnAttack && !bIsRolling && bCanAttack && !bOnHit && !playerAnim->bIsInAir && !playerAnim->bIsHanging)
	{	
		// 스태미나가 충분하지 않다면 공격 실패
		if (Stamina - ((MaxStamina / 100) * 10.f) < 0.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not enough Stamina"));
			return;
		}
		else
		{
			StaminaTimerActive();
			DecrementStamina((MaxStamina / 100) * 15.f);
		}

		// 무기 장비중 아니면 true로 변경
		if (!playerAnim->bIsEquip)
		{
			playerAnim->bIsEquip = true;
			Weapon->SetBladeOnOff(true);
		}

		if (playerAnim != nullptr)
		{
			Weapon->SetDamageAmount(DamageTypeAry[LightAttackNum].DamageAmount);
			SetLightAttackMontage();
			playerAnim->PlayMontage(currentMontage);
			currentMontage = nullptr;
			SetOnAttack(true);
			SetMoveable(false);
			bOnDefense = false;
		}

		bPlayLightAttackAuto = false;
		bPlayHeavyAttackAuto = false;

		ComboResetTimerActive();
	}
}

void ACS_Player::HeavyAttack()
{
	// 선입력 구간이 활성화 일때만 체크한다.
	if (bIsInputAttackKey)
	{
		// 다음 공격을 자동으로 실행한다.
		bPlayHeavyAttackAuto = true;
		bIsInputAttackKey = false;
	}

	if (Controller != NULL && !bOnAttack && !bIsRolling && bCanAttack && !bOnHit && !playerAnim->bIsInAir && !playerAnim->bIsHanging)
	{
		// 스태미나가 충분하지 않다면 공격 실패
		if (Stamina - ((MaxStamina / 100) * 10.f) < 0.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not enough Stamina"));
			return;
		}
		else
		{
			StaminaTimerActive();
			DecrementStamina((MaxStamina / 100) * 15.f);
		}
		playerAnim->bIsSprint = false;

		if (TargetSystemComponent->GetIsLockOn())
		{
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}

		// 무기 장비중 아니면 true로 변경
		if (!playerAnim->bIsEquip)
		{
			playerAnim->bIsEquip = true;
			Weapon->SetBladeOnOff(true);
		}

		if (playerAnim != nullptr)
		{
			Weapon->SetDamageAmount(DamageTypeAry[HeavyAttackNum + 2].DamageAmount);
			SetHeavyAttackMontage();
			playerAnim->PlayMontage(currentMontage);
			currentMontage = nullptr;
			SetOnAttack(true);
			SetMoveable(false);
			bOnDefense = false;
		}

		bPlayLightAttackAuto = false;
		bPlayHeavyAttackAuto = false;

		ComboResetTimerActive();
	}
}

void ACS_Player::SpecialAttack()
{
	if (Controller != NULL && !bOnAttack && !bIsRolling && !playerAnim->bIsSprint && bCanAttack && !bOnHit && !playerAnim->bIsInAir && !playerAnim->bIsHanging)
	{
		FHitResult lineHit;
		FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

		// 트레이스 시작, 끝 포지션
		FVector StartPoint = GetActorLocation();
		FVector EndPoint = GetActorLocation() + (GetActorForwardVector() * 150.f);

		//DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Yellow, false, 2.f);
		bool isLineHit = GetWorld()->LineTraceSingleByChannel(lineHit, StartPoint, EndPoint, ECollisionChannel::ECC_GameTraceChannel3, TraceParams);

		if (isLineHit)
		{
			ACS_Enemy* enemy = Cast<ACS_Enemy>(lineHit.GetActor());

			// 적 캐릭터 캐스팅 성공. 적 존재
			if (enemy != nullptr)
			{
				// 처형 조건 만족하지 않았다면 리턴한다.
				if (!enemy->IsExecuteReady()) return;

				// 보스일 경우
				if (enemy->GetEnemyType() == EEnemyType::ET_Boss)
				{
					currentMontage = playerAnim->SAttackMontage;
					enemy->Executed(this, EndPoint);
					TargetSystemComponent->SetUseSpringArmRotation(true);
				}
				// 일반 몬스터일 경우
				else
				{
					currentMontage = playerAnim->SAttack2Montage;
					enemy->Executed(this, EndPoint - (GetActorForwardVector() * 100.f));
				}

				ExecuteTarget = enemy;
			}
			else
				return;
		}
		else
			return;
			

		// 무기 장비중 아니면 true로 변경
		if (!playerAnim->bIsEquip)
		{
			playerAnim->bIsEquip = true;
		}

		if (playerAnim != nullptr)
		{
			playerAnim->PlayMontage(currentMontage);
			currentMontage = nullptr;
			SetOnAttack(true);
			SetMoveable(false);
			bOnDefense = false;
			bOnExecute = true;
		}
	}
}

void ACS_Player::Sprint()
{
	if (Controller != NULL && !bOnAttack && !bIsRolling && !bOnDefense && Stamina > 0.f && IsTryToMoveActor() && !playerAnim->bIsInAir && !ClimbSystemComponent->bIsHanging)
	{
		playerAnim->bIsSprint = true;

		if (TargetSystemComponent->GetIsLockOn())
		{
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
	}
}

void ACS_Player::StopSprint()
{
	if (Controller != NULL && !bOnAttack && !bIsRolling)
	{
		playerAnim->bIsSprint = false;

		if (TargetSystemComponent->GetIsLockOn())
		{
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
	}
}

void ACS_Player::EquipWeapon()
{
	// 공격, 구르기, 공중 일때는 실행하지 않는다.
	if (Controller == NULL || bOnAttack || bIsRolling || playerAnim->bIsInAir) return;
	// 적이 락온된 상태라면 락온을 해제한다.
	if (TargetSystemComponent->GetIsLockOn()) TargetSystemComponent->UnLockTarget();

	playerAnim->bIsEquip = !playerAnim->bIsEquip;

	currentMontage = playerAnim->bIsEquip ? playerAnim->EquipMontage : playerAnim->UnarmMontage;
	playerAnim->PlayMontage(currentMontage);

	Weapon->SetBladeOnOff(playerAnim->bIsEquip);
}
 
void ACS_Player::InputLMB()
{
	Attack();
}

void ACS_Player::InputSpaceBar()
{
	Slide();

	if (ClimbSystemComponent->bIsHanging)
	{
		if (ClimbSystemComponent->bIsTurnBack)
		{
			FVector LaunchVelocity = (GetActorForwardVector() * -400.f) + FVector(0.f, 0.f, 700.f);
			LaunchCharacter(LaunchVelocity, false, false);
			ClimbSystemComponent->ExitLedge();
			FRotator NewRot = UKismetMathLibrary::MakeRotator(GetActorRotation().Roll, GetActorRotation().Pitch, GetActorRotation().Yaw - 180.f);
			SetActorRotation(NewRot);
			return;
		}

		ClimbSystemComponent->ClimbJumpSide();
		ClimbSystemComponent->ClimbJumpUp();
	}
}

void ACS_Player::InputShift()
{
	bPressShift = true;

	Sprint();
}

void ACS_Player::ReleaseShift()
{
	bPressShift = false;

	StopSprint();
}

void ACS_Player::InputR()
{
	EquipWeapon();
}

void ACS_Player::InputMWheel()
{
	
}

void ACS_Player::InputTestBtn()
{
	playerAnim->PlayMontage(playerAnim->TestMontage);
}

void ACS_Player::InputTab()
{
	TargetSystemComponent->SetTargetSearchType(ETargetSearchType::TST_None);
	TargetSystemComponent->SearchTarget();
}

void ACS_Player::InputRMB()
{
	Defense();
}

void ACS_Player::InputQ()
{
	if(!ClimbSystemComponent->bIsHanging)
		ACharacter::Jump();
	else
		ClimbSystemComponent->ExitLedge();
}

void ACS_Player::ClimbUp()
{
	ClimbSystemComponent->ClimbLedge();
}

void ACS_Player::ClimbTurn()
{
	ClimbSystemComponent->ClimbTurn();
}

void ACS_Player::ClimbTurnBack()
{
	if (ClimbSystemComponent->bIsHanging)
	{
		ClimbSystemComponent->bIsTurnBack = true;
		playerAnim->PlayMontage(playerAnim->ClimbTurnBack_Montage);
	}
}

void ACS_Player::ClimbTurnBackEnd()
{
	if (ClimbSystemComponent->bIsHanging)
	{
		ClimbSystemComponent->bIsTurnBack = false;

		playerAnim->StopAllMontages(0.25f);
	}
}

void ACS_Player::MWheelUp()
{
	TargetSystemComponent->SetTargetSearchType(ETargetSearchType::TST_WheelUp);
	TargetSystemComponent->SearchTarget();
}

void ACS_Player::MWheelDown()
{
	TargetSystemComponent->SetTargetSearchType(ETargetSearchType::TST_WheelDown);
	TargetSystemComponent->SearchTarget();
}

void ACS_Player::SetOnAttack(bool Value)
{
	bOnAttack = Value;
}

void ACS_Player::SetRootRotation(bool Value)
{
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = Value;
}

void ACS_Player::AttachWeapon()
{
	Weapon = GetWorld()->SpawnActor<ACS_Weapon>(WeaponClass);

	// 무기 Attach
	Weapon->SetOwner(this);
	SetWeaponLocation("Socket_twinblade");
}

void ACS_Player::SetWeaponLocation(FName SocketName)
{
	const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName(SocketName);
	if (WeaponSocket)
	{
		WeaponSocket->AttachActor(Weapon, GetMesh());
	}
	Weapon->SetActorLocation(GetMesh()->GetSocketLocation(SocketName));
	Weapon->SetActorRotation(GetMesh()->GetSocketRotation(SocketName));
}

void ACS_Player::SetVelocityToDirection(FVector Direction, float Value)
{
	FVector DesiredMoveVector = Direction * Value;
	GetCharacterMovement()->Velocity = DesiredMoveVector;
}

bool ACS_Player::IsTryToMoveActor()
{
	// 마지막으로 입력받은 벡터가 0과 같다면 입력이 없다는 뜻이므로 false를 반환한다.
	if (InputVertical == 0.f && InputHorizontal == 0.f /*ForwardDirection == FVector(0.f) && RightDirection == FVector(0.f)*/)
		return false;
	
	// 입력이 있다면 true를 반환한다.
	return true;
}

void ACS_Player::PlayMovementAnimation()
{
	playerAnim->StopNowPlayingMontage();
	currentMontage = nullptr;
}

void ACS_Player::RotateActor()
{
	if (TargetSystemComponent->GetIsLockOn()) return;

	FRotator ControllRotation = GetControlRotation();
	ControllRotation = FRotator(0.f, ControllRotation.Yaw, 0.f);

	FRotator NewRotator = UKismetMathLibrary::RInterpTo(GetActorRotation(), ControllRotation, GetWorld()->GetDeltaSeconds(), 6.0f);

	SetActorRotation(NewRotator);
}

float ACS_Player::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bOnHit || bInvincibility || bOnExecute) return DamageAmount;
	if (bIsRolling) bIsRolling = false;
	if (GetCharacterMovement()->bOrientRotationToMovement && TargetSystemComponent->GetIsLockOn()) GetCharacterMovement()->bOrientRotationToMovement = false;
	if (bOnDefense)
	{
		// 방어 성공하였는지 검사
		if (IsDefenseSucceed(DamageCauser))
		{
			// 이미 가드 피격 애니메이션 실행 중인 경우에는 return 한다.
			if (playerAnim->GetCurrentActiveMontage() == playerAnim->DefenseHitMontage) return DamageAmount;

			// 이동 조작 불가로 설정한다.
			SetMoveable(false);
			// 가드 피격 애니메이션 실행
			playerAnim->PlayMontage(playerAnim->DefenseHitMontage);
			// 방어 성공시 스태미나 10퍼센트 차감하고 return한다
			DecrementStamina((MaxStamina / 100) * 20.f);
			GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(GuardCameraShake);
			// 스태미나 복구 타이머를 작동한다.
			StaminaTimerActive();
			// 가드 포인트 모두 감소시 가드브레이크 몽타주를 실행한다.
			if (Stamina <= 0.f)
			{
				playerAnim->PlayMontage(playerAnim->DefenseBroken_Montage);
			}
			return DamageAmount;
		}
	}

	DecrementHealth(DamageAmount);				// 데미지 적용
	GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(OnHitCameraShake);
	ChangeMatTimerActive();

	//if (Health <= 0.f)
	//{
	//	//Die();

	//	return DamageAmount;
	//}

	bOnHit = true;
	SetOnAttack(false);
	SetMoveable(false);
	SetHitMontage(DamageCauser);					// 피격 방향에 따라 애니메이션 세팅
	playerAnim->PlayMontage(currentMontage);		// 피격 애니메이션 실행
	currentMontage = nullptr;

	return DamageAmount;
}

void ACS_Player::DecrementHealth(float Amount)
{
	Health -= Amount;
}

void ACS_Player::DecrementStamina(float Amount)
{
	if (Stamina - Amount < 0)
	{
		Stamina = 0;
		return;
	}

	Stamina -= Amount;
}

void ACS_Player::SetHitMontage(AActor* DamageCauser)
{
	float HitAngle = ULJSMathHelpers::GetAngleToDamageCauser(this, DamageCauser);
	EnemyDamageCauser = DamageCauser;

	// Front
	if (-45.f <= HitAngle && HitAngle <= 45.f)
	{
		currentMontage = playerAnim->HitFrontMontage;
	}
	// Left
	else if (45.f <= HitAngle && HitAngle <= 135.f)
	{
		currentMontage = playerAnim->HitLeftMontage;
	}
	// Right
	else if (-135.f <= HitAngle && HitAngle <= -45.f)
	{
		currentMontage = playerAnim->HitRightMontage;
	}
	// Back
	else if ((-180.f <= HitAngle && HitAngle <= -135.f) ||
		(135.f <= HitAngle && HitAngle <= 180.f))
	{
		currentMontage = playerAnim->HitBackMontage;
	}
}

void ACS_Player::Defense()
{
	if (Controller != NULL && !bOnDefense && !bOnAttack && !bIsRolling && !bOnHit && !playerAnim->bIsInAir && !playerAnim->bIsHanging && playerAnim->bIsEquip)
	{
		bOnDefense = true;

		playerAnim->PlayMontage(playerAnim->DefenseMontage);
	}
}

void ACS_Player::DefenseLoop()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Defense Loop")));

	if (Controller != NULL && bOnDefense)
	{
		if (playerAnim->nowPlayingMontage != playerAnim->DefenseMontage)
		{
			playerAnim->Montage_JumpToSection("Loop", playerAnim->DefenseMontage);

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Jump To Loop Section")));
		}
	}
}

void ACS_Player::EndDefense()
{
	if (bOnAttack) return;

	bOnDefense = false;
	playerAnim->Montage_JumpToSection("End", currentMontage);
	currentMontage = nullptr;
}

bool ACS_Player::IsDefenseSucceed(AActor* DamageCauser)
{
	float HitAngle = ULJSMathHelpers::GetAngleToDamageCauser(this, DamageCauser);
	EnemyDamageCauser = DamageCauser;

	// Front
	// 플레이어 정면 기준으로 -60 ~ 60도 안에서 막았을 경우 방어 성공
	if (-60.f <= HitAngle && HitAngle <= 60.f) return true;
	else return false;
}

FVector ACS_Player::GetNewMoveDirection(MoveDirectionStatus Status)
{
	int v = 0, h = 0;

	switch (Status)
	{
	case MoveDirectionStatus::MDS_Forward:
		v = 1;
		h = 0;
		break;
	case MoveDirectionStatus::MDS_Left:
		v = 0;
		h = -1;
		break;
	case MoveDirectionStatus::MDS_Right:
		v = 0;
		h = 1;
		break;
	case MoveDirectionStatus::MDS_Backward:
		v = -1;
		h = 0;
		break;
	case MoveDirectionStatus::MDS_MAX:
		break;
	default:
		return FVector::ZeroVector;
	}

	return GetActorForwardVector() * v + GetActorRightVector() * h;
}

void ACS_Player::UpdateStats(float DeltaTime)
{	
	if (playerAnim->bIsSprint)
	{
		if (IsTryToMoveActor() && Stamina > 0.f)
		{
			Stamina -= 50 * DeltaTime;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Stamina - 20"));
		}

		// 스태미나가 0이하로 내려가면 스프린트 false로 바꾸기
		if (Stamina <= 0.f)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Stamina < 0.f"));

			playerAnim->bIsSprint = false;
			if (TargetSystemComponent->GetIsLockOn())
			{
				GetCharacterMovement()->bOrientRotationToMovement = true;
			}
		}
	}
	else 
	{
		// 스태미너 타이머가 끝난상태에서만 스태미너가 증가한다.
		if (Stamina < MaxStamina && bIsStaminaTimerEnd)
		{
			Stamina += 100.f * DeltaTime;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Stamina + 20"));
		}
	}

	if (Health < HealthCover)
	{
		HealthCover = FMath::Lerp<float, float>(HealthCover, Health, 2 * DeltaTime);
	}
}

void ACS_Player::Slide()
{
	if (Controller != NULL && !bIsRolling && !bOnAttack && !bOnHit && !playerAnim->bIsInAir && !playerAnim->bIsHanging)
	{
		// 스태미나가 충분하지 않다면 Dodge 실패
		if (Stamina - ((MaxStamina / 100) * 10.f) < 0.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not enough Stamina"));
			return;
		}
		else
		{
			StaminaTimerActive();
			DecrementStamina((MaxStamina / 100) * 15.f);
		}

		bool bRotate = false;

		if (!TargetSystemComponent->GetIsLockOn())
		{
			currentMontage = playerAnim->DodgeMontage;
			bRotate = true;
		}
		else
		{
			if ((InputVertical == 0 && InputHorizontal == 0) ||
				(InputVertical == 1 && InputHorizontal == 0) ||
				(InputVertical == 1 && InputHorizontal == 1) ||
				(InputVertical == 1 && InputHorizontal == -1) ||
				(InputVertical == -1 && InputHorizontal == 1) ||
				(InputVertical == -1 && InputHorizontal == -1))
			{
				currentMontage = playerAnim->SlideF_Montage;
				bRotate = true;
			}
			else if (InputVertical == 0 && InputHorizontal == 1)
				currentMontage = playerAnim->SlideR_Montage;
			else if (InputVertical == 0 && InputHorizontal == -1)
				currentMontage = playerAnim->SlideL_Montage;
			else if ((InputVertical == -1 && InputHorizontal == 0))
				currentMontage = playerAnim->SlideB_Montage;
		}

		SetMoveable(false);

		if (bRotate)
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			FVector FDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			// get right vector 
			FVector RDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			EvadeDirection = FDirection * InputVertical + RDirection * InputHorizontal;

			NotifyMoveVertical = InputVertical;
			NotifyMoveHorizontal = InputHorizontal;
			if(TargetSystemComponent->GetIsLockOn())
				GetCharacterMovement()->bOrientRotationToMovement = true;
			// 캐릭터를 마지막으로 입력받은 벡터로 회전시킨다
			SetActorRotation(FRotator(0.f, ULJSMathHelpers::GetAngleToDirection(FVector::ForwardVector, EvadeDirection), 0.f));
		}

		bOnDefense = false;
		bIsRolling = true;
		playerAnim->PlayMontage(currentMontage);
		currentMontage = nullptr;

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("V %f H %F"), InputVertical, InputHorizontal));
	}
}


FVector ACS_Player::GetUnitAxisForward()
{
	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	return FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
}

FVector ACS_Player::GetUnitAxisRight()
{
	// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get right vector 
	return FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
}

void ACS_Player::StaminaRestoreTimer()
{
	--StaminaRestoreTime;
	if (StaminaRestoreTime <= 0)
	{
		bIsStaminaTimerEnd = true;
		GetWorldTimerManager().ClearTimer(StaminaRestoreTimerHandle);
	}
}

void ACS_Player::StaminaTimerActive()
{
	// 이미 타이머 작동 중이라면, 작동 중지한다.
	if (GetWorldTimerManager().IsTimerActive(StaminaRestoreTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(StaminaRestoreTimerHandle);
	}

	StaminaRestoreTime = 1.f;
	bIsStaminaTimerEnd = false;
	GetWorldTimerManager().SetTimer(StaminaRestoreTimerHandle, this, &ACS_Player::StaminaRestoreTimer, 1.0f, true);
}

void ACS_Player::PlayCamShake()
{
	GetWorld()->GetFirstPlayerController()->ClientPlayCameraShake(AttackCameraShake);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Play CameraShake"));
}

void ACS_Player::ChangeMatTimer()
{
	ChangeMatTime -= 0.1f;

	if (ChangeMatTime <= 0.f)
	{
		GetMesh()->SetMaterial(0, Mat_Origin);
		GetWorldTimerManager().ClearTimer(ChangeMatTimerHandle);
	}
}

void ACS_Player::ChangeMatTimerActive()
{
	if (!GetWorldTimerManager().IsTimerActive(ChangeMatTimerHandle))
	{
		ChangeMatTime = 0.3f;
		GetMesh()->SetMaterial(0, Mat_GridInst);
		GetWorldTimerManager().SetTimer(ChangeMatTimerHandle, this, &ACS_Player::ChangeMatTimer, 0.1f, true);
	}
}

void ACS_Player::ExecuteApplyDamage(float DamageAmount)
{
	if (ExecuteTarget == nullptr) return;

	ExecuteTarget->ExecutedTakeDamage(DamageAmount);
}

void ACS_Player::ExecuteRotateToTarget()
{
	if (TargetSystemComponent->GetIsLockOn()) return;

	FRotator TargetRotator = UKismetMathLibrary::FindLookAtRotation(SpringArm->GetComponentLocation(), ExecuteTarget->GetActorLocation());

	FRotator DesiredRotator = FMath::RInterpTo(GetControlRotation(), TargetRotator, GetWorld()->GetDeltaSeconds(), 10.f);
	GetController()->SetControlRotation(DesiredRotator);
}

void ACS_Player::SetLightAttackMontage()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("LightAttackNum: %d"), LightAttackNum));

	currentMontage = playerAnim->LightAttackAry[LightAttackNum];
	LightAttackNum = LightAttackNum + 1 >= 2 ? 0 : LightAttackNum + 1;
}

void ACS_Player::SetHeavyAttackMontage()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("HeavyAttackNum: %d"), HeavyAttackNum));
		
	currentMontage = playerAnim->HeavyAttackAry[HeavyAttackNum];
	HeavyAttackNum = HeavyAttackNum + 1 >= 3 ? 0 : HeavyAttackNum + 1;
}

void ACS_Player::ComboResetTimer()
{
	ComboResetTime -= GetWorld()->GetDeltaSeconds();

	if (ComboResetTime <= 0.f)
	{
		LightAttackNum = 0;
		HeavyAttackNum = 0;
		GetWorldTimerManager().ClearTimer(ComboResetTimerHandle);
	}
}

void ACS_Player::ComboResetTimerActive()
{
	if (!GetWorldTimerManager().IsTimerActive(ComboResetTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(ComboResetTimerHandle);
	}

	ComboResetTime = 2.f;
	GetWorldTimerManager().SetTimer(ComboResetTimerHandle, this, &ACS_Player::ComboResetTimer, GetWorld()->GetDeltaSeconds(), true);
}