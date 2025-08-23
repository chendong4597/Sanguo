// Fill out your copyright notice in the Description page of Project Settings.


#include "NavActor.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Kismet/GameplayStatics.h"
#include "../ReichGameInstance.h"

// Sets default values
ANavActor::ANavActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANavActor::BeginPlay()
{
	Super::BeginPlay();
	
	{
		FString projectDir = FPaths::ProjectDir();
		projectDir += "Content/config/nav_maincity.txt";
		TArray<FString> StringArray;
		FFileHelper::LoadANSITextFileToStrings(*(projectDir), NULL, StringArray);
		for (int i = 0; i < StringArray.Num(); i++)
		{
			FString str = StringArray[i];
			FTransform trans;
			trans.InitFromString(str);
			NavAddMesh(trans);
		}
	}
	
	
	{
		FString projectDir = FPaths::ProjectDir();
		projectDir += "Content/config/nav_maincity.txt";
		TArray<FString> StringArray;

		std::string strPath = "/Game/effect/world/NavWall/BP_NavWall.BP_NavWall";
		FString path = UReichGameInstance::TransBPAssertPath(strPath);
		UObject* loadObj = StaticLoadObject(UBlueprint::StaticClass(), NULL, path.GetCharArray().GetData());//TEXT("Blueprint'/Game/BluePrint/TestObj.TestObj'")
		if (loadObj == nullptr)
		{
			return;
		}
		UBlueprint* ubp = Cast<UBlueprint>(loadObj);

		if (ubp) {
			TArray<AActor*> arrActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), TSubclassOf<AActor>(ubp->GeneratedClass), arrActors);

			for (int i = 0; i < arrActors.Num(); i++)
			{
				const FTransform& trans = arrActors[i]->GetTransform();
				//UE_LOG(LogClass, Log, TEXT("trans =  %s"), *(trans.ToString()));
				StringArray.Add(trans.ToString());
				arrActors[i]->Destroy();
			}

			//Ð´ÈëÍø¸ñ
			if (false)
				FFileHelper::SaveStringArrayToFile(StringArray, *(projectDir), FFileHelper::EEncodingOptions::ForceAnsi);
		}
	}
	
}

// Called every frame
void ANavActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

