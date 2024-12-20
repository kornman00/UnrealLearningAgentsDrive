﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EpicLearningCarsTrainingCoachBase.h"

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>

#include "EpicLearningCarsCoach.generated.h"

class UEpicLearningCarsTrainer;
class UEpicLearningCoachParameters;

// #TODO Rename to AEpicLearningCarsReinforcementCoach
UCLASS()
class EPICLEARNING_API AEpicLearningCarsCoach : public AEpicLearningCarsTrainingCoachBase
{
	GENERATED_BODY()
public:
	AEpicLearningCarsCoach();

protected:
	virtual const UEpicLearningTrainingCoachParametersBase* GetCoachParametersBase() const override;
	virtual bool InitializeTrainingManagers() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Objects managed by the Coach
protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Coach State|Learning Objects")
	UEpicLearningCarsTrainer* Trainer = nullptr;

	// Actor Properties
public:
	UPROPERTY(EditAnywhere, Category = "Coach Settings")
	TObjectPtr<UEpicLearningCoachParameters> CoachingParameters = nullptr;
};
