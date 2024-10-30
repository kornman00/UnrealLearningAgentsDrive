// Fill out your copyright notice in the Description page of Project Settings.

#include "EpicLearningCarsCoachBase.h"

#include "EpicLearningCarAgentsManagerComponent.h"
#include "EpicLearningCarsInteractor.h"
#include "EpicLearningLogging.h"
#include "EpicLearningNamesBPFL.h"
#include "EpicLearningWheeledVehiclePawn.h"
#include "WheeledVehicleTrackSplineFromLandscape.h"

#include <EngineUtils.h>
#include <VisualLogger/VisualLogger.h>

AEpicLearningCarsCoachBase::AEpicLearningCarsCoachBase()
{
	LearningAgentsManager = CreateDefaultSubobject<UEpicLearningCarAgentsManagerComponent>(UEpicLearningNamesBPFL::LearningAgentsManagerName());
}

void AEpicLearningCarsCoachBase::PostInitializeComponents()
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

void AEpicLearningCarsCoachBase::OnTrackSplineReady()
{
	if (WheeledVehicleTrackSplineFromLandscape)
	{
		InitializeForNewTrackSpline();
	}
	else
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Error,
			TEXT("Learning Cars Coach was not registered with any track splines"));
	}
}

void AEpicLearningCarsCoachBase::InitializeForNewTrackSpline()
{
	InitializeAgents();
}

bool AEpicLearningCarsCoachBase::FindWheeledVehicleTrackSplineFromLandscape()
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

void AEpicLearningCarsCoachBase::InitializeAgents()
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

void AEpicLearningCarsCoachBase::DeleteAllPossibleAgentPawnsInWorldExceptTheLocalPlayer() const
{
	TArray<AActor*> AllAgentsInTheWorld;

	for (FActorIterator It(this->GetWorld(), AEpicLearningWheeledVehiclePawn::StaticClass()); It; ++It)
	{
		auto* AgentInWorld = Cast<AEpicLearningWheeledVehiclePawn>(*It);

		if (AgentInWorld->GetLearningAgentId() != INDEX_NONE)
		{
			UE_LOGFMT(LogEpicLearning, Warning, "Not deleting {0} as it is already registered",
				AgentInWorld->GetName());
		}
		else if (AgentInWorld->IsPlayerControlled())
		{
			UE_LOGFMT(LogEpicLearning, Log, "Not deleting {0} as it is player controlled",
				AgentInWorld->GetName());
		}
		else
		{
			AllAgentsInTheWorld.Add(AgentInWorld);
		}
	}

	if (!AllAgentsInTheWorld.IsEmpty())
	{
		UE_LOGFMT(LogEpicLearning, Warning, "Deleting {0} possible agent pawns",
			AllAgentsInTheWorld.Num());
		for (AActor* AgentInWorld : AllAgentsInTheWorld)
		{
			this->GetWorld()->DestroyActor(AgentInWorld);
		}
	}
}

void AEpicLearningCarsCoachBase::ResetAgentsToRandomPointOnSpline() const
{
	const USplineComponent* TrackSpline = WheeledVehicleTrackSplineFromLandscape->GetTrackSplineComponent();

	// Get all agents
	const TConstArrayView<TObjectPtr<UObject>> AllAgentsArrayView = LearningAgentsManager->GetAgents();

	// Cast to array of actors
	TArray<AActor*> AllAgentsAsActors;
	AllAgentsAsActors.Reserve(AllAgentsArrayView.Num());
	for (TObjectPtr<UObject> Other : AllAgentsArrayView)
	{
		// At runtime, it is possible there are not Agents registered to the MaxAgentNum
		if (IsValid(Other))
		{
			AllAgentsAsActors.Add(StaticCast<AActor*>(Other));
		}
	}

	// If we're recording for imitation, it is possible that there is only one agent (the player).
	// An edge case, but we COULD make AllAgentsAsActors an optional parameter...

	// Reset to random point on track
	for (AActor* AgentActor : AllAgentsAsActors)
	{
		auto* Agent = Cast<AEpicLearningWheeledVehiclePawn>(AgentActor);
		Agent->ResetToRandomPointOnSpline(TrackSpline, AllAgentsAsActors);
	}
}
