// Fill out your copyright notice in the Description page of Project Settings.

#include "EpicLearningCarsRecordingCoach.h"

#include "EpicLearningCarAgentsManagerComponent.h"
#include "EpicLearningCarsInteractor.h"
#include "EpicLearningLogging.h"
#include "EpicLearningRecordingCoachParameters.h"
#include "WheeledVehicleTrackSplineFromLandscape.h"

#include <LearningAgentsController.h>
#include <LearningAgentsRecorder.h>

AEpicLearningCarsRecordingCoach::AEpicLearningCarsRecordingCoach()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

TSubclassOf<ULearningAgentsController> AEpicLearningCarsRecordingCoach::GetLearningAgentsControllerClass(
	EEpicLearningImitationSource InImitationSource) const
{
	TSubclassOf<ULearningAgentsController> Result = nullptr;

	switch (InImitationSource)
	{
		case EEpicLearningImitationSource::Human:
			Result = ControllerClassForHumans;
			break;
		case EEpicLearningImitationSource::NonHuman:
			Result = ControllerClassForNonHumans;
			break;

		default:
			checkNoEntry();
	}

	return Result;
}
ULearningAgentsRecording* AEpicLearningCarsRecordingCoach::GetLearningAgentsRecordingAsset(
	EEpicLearningImitationSource InImitationSource) const
{
	ULearningAgentsRecording* Result = nullptr;

	switch (InImitationSource)
	{
		case EEpicLearningImitationSource::Human:
			Result = RecordingForHumans;
			break;
		case EEpicLearningImitationSource::NonHuman:
			Result = RecordingForNonHumans;
			break;

		default:
			checkNoEntry();
	}

	return Result;
}

void AEpicLearningCarsRecordingCoach::InitializeForNewTrackSpline()
{
	Super::InitializeForNewTrackSpline();


}

bool AEpicLearningCarsRecordingCoach::InitializeRecordingManagers()
{
	// Should recording be re-initialized
	const bool bReInitialize = (CoachingMode == EEpicLearningCoachingMode::ReInitialize);

	const TSubclassOf<ULearningAgentsController> LearningAgentsControllerClass =
		GetLearningAgentsControllerClass(RecordingSource);
	if (LearningAgentsControllerClass == nullptr)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Warning,
			TEXT("AEpicLearningCarsRecordingCoach: No Controller class configured for RecordingSource."));
		return false;
	}

	ULearningAgentsRecording* const LearningAgentsRecordingAsset =
		GetLearningAgentsRecordingAsset(RecordingSource);
	if (LearningAgentsRecordingAsset == nullptr)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Warning,
			TEXT("AEpicLearningCarsRecordingCoach: No Recording asset configured for RecordingSource."));
		return false;
	}

	Interactor = Cast<UEpicLearningCarsInteractor>(ULearningAgentsInteractor::MakeInteractor(
		LearningAgentsManager, UEpicLearningCarsInteractor::StaticClass(), "EpicLearning Cars Recording Interactor"));
	if (Interactor == nullptr)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Warning,
			TEXT("AEpicLearningCarsRecordingCoach: Failed to make interactor object."));
		return false;
	}

	Interactor->TrackSpline = WheeledVehicleTrackSplineFromLandscape->GetTrackSplineComponent();

	Controller = ULearningAgentsController::MakeController(
		LearningAgentsManager, Interactor, LearningAgentsControllerClass);
	if (Controller == nullptr)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Warning,
			TEXT("AEpicLearningCarsRecordingCoach: Failed to make Controller object."));
		return false;
	}

	// Unlike BP_SportsCarRecordingManager, we are intentionally NOT trying to cast
	// the Controller to some C++ version of BP_SportsCarAIController to then set
	// its SplineComponent. Instead, that C++ version should cast its Interactor
	// to UEpicLearningCarsInteractor and get the SplineComponent from there.

	Recorder = ULearningAgentsRecorder::MakeRecorder(
		LearningAgentsManager, Interactor,
		ULearningAgentsRecorder::StaticClass(), "EpicLearning Cars Recorder",
		CoachingParameters->RecorderPathSettings, LearningAgentsRecordingAsset,
		bReInitialize);
	if (Recorder == nullptr)
	{
		UE_VLOG_ALWAYS_UELOG(this, LogEpicLearning, Warning,
			TEXT("AEpicLearningCarsRecordingCoach: Failed to make Recorder object."));
		return false;
	}

	Recorder->BeginRecording();

	return true;
}

void AEpicLearningCarsRecordingCoach::EndPlay(const EEndPlayReason::Type InEndPlayReason)
{
	Super::EndPlay(InEndPlayReason);

	// The Learning Agent Recorder's End Recording And Discard can be called instead if you have a
	// situation where the buffered data is not something you would like to save. For example, if the
	// agent died during the course of play and you were trying to only replicate successful episodes.
	if (Recorder != nullptr)
	{
		Recorder->EndRecording();
	}
}

void AEpicLearningCarsRecordingCoach::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Recorder != nullptr)
	{
		ResetAgentsTimer += DeltaTime;
		if (ResetAgentsTimer > CoachingParameters->AgentsResetFrequency)
		{
			Recorder->EndRecording();

			LearningAgentsManager->ResetAllAgents();

			ResetAgentsTimer = 0.0f;

			ResetAgentsToRandomPointOnSpline();

			Recorder->BeginRecording();
		}
	}
}
