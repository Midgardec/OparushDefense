// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "AI/Navigation/NavigationTypes.h"


#include "EnemyBase.h"

void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	
	AEnemyBase* EnemyAI = Cast<AEnemyBase>(this->GetPawn());
	if(EnemyAI && EnemyAI->canMove)
	{
		EnemyAI->MoveToWaypoint();
	}
}

