// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsInteractor.h"
#include "EpicLearningCarsInteractor.generated.h"

class AEpicLearningWheeledVehiclePawn;

UCLASS()
class EPICLEARNING_API UEpicLearningCarsInteractor : public ULearningAgentsInteractor
{
	GENERATED_BODY()

public:
	UEpicLearningCarsInteractor();

protected:
	virtual void SpecifyAgentObservation_Implementation(
		FLearningAgentsObservationSchemaElement& OutObservationSchemaElement,
		ULearningAgentsObservationSchema* InObservationSchema) override;

	virtual void GatherAgentObservation_Implementation(
		FLearningAgentsObservationObjectElement& OutObservationObjectElement,
		ULearningAgentsObservationObject* InObservationObject,
		const int32 AgentId) override;

	virtual void SpecifyAgentAction_Implementation(
		FLearningAgentsActionSchemaElement& OutActionSchemaElement,
		ULearningAgentsActionSchema* InActionSchema) override;

	virtual void PerformAgentAction_Implementation(
		const ULearningAgentsActionObject* InActionObject,
		const FLearningAgentsActionObjectElement& InActionObjectElement,
		const int32 AgentId) override;

public:
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Track")
	USplineComponent* TrackSpline = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observations")
	TArray<float> TrackDistanceSamples;

private:
	TArray<FName> PerformAgentActionNamesStorage;
	TArray<FLearningAgentsActionObjectElement> PerformAgentActionObjectsStorage;
};
