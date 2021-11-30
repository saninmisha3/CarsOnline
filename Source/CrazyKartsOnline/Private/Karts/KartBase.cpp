

#include "Karts/KartBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"

AKartBase::AKartBase()
{
 	PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

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

    MaxDrivingForce = 10000.f;
    Mass = 1000.f;
    Velocity = FVector::ZeroVector;
}

void AKartBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Set up gameplay key bindings
    check(PlayerInputComponent)
    
    PlayerInputComponent->BindAxis("MoveForward", this, &AKartBase::MoveForward);
}

void AKartBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKartBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    FindVelocity(DeltaTime);
}

void AKartBase::MoveForward(const float Amount)
{
    Throttle = Amount;
}

void AKartBase::FindVelocity(const float DeltaTime)
{
    const FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;
    const FVector Acceleration = Force / Mass;
    Velocity = Velocity + Acceleration * DeltaTime;
    
    AddActorWorldOffset(Velocity);
}
