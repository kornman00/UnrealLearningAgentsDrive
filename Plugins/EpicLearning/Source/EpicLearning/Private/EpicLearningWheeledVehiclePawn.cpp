// Fill out your copyright notice in the Description page of Project Settings.

#include "EpicLearningWheeledVehiclePawn.h"

#include "EpicLearningLogging.h"
#include "EpicLearningNamesBPFL.h"

#include <ChaosWheeledVehicleMovementComponent.h>
#include <Components/SplineComponent.h>
#include <EngineUtils.h>
#include <LearningAgentsManager.h>

AEpicLearningWheeledVehiclePawn::AEpicLearningWheeledVehiclePawn()
{
}

void AEpicLearningWheeledVehiclePawn::BeginPlay()
{
	Super::BeginPlay();

	RegisterWithLearningAgentsManager();
}

void AEpicLearningWheeledVehiclePawn::RegisterWithLearningAgentsManager()
{
	// https://dev.epicgames.com/community/learning/courses/kRm/unreal-engine-learning-agents-5-4/owjW/unreal-engine-learning-to-drive-5-4

	const FName LocalManagerName = UEpicLearningNamesBPFL::LearningAgentsManagerName();
	const ULearningAgentsManager* LocalLearningAgentManagerFirstRegistered = nullptr;

	for (FActorIterator It(this->GetWorld()); It; ++It)
	{
		if (const AActor* LocalActorInWorld = *It;
			LocalActorInWorld->ActorHasTag(LocalManagerName))
		{
			if (auto* LocalLearningAgentsManager = LocalActorInWorld->GetComponentByClass<ULearningAgentsManager>();
				IsValid(LocalLearningAgentsManager))
			{
				if (LocalLearningAgentManagerFirstRegistered == nullptr)
				{
					AgentId = LocalLearningAgentsManager->AddAgent(this);
					LocalLearningAgentManagerFirstRegistered = LocalLearningAgentsManager;
				}
				else
				{
					UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Warning,
						TEXT("Learning Agent was already registered with %s, ignoring %s"),
						*LocalLearningAgentManagerFirstRegistered->GetName(),
						*LocalLearningAgentsManager->GetName());
				}
			}
		}
	}

	if (AgentId == INDEX_NONE)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Error,
			TEXT("Learning Agent was not registered with any managers"));
	}
}

void AEpicLearningWheeledVehiclePawn::ResetToRandomPointOnSpline(
	const USplineComponent* Spline,
	const TArray<AActor*>& Agents)
{
	// https://dev.epicgames.com/community/learning/courses/kRm/unreal-engine-learning-agents-5-4/owjW/unreal-engine-learning-to-drive-5-4#resettingepisodes

	// Sanity check
	if (Spline == nullptr)
	{
		UE_LOGFMT(LogEpicLearning, Error,
			"AEpicLearningWheeledVehiclePawn::ResetToRandomPointOnSpline: Spline component is null.");
		return;
	}

	// Initialize local variables
	bool bInvalidTransform = true;
	FTransform CandidateTransform;
	int Attempts = 0;

	while (bInvalidTransform && Attempts <= MaxRetries)
	{
		// Increment attempts (to avoid infinite loop)
		Attempts++;

		// Compute random offsets for jittering
		const FVector RandomOffset = FVector(
			(FMath::FRand() - 0.5f) * LocationJitter,
			(FMath::FRand() - 0.5f) * LocationJitter,
			ResetHeight);
		const float YawOffset = (FMath::FRand() - 0.5f) * YawJitter;

		// Compute random location and rotation on spline
		const float RandomDistance = Spline->GetSplineLength() * FMath::FRand();
		const FVector RandomLocation = Spline->GetLocationAtDistanceAlongSpline(RandomDistance, ESplineCoordinateSpace::World);
		const FRotator RandomRotation = Spline->GetRotationAtDistanceAlongSpline(RandomDistance, ESplineCoordinateSpace::World);

		// Combine into a transform object
		CandidateTransform = FTransform(
			FRotator(RandomRotation.Pitch, RandomRotation.Yaw + YawOffset, RandomRotation.Roll),
			RandomLocation + RandomOffset);

		// Check proximity to all other agents
		bInvalidTransform = false;
		for (UObject* Agent : Agents)
		{
			if (const auto* AgentActor = Cast<AActor>(Agent);
				AgentActor != nullptr && AgentActor != this)
			{
				if (FVector::Distance(CandidateTransform.GetLocation(), AgentActor->GetActorLocation()) < 1500.0)
				{
					bInvalidTransform = true;
					break;
				}
			}
		}
	}

	if (Attempts > MaxRetries)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Error,
			TEXT("Reset failed due to too many retries"));
		return;
	}

	// Halt all current movement then reset transform
	if (auto* PhysicsComponent = Cast<UPrimitiveComponent>(this->GetRootComponent());
		PhysicsComponent != nullptr)
	{
		PhysicsComponent->SetWorldTransform(CandidateTransform, false, nullptr, ETeleportType::TeleportPhysics);

		// Interrupt any angular movement
		PhysicsComponent->SetPhysicsAngularVelocityInDegrees(FVector::Zero());
		// Interrupt any linear movement
		PhysicsComponent->SetPhysicsLinearVelocity(FVector::Zero());

		GetVehicleMovementComponent()->SetTargetGear(1, true);
	}
	else
	{
		UE_LOGFMT(LogEpicLearning, Error,
			"AEpicLearningWheeledVehiclePawn::ResetToRandomPointOnSpline: Failed to retrieve Primitive Component.");
	}
}

bool AEpicLearningWheeledVehiclePawn::IsVisualLoggerEnabledForAgent() const
{
	//return this->IsPlayerControlled();
	return true;
}

void AEpicLearningWheeledVehiclePawn::HandleEnhancedInputActionThrottle(const float InThrottleActionValue)
{
	ThrottleBrakeAction = InThrottleActionValue;
}

void AEpicLearningWheeledVehiclePawn::HandleEnhancedInputActionBrake(const float InBrakeActionValue)
{
	ThrottleBrakeAction = 0.0f - InBrakeActionValue;
}

void AEpicLearningWheeledVehiclePawn::HandleEnhancedInputActionSteering(const float InSteeringActionValue)
{
	SteeringAction = InSteeringActionValue;
}
