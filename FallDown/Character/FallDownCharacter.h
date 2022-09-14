// Copyright Mateusz Bernacki. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FallDownCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

// Declaration of the delegate that will be called when the Primary Action is triggered
// It is declared as dynamic so it can be accessed also in Blueprints
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

UCLASS(config=Game)
class AFallDownCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

public:
	AFallDownCharacter();

	UPROPERTY(BlueprintReadWrite)
	bool StopMovement = false;

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float TurnRateGamepad;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnUseItem;
protected:
	
	/** Fires a projectile. */
	void OnPrimaryAction();

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

//////////////////////////////////////////////////////////////////////////////////////////////
// 	   Shake camera
//Ta sekcja private skupia si� na funkcjach i zmiennych do wywyo�ania trz�sienia si� kamery 
private:
	//To jest timer, kt�ry jest u�ywany do odliczania, aby co 3600 sekund w��czy� matine shake camera
	FTimerHandle ShekeCameraDelayHandler;

	//Funkcja do cz�sania kamer�
	void ShakeCameraPlayer();

	//Przechowuje cz�sanie kamery (obiekt)
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

//////////////////////////////////////////////////////////////////////////////////////////////
// 	   Camera rotation
//Ta sekcja private skupia si� na zmiennych potrzebnych do p�ynnego dzia�ania kamery oraz za jej ruch
private:
	//To jest do movementu camery 

	//Przechowuje warto�� r�wnania 
	float UpDownFinterpOut;

	//Przechowuje warto�� r�wnania 
	float RightLeftFinterpOut;

	//Odpowiada za obr�t kamery (okr�canie : lewo - prawo)
	//Warto�� dodatnia = obracanie kamery w prawo 
	//Warto�� ujemna = obracanie kamery w lewo 
	void TurnAtRate(float Rate);

	//Odpowiada za obr�t kamery (okr�canie : g�ra - d�)
	//Warto�� dodatnia = okr�canie kamery do g�ry 
	//Warto�� ujemna = okr�canie kamery w d�
	void LookUpAtRate(float Rate);

//////////////////////////////////////////////////////////////////////////////////////////////
// 	   Character movement
//Ta sekcja private odpowiada za ruch postaci(aktora / character)
private:
	//Odpowiada za ruch do przodu
	//Warto�� dodatnia = ruch w prawo 
	//Warto�� ujemna = ruch w lewo 
	void MoveForward(float Val);

	//Odpowiada za ruch w prawo
	//Warto�� dodatnia = do przodu 
	//Warto�� ujemna = do ty�u
	void MoveRight(float Val);

//////////////////////////////////////////////////////////////////////////////////////////////
// 	   UserInterface
//Tworzenie interfacu u�ytkownika
//
// 
/////////////////////////////////////////
//	Widok widget
private:
	void CreateInterface();
	void HideInterface();
	void DestroyInterface();
	TSubclassOf<UUserWidget> WidokSubClass;
	UUserWidget* Widok;

/////////////////////////////////////////
//	Pause menu managment
private: 
	void SpawnPauseMenu();
	void DestroyPauseMenu();
	void PauseMenuClick();
	bool isPauseMenuLaunch = false;
	TSubclassOf<UUserWidget> PauseMenuSubClass;
	UUserWidget* PauseMenuWidget;

//////////////////////////////////////////
//	Equipment menu managment 
private:
	void LaunchWidgetClick();
	void LaunchEquipment();
	void DestroyEquipment();
	bool isEquipmentLaunch = false;
	TSubclassOf<UUserWidget> EquipmentWidgetSubClass;
	UUserWidget* EquipmentWidget;
};