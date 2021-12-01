
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "KartBase.generated.h"


class UBoxComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class CRAZYKARTSONLINE_API AKartBase : public APawn
{
	GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category="Vehicle")
    UBoxComponent* Collision;
    
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

    /** The maximal steering rotation angle per second **/
    UPROPERTY(EditDefaultsOnly, Category="Vehicle")
    float MaxDegreesPerSecond;

    /** Drag Coefficient for calculating Air Resistance **/
    UPROPERTY(EditDefaultsOnly, Category="Gravity")
    float DragCoefficient;

    /** Rolling Resistance Coefficient for ordinary car tires **/
    UPROPERTY(EditDefaultsOnly, Category="Gravity")
    float RollingResistanceCoefficient;

    UPROPERTY(EditDefaultsOnly, Category="Vehicle")
    float MinSteeringRadius;
    
    float Throttle;
    float Steering;
    
    FVector Velocity;
    
public:
	AKartBase();

protected:
	virtual void BeginPlay() override;

    /** Handle pressing forward **/
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_MoveForward(const float Amount);

    /** Handle pressing right **/
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_MoveRight(const float Amount);

    /** Calculate Force & Acceleration **/
    void UpdateVelocity(const float DeltaTime);

    /** Calculate Steering angle **/
    void ApplyRotation(const float DeltaTime);

    /** Calculate Air Resistance **/
    FVector GetAirResistance() const;

    /** Calculate Rolling Resistance **/
    FVector GetRollingResistance() const;
    
public:    
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
};