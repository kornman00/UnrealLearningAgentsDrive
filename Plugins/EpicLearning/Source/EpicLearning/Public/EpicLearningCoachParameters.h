// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsPolicy.h"
#include "LearningAgentsCritic.h"
#include "LearningAgentsTrainer.h"

#include "EpicLearningCoachParameters.generated.h"

// #TODO Rename to UEpicLearningReinforcementCoachParameters
UCLASS(BlueprintType)
class EPICLEARNING_API UEpicLearningCoachParameters : public UDataAsset
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

	/** Fed into MakeTrainer */
	UPROPERTY(EditAnywhere, Category = "Learning Settings")
	FLearningAgentsTrainerSettings TrainerSettings {};

	/** Fed into RunTraining */
	UPROPERTY(EditAnywhere, Category = "Learning Settings")
	FLearningAgentsTrainerTrainingSettings TrainerTrainingSettings {};

	/** Fed into RunTraining */
	UPROPERTY(EditAnywhere, Category = "Learning Settings")
	FLearningAgentsTrainerGameSettings TrainerGameSettings {};

	/** Fed into RunTraining */
	UPROPERTY(EditAnywhere, Category = "Learning Settings")
	FLearningAgentsTrainerPathSettings TrainerPathSettings {};
};
