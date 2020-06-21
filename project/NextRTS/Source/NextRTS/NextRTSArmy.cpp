// Fill out your copyright notice in the Description page of Project Settings.
#include "NextRTSArmy.h"
#include "NextRTS.h"

#include "Engine/StaticMesh.h"
#include "Kismet/KismetMathLibrary.h"

/**
* @TODO We will need to overload this component class
UHierarchicalInstancedStaticMeshComponentIndexed::UHierarchicalInstancedStaticMeshComponentIndexed(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}
*/

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

static int StaticFindOriginalVertexIndex(const TArray<FVector>& original, FVector input, float threshold = 0.0001f)
{
	const int nb = original.Num();
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
		/** @todo patch here */
		//InstancedArmyMesh->OverrideInstanceRandom = true;
		//InstancedArmyMesh->PerInstanceRenderData.Get()->InstanceBuffer.GetInstanceData().get
		FStaticMeshLODResources& LODModel = InstancedArmyMesh->GetStaticMesh()->RenderData->LODResources[0];
		//if (LODModel.ColorVertexBuffer.GetNumVertices() == 0)
		if (VertexIndicesMap.Num())
		{
			// Mesh doesn't have a color vertex buffer yet!  We'll create one now.
			LODModel.VertexBuffers.ColorVertexBuffer.InitFromSingleColor(
				FColor(255, 0, 0, 255), LODModel.GetNumVertices());
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
				LODModel.VertexBuffers.ColorVertexBuffer.VertexColor(i) = FColor(old_i / 255, old_i % 255, 0, 255);
			}
			// @todo MeshPaint: Make sure this is the best place to do this
			BeginInitResource(&LODModel.VertexBuffers.ColorVertexBuffer);
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
		const int32 numInstances = InstancedArmyMesh->GetInstanceCount();
		const float t = FPlatformTime::Seconds();
		static float pi_2 = UKismetMathLibrary::GetPI() / 2.0f;
		for (int32 i = 0; i < numInstances; i++)
		{
			//move in circles
			FTransform trans;
			InstancedArmyMesh->GetInstanceTransform(i, trans);
			float s, c;

			FMath::SinCos(&s, &c, t);
			s *= pi_2;
			c *= pi_2;
			trans.AddToTranslation(FVector(c, s, 0));
			InstancedArmyMesh->UpdateInstanceTransform(i, trans, false, i == numInstances - 1,
													   true);

		}
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
		InstancedArmyMesh->GetStaticMesh()->GetSourceModel(0).RawMeshBulkData->LoadRawMesh(rawMesh);
		VertexIndicesMap.Empty();
		for (int i = 0; i < LODModel.GetNumVertices(); i++)
		{
			int old_i;
			FVector v = LODModel.VertexBuffers.PositionVertexBuffer.VertexPosition(i);
			old_i = StaticFindOriginalVertexIndex(rawMesh.VertexPositions, v);
			if (old_i == -1)
			{
				VertexIndicesMap.Push(i);
			}
			else
			{
				VertexIndicesMap.Push(old_i);
			}
		}
	}
};
#endif
