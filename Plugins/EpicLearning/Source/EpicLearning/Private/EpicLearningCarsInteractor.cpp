// Fill out your copyright notice in the Description page of Project Settings.

#include "EpicLearningCarsInteractor.h"

#include "EpicLearningLogging.h"
#include "EpicLearningNamesBPFL.h"
#include "EpicLearningWheeledVehiclePawn.h"
#include "LearningAgentsManager.h"

#include <ChaosWheeledVehicleMovementComponent.h>
#include <Components/SplineComponent.h>

UEpicLearningCarsInteractor::UEpicLearningCarsInteractor()
{
	// https://dev.epicgames.com/community/learning/courses/kRm/unreal-engine-learning-agents-5-4/dpqe/unreal-engine-improving-observations-debugging-5-4#lookaheadobservations
	TrackDistanceSamples.Add(0.0f);
	TrackDistanceSamples.Add(500.0f);
	TrackDistanceSamples.Add(1000.0f);
	TrackDistanceSamples.Add(1500.0f);
	TrackDistanceSamples.Add(2000.0f);
	TrackDistanceSamples.Add(2500.0f);
	TrackDistanceSamples.Add(3000.0f);
}

void UEpicLearningCarsInteractor::SpecifyAgentObservation_Implementation(
	FLearningAgentsObservationSchemaElement& OutObservationSchemaElement,
	ULearningAgentsObservationSchema* InObservationSchema)
{
	// https://dev.epicgames.com/community/learning/courses/kRm/unreal-engine-learning-agents-5-4/owjW/unreal-engine-learning-to-drive-5-4#sportscarobservations
	// https://dev.epicgames.com/community/learning/courses/kRm/unreal-engine-learning-agents-5-4/dpqe/unreal-engine-improving-observations-debugging-5-4#lookaheadobservations

	const FLearningAgentsObservationSchemaElement TrackStructObservation =
		ULearningAgentsObservations::SpecifyStructObservationFromArrayViews(InObservationSchema,
		{
			UEpicLearningNamesBPFL::TrackLookaheadObservationNameLocation(),
			UEpicLearningNamesBPFL::TrackLookaheadObservationNameDirection(),
		},
		{
			ULearningAgentsObservations::SpecifyLocationAlongSplineObservation(InObservationSchema),
			ULearningAgentsObservations::SpecifyDirectionAlongSplineObservation(InObservationSchema),
		});
	// Define a Track struct in a static array
	const FLearningAgentsObservationSchemaElement TrackArrayObservations =
		ULearningAgentsObservations::SpecifyStaticArrayObservation(InObservationSchema,
		TrackStructObservation,
		TrackDistanceSamples.Num());

	// Define observation of own speed
	const FLearningAgentsObservationSchemaElement SelfStructObservations =
		ULearningAgentsObservations::SpecifyStructObservationFromArrayViews(InObservationSchema,
		{
			UEpicLearningNamesBPFL::CarObservationNameVelocity(),
		},
		{
			ULearningAgentsObservations::SpecifyVelocityObservation(InObservationSchema),
		});

	// Set to outward bound schema element, packed in a single struct
	OutObservationSchemaElement =
		ULearningAgentsObservations::SpecifyStructObservationFromArrayViews(InObservationSchema,
		{
			UEpicLearningNamesBPFL::CarAgentObservationNameTrackLookahead(),
			UEpicLearningNamesBPFL::CarAgentObservationNameSelf(),
		},
		{
			TrackArrayObservations,
			SelfStructObservations,
		});
}

