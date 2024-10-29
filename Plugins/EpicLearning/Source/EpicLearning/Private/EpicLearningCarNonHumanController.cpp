// Fill out your copyright notice in the Description page of Project Settings.

#include "EpicLearningCarNonHumanController.h"

#include "EpicLearningCarsInteractor.h"

#include <Components/SplineComponent.h>
#include <LearningAgentsManager.h>

void UEpicLearningCarNonHumanController::EvaluateAgentSteeringAndThrottleBrake(
	const int32 AgentId,
	float& OutSteering,
	float& OutThrottleBrake) const
{
	// https://dev.epicgames.com/community/learning/courses/kRm/unreal-engine-learning-agents-5-4/6deP/unreal-engine-learning-to-drive-imitation-5-4#learningagentscontroller
	// "This code creates an agent that will attempt to steer towards the center of the track and will slow down around turns."

	// To any future readers:
	//	The original BP did not include node comments or intermediate variable names. Variable names and comments below
	//	were conjured up via reverse engineering and assistance with Copilot. An effort was made to sanity check its
	//	feedback, but there remains a chance Copilot or the author of this comment were under the influence of
	//	thinking they know more than they actually do.

	const auto* CarsInteractor = Cast<UEpicLearningCarsInteractor>(this->Interactor);

	const auto* AgentActor = Cast<AActor>(Manager->GetAgent(AgentId));
	const FVector AgentWorldLocation = AgentActor->GetActorLocation();
	const FRotator AgentRotation = AgentActor->GetActorRotation();

	FVector EgoCentricAgentVectorTowardsCurve;
	{
		const FVector ClosestCurveLocationInWorldSpace =
			CarsInteractor->TrackSpline->FindLocationClosestToWorldLocation(AgentWorldLocation, ESplineCoordinateSpace::World);
		const FVector VectorFromAgentTowardsCurve = ClosestCurveLocationInWorldSpace - AgentWorldLocation;
		// Get the vector towards the curve without the agent's own world rotation in the mix
		EgoCentricAgentVectorTowardsCurve = AgentRotation.UnrotateVector(VectorFromAgentTowardsCurve);
	};

	FVector EgoCentricDirectionTowardsCurve;
	{
		const FVector AgentForwardVector = AgentActor->GetActorForwardVector();

		const FVector ClosestUnitDirectionToSpline =
			CarsInteractor->TrackSpline->FindDirectionClosestToWorldLocation(AgentWorldLocation, ESplineCoordinateSpace::Local);
		const FVector DirectionFromAgentTowardsCurve = ClosestUnitDirectionToSpline - AgentForwardVector;
		// Get the direction without the agent's own world rotation in the mix
		EgoCentricDirectionTowardsCurve = AgentRotation.UnrotateVector(DirectionFromAgentTowardsCurve);
	};

	// This small scaling factor likely reduces the influence of the agent's position relative to the track spline.
	// It ensures that minor positional deviations don't cause overly aggressive steering adjustments.
	//	i.e., Reduces the impact of positional deviations, preventing overreaction to small changes in position.
	constexpr float AgentPositionScalar = 0.01f;
	// This larger scaling factor amplifies the influence of the agent's orientation towards the track spline.
	// It makes the steering input more sensitive to the direction the agent is facing, which is crucial for maintaining proper alignment with the track.
	//	i.e., Increases the impact of directional deviations, ensuring quick and effective orientation corrections
	constexpr float AgentOrientationScalar = 15.0f;

	// By adjusting the throttle or brake value based on the current speed and steering input, the code helps in
	// maintaining a smooth and realistic driving experience. For instance, if the vehicle is moving too fast, the
	// ThrottleBrakeValue will be lower, indicating the need to apply brakes or reduce throttle. Conversely,
	// if the vehicle is moving slowly, the value will be higher, suggesting an increase in throttle.
	const float YFromEgoCentricAgentVectorTowardsCurveScaled = EgoCentricAgentVectorTowardsCurve.Y * AgentPositionScalar;
	const float YFromEgoCentricDirectionTowardsCurveScaled = EgoCentricDirectionTowardsCurve.Y * AgentOrientationScalar;

	// positive (turning right) or negative (turning left)
	// SetSteeringInput implicitly clamps this to [-1.0,+1.0]
	const float SteeringValue =
		YFromEgoCentricAgentVectorTowardsCurveScaled + YFromEgoCentricDirectionTowardsCurveScaled;

	// When a vehicle is turning, it generally needs to slow down to maintain control and avoid skidding.
	// By reducing the throttle or increasing the brake when steering input is high, the code helps ensure
	// that the vehicle doesn't move too fast while turning, enhancing stability and control.
	float ThrottleBrakeValue;
	{
		constexpr float BenchmarkSpeed = 3000.0f;
		constexpr float CentimetersToMetersScalar = 0.01f;
		const FVector AgentVelocity = AgentActor->GetVelocity();
		const float AgentVelocityLength = AgentVelocity.Length();

		// If moving slower, this will be positive and the agent needs to speed up
		// If moving faster, this will be negative and the agent needs to slow down
		float VelocityDifference = BenchmarkSpeed - AgentVelocityLength;
		// The original BP used divide-by-100, but we're going to multiply for the same effect
		VelocityDifference *= CentimetersToMetersScalar;
		// The greater the driver is steering in an absolute sense, the lower the braking will be
		ThrottleBrakeValue = VelocityDifference - FMath::Abs(SteeringValue);
	};

	OutSteering = SteeringValue;
	OutThrottleBrake = ThrottleBrakeValue;
}
