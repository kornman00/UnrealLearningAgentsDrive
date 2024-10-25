// Fill out your copyright notice in the Description page of Project Settings.

#include "EpicLearningCarsCoach.h"

#include "EpicLearningCarAgentsManagerComponent.h"
#include "EpicLearningCarsInteractor.h"
#include "EpicLearningCarsTrainer.h"
#include "EpicLearningCoachParameters.h"
#include "EpicLearningLogging.h"
#include "EpicLearningNamesBPFL.h"
#include "EpicLearningNeuralNetworks.h"
#include "EpicLearningWheeledVehiclePawn.h"
#include "WheeledVehicleTrackSplineFromLandscape.h"

#include <EngineUtils.h>
#include <VisualLogger/VisualLogger.h>

AEpicLearningCarsCoach::AEpicLearningCarsCoach()
{
	LearningAgentsManager = CreateDefaultSubobject<UEpicLearningCarAgentsManagerComponent>(UEpicLearningNamesBPFL::LearningAgentsManagerName());

	PrimaryActorTick.bCanEverTick = true;
}

void AEpicLearningCarsCoach::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (FindWheeledVehicleTrackSplineFromLandscape())
	{
		WheeledVehicleTrackSplineFromLandscape->TrackSplineReadyEvent.AddUniqueDynamic(this, &ThisClass::OnTrackSplineReady);
	}
	else if (const UWorld* TheWorld = GetWorld();
		(TheWorld->IsGameWorld() || TheWorld->IsEditorWorld()) && !TheWorld->IsPreviewWorld())
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Error,
			TEXT("Learning Cars Coach could not bind with any track splines"));
	}
}

void AEpicLearningCarsCoach::BeginPlay()
{
	Super::BeginPlay();
}

void AEpicLearningCarsCoach::OnTrackSplineReady()
{
	if (WheeledVehicleTrackSplineFromLandscape)
	{
		InitializeAgents();
		InitializeManager();
	}
	else
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Error,
			TEXT("Learning Cars Coach was not registered with any track splines"));
	}
}

bool AEpicLearningCarsCoach::FindWheeledVehicleTrackSplineFromLandscape()
{
	for (FActorIterator It(this->GetWorld(), AWheeledVehicleTrackSplineFromLandscape::StaticClass()); It; ++It)
	{
		auto* TrackSplineFromLandscapeInWorld = Cast<AWheeledVehicleTrackSplineFromLandscape>(*It);

		if (WheeledVehicleTrackSplineFromLandscape == nullptr)
		{
			WheeledVehicleTrackSplineFromLandscape = TrackSplineFromLandscapeInWorld;
		}
		else
		{
			UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Warning,
				TEXT("Learning Cars Coach was already registered with %s, ignoring %s"),
				*WheeledVehicleTrackSplineFromLandscape->GetName(),
				*TrackSplineFromLandscapeInWorld->GetName());
			break;
		}
	}

	return WheeledVehicleTrackSplineFromLandscape != nullptr;
}

void AEpicLearningCarsCoach::InitializeAgents()
{
	// In inference mode, we need them all to begin at a random spot.
	// When performing the reset, they need to know about the locations of all the others.
	TArray<AActor*> AllAgentsInTheWorld;

	for (FActorIterator It(this->GetWorld(), AEpicLearningWheeledVehiclePawn::StaticClass()); It; ++It)
	{
		auto* AgentInWorld = Cast<AEpicLearningWheeledVehiclePawn>(*It);

		// The coach needs to tick before the pawns
		AgentInWorld->AddTickPrerequisiteActor(this);

		if (CoachingMode == EEpicLearningCoachingMode::InferenceMode)
		{
			AllAgentsInTheWorld.Add(AgentInWorld);
		}
	}

	if (CoachingMode == EEpicLearningCoachingMode::InferenceMode)
	{
		for (AActor* AgentInWorld : AllAgentsInTheWorld)
		{
			Cast<AEpicLearningWheeledVehiclePawn>(AgentInWorld)->ResetToRandomPointOnSpline(
				WheeledVehicleTrackSplineFromLandscape->GetTrackSplineComponent(),
				AllAgentsInTheWorld);
		}
	}
}

void AEpicLearningCarsCoach::InitializeManager()
{
	// Should neural networks be re-initialized
	const bool bReInitialize = (CoachingMode == EEpicLearningCoachingMode::ReInitialize);

	Interactor = Cast<UEpicLearningCarsInteractor>(ULearningAgentsInteractor::MakeInteractor(
		LearningAgentsManager, UEpicLearningCarsInteractor::StaticClass(), "EpicLearning Cars Interactor"));
	if (Interactor == nullptr)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Warning,
			TEXT("AEpicLearningCarsCoach: Failed to make interactor object."));
		return;
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
			TEXT("AEpicLearningCarsCoach: Failed to make policy object."));
		return;
	}

	Critic = ULearningAgentsCritic::MakeCritic(LearningAgentsManager, Interactor, Policy,
		ULearningAgentsCritic::StaticClass(), "EpicLearning Agents Critic",
		NeuralNetworks->CriticNeuralNetwork, bReInitialize, CoachingParameters->CriticSettings, CoachingParameters->RandomSeed);
	if (Critic == nullptr)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Warning,
			TEXT("AEpicLearningCarsCoach: Failed to make critic object."));
		return;
	}

	Trainer = Cast<UEpicLearningCarsTrainer>(ULearningAgentsTrainer::MakeTrainer(
		LearningAgentsManager, Interactor, Policy, Critic,
		UEpicLearningCarsTrainer::StaticClass(), "EpicLearning Car Trainer",
		CoachingParameters->TrainerSettings));
	if (Trainer == nullptr)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Warning,
			TEXT("AEpicLearningCarsCoach: Failed to make autonomous car trainer object."));
		return;
	}

	Trainer->TrackSpline = WheeledVehicleTrackSplineFromLandscape->GetTrackSplineComponent();
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
