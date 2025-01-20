// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResetActorComponent.generated.h"

class UResetActorComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorReset, AActor*, Actor, UResetActorComponent*, Component, const FTransform&, ActorTransform);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROBOTARM_API UResetActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UResetActorComponent();

	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Reset Actor Component")
	void ResetActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reset Actor Component")
	bool ResetActorTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Reset Actor Component")
	FTransform ActorTransform;

	UPROPERTY(BlueprintAssignable, Category = "Reset Actor Component")
	FOnActorReset OnActorReset;
};
