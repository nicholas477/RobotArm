// Fill out your copyright notice in the Description page of Project Settings.


#include "Devices/GaylordMover.h"

#include "DefaultMovementSet/CharacterMoverComponent.h"
#include "DefaultMovementSet/NavMoverComponent.h"

// Sets default values
AGaylordMover::AGaylordMover()
{
}

void AGaylordMover::BeginPlay()
{
	Super::BeginPlay();

	NavMoverComponent = FindComponentByClass<UNavMoverComponent>();
	MotionComponent = FindComponentByClass<UMoverComponent>();
}

void AGaylordMover::ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& OutInputCmd)
{
	// Generate user commands. Called right before the Character movement simulation will tick (for a locally controlled pawn)
	// This isn't meant to be the best way of doing a camera system. It is just meant to show a couple of ways it may be done
	// and to make sure we can keep distinct the movement, rotation, and view angles.
	// Styles 1-3 are really meant to be used with a gamepad.
	//
	// Its worth calling out: the code that happens here is happening *outside* of the Character movement simulation. All we are doing
	// is generating the input being fed into that simulation. That said, this means that A) the code below does not run on the server
	// (and non controlling clients) and B) the code is not rerun during reconcile/resimulates. Use this information guide any
	// decisions about where something should go (such as aim assist, lock on targeting systems, etc): it is hard to give absolute
	// answers and will depend on the game and its specific needs. In general, at this time, I'd recommend aim assist and lock on 
	// targeting systems to happen /outside/ of the system, i.e, here. But I can think of scenarios where that may not be ideal too.

	FCharacterDefaultInputs& CharacterInputs = OutInputCmd.InputCollection.FindOrAddMutableDataByType<FCharacterDefaultInputs>();

	if (Controller == nullptr)
	{
		if (GetLocalRole() == ENetRole::ROLE_Authority && GetRemoteRole() == ENetRole::ROLE_SimulatedProxy)
		{
			static const FCharacterDefaultInputs DoNothingInput;
			// If we get here, that means this pawn is not currently possessed and we're choosing to provide default do-nothing input
			CharacterInputs = DoNothingInput;
		}

		// We don't have a local controller so we can't run the code below. This is ok. Simulated proxies will just use previous input when extrapolating
		return;
	}

	CharacterInputs.ControlRotation = FRotator::ZeroRotator;

	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC)
	{
		CharacterInputs.ControlRotation = PC->GetControlRotation();
	}

	bool bRequestedNavMovement = false;
	if (NavMoverComponent)
	{
		bRequestedNavMovement = NavMoverComponent->bRequestedNavMovement;
		if (bRequestedNavMovement)
		{
			CachedMoveInputIntent = NavMoverComponent->CachedNavMoveInputIntent;
			CachedMoveInputVelocity = NavMoverComponent->CachedNavMoveInputVelocity;

			NavMoverComponent->bRequestedNavMovement = false;
			NavMoverComponent->CachedNavMoveInputIntent = FVector::ZeroVector;
			NavMoverComponent->CachedNavMoveInputVelocity = FVector::ZeroVector;
		}
	}

	// Favor velocity input 
	bool bUsingInputIntentForMove = CachedMoveInputVelocity.IsZero();

	if (bUsingInputIntentForMove)
	{
		FRotator Rotator = CharacterInputs.ControlRotation;
		FVector FinalDirectionalIntent;
		if (MotionComponent)
		{
			//if (MotionComponent->IsOnGround() || MotionComponent->IsFalling())
			//{
			//	const FVector RotationProjectedOntoUpDirection = FVector::VectorPlaneProject(Rotator.Vector(), MoverComp->GetUpDirection()).GetSafeNormal();
			//	Rotator = RotationProjectedOntoUpDirection.Rotation();
			//}

			FinalDirectionalIntent = Rotator.RotateVector(CachedMoveInputIntent);
		}

		CharacterInputs.SetMoveInput(EMoveInputType::DirectionalIntent, FinalDirectionalIntent);
	}
	else
	{
		CharacterInputs.SetMoveInput(EMoveInputType::Velocity, CachedMoveInputVelocity);
	}

	// Normally cached input is cleared by OnMoveCompleted input event but that won't be called if movement came from nav movement
	if (bRequestedNavMovement)
	{
		CachedMoveInputIntent = FVector::ZeroVector;
		CachedMoveInputVelocity = FVector::ZeroVector;
	}

	static float RotationMagMin(1e-3);

	const bool bHasAffirmativeMoveInput = (CharacterInputs.GetMoveInput().Size() >= RotationMagMin);

	// Figure out intended orientation
	CharacterInputs.OrientationIntent = FVector::ZeroVector;


	if (bHasAffirmativeMoveInput)
	{
		if (bOrientRotationToMovement)
		{
			// set the intent to the actors movement direction
			CharacterInputs.OrientationIntent = CharacterInputs.GetMoveInput().GetSafeNormal();
		}
		else
		{
			// set intent to the the control rotation - often a player's camera rotation
			CharacterInputs.OrientationIntent = CharacterInputs.ControlRotation.Vector().GetSafeNormal();
		}

		LastAffirmativeMoveInput = CharacterInputs.GetMoveInput();
	}
	//else if (bMaintainLastInputOrientation)
	//{
	//	// There is no movement intent, so use the last-known affirmative move input
	//	CharacterInputs.OrientationIntent = LastAffirmativeMoveInput;
	//}

	//if (bShouldRemainVertical)
	//{
	//	// canceling out any z intent if the actor is supposed to remain vertical
	//	CharacterInputs.OrientationIntent = CharacterInputs.OrientationIntent.GetSafeNormal2D();
	//}

	//CharacterInputs.bIsJumpPressed = bIsJumpPressed;
	//CharacterInputs.bIsJumpJustPressed = bIsJumpJustPressed;

	//if (bShouldToggleFlying)
	//{
	//	if (!bIsFlyingActive)
	//	{
	//		CharacterInputs.SuggestedMovementMode = DefaultModeNames::Flying;
	//	}
	//	else
	//	{
	//		CharacterInputs.SuggestedMovementMode = DefaultModeNames::Falling;
	//	}

	//	bIsFlyingActive = !bIsFlyingActive;
	//}
	//else
	{
		CharacterInputs.SuggestedMovementMode = NAME_None;
	}

	// Convert inputs to be relative to the current movement base (depending on options and state)
	CharacterInputs.bUsingMovementBase = false;

	//if (bUseBaseRelativeMovement)
	//{
	//	if (const UCharacterMoverComponent* MoverComp = GetComponentByClass<UCharacterMoverComponent>())
	//	{
	//		if (UPrimitiveComponent* MovementBase = MoverComp->GetMovementBase())
	//		{
	//			FName MovementBaseBoneName = MoverComp->GetMovementBaseBoneName();

	//			FVector RelativeMoveInput, RelativeOrientDir;

	//			UBasedMovementUtils::TransformWorldDirectionToBased(MovementBase, MovementBaseBoneName, CharacterInputs.GetMoveInput(), RelativeMoveInput);
	//			UBasedMovementUtils::TransformWorldDirectionToBased(MovementBase, MovementBaseBoneName, CharacterInputs.OrientationIntent, RelativeOrientDir);

	//			CharacterInputs.SetMoveInput(CharacterInputs.GetMoveInputType(), RelativeMoveInput);
	//			CharacterInputs.OrientationIntent = RelativeOrientDir;

	//			CharacterInputs.bUsingMovementBase = true;
	//			CharacterInputs.MovementBase = MovementBase;
	//			CharacterInputs.MovementBaseBoneName = MovementBaseBoneName;
	//		}
	//	}
	//}

	// Clear/consume temporal movement inputs. We are not consuming others in the event that the game world is ticking at a lower rate than the Mover simulation. 
	// In that case, we want most input to carry over between simulation frames.
	//{

	//	bIsJumpJustPressed = false;
	//	bShouldToggleFlying = false;
	//}
}
