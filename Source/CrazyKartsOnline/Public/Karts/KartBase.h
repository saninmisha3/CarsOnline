
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "KartBase.generated.h"

class UChaosWheeledVehicleMovementComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class CRAZYKARTSONLINE_API AKartBase : public APawn
{
	GENERATED_BODY()

    /** The main skeletal mesh for Vehicle **/
    UPROPERTY(VisibleAnywhere, Category="Vehicle")
    USkeletalMeshComponent* Mesh;
    
    /** Spring arm that will offset the camera **/
    UPROPERTY(VisibleAnywhere, Category="Camera")
    USpringArmComponent* SpringArm;

    /** Camera component that will be our viewpoint **/
    UPROPERTY(VisibleAnywhere, Category="Camera")
    UCameraComponent* Camera;

    /** The mass of vehicle. Default = 100 kg. **/
    UPROPERTY(EditDefaultsOnly, Category="Vehicle")
    float Mass;

    /** Set up max force for vehicle. Default = 10000 N. **/
    UPROPERTY(EditDefaultsOnly, Category="Vehicle")
    float MaxDrivingForce;

    float Throttle;
    FVector Velocity;
    
public:
	AKartBase();

protected:
	virtual void BeginPlay() override;

    /** Handle pressing forward **/
    void MoveForward(const float Amount);

    /** Calculate Force & Acceleration of the Vehicle **/
    void FindVelocity(const float DeltaTime);

public:    
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
};
