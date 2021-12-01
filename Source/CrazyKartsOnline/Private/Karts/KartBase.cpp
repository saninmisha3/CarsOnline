

#include "Karts/KartBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"

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

void AKartBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Set up gameplay key bindings
    check(PlayerInputComponent)
    
    PlayerInputComponent->BindAxis("MoveForward", this, &AKartBase::Server_MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AKartBase::Server_MoveRight);
}

void AKartBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKartBase::Server_MoveForward_Implementation(const float Amount)
{
    Throttle = Amount;
}

bool AKartBase::Server_MoveForward_Validate(const float Amount)
{
    return Amount >= -1 && Amount <= 1;
}

void AKartBase::Server_MoveRight_Implementation(const float Amount)
{
    Steering = Amount;
}

bool AKartBase::Server_MoveRight_Validate(const float Amount)
{
    return Amount >= -1 && Amount <= 1;
}

void AKartBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    UpdateVelocity(DeltaTime);
}

FVector AKartBase::GetAirResistance() const
{
    return - Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector AKartBase::GetRollingResistance() const
{
    const auto CorrectGravity = -(GetWorld()->GetGravityZ() / 100); // Unreal Gravity — -980.0, Correct Gravity — 9.81
    const auto NormalForce = Mass * CorrectGravity;
    return - Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;
}

void AKartBase::ApplyRotation(const float DeltaTime)
{
    // Find the rotation angle
    const float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
    // MinSteeringRadius = 10.f
    const float RotationAngle = DeltaLocation / MinSteeringRadius * Steering;
    const FQuat RotationDelta(GetActorUpVector(), RotationAngle);

    Velocity = RotationDelta.RotateVector(Velocity);
    AddActorWorldRotation(RotationDelta);
}

void AKartBase::UpdateVelocity(const float DeltaTime)
{
    // Find the direction & speed for movement
    FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;
    
    // Add AirResistance in order to limit max speed 
    Force += GetAirResistance();

    // Add RollingResistance in order to stop vehicle if no force
    Force += GetRollingResistance();
    
    const FVector Acceleration = Force / Mass;


    Velocity = Velocity + Acceleration * DeltaTime;
    
    FHitResult HitResult;
    AddActorWorldOffset(Velocity,true, &HitResult);
    ApplyRotation(DeltaTime);
    
    if(HitResult.bBlockingHit)
    {
        Velocity = FVector::ZeroVector;
    }
}
