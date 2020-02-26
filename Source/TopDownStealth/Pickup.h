// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class TOPDOWNSTEALTH_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

	UFUNCTION()
	void OnOverlap (UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health)
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health)
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
	float SprintSoundRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
	int32 Speed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
	int32 SprintSpeed;	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
	int32 NormalArrowNum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
	int32 FireArrowNum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
	int32 DissolveArrowNum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
	int32 TeamSwitchArrowNum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
	FString Name;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollisionComp;

};
