// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TopDownStealthCharacter.generated.h"

UCLASS(Blueprintable)
class ATopDownStealthCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATopDownStealthCharacter();

	//Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	
	//Light related methods
	UFUNCTION(BlueprintCallable, Category = "Stealth") //This will be called every 0.1 seconds in blueprint because I don't want to use a timer
	void UpdateInLight();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Animation")
	void CancelBow();

	UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
	void bowAnimStart();

	UFUNCTION()
	void GetLights();

	//Movement related methods
	UFUNCTION()
	void MoveForward(float Val);

	UFUNCTION()
	void MoveRight(float Val);

	UFUNCTION()
	void RotateCharToMouse(FVector MousePos);

	UFUNCTION()
	void StartSprinting();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopSprinting();

	//Weapon related methods
	UFUNCTION()
	void DrawBow();

	UFUNCTION()
	void FireBow();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CancelDraw();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickups")
	void UpdatePickup(class APickup* Pickup);

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickups")
	void PickupEnd();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Die();

	//Look at those booleans
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
	bool bIsSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weaponry)
	bool bIsAiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weaponry)
	bool bIsDrawingBow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weaponry)
	bool bIsFiringBow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	bool bIsDodging;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weaponry)
	bool bCanFire;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Visibility)
	bool bIsInLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Visibility)
	bool bIsDead;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Hit)
	bool bGotHit;
	

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent);

	virtual void BeginPlay() override;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	UPROPERTY(EditDefaultsOnly, Category = Weaponry, meta = (AllowPrivateAccess = "true"))
	USceneComponent* ArrowSpawn;

	//Movement num variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	int32 WalkSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	int32 RunSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	int32 AimingSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 NormalArrowNum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 FireArrowNum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 DissolveArrowNum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 TeamSwitchArrowNum;

	UPROPERTY(BlueprintReadWrite, Category = Health, meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(BlueprintReadWrite, Category = Health, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float SprintSoundRadius;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 ArrowTypeNum;

	UPROPERTY(EditDefaultsOnly, Category = Weaponry, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AArrowProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = Weaponry, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AArrowProjectile> FireProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = Weaponry, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AArrowProjectile> DissolveProjectileClass;

	UPROPERTY(BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimSequence* DeathAnim;

	UPROPERTY()
	TArray<AActor*> PointLights;

	UPROPERTY()
	TArray<AActor*> SpotLights;

	UPROPERTY()
	TArray<AActor*> DirectionalLights;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Collision, meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> GroundPlane;
};

