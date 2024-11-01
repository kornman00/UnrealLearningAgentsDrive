// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EpicLearningCarsCoachBase.h"

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>

#include "EpicLearningCarsTrainingCoachBase.generated.h"

class UEpicLearningTrainingCoachParametersBase;
class UEpicLearningNeuralNetworks;

class ULearningAgentsPolicy;
class ULearningAgentsCritic;

UCLASS(Abstract)
class EPICLEARNING_API AEpicLearningCarsTrainingCoachBase : public AEpicLearningCarsCoachBase
{
	GENERATED_BODY()

public:
	AEpicLearningCarsTrainingCoachBase();

protected:
	virtual void InitializeForNewTrackSpline() override;
	virtual bool InitializeTrainingManagers();

	virtual const UEpicLearningTrainingCoachParametersBase* GetCoachParametersBase() const PURE_VIRTUAL(,return nullptr;);

	// Objects managed by the Coach
protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Coach State|Learning Objects")
	ULearningAgentsPolicy* Policy = nullptr;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Coach State|Learning Objects")
	ULearningAgentsCritic* Critic = nullptr;

	// Actor Properties
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coach Settings")
	TObjectPtr<UEpicLearningNeuralNetworks> NeuralNetworks = nullptr;
};
