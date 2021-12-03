

#include "Components/KartReplicationComponent.h"
#include "KartBase.h"
#include "Net/UnrealNetwork.h"

UKartReplicationComponent::UKartReplicationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicated(true);
}

void UKartReplicationComponent::BeginPlay()
{
	Super::BeginPlay();

    OwnerMovementComponent = GetOwner<AKartBase>()->GetKartMovement();
    check(OwnerMovementComponent);
    
    if(GetOwnerRole() == ROLE_Authority)
    {
        GetOwner()->NetUpdateFrequency = 1;
    }
}

void UKartReplicationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    UploadMovement(DeltaTime);
}

void UKartReplicationComponent::UploadMovement(const float DeltaTime)
{
    const auto LastMove = OwnerMovementComponent->GetLastMove();
    
    if(GetOwnerRole() == ROLE_AutonomousProxy)
    {
        OwnerMovementComponent->UnacknowledgedMoves.Add(LastMove);
        Server_SendMove(LastMove);
    }

    if(GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
    {
        UpdateServerState(LastMove);
    }

    if(GetOwnerRole() == ROLE_SimulatedProxy)
    {
        ClientTick(DeltaTime);
    }
}

void UKartReplicationComponent::UpdateServerState(const FMoveData& Move)
{
    ServerMoveState.Transform = GetOwner()->GetActorTransform();
    ServerMoveState.Velocity = OwnerMovementComponent->GetVelocity();
    ServerMoveState.LastMove = Move;
}

void UKartReplicationComponent::ClientTick(const float DeltaTime)
{
    ClientTimeSinceUpdate += DeltaTime;

    if(ClientTimeSinceUpdate < KINDA_SMALL_NUMBER) return;

    const auto TargetLocation = ServerMoveState.Transform.GetLocation();
    const auto StartLocation = ClientStartTransform.GetLocation();
    const auto LerpRatio = ClientTimeSinceUpdate / ClientTimeBetweenUpdates;
    const auto VelocityToDerivative = ClientTimeBetweenUpdates * 100.f;
    const auto StartDerivative = ClientStartVelocity * VelocityToDerivative;
    const auto TargetDerivative = ServerMoveState.Velocity * VelocityToDerivative;
    
    const auto NewLocation = FMath::CubicInterp(StartLocation, StartDerivative, TargetLocation, TargetDerivative, LerpRatio);
    GetOwner()->SetActorLocation(NewLocation);

    const auto NewDerivative = FMath::CubicInterpDerivative(StartLocation, StartDerivative, TargetLocation, TargetDerivative, LerpRatio);
    const auto NewVelocity = NewDerivative / VelocityToDerivative;
    OwnerMovementComponent->SetVelocity(NewVelocity);

    const FQuat TargetRotation = ServerMoveState.Transform.GetRotation();
    const FQuat StartRotation = ClientStartTransform.GetRotation();

    const auto NewRotation = FQuat::Slerp(StartRotation, TargetRotation, LerpRatio);
    GetOwner()->SetActorRotation(NewRotation);
}


void UKartReplicationComponent::OnRep_ServerMoveState()
{
    switch(GetOwnerRole()) 
    {
        case ROLE_SimulatedProxy:
            SimulatedProxy_OnRep_ServerMoveState();
            break;
        case ROLE_AutonomousProxy:
            AutonomousProxy_OnRep_ServerMoveState();
            break;
        default: ;
    }
}

void UKartReplicationComponent::AutonomousProxy_OnRep_ServerMoveState()
{
    GetOwner()->SetActorTransform(ServerMoveState.Transform);
    OwnerMovementComponent->SetVelocity(ServerMoveState.Velocity);

    OwnerMovementComponent->ClearUnacknowledgedMove(ServerMoveState.LastMove);

    for(const auto& Move : OwnerMovementComponent->UnacknowledgedMoves)
    {
        OwnerMovementComponent->SimulatingMove(Move);
    }
}

void UKartReplicationComponent::SimulatedProxy_OnRep_ServerMoveState()
{
    ClientTimeBetweenUpdates = ClientTimeSinceUpdate;
    ClientTimeSinceUpdate = 0;

    ClientStartTransform = GetOwner()->GetActorTransform();
    ClientStartVelocity = GetOwner()->GetVelocity();
}

void UKartReplicationComponent::Server_SendMove_Implementation(const FMoveData& MoveData)
{
    ClientSimulatedTime += MoveData.DeltaTime;
    OwnerMovementComponent->SimulatingMove(MoveData);
    UpdateServerState(MoveData);
}

bool UKartReplicationComponent::Server_SendMove_Validate(const FMoveData& MoveData)
{
    if(!MoveData.IsValid()) return false;

    const auto ProposedTime = ClientSimulatedTime + MoveData.DeltaTime;
    if(ProposedTime > GetWorld()->GetTimeSeconds()) return false;
    
    return true;
}

void UKartReplicationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UKartReplicationComponent, ServerMoveState);
}
