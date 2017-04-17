// Fill out your copyright notice in the Description page of Project Settings.

#include "NextRTS.h"
#include "NextRTSArmy.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"

//PRAGMA_DISABLE_OPTIMIZATION
// Sets default values
ANextRTSArmy::ANextRTSArmy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create a gun mesh component
	InstancedArmyMesh = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("ArmyMesh"));
	//FP_Gun->SetupAttachment(RootComponent);
	MeshIsDirty = false;
}

static int StaticFindOriginalVertexIndex(const TArray<FVector>&  original, FVector input, float threshold = 0.0001f)
{
	
	int nb = original.Num();
	//input.Y = -input.Y;
	for (int i = 0; i < nb; i++)
	{
		if (FVector::Distance(input, original[i]) < threshold)
		{
			return i; 
		}
	}
	return -1;
}


// Called when the game starts or when spawned
void ANextRTSArmy::BeginPlay()
{
	Super::BeginPlay();
	if (InstancedArmyMesh->GetStaticMesh())
	{
		InstancedArmyMesh->OverrideInstanceRandom = true;
		FStaticMeshLODResources& LODModel = InstancedArmyMesh->GetStaticMesh()->RenderData->LODResources[0];
		//if (LODModel.ColorVertexBuffer.GetNumVertices() == 0)
		if (VertexIndicesMap.Num())
		{
			// Mesh doesn't have a color vertex buffer yet!  We'll create one now.
			LODModel.ColorVertexBuffer.InitFromSingleColor(FColor(255, 0, 0, 255), LODModel.GetNumVertices());
			for (int i = 0; i < LODModel.GetNumVertices(); i++)
			{
				int old_i;
				if (VertexIndicesMap.Num() > i)
				{
					old_i = VertexIndicesMap[i];
				}
				else
				{
					old_i = i;
				}
				LODModel.ColorVertexBuffer.VertexColor(i) = FColor(old_i / 255, old_i % 255, 0, 255);
			}
			// @todo MeshPaint: Make sure this is the best place to do this
			BeginInitResource(&LODModel.ColorVertexBuffer);
		}
		InstancedArmyMesh->GetStaticMesh()->InitResources();
		InstancedArmyMesh->MarkRenderStateDirty();
	}
	
}

// Called every frame
void ANextRTSArmy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (InstancedArmyMesh->GetStaticMesh())
	{
		//for (int i = 0; i < InstancedArmyMesh->GetInstanceCount(); i++)
		//{
		//	//test code 
		//	FTransform trans; 
		//	InstancedArmyMesh->GetInstanceTransform(i, trans);
		//	float s, c; 
		//	float t = FPlatformTime::Seconds()/10.f;
		//	FMath::SinCos(&s, &c, t);
		//	s *= 1.f; 
		//	c *= 1.f; 
		//	trans.AddToTranslation(FVector(s, c, 0));
		//	InstancedArmyMesh->UpdateInstanceTransform(i, trans, false, i == InstancedArmyMesh->GetInstanceCount() - 1, true);
		//}
	}; 

}

#if WITH_EDITOR

#include "RawMesh.h"
 void ANextRTSArmy::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{

	 Super::PostEditChangeProperty(PropertyChangedEvent);
	 if (InstancedArmyMesh->GetStaticMesh())
	 {
		 FStaticMeshLODResources& LODModel = InstancedArmyMesh->GetStaticMesh()->RenderData->LODResources[0];
		 FString text;
		 text += text.Printf(TEXT("%d \r\n"), LODModel.GetNumVertices());
		 FRawMesh rawMesh;
		 InstancedArmyMesh->GetStaticMesh()->SourceModels[0].RawMeshBulkData->LoadRawMesh(rawMesh);
		 //FFileHelper::SaveStringToFile(text, TEXT("e:\\index.txt"));
		 VertexIndicesMap.Empty();
		 for (int i = 0; i < LODModel.GetNumVertices(); i++)
		 {
			 int old_i;
			 FVector v = LODModel.PositionVertexBuffer.VertexPosition(i);
			 old_i = StaticFindOriginalVertexIndex(rawMesh.VertexPositions,v);
			 if (old_i == -1)
			 {
				 VertexIndicesMap.Push( i);
			 }
			 else
			 {
				 VertexIndicesMap.Push(old_i);
			 }
		 }
	 }
};
#endif