// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "common/Timer.h"
#include "WeaponEffect.generated.h"

UENUM(BlueprintType)
enum class WEAPONEFFTPY :uint8 //…Ë÷√uint8¿‡–Õ
{
	WEAPONEFFTPY_F UMETA(DisplayName = "params float"),
	WEAPONEFFTPY_V UMETA(DisplayName = "params vec"),
};

USTRUCT(BlueprintType)
struct FStuWeaponEffInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AWeaponEffect | FStuWeaponEffInfo")
	FString strParam = FString("");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AWeaponEffect | FStuWeaponEffInfo")
	WEAPONEFFTPY emTpy = WEAPONEFFTPY::WEAPONEFFTPY_F;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AWeaponEffect | FStuWeaponEffInfo")
	float fValue = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AWeaponEffect | FStuWeaponEffInfo")
	FVector vecParams = FVector(0.f , 0.f , 0.f);
};

UCLASS()
class SANGUO_API AWeaponEffect : public AActor
{
	GENERATED_BODY()
	//DECLARE_TIMER_HOST();
public:	
	// Sets default values for this actor's properties
	AWeaponEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "WEAPON EFFECT")
	void AddFloatEffParams(FString strName , float fValue);

	UFUNCTION(BlueprintCallable, Category = "WEAPON EFFECT")
	void AddVectorEffParams(FString strName, FVector vecParams);

	UFUNCTION(BlueprintCallable, Category = "WEAPON EFFECT")
	void ClearVectorEffParams();

	UFUNCTION(BlueprintCallable, Category = "WEAPON EFFECT")
	const TArray<FStuWeaponEffInfo>& GetEffParams() {return m_arrayEffParms;}

	void DieDelay(float tm);
private:
	TArray<FStuWeaponEffInfo> m_arrayEffParms;

	int m_idTimerDie{ 0 };
	float m_fTmDie;
	float m_fCurTm;
};
