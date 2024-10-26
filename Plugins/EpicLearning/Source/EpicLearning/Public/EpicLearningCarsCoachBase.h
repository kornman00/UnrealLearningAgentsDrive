// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EpicLearningCoachingMode.h"

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>

#include "EpicLearningCarsCoachBase.generated.h"

class UEpicLearningCarAgentsManagerComponent;
class UEpicLearningCarsInteractor;
class AWheeledVehicleTrackSplineFromLandscape;

UCLASS(Abstract)
class EPICLEARNING_API AEpicLearningCarsCoachBase : public AActor
{
	GENERATED_BODY()

public:
	AEpicLearningCarsCoachBase();

protected:
	virtual void PostInitializeComponents() override;

	// Respond to spline ready event
	UFUNCTION()
	void OnTrackSplineReady();

	// Internal initialization functions
	virtual void InitializeForNewTrackSpline();
	bool FindWheeledVehicleTrackSplineFromLandscape();
	void InitializeAgents();

	void ResetAgentsToRandomPointOnSpline();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UEpicLearningCarAgentsManagerComponent* LearningAgentsManager = nullptr;

	// Expected to be in the instanced in the World
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Coach State")
	AWheeledVehicleTrackSplineFromLandscape* WheeledVehicleTrackSplineFromLandscape = nullptr;

	// Objects managed by the Coach
protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Coach State|Learning Objects")
	UEpicLearningCarsInteractor* Interactor = nullptr;

	// Actor Properties
public:
	UPROPERTY(EditAnywhere, Category = "Coach Settings")
	EEpicLearningCoachingMode CoachingMode = EEpicLearningCoachingMode::ReInitialize;
};
