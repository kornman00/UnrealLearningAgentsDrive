// Fill out your copyright notice in the Description page of Project Settings.

#include "EpicLearningCarsTrainer.h"

#include "EpicLearningLogging.h"
#include "EpicLearningNamesBPFL.h"
#include "EpicLearningWheeledVehiclePawn.h"

#include <Components/SplineComponent.h>
#include <LearningAgentsManager.h>
#include <LearningAgentsRewards.h>

UEpicLearningCarsTrainer::UEpicLearningCarsTrainer()
{
}

void UEpicLearningCarsTrainer::GatherAgentReward_Implementation(
	float& OutReward,
	const int32 AgentId)
{
	// Get reference to agent and it's movement component
	const auto* Agent = Cast<AEpicLearningWheeledVehiclePawn>(Manager->GetAgent(AgentId));
	if (Agent == nullptr)
	{
		UE_LOG(LogEpicLearning, Error, TEXT("GatherAgentReward: Casting of agent failed."));
		return;
	}

	// Reward going fast
	const float SpeedReward = ULearningAgentsRewards::MakeRewardFromVelocityAlongSpline(
		TrackSpline, Agent->GetActorLocation(), Agent->GetVelocity(), 1000.0f);

	// Penalize going off track
	const FVector TrackLocation = TrackSpline->FindLocationClosestToWorldLocation(Agent->GetActorLocation(), ESplineCoordinateSpace::World);
	const float OffTrackPenalty = ULearningAgentsRewards::MakeRewardOnLocationDifferenceAboveThreshold(
		Agent->GetActorLocation(), TrackLocation, OffTrackThreshold, -10.0f);

	// Sum reward and assign to outward bound float
	OutReward = 0
		+ SpeedReward
		+ OffTrackPenalty
	;
}

void UEpicLearningCarsTrainer::GatherAgentCompletion_Implementation(
	ELearningAgentsCompletion& OutCompletion,
	const int32 AgentId)
{
	// Get reference to agent
	const auto* Agent = Cast<AEpicLearningWheeledVehiclePawn>(Manager->GetAgent(AgentId));
	if (Agent == nullptr)
	{
		UE_LOG(LogEpicLearning, Error, TEXT("GatherAgentCompletion: Casting of agent failed."));
		return;
	}

	const bool bVisualLoggerEnabled = Agent->IsVisualLoggerEnabledForAgent();

	// Terminate if off track too far
	const FVector TrackLocation = TrackSpline->FindLocationClosestToWorldLocation(Agent->GetActorLocation(), ESplineCoordinateSpace::World);
	const ELearningAgentsCompletion TrackCompletion = ULearningAgentsCompletions::MakeCompletionOnLocationDifferenceAboveThreshold(
		Agent->GetActorLocation(), TrackLocation, OffTrackThreshold,
		ELearningAgentsCompletion::Termination, UEpicLearningNamesBPFL::DefaultNameForLocationDifferenceAboveThresholdCompletion(),
		bVisualLoggerEnabled, nullptr, AgentId,
		Agent->GetActorLocation());
	if (TrackCompletion == ELearningAgentsCompletion::Termination)
	{
		OutCompletion = ELearningAgentsCompletion::Termination;
	}
}

void UEpicLearningCarsTrainer::ResetAgentEpisode_Implementation(const int32 AgentId)
{
	// Get reference to agent
	auto* Agent = Cast<AEpicLearningWheeledVehiclePawn>(Manager->GetAgent(AgentId));
	if (Agent == nullptr)
	{
		UE_LOG(LogEpicLearning, Error, TEXT("ResetAgentEpisode: Casting of agent failed."));
		return;
	}

	// Get all agents
	const TConstArrayView<TObjectPtr<UObject>> AllAgentsArrayView = Manager->GetAgents();

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

	// Reset to random point on track
	Agent->ResetToRandomPointOnSpline(TrackSpline, AllAgentsAsActors);
}
