// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <LearningAgentsCritic.h>
#include <LearningAgentsImitationTrainer.h>
#include <LearningAgentsPolicy.h>
#include <LearningAgentsTrainer.h>

#include "EpicLearningCoachParameters.generated.h"

UCLASS(BlueprintType, Abstract)
class EPICLEARNING_API UEpicLearningTrainingCoachParametersBase : public UDataAsset
{
	GENERATED_BODY()
public:
	/** Fed into MakePolicy and MakeCritic */
	UPROPERTY(EditAnywhere, Category = "Manager Settings")
	int RandomSeed = 0;


	/** Fed into MakePolicy */
	UPROPERTY(EditAnywhere, Category = "Learning Settings")
	FLearningAgentsPolicySettings PolicySettings {};

	/** Fed into MakeCritic */
	UPROPERTY(EditAnywhere, Category = "Learning Settings")
	FLearningAgentsCriticSettings CriticSettings {};


	/** Fed into RunTraining */
	UPROPERTY(EditAnywhere, Category = "Learning Settings")
	FLearningAgentsTrainerGameSettings TrainerGameSettings {};

	/** Fed into RunTraining */
	UPROPERTY(EditAnywhere, Category = "Learning Settings")
	FLearningAgentsTrainerPathSettings TrainerPathSettings {};
};

// #TODO Rename to UEpicLearningReinforcementCoachParameters
UCLASS(BlueprintType)
class EPICLEARNING_API UEpicLearningCoachParameters : public UEpicLearningTrainingCoachParametersBase
{
	GENERATED_BODY()
public:
	/** Fed into MakeTrainer */
	UPROPERTY(EditAnywhere, Category = "Learning Settings|Reinforcement")
	FLearningAgentsTrainerSettings TrainerSettings {};

	/** Fed into RunTraining */
	UPROPERTY(EditAnywhere, Category = "Learning Settings|Reinforcement")
	FLearningAgentsTrainerTrainingSettings TrainerTrainingSettings {};
};

UCLASS(BlueprintType)
class EPICLEARNING_API UEpicLearningImitationCoachParameters : public UEpicLearningTrainingCoachParametersBase
{
	GENERATED_BODY()
public:
	/** Fed into MakeTrainer */
	UPROPERTY(EditAnywhere, Category = "Learning Settings|Imitation")
	FLearningAgentsImitationTrainerSettings TrainerSettings {};

	/** Fed into RunTraining */
	UPROPERTY(EditAnywhere, Category = "Learning Settings|Imitation")
	FLearningAgentsImitationTrainerTrainingSettings TrainerTrainingSettings {};

	/** Fed into RunInference */
	UPROPERTY(EditAnywhere, Category = "Learning Settings|Imitation")
	float ActionNoiseScale = 1.0f;

	/** Agents are reset after this many seconds */
	UPROPERTY(EditAnywhere, Category = "Learning Settings|Imitation")
	float AgentsResetFrequency = 10.0f;
};
