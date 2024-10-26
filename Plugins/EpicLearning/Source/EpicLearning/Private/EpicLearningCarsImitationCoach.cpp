// Fill out your copyright notice in the Description page of Project Settings.

#include "EpicLearningCarsImitationCoach.h"

#include "EpicLearningCarAgentsManagerComponent.h"
#include "EpicLearningCarsInteractor.h"
#include "EpicLearningCoachParameters.h"
#include "EpicLearningLogging.h"

#include <LearningAgentsImitationTrainer.h>
#include <VisualLogger/VisualLogger.h>

AEpicLearningCarsImitationCoach::AEpicLearningCarsImitationCoach()
{
	PrimaryActorTick.bCanEverTick = true;
}

const UEpicLearningTrainingCoachParametersBase* AEpicLearningCarsImitationCoach::GetCoachParametersBase() const
{
	return CoachingParameters;
}

bool AEpicLearningCarsImitationCoach::InitializeTrainingManagers()
{
	if (!Super::InitializeTrainingManagers())
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Warning,
			TEXT("AEpicLearningCarsImitationCoach: Base class InitializeManagers failed."));
		return false;
	}

	Trainer = ULearningAgentsImitationTrainer::MakeImitationTrainer(
		LearningAgentsManager, Interactor, Policy,
		ULearningAgentsImitationTrainer::StaticClass());
	if (Trainer == nullptr)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Warning,
			TEXT("AEpicLearningCarsCoach: Failed to make autonomous car trainer object."));
		return false;
	}

	return true;
}

void AEpicLearningCarsImitationCoach::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Trainer != nullptr)
	{
		Trainer->RunTraining(
			Recording,
			CoachingParameters->TrainerSettings,
			CoachingParameters->TrainerTrainingSettings,
			CoachingParameters->TrainerPathSettings);

		Policy->RunInference(CoachingParameters->ActionNoiseScale);

		ResetAgentsTimer += DeltaTime;
		if (ResetAgentsTimer > CoachingParameters->AgentsResetFrequency)
		{
			LearningAgentsManager->ResetAllAgents();

			ResetAgentsTimer = 0.0f;

			ResetAgentsToRandomPointOnSpline();
		}
	}
}