void UEpicLearningCarsInteractor::GatherAgentObservation_Implementation(
	FLearningAgentsObservationObjectElement& OutObservationObjectElement,
	ULearningAgentsObservationObject* InObservationObject,
	const int32 AgentId)
{
	const auto* Agent = Cast<AEpicLearningWheeledVehiclePawn>(Manager->GetAgent(AgentId));
	if (Agent == nullptr)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Error,
			TEXT("GatherAgentObservation: Casting of agent failed."));
		return;
	}

	const bool bVisualLoggerEnabled = Agent->IsVisualLoggerEnabledForAgent();

	// Make the track observations
	FLearningAgentsObservationObjectElement TrackObservationsArray;
	if (TrackSpline != nullptr)
	{
		TArray<FLearningAgentsObservationObjectElement> TrackObservations;
		const float Distance = TrackSpline->GetDistanceAlongSplineAtLocation(Agent->GetActorLocation(), ESplineCoordinateSpace::World);
		for (const float Offset : TrackDistanceSamples)
		{
			const FVector VisualLoggerLocation =
				TrackSpline->GetLocationAtDistanceAlongSpline(Distance + Offset, ESplineCoordinateSpace::World);

			// Compute location and distance along track closest to agent
			const FLearningAgentsObservationObjectElement LocationObservation =
				ULearningAgentsObservations::MakeLocationAlongSplineObservation(InObservationObject,
					TrackSpline, Distance + Offset, Agent->GetActorTransform(),10000.0f,
					UEpicLearningNamesBPFL::DefaultNameForLocationAlongSplineObservation(),
					bVisualLoggerEnabled,nullptr, AgentId,
					VisualLoggerLocation);

			const FLearningAgentsObservationObjectElement DirectionObservation =
				ULearningAgentsObservations::MakeDirectionAlongSplineObservation(InObservationObject,
					TrackSpline, Distance + Offset, Agent->GetActorTransform(),
					UEpicLearningNamesBPFL::DefaultNameForDirectionAlongSplineObservation(),
					bVisualLoggerEnabled,nullptr, AgentId,
					VisualLoggerLocation);

			TrackObservations.Add(
				ULearningAgentsObservations::MakeStructObservationFromArrayViews(InObservationObject,
					{
						UEpicLearningNamesBPFL::TrackLookaheadObservationNameLocation(),
						UEpicLearningNamesBPFL::TrackLookaheadObservationNameDirection(),
					},
					{
						LocationObservation,
						DirectionObservation,
					})
			);
		}

		TrackObservationsArray = ULearningAgentsObservations::MakeStaticArrayObservation(InObservationObject, TrackObservations);
	}

	// Make observations of the underlying car
	FLearningAgentsObservationObjectElement SelfObservations;
	{
		const FVector VisualLoggerLocation = Agent->GetActorLocation();

		const FLearningAgentsObservationObjectElement VelocityObservation =
			ULearningAgentsObservations::MakeVelocityObservation(InObservationObject,
				Agent->GetVelocity(), Agent->GetActorTransform(), 200.0f,
				UEpicLearningNamesBPFL::DefaultNameForVelocityObservation(),
				bVisualLoggerEnabled,nullptr, AgentId,
				VisualLoggerLocation, VisualLoggerLocation);

		SelfObservations =
			ULearningAgentsObservations::MakeStructObservationFromArrayViews(InObservationObject,
				{
					UEpicLearningNamesBPFL::CarObservationNameVelocity(),
				},
				{
					VelocityObservation,
				});
	};

	// Assign to outward bound observation element, packed into a single struct
	OutObservationObjectElement =
		ULearningAgentsObservations::MakeStructObservationFromArrayViews(InObservationObject,
			{
				UEpicLearningNamesBPFL::CarAgentObservationNameTrackLookahead(),
				UEpicLearningNamesBPFL::CarAgentObservationNameSelf(),
			},
			{
				TrackObservationsArray,
				SelfObservations,
			});
}

void UEpicLearningCarsInteractor::SpecifyAgentAction_Implementation(
	FLearningAgentsActionSchemaElement& OutActionSchemaElement,
	ULearningAgentsActionSchema* InActionSchema)
{
	// https://dev.epicgames.com/community/learning/courses/kRm/unreal-engine-learning-agents-5-4/owjW/unreal-engine-learning-to-drive-5-4#sportscaractions

	// Set to outward bound struct element
	OutActionSchemaElement =
		ULearningAgentsActions::SpecifyStructActionFromArrays(InActionSchema,
			{
				UEpicLearningNamesBPFL::CarAgentActionNameSteering(),
				UEpicLearningNamesBPFL::CarAgentActionNameThrottleBrake(),
			},
			{
				ULearningAgentsActions::SpecifyFloatAction(InActionSchema,
					UEpicLearningNamesBPFL::CarAgentActionNameSteering()),
				ULearningAgentsActions::SpecifyFloatAction(InActionSchema,
					UEpicLearningNamesBPFL::CarAgentActionNameThrottleBrake()),
			});

	constexpr int ActionElementsCount = 2;
	PerformAgentActionNamesStorage.Reserve(ActionElementsCount);
	PerformAgentActionObjectsStorage.Reserve(ActionElementsCount);
}

