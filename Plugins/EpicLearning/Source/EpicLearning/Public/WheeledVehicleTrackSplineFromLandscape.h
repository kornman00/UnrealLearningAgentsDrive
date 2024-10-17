// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WheeledVehicleTrackSplineFromLandscape.generated.h"

class USplineComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWheeledVehicleTrackSplineFromLandscapeReady);

UCLASS()
class EPICLEARNING_API AWheeledVehicleTrackSplineFromLandscape : public AActor
{
	GENERATED_BODY()

public:
	AWheeledVehicleTrackSplineFromLandscape();

protected:
	virtual void BeginPlay() override;

public:
	// Builds the spline component from an existing landscape spline
	bool BuildTrackSplineFromLandscapeSpline() const;

	USplineComponent* GetTrackSplineComponent() const { return TrackSpline; }

protected:
	/** Spline component for the track */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Track Spline", meta = (AllowPrivateAccess = "true"))
	USplineComponent* TrackSpline;

public:
	UPROPERTY(BlueprintAssignable)
	FWheeledVehicleTrackSplineFromLandscapeReady TrackSplineReadyEvent;

	/** Flag for when the spline is initialized */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Spline")
	bool bCloseSpline = true;
};
