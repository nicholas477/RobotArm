// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RobotDeviceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeviceStart, URobotDeviceComponent*, Device);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeviceStop, URobotDeviceComponent*, Device);
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(TArray<int32>, FGetDeviceStatus, URobotDeviceComponent*, Device);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROBOTARM_API URobotDeviceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URobotDeviceComponent();

	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

	UFUNCTION(BlueprintCallable, Category = "Robot Device")
	void StartDevice()
	{
		OnDeviceStart.Broadcast(this);
	}

	UFUNCTION(BlueprintCallable, Category = "Robot Device")
	void StopDevice()
	{
		OnDeviceStop.Broadcast(this);
	}

	UFUNCTION(BlueprintCallable, Category = "Robot Device")
	TArray<int32> GetDeviceStatus();

	// Device base name, before a number is appended to the end
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Robot Device")
	FName DeviceBaseName;

	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere, Category = "Robot Device")
	int32 DeviceNumber;

	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere, Category = "Robot Device")
	FName DeviceName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Device")
	bool bRegisterDevice;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category = "Robot Device")
	FOnDeviceStart OnDeviceStart;

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly, Category = "Robot Device")
	FOnDeviceStop OnDeviceStop;

	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Robot Device")
	FGetDeviceStatus GetDeviceStatusDelegate;
};
