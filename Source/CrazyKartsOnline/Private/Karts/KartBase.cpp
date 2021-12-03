
#include "Karts/KartBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BoxComponent.h"
#include "Components/KartMovementComponent.h"
#include "Components/KartReplicationComponent.h"

AKartBase::AKartBase()
{
 	PrimaryActorTick.bCanEverTick = false;    
    
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
    ReplicationComponent = CreateDefaultSubobject<UKartReplicationComponent>(TEXT("ReplicationComponent"));
}

void AKartBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    SetReplicates(true);
    SetReplicateMovement(false);
    
    // Set up gameplay key bindings
    check(PlayerInputComponent)
    
    PlayerInputComponent->BindAxis("MoveForward", this, &AKartBase::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AKartBase::MoveRight);
}

void AKartBase::BeginPlay()
{
	Super::BeginPlay();

    check(MovementComponent);
    check(ReplicationComponent);
}

void AKartBase::MoveForward(const float Amount)
{
    GetKartMovement()->SetThrottle(Amount);
}

void AKartBase::MoveRight(const float Amount)
{
    GetKartMovement()->SetSteering(Amount);
}