void UEpicLearningCarsInteractor::PerformAgentAction_Implementation(
	const ULearningAgentsActionObject* InActionObject,
	const FLearningAgentsActionObjectElement& InActionObjectElement,
	const int32 AgentId)
{
	const auto* Agent = Cast<AEpicLearningWheeledVehiclePawn>(Manager->GetAgent(AgentId));
	if (Agent == nullptr)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Error,
			TEXT("PerformAgentAction: Casting of agent failed."));
		return;
	}

	auto* VehicleMovement = Cast<UChaosWheeledVehicleMovementComponent>(Agent->GetVehicleMovementComponent());
	if (VehicleMovement == nullptr)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Error,
			TEXT("PerformAgentAction: Failed to get VehicleMovementComponent."));
		return;
	}

	const bool bVisualLoggerEnabled = Agent->IsVisualLoggerEnabledForAgent();

	TArrayView<FName> ActionElementNames{ PerformAgentActionNamesStorage };
	TArrayView<FLearningAgentsActionObjectElement> ActionElements{ PerformAgentActionObjectsStorage };
	ULearningAgentsActions::GetStructActionToArrayViews(ActionElementNames, ActionElements, InActionObject, InActionObjectElement);

	float SteeringValue = 0.0f, ThrottleBrakeValue = 0.0f;
	{
		const int32 SteeringActionIndex = ActionElementNames.Find(UEpicLearningNamesBPFL::CarAgentActionNameSteering());
		const int32 ThrottleBrakeActionIndex = ActionElementNames.Find(UEpicLearningNamesBPFL::CarAgentActionNameThrottleBrake());
		if (SteeringActionIndex == INDEX_NONE ||
			ThrottleBrakeActionIndex == INDEX_NONE)
		{
			UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Error,
				TEXT("PerformAgentAction: Missing one or more expected actions"));
			return;
		}

		const FLearningAgentsActionObjectElement& SteeringAction = ActionElements[SteeringActionIndex];
		const FLearningAgentsActionObjectElement& ThrottleBrakeAction = ActionElements[ThrottleBrakeActionIndex];

		bool bGetActionsSuccess = true;
		bGetActionsSuccess = bGetActionsSuccess &&
			ULearningAgentsActions::GetFloatAction(SteeringValue, InActionObject, SteeringAction, 1.0f, UEpicLearningNamesBPFL::CarAgentActionNameSteering(),
				bVisualLoggerEnabled, nullptr, AgentId, Agent->GetActorLocation());
		bGetActionsSuccess = bGetActionsSuccess &&
			ULearningAgentsActions::GetFloatAction(ThrottleBrakeValue, InActionObject, ThrottleBrakeAction, 1.0f, UEpicLearningNamesBPFL::CarAgentActionNameThrottleBrake(),
				bVisualLoggerEnabled, nullptr, AgentId, Agent->GetActorLocation());
		if (!bGetActionsSuccess)
		{
			UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Error,
				TEXT("PerformAgentAction: Failed to get one or more action values"));
			return;
		}
	}

	// Apply AI input to pawn
	VehicleMovement->SetSteeringInput(SteeringValue);
	if (ThrottleBrakeValue > 0.0f)
	{
		VehicleMovement->SetThrottleInput(ThrottleBrakeValue);
		VehicleMovement->SetBrakeInput(0.0f);
	}
	else
	{
		VehicleMovement->SetThrottleInput(0.0f);
		VehicleMovement->SetBrakeInput(ThrottleBrakeValue);
	}
}
