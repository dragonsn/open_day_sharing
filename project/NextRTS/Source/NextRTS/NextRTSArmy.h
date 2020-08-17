// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Private/InstancedStaticMesh.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "NextRTSArmy.generated.h"

/**
* @TODO We will need to overload this component class
UCLASS(ClassGroup=Rendering, meta=(BlueprintSpawnableComponent))
class NEXTRTS_API UHierarchicalInstancedStaticMeshComponentIndexed: public UHierarchicalInstancedStaticMeshComponent
{
	GENERATED_UCLASS_BODY()


};*/


UCLASS()
class NEXTRTS_API ANextRTSArmy : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANextRTSArmy();

	/** instanced meshes*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	class UHierarchicalInstancedStaticMeshComponent*  InstancedArmyMesh;

	/** instanced meshes*/
	UPROPERTY(EditAnywhere, Category = Mesh)
	bool MeshIsDirty;

	/** instanced meshes*/
	UPROPERTY(EditAnywhere, Category = Mesh)
	TArray<int> VertexIndicesMap;


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

};
