// Fill out your copyright notice in the Description page of Project Settings.


#include "Teleport.h"

#include "Main.h"

ATeleport::ATeleport()
{
	Destination = FVector(0.f);
	DestinationRotation = FRotator(0.f);
}

void ATeleport::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                               const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (!Main)
		{
			return;
		}
		Main->SetActorLocation(Destination + GetActorLocation());
		Main->SetActorRotation(DestinationRotation);
		Main->GetController()->SetControlRotation(DestinationRotation);
		Main->SaveGame();
	}
}
