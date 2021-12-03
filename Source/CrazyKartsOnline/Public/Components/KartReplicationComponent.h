
#pragma once

#include "CoreMinimal.h"
#include "KartMovementComponent.h"
#include "Components/ActorComponent.h"
#include "KartReplicationComponent.generated.h"

class UKartMovementComponent;

USTRUCT()
struct FMoveState
{
    GENERATED_BODY()

    UPROPERTY()
    FMoveData LastMove;

    UPROPERTY()
    FTransform Transform;
    
    UPROPERTY()
    FVector Velocity;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRAZYKARTSONLINE_API UKartReplicationComponent : public UActorComponent
{
	GENERATED_BODY()
    
    UPROPERTY(ReplicatedUsing="OnRep_ServerMoveState")
    FMoveState ServerMoveState;

    UFUNCTION()
    void OnRep_ServerMoveState();
    void AutonomousProxy_OnRep_ServerMoveState();
    void SimulatedProxy_OnRep_ServerMoveState();
    
public:	
	UKartReplicationComponent();

protected:
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SendMove(const FMoveData& MoveData);

    UPROPERTY()
    UKartMovementComponent* OwnerMovementComponent;

    void UploadMovement(const float DeltaTime);
    void UpdateServerState(const FMoveData& Move);
    void ClientTick(const float DeltaTime);
    
	virtual void BeginPlay() override;

    float ClientTimeSinceUpdate;
    float ClientTimeBetweenUpdates;
    float ClientSimulatedTime;
    FTransform ClientStartTransform;
    FVector ClientStartVelocity;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
		
};
