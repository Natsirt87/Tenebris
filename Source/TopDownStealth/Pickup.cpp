// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup.h"
#include "Components/SphereComponent.h"
#include "TopDownStealthCharacter.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionComp->InitSphereRadius(50.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");
	CollisionComp->SetupAttachment(RootComponent);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnOverlap);
	CollisionComp->OnComponentEndOverlap.AddDynamic(this, &APickup::EndOverlap);

}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickup::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATopDownStealthCharacter* Player = Cast<ATopDownStealthCharacter>(OtherActor);

	UE_LOG(LogTemp, Warning, TEXT("Large F"));

	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Large Cast"));
		Player->UpdatePickup(this);
	}
}

void APickup::EndOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	ATopDownStealthCharacter* Player = Cast<ATopDownStealthCharacter>(OtherActor);

	if (Player)
	{
		Player->PickupEnd();
	}
}
