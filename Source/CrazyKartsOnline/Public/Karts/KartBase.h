
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "KartBase.generated.h"

class UBoxComponent;
class USpringArmComponent;
class UCameraComponent;
class UKartMovementComponent;
class UKartReplicationComponent;

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
    
public:
	AKartBase();

    FORCEINLINE UKartMovementComponent* GetKartMovement() const { return MovementComponent; }
    
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UKartMovementComponent* MovementComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UKartReplicationComponent* ReplicationComponent;
    
    virtual void BeginPlay() override;
    
    /** Handle pressing forward **/
    void MoveForward(const float Amount);
    
    /** Handle pressing right **/
    void MoveRight(const float Amount);
    
public:    
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
};