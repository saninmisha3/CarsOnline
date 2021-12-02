

#include "Karts/KartBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"


AKartBase::AKartBase()
{
 	PrimaryActorTick.bCanEverTick = true;
    
    Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
    Collision->InitBoxExtent(FVector(220.f, 85.f,55.f));
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Collision->SetCollisionProfileName(NAME_Pawn);
    RootComponent = Collision;
    
    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(GetRootComponent());
    
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->TargetOffset = FVector(0.f,0.f,200.f);
    SpringArm->SetRelativeRotation(FRotator(-15.f,0.f,0.f));
    SpringArm->TargetArmLength = 600.f;
    SpringArm->bEnableCameraRotationLag = true;
    SpringArm->CameraRotationLagSpeed = 7.f;
    SpringArm->bInheritPitch = false;
    SpringArm->bInheritRoll = false;
    SpringArm->SetupAttachment(GetRootComponent());

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;
    Camera->FieldOfView = 90.f;

    MovementComponent = CreateDefaultSubobject<UKartMovementComponent>(TEXT("MovementComponent"));
}

void AKartBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Set up gameplay key bindings
    check(PlayerInputComponent)
    
    PlayerInputComponent->BindAxis("MoveForward", this, &AKartBase::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AKartBase::MoveRight);
}

void AKartBase::BeginPlay()
{
	Super::BeginPlay();

    check(MovementComponent);
    
	if(GetLocalRole() == ROLE_Authority)
	{
	    NetUpdateFrequency = 1;
	}
}

void AKartBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AKartBase::MoveForward(const float Amount)
{
    GetKartMovement()->SetThrottle(Amount);
}

void AKartBase::MoveRight(const float Amount)
{
    GetKartMovement()->SetSteering(Amount);
}

void AKartBase::OnRep_ServerMoveState()
{
    SetActorTransform(ServerMoveState.Transform);
    GetKartMovement()->SetVelocity(ServerMoveState.Velocity);

    GetKartMovement()->ClearUnacknowledgedMove(ServerMoveState.LastMove);

    for(const auto& Move : GetKartMovement()->GetUnacknowledgedMoves())
    {
         GetKartMovement()->SimulatingMove(Move);
    }
}

void AKartBase::Server_SendMove_Implementation(const FMoveData& MoveData)
{
    GetKartMovement()->SimulatingMove(MoveData);

    ServerMoveState.Transform = GetActorTransform();
    ServerMoveState.Velocity = GetKartMovement()->GetVelocity();
    ServerMoveState.LastMove = MoveData;
}

bool AKartBase::Server_SendMove_Validate(const FMoveData& MoveData)
{
    return true;
}

void AKartBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AKartBase, ServerMoveState);
}
