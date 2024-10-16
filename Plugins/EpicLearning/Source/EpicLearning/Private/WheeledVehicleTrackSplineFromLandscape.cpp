// Fill out your copyright notice in the Description page of Project Settings.

#include "WheeledVehicleTrackSplineFromLandscape.h"

#include "EpicLearningLogging.h"

#include <EngineUtils.h>
#include <LandscapeSplineActor.h>
#include <LandscapeSplinesComponent.h>
#include <Components/SplineComponent.h>

AWheeledVehicleTrackSplineFromLandscape::AWheeledVehicleTrackSplineFromLandscape()
{
	// Initialize our spline component
	TrackSpline = CreateDefaultSubobject<USplineComponent>("Track Spline");
	SetRootComponent(TrackSpline);
}

void AWheeledVehicleTrackSplineFromLandscape::BeginPlay()
{
	Super::BeginPlay();

	// Build and set the track spline
	if (!BuildTrackSplineFromLandscapeSpline())
	{
		UE_LOGFMT(LogEpicLearning, Warning,
			"Failed to make the track spline object");
	}
}

bool AWheeledVehicleTrackSplineFromLandscape::BuildTrackSplineFromLandscapeSpline() const
{
	ULandscapeSplinesComponent* LandscapeSpline = nullptr;
	{
		// Find the first LandscapeSplineActor, and warn if we find more than 1
		const ALandscapeSplineActor* LandscapeSplineActor = nullptr;
		for (FActorIterator It(this->GetWorld(), ALandscapeSplineActor::StaticClass()); It; ++It)
		{
			if (!LandscapeSplineActor)
			{
				LandscapeSplineActor = Cast<ALandscapeSplineActor>(*It);
			}
			else
			{
				UE_LOGFMT(LogEpicLearning, Warning,
					"Multiple Landscape Spline Actors found. Defaulting to {0}.",
					LandscapeSplineActor->GetName());
				break;
			}
		}

		if (!LandscapeSplineActor)
		{
			UE_LOGFMT(LogEpicLearning, Error,
				"Failed to find Landscape Spline Actor. Unable to create Track Spline.");
			return false;
		}

		// Retrieve the landscape spline component
		LandscapeSpline = Cast<ULandscapeSplinesComponent>(LandscapeSplineActor->GetRootComponent());
		if (LandscapeSpline == nullptr)
		{
			// Indicate failure
			UE_LOGFMT(LogEpicLearning, Error,
				"Root component of Landscape Spline Actor is not a Landscape Splines Component (should not happen).");
			return false;
		}
	}

	// Copy from the landscape spline to our spline and return success
	LandscapeSpline->CopyToSplineComponent(TrackSpline);
	TrackSpline->SetClosedLoop(bCloseSpline);
	for (int i = 0; i < TrackSpline->GetNumberOfSplinePoints(); i++)
	{
		constexpr bool bUpdateSpline = false;
		TrackSpline->SetSplinePointType(i, ESplinePointType::Type::Curve, bUpdateSpline);
	}

	// Process the spline and fire the event
	TrackSpline->UpdateSpline();

	UE_LOGFMT(LogEpicLearning, Verbose, "Broadcasting TrackSplineReadyEvent.");
	TrackSplineReadyEvent.Broadcast();

	// Indicate success
	return true;
}
