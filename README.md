# 언리얼 엔진 포트폴리오
언리얼 엔진4를 이용한 액션RPG 게임 기능 구현


# 목차
* [소개 영상](https://youtu.be/b2idKuy5u80)
* [구현 내용](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#구현-내용)
  
  
# 구현 내용
* [타겟팅 시스템](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#타겟팅-시스템)
* [연계 공격](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#연계-공격)
* [몬스터 FSM](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#몬스터-FSM)
* [Climb(파쿠르)시스템](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#Climb-System)
* [Foot IK 시스템](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#Foot-IK-System)

## 타겟팅 시스템
#### Class Name : CS_TargetingSystem  
  
  
### 1. 주변 적 액터 구하기
  
![11](https://user-images.githubusercontent.com/48229283/125843707-53b42daf-57ee-46c3-a5aa-78ee2066c37d.PNG)
  
* SweepMultiByChannel을 사용하여 스윕멀티
* DebugShape로 원을 그린다
* 적 액터 탐색하여 배열에 Add
  
```cpp
void UCS_TargetingSystem::DetectEnemyObjects()
{
	// 모든 배열 초기화
	DetectedEnemy.Empty();
	EnemyInScreen.Empty();
	EnemyInSight.Empty();

	TArray<FHitResult> OutHits;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	// 충돌 확인용 그리기
	FCollisionShape DebugShape = FCollisionShape::MakeSphere(600.f);
	if(bIsDrawDebug)
		DrawDebugSphere(GetWorld(), Player->GetActorLocation(), DebugShape.GetSphereRadius(), 10, FColor::Blue, false, 1.5f);

	// 스윕 멀티 트레이스
	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, Player->GetActorLocation(), Player->GetActorLocation(), FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel2, DebugShape, TraceParams);

	if (isHit)
	{
		for (auto& Hit : OutHits)
		{
			ACS_Enemy* enemy = Cast<ACS_Enemy>(Hit.GetActor());
			if (enemy && enemy->IsAlive())
			{
				// 중복된 것 Add하지 않음
				DetectedEnemy.AddUnique(enemy);
			}
		}
	}
}
```
  
  
### 2. 장애물 감지하기
  
![22](https://user-images.githubusercontent.com/48229283/125843708-00d9c8de-7bf9-449c-a835-e1ba6d8767f1.PNG)
  
* LineTraceSingleByChannel을 통해 미리 탐색한 적을 향해 라인트레이스
* 벽과 충돌한다면 해당 적은 제외
  
```cpp
bool UCS_TargetingSystem::IsWallExist(AActor* Enemy)
{
	FHitResult HitResult;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	// 트레이스 시작, 끝 포지션
	// 플레이어 위치 -> 적 위치
	FVector StartPoint = Player->GetActorLocation();
	FVector EndPoint = Enemy->GetActorLocation();

	float distance = FVector::Dist(StartPoint, EndPoint);

	// 플레이어 -> 적 라인 트레이스
	bool isHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECollisionChannel::ECC_GameTraceChannel3, TraceParams);
	if (bIsDrawDebug)
		DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Green, false, 3.f);

	if (isHit)
	{
		ACS_Enemy* HitEnemy = Cast<ACS_Enemy>(HitResult.GetActor());
		if (HitEnemy != nullptr && HitEnemy == Enemy)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Hit Success : %s"), *HitEnemy->GetName());
			return true;
		}
	}

	return false;
}
```
  
  
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
#### Class Name : CS_Player  
  
Light Attack | Heavy Attack | Combined
:-------------------------:|:-------------------------:|:-------------------------:
![콤보공격_Trim](https://user-images.githubusercontent.com/48229283/125266409-c48e3c00-e340-11eb-86fe-b8af8cee4375.gif) | ![강공격](https://user-images.githubusercontent.com/48229283/125267953-35822380-e342-11eb-96bd-7a0495e9db73.gif) | ![혼합공격](https://user-images.githubusercontent.com/48229283/125268668-dffa4680-e342-11eb-9f98-543d33519a55.gif)


* 캐릭터의 약 공격과 강 공격을 서로 연계하여 사용
* 각 공격에 대한 Index 변수를 증가 시킴에 따라 공격 애니메이션을 결정


## 몬스터 FSM
### 유한상태 기계 플로우

<img src="https://user-images.githubusercontent.com/48229283/125384154-6f4f3a80-e3d3-11eb-92ff-ba9dbbec2e97.PNG" width="510" height="300">  
  
* 공격패턴마다 해당하는 사거리(Range)와 시야각(Angle)을 가진다.
* 공격 딜레이 중 일때는 랜덤한 방향으로 이동
* 공격 딜레이가 끝날 시에는 타겟을 추적하여 이동
* 타겟 추적 중 사거리와 시야각을 만족하면 공격
  
  
### 클래스 상속
<img src="https://user-images.githubusercontent.com/48229283/125539631-d493bacf-c541-4bdf-95e8-2b9cffc388f2.PNG" width="400" height="250">  
  
* Enemy 베이스 클래스를 상속하여 override하여 사용
  
  
### 상세 기능


### 1. 사거리 및 시야각
  
* 사거리와 시야각 만족시 공격
  
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
  
<img src="https://user-images.githubusercontent.com/48229283/125547501-2755cb13-1941-401e-8130-49fd639e672c.PNG" width="430" height="220">  
  
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
잔상1 | 잔상2
  
  
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
  
### HipOffset
