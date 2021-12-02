
#pragma once

#include "CoreMinimal.h"
#include "Components/KartMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "KartBase.generated.h"

class UBoxComponent;
class USpringArmComponent;
class UCameraComponent;

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
    
    UPROPERTY(ReplicatedUsing="OnRep_ServerMoveState")
    FMoveState ServerMoveState;

    UFUNCTION()
    void OnRep_ServerMoveState();
    
public:
	AKartBase();

    FORCEINLINE UKartMovementComponent* GetKartMovement() const { return MovementComponent; }
    
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
    UKartMovementComponent* MovementComponent;
    
    virtual void BeginPlay() override;
    /** Handle pressing forward **/
    void MoveForward(const float Amount);
    
    /** Handle pressing right **/
    void MoveRight(const float Amount);
    
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SendMove(const FMoveData& MoveData);
    
public:    
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
};