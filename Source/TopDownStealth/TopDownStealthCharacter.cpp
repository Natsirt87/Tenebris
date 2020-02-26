// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TopDownStealthCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "ArrowProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"
#include "Engine/DirectionalLight.h"
#include "Runtime/Engine/Classes/Components/PointLightComponent.h"
#include "Engine/World.h"
#include "Engine/Public/TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Pickup.h"

ATopDownStealthCharacter::ATopDownStealthCharacter()
{
	bIsInLight = false;

	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Initialize movement variables
	WalkSpeed = 250;
	RunSpeed = 600;
	AimingSpeed = 225;
	Health = 100.0f;
	MaxHealth = 100.0f;
	SprintSoundRadius = 700.0f;
	ArrowTypeNum = 1;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 1000.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	ArrowSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("ArrowSpawn"));
	ArrowSpawn->SetupAttachment(Cast<USceneComponent>(GetMesh()));

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATopDownStealthCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetLights();
}

//Runs every frame, fixes a bug with sprinting and runs the method to rotate the character to look at the mouse
void ATopDownStealthCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (GetVelocity().Size() <= 10.0f && bIsSprinting) 
	{
		StopSprinting();
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (!bIsSprinting)
		{
			FHitResult MouseHitResult;
			PC->GetHitResultUnderCursorForObjects(GroundPlane, true, MouseHitResult);

			if (!bIsDodging && !bGotHit) {
				RotateCharToMouse(MouseHitResult.Location);
			}

		}
	}
}

//Light related methods and stuff
void ATopDownStealthCharacter::UpdateInLight()
{
	bIsInLight = false;

	//Iterate through all lights in the scene
	for (int32 i = 0; i < PointLights.Num(); i++)
	{
		//Make sure it's a point light and it exists
		APointLight* light = Cast<APointLight>(PointLights[i]);
		if (light)
		{
			float distance = (light->GetActorLocation() - GetActorLocation()).Size();

			//If the character is within the attenuation of the light (the light reaches the character)
			if (distance < light->PointLightComponent->AttenuationRadius)
			{
				
				FCollisionQueryParams collisionParams;
				collisionParams.AddIgnoredActor(this);

				FHitResult hitResult;

				//Do a line trace from the light to the character to make sure they aren't behind anything, and if they aren't, then they're in the light
				if (!GetWorld()->LineTraceSingleByChannel(hitResult, GetActorLocation(), light->GetActorLocation(), ECC_Visibility, collisionParams))
				{
					bIsInLight = true;
				}

			}
		}
	}
}

//Getting all the lights in the scene (just point and spot because that's all we need)
void ATopDownStealthCharacter::GetLights()
{
	UGameplayStatics::GetAllActorsOfClass(this, APointLight::StaticClass(), PointLights);
	UGameplayStatics::GetAllActorsOfClass(this, ASpotLight::StaticClass(), SpotLights);
}

//Basic input initialization
void ATopDownStealthCharacter::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	check(InputComponent);
	

	// Movement axes
	InputComponent->BindAxis("MoveForward", this, &ATopDownStealthCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATopDownStealthCharacter::MoveRight);

	// Sprinting 
	InputComponent->BindAction("Sprint", IE_Pressed, this, &ATopDownStealthCharacter::StartSprinting);
	InputComponent->BindAction("Sprint", IE_Released, this, &ATopDownStealthCharacter::StopSprinting);

	// Drawing, canceling the draw, and firing the bow
	InputComponent->BindAction("Fire", IE_Pressed, this, &ATopDownStealthCharacter::DrawBow);
	InputComponent->BindAction("Fire", IE_Released, this, &ATopDownStealthCharacter::FireBow);
	InputComponent->BindAction("CancelFire", IE_Pressed, this, &ATopDownStealthCharacter::CancelDraw);
}

//To be called on tick, rotating the character to look at the mouse location
void ATopDownStealthCharacter::RotateCharToMouse(FVector MousePos)
{
	if (!bIsDead) 
	{
		FVector direction = GetActorLocation() - MousePos;

		direction = FVector(direction.X, direction.Y, 0);
		FRotator newRot = FRotationMatrix::MakeFromX(-direction).Rotator();
		GetController()->SetControlRotation(newRot);
	}
}

//Basic movement
void ATopDownStealthCharacter::MoveForward(float Val)
{
	if (Val != 0.0f && !bGotHit)
	{
		AddMovementInput(TopDownCameraComponent->GetUpVector(), Val, false);
	}
}

void ATopDownStealthCharacter::MoveRight(float Val)
{
	if (Val != 0.0f && !bGotHit)
	{
		AddMovementInput(TopDownCameraComponent->GetRightVector(), Val, false);
	}
}

//Sprinting, setting animation, speed, and other variables related to sprinting
void ATopDownStealthCharacter::StartSprinting()
{
	if (GetVelocity().Size() >= 10.0f && !bIsAiming && !bIsDodging)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 350.f, 0.f);
		bIsSprinting = true;
		CancelDraw();
	}
}

void ATopDownStealthCharacter::StopSprinting()
{
	if (bIsSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 700.f, 0.f);
		bIsSprinting = false;
	}
}

//Drawing and firing the bow, including animation variable changing
void ATopDownStealthCharacter::DrawBow()
{
	if (!bIsSprinting && !bIsDodging)
	{

		if (ArrowTypeNum == 1 && NormalArrowNum == 0) 
		{
			return;
		}
		else if (ArrowTypeNum == 2 && FireArrowNum == 0) 
		{
			return;
		}
		else if (ArrowTypeNum == 3 && DissolveArrowNum == 0)
		{
			return;
		}

		GetCharacterMovement()->MaxWalkSpeed = AimingSpeed;
		bIsDrawingBow = true;
		bIsAiming = true;
		bowAnimStart();
	}
}

void ATopDownStealthCharacter::FireBow()
{
	UE_LOG(LogTemp, Warning, TEXT("Your message"));

	if (!bIsSprinting && bIsAiming && bCanFire && !bIsDodging)
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		bIsFiringBow = true;
		bIsAiming = false;

		FVector spawnLocation = ArrowSpawn->GetComponentLocation();
		FRotator spawnRotation = GetController()->GetControlRotation();

		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		TSubclassOf<AArrowProjectile> arrowToFire;

		if (ArrowTypeNum == 1) {
			arrowToFire = ProjectileClass;
			NormalArrowNum--;
		}
		else if (ArrowTypeNum == 2) 
		{
			arrowToFire = FireProjectileClass;
			FireArrowNum--;
		}
		else if (ArrowTypeNum == 3)
		{
			arrowToFire = DissolveProjectileClass;
			DissolveArrowNum--;
		}

		AArrowProjectile* arrow = GetWorld()->SpawnActor<AArrowProjectile>(arrowToFire, spawnLocation, spawnRotation, spawnParams);
	}
	else
	{
		CancelDraw();
		CancelBow();
	}

}

void ATopDownStealthCharacter::CancelDraw()
{
	if (bIsAiming) {
		bIsAiming = false;
		bIsDrawingBow = false;
		bCanFire = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		CancelBow();
	}
}

void ATopDownStealthCharacter::Die()
{
	bIsDead = true;

	APlayerController* playerController = Cast<APlayerController>(GetController());
	DisableInput(playerController);

	FString levelString = GetWorld()->GetMapName();
	FName currentLevel = FName(*levelString);
	//UGameplayStatics::OpenLevel(GetWorld(), "TopDownExampleMap");
}
