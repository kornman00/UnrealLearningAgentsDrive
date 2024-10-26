// Fill out your copyright notice in the Description page of Project Settings.

#include "EpicLearningCarsCoach.h"

#include "EpicLearningCarAgentsManagerComponent.h"
#include "EpicLearningCarsInteractor.h"
#include "EpicLearningCarsTrainer.h"
#include "EpicLearningCoachParameters.h"
#include "EpicLearningLogging.h"
#include "WheeledVehicleTrackSplineFromLandscape.h"

#include <VisualLogger/VisualLogger.h>

AEpicLearningCarsCoach::AEpicLearningCarsCoach()
{
	PrimaryActorTick.bCanEverTick = true;
}

const UEpicLearningTrainingCoachParametersBase* AEpicLearningCarsCoach::GetCoachParametersBase() const
{
	return CoachingParameters;
}

bool AEpicLearningCarsCoach::InitializeTrainingManagers()
{
	if (!Super::InitializeTrainingManagers())
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Warning,
			TEXT("AEpicLearningCarsCoach: Base class InitializeManagers failed."));
		return false;
	}

	Trainer = Cast<UEpicLearningCarsTrainer>(ULearningAgentsTrainer::MakeTrainer(
		LearningAgentsManager, Interactor, Policy, Critic,
		UEpicLearningCarsTrainer::StaticClass(), "EpicLearning Reinforcement Car Trainer",
		CoachingParameters->TrainerSettings));
	if (Trainer == nullptr)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Warning,
			TEXT("AEpicLearningCarsCoach: Failed to make autonomous car trainer object."));
		return false;
	}

	Trainer->TrackSpline = WheeledVehicleTrackSplineFromLandscape->GetTrackSplineComponent();

	return true;
}

void AEpicLearningCarsCoach::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CoachingMode == EEpicLearningCoachingMode::InferenceMode)
	{
		if (Policy != nullptr)
		{
			Policy->RunInference();
		}
	}
	else
	{
		if (Trainer != nullptr)
		{
			constexpr bool bResetAgentsOnBegin = true;
			constexpr bool bResetAgentsOnUpdate = true;
			Trainer->RunTraining(
				CoachingParameters->TrainerTrainingSettings, CoachingParameters->TrainerGameSettings, CoachingParameters->TrainerPathSettings,
				bResetAgentsOnBegin, bResetAgentsOnUpdate);
		}
	}
}
