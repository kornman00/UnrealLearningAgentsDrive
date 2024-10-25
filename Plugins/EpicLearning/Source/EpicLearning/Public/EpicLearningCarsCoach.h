// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EpicLearningCoachingMode.h"

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>

#include "EpicLearningCarsCoach.generated.h"

class UEpicLearningCarAgentsManagerComponent;
class UEpicLearningCarsInteractor;
class UEpicLearningCarsTrainer;
class UEpicLearningCoachParameters;
class UEpicLearningNeuralNetworks;
class AWheeledVehicleTrackSplineFromLandscape;

class ULearningAgentsCritic;
class ULearningAgentsPolicy;

// #TODO Rename to AEpicLearningCarsReinforcementCoach
UCLASS()
class EPICLEARNING_API AEpicLearningCarsCoach : public AActor
{
	GENERATED_BODY()
public:
	AEpicLearningCarsCoach();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	// Respond to spline ready event
	UFUNCTION()
	void OnTrackSplineReady();

	// Internal initialization functions
	bool FindWheeledVehicleTrackSplineFromLandscape();
	void InitializeAgents();
	void InitializeManager();

public:
	virtual void Tick(float DeltaTime) override;

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

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Coach State|Learning Objects")
	ULearningAgentsPolicy* Policy = nullptr;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Coach State|Learning Objects")
	ULearningAgentsCritic* Critic = nullptr;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Coach State|Learning Objects")
	UEpicLearningCarsTrainer* Trainer = nullptr;

	// Actor Properties
public:
	UPROPERTY(EditAnywhere, Category = "Coach Settings")
	EEpicLearningCoachingMode CoachingMode = EEpicLearningCoachingMode::ReInitialize;

	UPROPERTY(EditAnywhere, Category = "Coach Settings")
	TObjectPtr<UEpicLearningCoachParameters> CoachingParameters = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coach Settings")
	TObjectPtr<UEpicLearningNeuralNetworks> NeuralNetworks = nullptr;
};
