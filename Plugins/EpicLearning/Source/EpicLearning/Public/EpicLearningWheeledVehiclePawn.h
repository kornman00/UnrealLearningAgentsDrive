// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "EpicLearningWheeledVehiclePawn.generated.h"

/**
The UE 5.4 tutorial originally implemented this functionality in the SportsCar_Pawn BP
*/
UCLASS()
class EPICLEARNING_API AEpicLearningWheeledVehiclePawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	AEpicLearningWheeledVehiclePawn();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Reset State")
	void ResetToRandomPointOnSpline(USplineComponent* Spline, const TArray<AActor*>& Agents);

	bool IsVisualLoggerEnabledForAgent() const;

protected:
	void RegisterWithLearningAgentsManager();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Reset State")
	int MaxRetries = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Reset State")
	float LocationJitter = 1200.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Reset State")
	float YawJitter = 90.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Reset State")
	float ResetHeight = 50.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Agent State")
	int32 AgentId = INDEX_NONE;
};
