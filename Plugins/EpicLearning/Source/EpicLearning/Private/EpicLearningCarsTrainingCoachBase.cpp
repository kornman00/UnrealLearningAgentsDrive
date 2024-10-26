// Fill out your copyright notice in the Description page of Project Settings.


#include "EpicLearningCarsTrainingCoachBase.h"

#include "EpicLearningCarAgentsManagerComponent.h"
#include "EpicLearningCarsInteractor.h"
#include "EpicLearningCoachParameters.h"
#include "EpicLearningLogging.h"
#include "EpicLearningNeuralNetworks.h"
#include "WheeledVehicleTrackSplineFromLandscape.h"

#include <LearningAgentsPolicy.h>
#include <VisualLogger/VisualLogger.h>

AEpicLearningCarsTrainingCoachBase::AEpicLearningCarsTrainingCoachBase()
{
}

void AEpicLearningCarsTrainingCoachBase::InitializeForNewTrackSpline()
{
	Super::InitializeForNewTrackSpline();

	if (InitializeTrainingManagers())
	{
	}
}

bool AEpicLearningCarsTrainingCoachBase::InitializeTrainingManagers()
{
	const UEpicLearningTrainingCoachParametersBase* CoachingParameters = GetCoachParametersBase();

	// Should neural networks be re-initialized
	const bool bReInitialize = (CoachingMode == EEpicLearningCoachingMode::ReInitialize);

	Interactor = Cast<UEpicLearningCarsInteractor>(ULearningAgentsInteractor::MakeInteractor(
		LearningAgentsManager, UEpicLearningCarsInteractor::StaticClass(), "EpicLearning Cars Interactor"));
	if (Interactor == nullptr)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Warning,
			TEXT("AEpicLearningCarsTrainingCoachBase: Failed to make interactor object."));
		return false;
	}

	Interactor->TrackSpline = WheeledVehicleTrackSplineFromLandscape->GetTrackSplineComponent();

	Policy = ULearningAgentsPolicy::MakePolicy(LearningAgentsManager, Interactor,
		ULearningAgentsPolicy::StaticClass(), "EpicLearning Agents Policy",
		NeuralNetworks->EncoderNeuralNetwork, NeuralNetworks->PolicyNeuralNetwork,NeuralNetworks->DecoderNeuralNetwork,
		bReInitialize, bReInitialize, bReInitialize,
		CoachingParameters->PolicySettings, CoachingParameters->RandomSeed);
	if (Policy == nullptr)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Warning,
			TEXT("AEpicLearningCarsTrainingCoachBase: Failed to make policy object."));
		return false;
	}

	Critic = ULearningAgentsCritic::MakeCritic(LearningAgentsManager, Interactor, Policy,
		ULearningAgentsCritic::StaticClass(), "EpicLearning Agents Critic",
		NeuralNetworks->CriticNeuralNetwork, bReInitialize, CoachingParameters->CriticSettings, CoachingParameters->RandomSeed);
	if (Critic == nullptr)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Warning,
			TEXT("AEpicLearningCarsTrainingCoachBase: Failed to make critic object."));
		return false;
	}

	// Child classes need to override this method and also initialize a Trainer!

	return true;
}
