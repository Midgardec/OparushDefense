// Fill out your copyright notice in the Description page of Project Settings.


#include "Waypoint.h"

int AWaypoint::GetWaypointOrder()
{
	return WaypointOrder;
}

void AWaypoint::SetWaypoinOrder(int Indx)
{
	WaypointOrder = Indx;
}
