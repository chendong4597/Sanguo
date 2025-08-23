// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Role.h"
#include "Weapon.generated.h"

UCLASS()
class SANGUO_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetMeshMaterialScalerParamer(FName name, float fValue);

public:
	//…Ë÷√ «∑Ò≈ˆ◊≤…À∫¶
	void SetWeaponId(int nId);
	void SetOwnerRole(ARole* aRole);
	void SetWeaponTpy(int nWeaponTpy) { m_nWeaponTpy = nWeaponTpy; }
	int  GetWeaponTpy() {return m_nWeaponTpy;}
	void RegisterOverlayEvents();

	void StartPreHurt();
	void EndHurt();

	UFUNCTION()
	virtual void OnOverlayBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnOverlayEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	int  m_nWeaponId{};
	int  m_nWeaponTpy{};
	ARole* m_ownerRole{};
	TArray<ARole*> m_arrayRoles;
	TArray<ARole*> m_hurtRoles;
	bool m_bHurting{};

};
