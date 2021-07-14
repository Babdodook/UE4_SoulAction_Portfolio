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
  
<img src="https://user-images.githubusercontent.com/48229283/125272355-b5aa8800-e346-11eb-8061-6faba0a0d21d.PNG" width="300" height="300">  
  
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
  
<img src="https://user-images.githubusercontent.com/48229283/125273296-bc85ca80-e347-11eb-9d40-71e2708999e6.PNG" width="300" height="300">  
  
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
  
<img src="https://user-images.githubusercontent.com/48229283/125273580-08387400-e348-11eb-8bc6-3a4a6720f15a.PNG" width="500" height="300">  
  
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
  
#### Class Name : CS_Enemy


  
## Climb System
  
## Foot IK System
  
