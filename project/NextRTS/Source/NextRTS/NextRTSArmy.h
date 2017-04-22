// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NextRTSArmy.generated.h"

UCLASS()
class NEXTRTS_API ANextRTSArmy : public AActor
{
	GENERATED_BODY()
	 
public:	
	// Sets default values for this actor's properties
	ANextRTSArmy();

	/** instanced meshes*/
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UInstancedStaticMeshComponent*  InstancedArmyMesh;

	/** instanced meshes*/
	UPROPERTY(EditAnywhere, Category = Mesh)
	bool MeshIsDirty;

	/** index remap */
	UPROPERTY()
	TArray<int> VertexIndicesMap;

	// store the instance map 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Army)
	UTexture2D* DynamicInstanceParamTexture;

	UPROPERTY()
	int			DynamicTextureWidth; 

	UPROPERTY()
	int			DynamicTextureHeight;
	// The current video frame's corresponding texture
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Army)
	int  ArmyMatrixSize;

	/** unit positions */
	UPROPERTY(BlueprintReadWrite, Category = Army)
	TArray<FTransform> ArmyTransforms;

	UPROPERTY(BlueprintReadWrite, Category = Army)
	TArray<FString> ArmyAnimationStates;

	UPROPERTY(BlueprintReadWrite, Category = Army)
	TArray<int> ArmyAnimationFrames;

	/** unit empty space*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Army)
	float ArmyInitSpace;

	/** animation meshes*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = Army)
	TArray<int> AnimationStartFrames;
	
	/** animation meshes*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = Army)
	TArray<FString> AnimationNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Army)
	UMaterial* ArmySrcMaterial;

	UPROPERTY(BlueprintReadWrite, Category = Army)
	UMaterialInstanceDynamic * DynamicMaterialInstance;

protected:
	
	TArray< TArray<float> > ParamDoubleBuffer; 
	int		CurrentBufferIdx;
	float   CurrentTime;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
	//ENGINE_API virtual void PreEditChange(UProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//ENGINE_API virtual bool CanEditChange(const UProperty* InProperty) const override;
#endif
	//
	void	UploadParameters(); 
	//
	void	InitArmy(); 
};
