# 언리얼 엔진 포트폴리오
언리얼 엔진4를 이용한 액션RPG 게임 기능 구현


# 목차
* [소개 영상](https://youtu.be/b2idKuy5u80)
* [구현 내용](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#구현-내용)
  
  
# 구현 내용
* [연계 공격](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#연계-공격)
* [타겟팅 시스템](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#타겟팅-시스템)
* [몬스터 FSM](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#몬스터-FSM)
* [Climb(파쿠르)시스템](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#Climb-System)
* [Foot IK 시스템](https://github.com/Babdodook/UE4_SoulAction_Portfolio/blob/main/README.md#Foot-IK-System)

# 연계 공격
### Class Name : CS_Player  
  
Light Attack | Heavy Attack | Combined
:-------------------------:|:-------------------------:|:-------------------------:
![콤보공격_Trim](https://user-images.githubusercontent.com/48229283/125266409-c48e3c00-e340-11eb-86fe-b8af8cee4375.gif) | ![강공격](https://user-images.githubusercontent.com/48229283/125267953-35822380-e342-11eb-96bd-7a0495e9db73.gif) | ![혼합공격](https://user-images.githubusercontent.com/48229283/125268668-dffa4680-e342-11eb-9f98-543d33519a55.gif)


캐릭터의 약 공격과 강 공격을 서로 연계하여 사용할 수 있습니다.

# 타겟팅 시스템
### Class Name : CS_TargetingSystem  
  
**1. 주변 적 액터 구하기**  
<img src="https://user-images.githubusercontent.com/48229283/125272355-b5aa8800-e346-11eb-8061-6faba0a0d21d.PNG" width="300" height="300">  
SweepMultiByChannel 사용, DebugShape로 원을 그리며 적 액터 탐색하여 배열에 Add
  
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
  
---
  
**2. 장애물 감지하기**  
<img src="https://user-images.githubusercontent.com/48229283/125273296-bc85ca80-e347-11eb-9d40-71e2708999e6.PNG" width="300" height="300">  
LineTraceSingleByChannel을 통해 미리 탐색한 적을 향해 라인트레이스 합니다.  
벽과 충돌한다면 해당 적은 제외시킵니다.  
  
---
  
**3. 게임 화면안에 적 액터가 있는지 확인하기**  
<img src="https://user-images.githubusercontent.com/48229283/125273580-08387400-e348-11eb-8bc6-3a4a6720f15a.PNG" width="500" height="300">  
화면 해상도(Resolution)를 구하여 화면 안에 적이 있는지 확인합니다  
화면 안에 존재하지 않는다면 해당 적은 제외됩니다  
DebugShape로 원을 그리면서 탐색하였기 때문에 플레이어의 시야에서 벗어난 적을 걸러내기 위한 작업입니다  
  
---
  
**4. 위 조건을 모두 통과한 적 액터 중 플레이어와 가장 가까운 적을 타겟으로 지정**  
  
# 몬스터 FSM
  
# Climb System
  
# Foot IK System
  
