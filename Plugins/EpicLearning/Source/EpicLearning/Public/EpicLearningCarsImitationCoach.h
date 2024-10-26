// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EpicLearningCarsTrainingCoachBase.h"

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>

#include "EpicLearningCarsImitationCoach.generated.h"

class UEpicLearningImitationCoachParameters;

class ULearningAgentsImitationTrainer;
class ULearningAgentsRecording;

UCLASS()
class EPICLEARNING_API AEpicLearningCarsImitationCoach : public AEpicLearningCarsTrainingCoachBase
{
	GENERATED_BODY()

public:
	AEpicLearningCarsImitationCoach();

protected:
	virtual const UEpicLearningTrainingCoachParametersBase* GetCoachParametersBase() const override;
	virtual bool InitializeTrainingManagers() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Objects managed by the Coach
protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Coach State")
	float ResetAgentsTimer = 0.0f;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Coach State|Learning Objects")
	ULearningAgentsImitationTrainer* Trainer = nullptr;

	// Actor Properties
public:
	UPROPERTY(EditAnywhere, Category = "Coach Settings")
	TObjectPtr<UEpicLearningImitationCoachParameters> CoachingParameters = nullptr;

	UPROPERTY(EditAnywhere, Category = "Coach Settings")
	TObjectPtr<ULearningAgentsRecording> Recording = nullptr;
};
