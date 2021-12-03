
#include "Components/KartMovementComponent.h"
#include "GameFramework/GameStateBase.h"

UKartMovementComponent::UKartMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

    SetIsReplicated(true);
    
    // Vehicle Params
    MaxDrivingForce = 10000.f;
    Mass = 1000.f;
    Velocity = FVector::ZeroVector;
    MaxDegreesPerSecond = 90.f;
    MinSteeringRadius = 10.f;

    // Gravity Params
    DragCoefficient = 16.f;
    RollingResistanceCoefficient = 0.015f;
}

FVector UKartMovementComponent::GetAirResistance() const
{
    return - Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector UKartMovementComponent::GetRollingResistance() const
{
    const auto CorrectGravity = -(GetWorld()->GetGravityZ() / 100); // Unreal Gravity — -980.0, Correct Gravity — 9.81
    const auto NormalForce = Mass * CorrectGravity;
    return - Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;
}

void UKartMovementComponent::TickComponent(float DeltaTime, ELevelTick Tick, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, Tick, ThisTickFunction);

    if(GetOwnerRole() == ROLE_AutonomousProxy || GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
    {
        LastMove = GetMoveData(DeltaTime);
        SimulatingMove(LastMove);
    }
}

void UKartMovementComponent::ApplyRotation(const float DeltaTime, const float SteeringThrow)
{
    // Find the rotation angle
    const float DeltaLocation = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * DeltaTime;
    // MinSteeringRadius = 10.f
    const float RotationAngle = DeltaLocation / MinSteeringRadius * SteeringThrow;
    const FQuat RotationDelta(GetOwner()->GetActorUpVector(), RotationAngle);

    Velocity = RotationDelta.RotateVector(Velocity);
    GetOwner()->AddActorWorldRotation(RotationDelta);
}

void UKartMovementComponent::SimulatingMove(const FMoveData& MoveData)
{
    // Find the direction & speed for movement
    FVector Force = GetOwner()->GetActorForwardVector() * MaxDrivingForce * MoveData.Throttle;
    
    // Add AirResistance in order to limit max speed 
    Force += GetAirResistance();

    // Add RollingResistance in order to stop vehicle if no force
    Force += GetRollingResistance();
    
    const FVector Acceleration = Force / Mass;
    
    Velocity = Velocity + Acceleration * MoveData.DeltaTime;
    
    FHitResult HitResult;
    GetOwner()->AddActorWorldOffset(Velocity,true, &HitResult);
    ApplyRotation(MoveData.DeltaTime, MoveData.Steering);
    
    if(HitResult.bBlockingHit)
    {
        Velocity = FVector::ZeroVector;
    }
}

void UKartMovementComponent::ClearUnacknowledgedMove(const FMoveData& CurMove)
{
    TArray<FMoveData> NewMoves;
    for(const auto& Move: UnacknowledgedMoves)
    {
        if(Move.Time > CurMove.Time)
        {
            NewMoves.Add(Move);
        }
    }
    UnacknowledgedMoves = NewMoves;
}

FMoveData UKartMovementComponent::GetMoveData(const float DeltaTime) const
{
    FMoveData MoveData;
    MoveData.Throttle = Throttle;
    MoveData.Steering = Steering;
    MoveData.DeltaTime = DeltaTime;
    MoveData.Time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
    return MoveData;
}


