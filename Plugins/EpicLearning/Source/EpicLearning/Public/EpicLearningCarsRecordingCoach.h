// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EpicLearningImitationSource.h"
#include "EpicLearningCarsCoachBase.h"

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>

#include "EpicLearningCarsRecordingCoach.generated.h"

class UEpicLearningRecordingCoachParameters;

class ULearningAgentsController;
class ULearningAgentsRecorder;
class ULearningAgentsRecording;

UCLASS()
class EPICLEARNING_API AEpicLearningCarsRecordingCoach : public AEpicLearningCarsCoachBase
{
	GENERATED_BODY()

public:
	AEpicLearningCarsRecordingCoach();

	TSubclassOf<ULearningAgentsController> GetLearningAgentsControllerClass(
		EEpicLearningImitationSource InImitationSource) const;
	ULearningAgentsRecording* GetLearningAgentsRecordingAsset(
		EEpicLearningImitationSource InImitationSource) const;

protected:
	virtual void InitializeForNewTrackSpline() override;
	bool InitializeRecordingManagers();

	virtual void EndPlay(const EEndPlayReason::Type InEndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	// Objects managed by the Coach
protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Coach State")
	float ResetAgentsTimer = 0.0f;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Coach State|Learning Objects")
	ULearningAgentsRecorder* Recorder = nullptr;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Coach State|Learning Objects")
	ULearningAgentsController* Controller = nullptr;

	// Actor Properties
public:
	UPROPERTY(EditAnywhere, Category = "Coach Settings")
	EEpicLearningImitationSource RecordingSource = EEpicLearningImitationSource::NonHuman;

	UPROPERTY(EditAnywhere, Category = "Coach Settings")
	bool bRemoveAllNonPlayerAgentsAtStart = false;

	UPROPERTY(EditAnywhere, Category = "Coach Settings")
	TObjectPtr<UEpicLearningRecordingCoachParameters> CoachingParameters = nullptr;

	UPROPERTY(EditAnywhere, Category = "Coach Settings|Human")
	TSubclassOf<ULearningAgentsController> ControllerClassForHumans = nullptr;
	UPROPERTY(EditAnywhere, Category = "Coach Settings|Human")
	TObjectPtr<ULearningAgentsRecording> RecordingForHumans = nullptr;

	UPROPERTY(EditAnywhere, Category = "Coach Settings|NonHuman")
	TSubclassOf<ULearningAgentsController> ControllerClassForNonHumans = nullptr;
	UPROPERTY(EditAnywhere, Category = "Coach Settings|NonHuman")
	TObjectPtr<ULearningAgentsRecording> RecordingForNonHumans = nullptr;
};
