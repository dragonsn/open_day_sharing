// Fill out your copyright notice in the Description page of Project Settings.

#include "NextRTS.h"
#include "NextRTSArmy.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"

//PRAGMA_DISABLE_OPTIMIZATION
// Sets default values
static int	SingleDynamicParamSize=8;

ANextRTSArmy::ANextRTSArmy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create a gun mesh component
	InstancedArmyMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ArmyMesh"));
	//FP_Gun->SetupAttachment(RootComponent);
	MeshIsDirty = false;
	ArmyMatrixSize = 256; 
	CurrentTime = 0;
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
	this->InitArmy();
	
}

// Called every frame
void ANextRTSArmy::Tick(float DeltaTime)
{
	CurrentTime += DeltaTime;
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
	UploadParameters();
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

 //
 void	ANextRTSArmy::UploadParameters()
 {
	 FName TexParamName(TEXT("TexInstanceDataEx"));
	 UTexture * tex; 
	 if (DynamicMaterialInstance->GetTextureParameterValue(TexParamName,tex))
	 {
		 DynamicMaterialInstance->SetTextureParameterValue(TexParamName, DynamicInstanceParamTexture);
	 }
	 CurrentBufferIdx = !CurrentBufferIdx; 
	 //test code , change to upload your real parameter here
	 for (int i = 0; i < ArmyMatrixSize * ArmyMatrixSize; i++)
	 {
		 int UnitParamIdx = i *SingleDynamicParamSize;
		 ParamDoubleBuffer[CurrentBufferIdx][UnitParamIdx] = (float)i;
		 ParamDoubleBuffer[CurrentBufferIdx][UnitParamIdx+1] = (float)FMath::Sin(CurrentTime);
		 ParamDoubleBuffer[CurrentBufferIdx][UnitParamIdx+2] = (float)FMath::Cos(CurrentTime);
	 }

	 uint8 * pData = (uint8*)(&ParamDoubleBuffer[CurrentBufferIdx][0]);
	 ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
		 UpdateDynamicTextureCode,
		 UTexture2D*, pTexture, DynamicInstanceParamTexture,
		 const uint8*, pData, pData,
		 {
			 FUpdateTextureRegion2D region;
			 region.SrcX = 0;
			 region.SrcY = 0;
			 region.DestX = 0;
			 region.DestY = 0;
			 region.Width = pTexture->GetSizeX();
			 region.Height = pTexture->GetSizeY();
			 FTexture2DResource* resource = (FTexture2DResource*)pTexture->Resource;
			 RHIUpdateTexture2D(resource->GetTexture2DRHI(), 0, region, region.Width * 16, pData);
		 });

 };
 
 void	ANextRTSArmy::InitArmy()
 {
	 //init army material 
	 DynamicTextureWidth = ArmyMatrixSize * 2;
	 DynamicTextureHeight = ArmyMatrixSize;
	 DynamicInstanceParamTexture = UTexture2D::CreateTransient(DynamicTextureWidth, DynamicTextureHeight, PF_A32B32G32R32F);
	 DynamicInstanceParamTexture->UpdateResource();
	 DynamicInstanceParamTexture->AddressX = TA_Clamp;
	 DynamicInstanceParamTexture->AddressY = TA_Clamp;
	 DynamicInstanceParamTexture->Filter = TF_Nearest;
	 DynamicInstanceParamTexture->RefreshSamplerStates();
	 ParamDoubleBuffer.SetNum(2); 
	 ParamDoubleBuffer[0].SetNum(DynamicTextureWidth* DynamicTextureHeight * 4);
	 ParamDoubleBuffer[1].SetNum(DynamicTextureWidth* DynamicTextureHeight * 4);
	 for (int i = 0; i < ArmyMatrixSize * ArmyMatrixSize; i++)
	 {
		 int UnitParamIdx = i *SingleDynamicParamSize;
		 ParamDoubleBuffer[0][UnitParamIdx] = (float)i;
		 ParamDoubleBuffer[1][UnitParamIdx] = (float)i;
		 for (int j = 1; j < SingleDynamicParamSize; j++)
		 {
			 ParamDoubleBuffer[0][UnitParamIdx+j] = 0;
			 ParamDoubleBuffer[1][UnitParamIdx+j] = 0;
		 }
	 }

	 //init army mesh
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

	 DynamicMaterialInstance= InstancedArmyMesh->CreateDynamicMaterialInstance(0, ArmySrcMaterial); 
	 InstancedArmyMesh->SetMaterial(0,DynamicMaterialInstance);


 };