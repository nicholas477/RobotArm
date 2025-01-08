// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "RobotUtilsTypes.h"
#include "DynamicMeshBuilder.h"
#include "RobotUtilsFunctionLibrary.h"
#include "RobotJointComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRobotJointRotate, URobotJointComponent*, Joint, float, DeltaSeconds, float, RotationDelta);

UCLASS( ClassGroup=(Robot), meta=(BlueprintSpawnableComponent) )
class ROBOTUTILS_API URobotJointComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URobotJointComponent(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeComponent() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Joint Component")
	FRobotJoint Joint;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Joint Component|Draw")
	bool DrawJoint;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Joint Component|Draw")
	float DrawSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Joint Component|Draw")
	float JointRotationOffset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Joint Component|Draw", meta = (Units = "Degrees"))
	FTransform DrawOffset;

	UPROPERTY(Transient)
	FTransform StartingRelativeTransform;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Joint Component", meta = (Units = "Degrees"))
	float RotationInterpSpeed;

	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere, Category = "Joint Component", meta = (Units = "Degrees"))
	float Rotation;

	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere, Category = "Joint Component", meta = (Units = "Degrees"))
	float TargetRotation;

	// How close Rotation is to TargetRotation when this joint decides to stop rotating
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Joint Component", meta = (Units = "Degrees"))
	float RotationStopThreshold;

	UFUNCTION(BlueprintCallable, Category = "Joint Component")
	bool MakeChain(USceneComponent* ChainTip, FRobotChain& OutChain, TArray<USceneComponent*>& OutJoints) const;

	UFUNCTION(BlueprintCallable, Category = "Joint Component")
	bool GetJointRotations(USceneComponent* ChainTip, FRobotJointArray& Rotations) const;

	UFUNCTION(BlueprintCallable, Category = "Joint Component")
	bool ApplyJointRotations(USceneComponent* ChainTip, const FRobotJointArray& Rotations);

	UFUNCTION(BlueprintCallable, Category = "Joint Component")
	bool SolveIK(const FSolveIKOptions& Options, const FRobotChain& Chain, TArray<USceneComponent*> Joints, const FTransform& DesiredEffectorTransform, FSolveIKResult& Result, bool bApplyIK);

	UFUNCTION(BlueprintCallable, Category = "Joint Component")
	virtual void SetTargetRotation(float InTargetRotation);

	// Returns true if Rotation is within TargetRotation by +/-RotationStopThreshold degrees
	UFUNCTION(BlueprintPure, Category = "Joint Component")
	bool IsWithinRotationThreshold() const;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable, Category = "Joint Component")
	FOnRobotJointRotate OnRobotJointRotate;

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ End UPrimitiveComponent Interface.
};

class ROBOTUTILS_API FRobotJointSceneProxy : public FPrimitiveSceneProxy
{
public:

	SIZE_T GetTypeHash() const override;
	FRobotJointSceneProxy(const URobotJointComponent* InComponent);
	virtual ~FRobotJointSceneProxy();

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;

	/**
	*  Returns a struct that describes to the renderer when to draw this proxy.
	*	@param		Scene view to use to determine our relevence.
	*  @return		View relevance struct
	*/
	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;
	virtual uint32 GetMemoryFootprint(void) const override;
	uint32 GetAllocatedSize(void) const;

	virtual void OnTransformChanged(FRHICommandListBase& RHICmdList) override;

private:
	FStaticMeshVertexBuffers VertexBuffers;
	FDynamicMeshIndexBuffer32 IndexBuffer;
	FLocalVertexFactory VertexFactory;

	// Joint axis type
	FRobotJoint Joint;

	bool DrawJoint;
	float DrawSize;
	FTransform DrawOffset;
	float JointRotationOffset;
	FTransform StartingRelativeTransform;
	FTransform ParentTransform;

	const URobotJointComponent* RobotJointComponent;
};