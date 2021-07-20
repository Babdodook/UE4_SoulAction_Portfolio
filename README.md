# 언리얼 엔진 포트폴리오
언리얼 엔진4를 이용한 액션RPG 게임 기능 구현


# 목차
* [소개 영상](https://youtu.be/b2idKuy5u80)
* [구현 내용](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#구현-내용)
* [마무리](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#마무리)
  
  
# 구현 내용
* [몬스터 FSM](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#몬스터-FSM)
* [Climb(파쿠르)시스템](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#Climb-System)
* [Foot IK 시스템](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#Foot-IK-System)
* [타겟팅 시스템](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#타겟팅-시스템)
* [연계 공격](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#연계-공격)
  
## 몬스터 FSM
  
![attack1](https://user-images.githubusercontent.com/48229283/126050728-327a1136-3c65-435b-accb-d81a43aac9a2.gif) | ![attack3](https://user-images.githubusercontent.com/48229283/126050733-68c43cac-b7ed-4b30-bd46-45e8e421bcfa.gif)
:-------------------------:|:-------------------------:
  
### 유한상태 기계 플로우

![fsm2](https://user-images.githubusercontent.com/48229283/125847148-91bfbfc7-72bf-46ca-b882-9f1e10cf061f.PNG)
  
* 공격패턴마다 해당하는 사거리(Range)와 시야각(Angle)을 가진다.
* 공격 딜레이 중 일때는 랜덤한 방향으로 이동
* 공격 딜레이가 끝날 시에는 타겟을 추적하여 이동
* 타겟 추적 중 사거리와 시야각을 만족하면 공격
  
  
### 클래스 상속
  
![클래스상속22](https://user-images.githubusercontent.com/48229283/125847161-427f0e2b-eaca-4915-8cb4-def8d738c40f.PNG)
  
* Enemy 베이스 클래스를 상속하여 override하여 사용
  
  
### 상세 기능


### 1. 사거리 및 시야각
  
* 사거리와 시야각 만족시 공격
  
![사거리시야각](https://user-images.githubusercontent.com/48229283/126049883-d90d1373-9f6d-4367-86ad-790db0b008d0.PNG)
  
```cpp
	// 공격 사정거리와 각도 모두 만족할 경우에 공격이 가능하도록 한다.
	// 공격 딜레이가 0이하 일때 실행한다.
	if (IsTargetExistInAttackArea() && IsAttackDelayZero())
	{
		// 공격을 실행한다.
		Attack();
	}
```
  
* 사거리와 시야각 조건 검사
  
```cpp
bool ACS_Enemy::IsTargetExistInAttackArea()
{
	if (CombatTarget == nullptr) return false;

	// 공격 거리와 공격각도 모두 만족할 경우에 true를 반환한다. 그렇지 않은 경우 false를 반환한다.
	if (GetTargetDistance() <= AttackRange && ULJSMathHelpers::GetAngleToTarget(this, CombatTarget) <= AttackAngle) return true;
	else return false;
}
```
  
* 벡터 내적을 통해 타겟과의 각도 구하기
  
```cpp
float ULJSMathHelpers::GetAngleToTarget(AActor* OwnActor, AActor* Target)
{
	// 타겟과 현재 액터의 방향 벡터를 구함
	FVector direction = (Target->GetActorLocation() - OwnActor->GetActorLocation()).GetSafeNormal();

	// 현재 액터의 forward벡터와 direction 벡터의 내적, 라디안각도 구함
	float dot = FVector::DotProduct(OwnActor->GetActorForwardVector(), direction);

	// 호도 -> 각도 변환
	float Angle = FMath::RadiansToDegrees(FMath::Acos(dot));

	return Angle;
}
```
  
  
### 2. 공격 패턴 결정
  
* 배열에 랜덤으로 결정된 공격패턴 Add
  
![컴뱃액션2](https://user-images.githubusercontent.com/48229283/125847171-c53b842b-6352-4d17-aa3e-0d11056c5ca5.PNG)
  
* 배열에 추가하면서 공격에 해당하는 사거리(AttackRange)와 시야각(AttackAngle) 할당
  
```cpp
int32 randnum = GenerateCombatRandNum();

	switch (randnum)
	{
	case 1:
		CombatActions.Add(EnemyAnim->Attack1Montage);
		SetAttackArea(AICombatDataAry[0].AttackRange, AICombatDataAry[0].AttackAngle);

		break;
	case 2:
		CombatActions.Add(EnemyAnim->Attack2Montage);
		SetAttackArea(AICombatDataAry[1].AttackRange, AICombatDataAry[1].AttackAngle);

		break;
```
  
* 사거리와 시야각 정보는 DataTable로 연동
  
<img src="https://user-images.githubusercontent.com/48229283/125547881-d672a383-a885-47b1-87cd-c6db872624b3.PNG" width="300" height="150">  
  
```cpp
USTRUCT(Atomic, BlueprintType)
struct FAICombatStruct : public FTableRowBase
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	float AttackRange;

	UPROPERTY(EditAnywhere)
	float AttackAngle;
};
```
  
### 3. 잔상 구현
  
![잔상1](https://user-images.githubusercontent.com/48229283/125550644-92a17e64-d83b-44f4-986e-e7e2d5029476.gif) | ![잔상2](https://user-images.githubusercontent.com/48229283/125550654-b84c3b63-1585-49ff-b4b8-618fcf2ab462.gif)
:-------------------------:|:-------------------------:
  
  
* 잔상 머터리얼 준비
  
![잔상](https://user-images.githubusercontent.com/48229283/125549805-2182b869-2587-466f-ab6e-6f3193898ed3.PNG) | ![잔상인스턴스](https://user-images.githubusercontent.com/48229283/125549815-350e5495-0252-43c9-b7ca-91b77251aa35.PNG)
:-------------------------:|:-------------------------:
잔상 머터리얼 | 머터리얼 인스턴스
  
  
* PoseableMesh 컴포넌트를 가진 액터를 스폰
  
![bp고스트](https://user-images.githubusercontent.com/48229283/125551875-25bae332-6fe4-4dae-a27e-c272d3d9cbb8.PNG)
  
```cpp
void ACS_Boss1::SpawnGhostTrail(float SpawnDelay)
{
	// 타이머 실행중이라면 실행하지 않는다.
	if (GetWorldTimerManager().IsTimerActive(SpawnGhostTrailTimerHandle)) return;

	// 스폰 딜레이를 지정한다.
	SpawnGhostTrailTime = SpawnDelay;
	GetWorldTimerManager().SetTimer(SpawnGhostTrailTimerHandle, this, &ACS_Boss1::SpawnGhostTrailTimer, GetWorld()->GetDeltaSeconds(), true);

	// 고스트 트레일 액터를 스폰한다.
	ACS_GhostTrail* GhostTrail = GetWorld()->SpawnActor<ACS_GhostTrail>(GhostTrailClass, GetMesh()->GetComponentTransform());
	// 현재 Boss1의 메쉬를 포즈로 지정한다.
	GhostTrail->SetPose(GetMesh());
}
```
  
* 액터의 포즈를 복사
  
```cpp
void ACS_GhostTrail::SetPose(USkeletalMeshComponent* Mesh)
{
	PoseableMesh->CopyPoseFromSkeletalComponent(Mesh);
}
```
  
* 타이머로 사라지게 만들기
  
```cpp
void ACS_GhostTrail::TrailDissapearTimer()
{
	Opacity = FMath::Lerp<float, float>(Opacity, -0.5f, 5.f * GetWorld()->GetDeltaSeconds());

	for (int i = 0; i < PoseableMesh->GetNumMaterials(); i++)
	{
		UMaterialInstanceDynamic* mat = Cast<UMaterialInstanceDynamic>(PoseableMesh->GetMaterial(i));
		mat->SetScalarParameterValue("Opacity", Opacity);
	}

	if (Opacity <= 0.f)
	{
		GetWorldTimerManager().ClearTimer(TrailDissapearTimeHandle);
		Destroy();
	}
}
```
  
## Climb System
  
![climb1](https://user-images.githubusercontent.com/48229283/126050236-6f21307e-f7ec-4a6a-b8e6-c10d9e0a86f2.gif) | ![climb2 gif](https://user-images.githubusercontent.com/48229283/126050238-66b76e65-e072-49c7-b09f-12c9ce451f96.gif)
:-------------------------:|:-------------------------:
  
### Grab Ledge
  
* Pelvis 본에 소켓을 추가한다.
  
![socketpelvis](https://user-images.githubusercontent.com/48229283/125717988-07c74545-05d1-49df-a759-464da7161e49.PNG)
  
* 캐릭터 전방으로 트레이스 Forward Trace
* 캐릭터 전방 위에서 밑으로 트레이스 Height Trace
  
![1](https://user-images.githubusercontent.com/48229283/125782313-1a702cc4-512f-4f3f-bb1f-fc9f133f03d2.PNG)
  
* 캐릭터가 점프 하였을 때 Hit 상황
  
![2](https://user-images.githubusercontent.com/48229283/125782318-f9d98fde-449b-4ca7-b3f3-16f3a1b285fb.PNG)
  
* Height Trace의 outHit.Location의 Z값과 Socket_Pelvis의 Z값으로 ZValue를 구한다.
  
![3](https://user-images.githubusercontent.com/48229283/125835081-d6e815f8-704d-48f9-bc98-2144d26f2d04.PNG)

* InRange로 ZValue가 적정 구간에 있는지 검사한다.
  
```cpp
bool UCS_ClimbSystem::IsHipToLedge()
{
    FVector SocketLocation = Cast<ACharacter>(OwnerActor)->GetMesh()->GetSocketLocation("Socket_Pelvis");

    float ZValue = SocketLocation.Z - HeightLocation.Z;
    // ZValue가 HipToLedgeMin ~ HipToLedgeMax 사이에 있다면 True를 반환한다.
    return UKismetMathLibrary::InRange_FloatFloat(ZValue, HipToLedgeMin, HipToLedgeMax);
}
```
  
* ZValue가 조건을 만족한다면 난간 붙잡기(Grab Ledge)를 실행한다.
  
```cpp
	// Pelvis 소켓이 난간을 붙잡을 수 있는 범위에 있는지?
        // 난간 올라서는 중 아닐때
        // 공중에 떠있는 상태일때만 가능
        if (IsHipToLedge() && !bIsClimbingLedge && playerClass->playerAnim->bIsInAir)
            GrabLedge();    // 난간 잡기 실행
```
  
### Move To Ledge
  
* Grab Ledge를 실행하게 되면 캐릭터를 난간으로 이동시켜야 한다.
* 캐릭터가 위치해야할 난간의 위치(Location)를 구한다.
  
![4](https://user-images.githubusercontent.com/48229283/125836506-7e4778fe-c4e3-41e1-a95a-a6bcca3217b6.PNG)
  
![5](https://user-images.githubusercontent.com/48229283/125836510-d7c1c324-f324-4863-b6a8-26c4420000a6.PNG)
  
![6](https://user-images.githubusercontent.com/48229283/125836514-0b564388-3eec-4bc1-b111-b2aa782e970f.PNG)
  
![7](https://user-images.githubusercontent.com/48229283/125836516-4338f86c-6757-4956-a5bd-f0e1b67cde0e.PNG)
  
* 난간의 위치를 구하고 캐릭터가 배치될 Z값 만큼 빼준다.
  
```cpp
FVector UCS_ClimbSystem::MoveToLocation()
{
    FVector NewWallNormal = WallNormal * FVector(22.f,22.f,0.f);
    float MakeXValue = NewWallNormal.X + WallLocation.X;
    float MakeYValue = NewWallNormal.Y + WallLocation.Y;
    float MakeZValue = WallZLocation + (WallZSize * 0.5f) - 100.f;
    FVector DesiredLocation = FVector(MakeXValue, MakeYValue, MakeZValue);
    return DesiredLocation;
}
```
  
* MoveComponentTo로 이동
  
```cpp
	// 붙잡아야하는 난간의 위치와 회전값을 할당한다.
        TargetRelativeLocation = MoveToLocation();
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
            0.2f,                               // 보간 시간
            false,                              // true이면 회전에 항상 최단 경로를 사용
            EMoveComponentAction::Move,         // 필요한 움직임 행동
            LatentInfo                          // The latent action
        );
```
  
## Foot IK System
  
![footik1](https://user-images.githubusercontent.com/48229283/125841320-159c57a7-521d-4c89-8707-4767205570a9.gif) | ![footik2](https://user-images.githubusercontent.com/48229283/125841334-7d79a38e-cbb9-4f41-8923-04cd002a312a.png)
:-------------------------:|:-------------------------:
IK1 | IK2
  
### Hip displacement

* 캐릭터의 발 밑으로 트레이스를 한다.
  
![hipoffset1](https://user-images.githubusercontent.com/48229283/126048197-1886e42e-1938-4bc2-ace1-35144dccfc33.PNG)
  
* 두 트레이스된 Distance를 비교하여 큰 쪽을 반환한다.
  
![hipoffset2](https://user-images.githubusercontent.com/48229283/126048443-8883956d-5fc9-4ebc-bd94-0df5be15e627.PNG)
  
* Distance만큼 루트 본을 캐릭터의 캡슐보다 밑으로 내려주어야 한다.

```cpp
void UCS_IKFootSystem::IKProcessing()
{
	// Hip displacement
	RFootDistance = IKHipTrace(RFootSocketName);
	LFootDistance = IKHipTrace(LFootSocketName);

	HipOffset = HipDisplacement(RFootDistance >= LFootDistance ? RFootDistance : LFootDistance);

	//... 코드 생략
}

float UCS_IKFootSystem::HipDisplacement(float distanceToGround)
{
	float displacement;
	// CapsuleHalfHeight를 빼는 이유는 트레이스 시작위치가 캡슐의 절반 높이이기 때문.
	if (distanceToGround != 0.f)
		displacement = CapsuleHalfHeight - distanceToGround;
	else
		displacement = 0.f;

	return UKismetMathLibrary::FInterpTo(HipOffset, displacement, GetWorld()->GetDeltaSeconds(), InterpSpeed);
}
```

* HipOffset값을 애니메이션 블루프린트의 본 트랜스폼 변경을 이용하여 Z값을 조절한다.
* **결과적으로 HipOffset값만큼 루트 본이 캡슐 아래로 내려가게 된다.**
  
![hipoffset4](https://user-images.githubusercontent.com/48229283/126048827-4fb85ed0-e4e5-4d1c-b1e0-022f72d1f0fc.PNG) | ![hipoffset3](https://user-images.githubusercontent.com/48229283/126048723-d9df33bb-7c21-49ee-9628-57c9a0512edc.PNG)
:-------------------------:|:-------------------------:
  
### Foot displacement
  
* 위의 Hip displacement과 마찬가지로 지면과 트레이스 한다.
* 하나 추가된 것은 Normal벡터를 구해온다.
  
```cpp
void UCS_IKFootSystem::IKProcessing()
{
	//...
	
	// Foot displacement
	RFootDistance = IKFootTrace(RFootSocketName, RNormal);
	LFootDistance = IKFootTrace(LFootSocketName, LNormal);
	
	//...
}
```
  
* 애니메이션 블루프린트에서 레그IK를 사용한다.
  
![footdisplacement1](https://user-images.githubusercontent.com/48229283/126049061-c0e0b246-6131-47ee-bad5-8e6c760cbeb7.PNG)
  
* 왼쪽, 오른쪽 발 본을 세팅한다.
  
![footdisplacement2](https://user-images.githubusercontent.com/48229283/126049104-b1253661-690a-4cf7-9746-269e791dfb8f.PNG) | ![footdisplacement3](https://user-images.githubusercontent.com/48229283/126049105-e354ec38-1f02-4fa4-b0fe-389a38752ecc.PNG)
:-------------------------:|:-------------------------:
  
* Leg의 역운동 정도를 결정하는 Alpha값을 지정해준다.
  
```cpp
void UCS_IKFootSystem::IKProcessing()
{
	//...

	RFootAlpha = FootDisplacement(RFootDistance, RFootAlpha);
	LFootAlpha = FootDisplacement(LFootDistance, LFootAlpha);

	//...
}

float UCS_IKFootSystem::FootDisplacement(float distanceToGround, float footAlpha)
{
	float displacement; 
	// 나누어주는 AlphaDivValue는 여러 차례 테스트하며 적정한 값을 사용하였다.
	if (distanceToGround != 0.f)
		displacement = (TraceDistance - distanceToGround) / AlphaDivValue;
	else
		displacement = 0.f;
	
	return UKismetMathLibrary::FInterpTo(footAlpha, displacement, GetWorld()->GetDeltaSeconds(), InterpSpeed);
}
```
  
* 루트본은 내려가고 왼쪽 다리는 Alpha값만큼 올라간 모습
  
![footdisplacement4](https://user-images.githubusercontent.com/48229283/126049341-df69edfa-cb85-460d-9119-7f6a5caef147.PNG)
    
### Foot Rotation
  
* 발이 회전해야할 각도 구하기
  
![footrotation2](https://user-images.githubusercontent.com/48229283/126257868-79e64451-d0a8-402d-b7e0-e7b116a6b14e.PNG)
  
* 역탄젠트로 각도 구하기
  
![탄젠트](https://user-images.githubusercontent.com/48229283/126259295-ca927261-1c2c-4534-8229-0936283e62f9.PNG)
  
* 코드로 옮긴 모습
  
```cpp
FRotator UCS_IKFootSystem::FootRotate(FRotator FootRotation, FVector Normal)
{
	// Degree로 받기위하여 DegAtan2사용
	float X = UKismetMathLibrary::DegAtan2(Normal.Y, Normal.Z);
	float Y = UKismetMathLibrary::DegAtan2(Normal.X, Normal.Z);

	FRotator desiredRotation = UKismetMathLibrary::MakeRotator(X, -Y, 0.f);
	// 보간하여 반환
	return UKismetMathLibrary::RInterpTo(FootRotation, desiredRotation, GetWorld()->GetDeltaSeconds(), InterpSpeed);
}
```
  
![footrotation적용](https://user-images.githubusercontent.com/48229283/126260229-fbfb8894-43c7-4444-bcce-91ca1f4fecc4.PNG)
  
  
## 타겟팅 시스템
  
![targeting1](https://user-images.githubusercontent.com/48229283/126050856-b37c2e91-1399-453c-adeb-63dd7e67a084.gif)
  
### 1. 주변 적 액터 구하기
  
![11](https://user-images.githubusercontent.com/48229283/125843707-53b42daf-57ee-46c3-a5aa-78ee2066c37d.PNG)
  
* SweepMultiByChannel을 사용하여 스윕멀티
* DebugShape로 원을 그린다
* 적 액터 탐색하여 배열에 Add  
  
### 2. 장애물 감지하기
  
![22](https://user-images.githubusercontent.com/48229283/125843708-00d9c8de-7bf9-449c-a835-e1ba6d8767f1.PNG)
  
* LineTraceSingleByChannel을 통해 미리 탐색한 적을 향해 라인트레이스
* 벽과 충돌한다면 해당 적은 제외
  
### 3. 게임 화면안에 적 액터가 있는지 확인하기
  
![33](https://user-images.githubusercontent.com/48229283/125843710-49e95fbb-a6b2-4250-9a6a-36550ba3ed2e.PNG)
  
* 화면 해상도(Resolution)를 구하여 화면 안에 적이 있는지 확인 
* 화면 안에 존재하지 않는다면 해당 적은 제외
* DebugShape로 원을 그리면서 탐색하였기 때문에 플레이어의 시야에서 벗어난 적을 걸러내기 위한 작업
  
```cpp
bool UCS_TargetingSystem::IsEnemyInScreen(AActor* Enemy)
{
	// APlayerController 캐스팅
	const APlayerController* const PlayerController = Cast<const APlayerController>(Player->GetController());

	// 월드 좌표 -> 스크린 좌표 변환
	FVector2D ScreenLocation;
	PlayerController->ProjectWorldLocationToScreen(Enemy->GetActorLocation(), ScreenLocation);

	int32 ScreenX = ScreenLocation.X;
	int32 ScreenY = ScreenLocation.Y;

	// 타겟이 스크린좌표에서 스크린 크기와 높이 보다 큰 값을 가진다면, 화면 밖에 있는 것으로 판정한다.
	if ((0 < ScreenX && ScreenX < ScreenWidth) && (0 < ScreenY && ScreenY < ScreenHeight))
		return true;
	else
		return false;
}
```
  
  
### 4. 위 조건을 모두 통과한 적 액터 중 플레이어와 가장 가까운 적을 타겟으로 지정
  
```cpp
// 플레이어와 가장 가까운 적을 찾아냄
	ACS_Enemy* minDistanceEnemy = EnemyInSight[0];
	for (auto& enemy : EnemyInSight)
	{
		if (FVector::Dist(Player->GetActorLocation(), minDistanceEnemy->GetActorLocation()) >= FVector::Dist(Player->GetActorLocation(), enemy->GetActorLocation()))
		{
			minDistanceEnemy = enemy;
		}
	}

	// 가장 가까운 적 타겟으로 지정
	LockOnTarget(minDistanceEnemy);
```
  
  
## 연계 공격
  
![약공격1](https://user-images.githubusercontent.com/48229283/126050912-522f747c-483d-4caa-811c-f33871767d00.gif) | ![강공격1](https://user-images.githubusercontent.com/48229283/126050914-6a2a9675-34c2-4f5a-a30d-1827a71b52d6.gif) | ![혼합공격1](https://user-images.githubusercontent.com/48229283/126050916-eff4164e-5efc-4c56-a75c-6766e888514a.gif)
:-------------------------:|:-------------------------:|:-------------------------:
Light Attack | Heavy Attack | Combined
  
* 캐릭터의 약 공격과 강 공격을 서로 연계하여 사용
* 각 공격에 대한 Index 변수를 증가 시킴에 따라 공격 애니메이션을 결정
  
# 마무리
  
* 신경썼던 부분
Climb System, Foot IK 등 하나의 기능을 구현하여 다른 프로젝트에서도 사용할 수 있도록 확장성을 고려하며 개발하였습니다.
  
* 아쉬운 부분
하지만, 캐릭터와 몬스터간의 전투에서 큰 틀을 만들고 좀 더 디테일한 전투 설계를 원하다보니 조잡한 부분이 많이 보입니다.
  
긴글 봐주셔서 감사합니다.
