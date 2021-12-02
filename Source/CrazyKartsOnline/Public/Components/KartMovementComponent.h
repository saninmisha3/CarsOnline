
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KartMovementComponent.generated.h"

USTRUCT()
struct FMoveData
{
    GENERATED_BODY()

    UPROPERTY()
    float Throttle;

    UPROPERTY()
    float Steering;

    UPROPERTY()
    float DeltaTime;

    UPROPERTY()
    float Time;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRAZYKARTSONLINE_API UKartMovementComponent : public UActorComponent
{
    GENERATED_BODY()

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

    TArray<FMoveData> UnacknowledgedMoves;
    
    float Throttle;
    float Steering;
    
    FVector Velocity;

    FMoveData GetMoveData(float DeltaTime) const;

public:	
	UKartMovementComponent();
    
    void SimulatingMove(const FMoveData MoveData);
    void ClearUnacknowledgedMove(const FMoveData LastMove);
    
    FORCEINLINE FVector GetVelocity() const { return Velocity; }
    FORCEINLINE void SetVelocity(const FVector NewVelocity) { Velocity = NewVelocity; }
    
    FORCEINLINE void SetThrottle(const float Amount) { Throttle = Amount; }
    FORCEINLINE void SetSteering(const float Amount) { Steering = Amount; }

    FORCEINLINE TArray<FMoveData>& GetUnacknowledgedMoves() { return UnacknowledgedMoves; }

protected:
	virtual void BeginPlay() override;

    /** Calculate Force & Acceleration **/
    void UploadMovement(const float DeltaTime);

    /** Calculate Steering angle **/
    void ApplyRotation(const float DeltaTime, const float SteeringThrow);

    /** Calculate Air Resistance **/
    FVector GetAirResistance() const;

    /** Calculate Rolling Resistance **/
    FVector GetRollingResistance() const;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
