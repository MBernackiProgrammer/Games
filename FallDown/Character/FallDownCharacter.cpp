// Copyright Mateusz Bernacki. All Rights Reserved.

#include "FallDownCharacter.h"
#include "FallDownProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
//#include "Widok.h"
#include "UserInterface/Widok/Widok.h"

//////////////////////////////////////////////////////////////////////////
// AFallDownCharacter

AFallDownCharacter::AFallDownCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	//////////////////////////////////////////////////////////////////////////////////////////////
	//Wyszukiwanie asset�w
	ConstructorHelpers::FClassFinder<UCameraShakeBase> ShakeCameraMatineFinder(TEXT("/Game/Assets/CameraShake/BaseCameraShake"));
	CameraShakeClass = ShakeCameraMatineFinder.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> WidokWidgetFinder(TEXT("/Game/Assets/UserInterface/Equipment/Equipment"));
	WidokSubClass = WidokWidgetFinder.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> PauseMenuWidgetFinder(TEXT("/Game/Assets/UserInterface/PauseMenu/PauseMenu"));
	PauseMenuSubClass = PauseMenuWidgetFinder.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> EquipmentWidgetFinder(TEXT("/Game/Assets/UserInterface/Equipment/Equipment"));
	EquipmentWidgetSubClass = EquipmentWidgetFinder.Class;
}

void AFallDownCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	ShakeCameraPlayer();

	
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Wykrywaj input gracza 
//
void AFallDownCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	//////////////////////////////////////////////////////////////////////////////////////////////
	// Input 
	// Gracz skacze 
	//
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//////////////////////////////////////////////////////////////////////////////////////////////
	// Input 
	// Gracz odpala Pause Menu
	//
	PlayerInputComponent->BindAction("PauseMenu", IE_Pressed, this, &AFallDownCharacter::PauseMenuClick);

	//////////////////////////////////////////////////////////////////////////////////////////////
	// Input 
	// Gracz odpala equipment 
	//
	PlayerInputComponent->BindAction("EquipmentMenu", IE_Pressed, this, &AFallDownCharacter::LaunchWidgetClick);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &AFallDownCharacter::OnPrimaryAction);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	//////////////////////////////////////////////////////////////////////////////////////////////
	// Input
	// Move Forward / Backward - Ruch do przodu do ty�u 
	// 
	// Move Right / Left - Ruch do przodu i do ty�u
	//
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AFallDownCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AFallDownCharacter::MoveRight);

	//////////////////////////////////////////////////////////////////////////////////////////////
	// Okr�canie kamer�
	//
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &AFallDownCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &AFallDownCharacter::LookUpAtRate);
}

void AFallDownCharacter::OnPrimaryAction()
{
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
}

void AFallDownCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnPrimaryAction();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AFallDownCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Ruch postaci - do przedu : do ty�u
//
void AFallDownCharacter::MoveForward(float Value)
{
	if (!StopMovement)
	{
		if (Value != 0.0f)
		{
			// add movement in that direction

			AddMovementInput(FirstPersonCameraComponent->GetForwardVector(), Value);
		}
	}
	
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Ruch postaci w - prawo : lewo
//
void AFallDownCharacter::MoveRight(float Value)
{
	if (!StopMovement)
	{
		if (Value != 0.0f)
		{
			// add movement in that direction
			AddMovementInput(GetActorRightVector(), Value);
		}
	}
	
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Tworzenie (spawnowanie) widgetu widoku
//
void AFallDownCharacter::CreateInterface()
{
	if (!Widok)
	{
		Widok = CreateWidget<UUserWidget>(GetWorld()->GetGameInstance(), WidokSubClass);
		Widok->AddToViewport();

		if (UWidok* WidokObj = Cast<UWidok>(Widok))
		{
			WidokObj->ShowWidok();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//	Ukrywanie widgetu widoku, chowanie go poprzez animacje
//
void AFallDownCharacter::HideInterface()
{
	if (Widok)
	{
		if (UWidok* WidokObj = Cast<UWidok>(Widok))
		{
			WidokObj->HideWidok();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Ca�kowite usuni�cie widkou z vieportu gracza
//
void AFallDownCharacter::DestroyInterface()
{
	if (Widok)
	{
		Widok->RemoveFromParent();
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////
// Okr�canie kamer� - g�ra : d�
//
void AFallDownCharacter::TurnAtRate(float Rate)
{
	if (!StopMovement)
	{
		RightLeftFinterpOut = FMath::FInterpTo(RightLeftFinterpOut, Rate * 0.5, GetWorld()->DeltaTimeSeconds, 7.f);

		AddControllerYawInput(RightLeftFinterpOut);
	}
	
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Okr�canie kamer� - lewo : prawo 
//
void AFallDownCharacter::LookUpAtRate(float Rate)
{
	if (!StopMovement)
	{
		UpDownFinterpOut = FMath::FInterpTo(UpDownFinterpOut, Rate * 0.5, GetWorld()->DeltaTimeSeconds, 7.f);

		AddControllerPitchInput(UpDownFinterpOut);
	}
}

bool AFallDownCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AFallDownCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AFallDownCharacter::EndTouch);

		return true;
	}
	
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Shake camera nie wiem jak w polandyjskim j�zyku xD
//
void AFallDownCharacter::ShakeCameraPlayer()
{
	UGameplayStatics::PlayWorldCameraShake(GetWorld(), CameraShakeClass, this->GetActorLocation(), 0, 5000, 1);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("It's working"));


	GetWorldTimerManager().SetTimer(ShekeCameraDelayHandler, this, &AFallDownCharacter::ShakeCameraPlayer, 1.0f, true, 3600.f);
}

void AFallDownCharacter::PauseMenuClick()
{

	if (isPauseMenuLaunch)
	{
		isPauseMenuLaunch = false;
		DestroyPauseMenu();
	}
	else
	{
		isPauseMenuLaunch = true;
		DestroyPauseMenu();
	}
}

void AFallDownCharacter::SpawnPauseMenu()
{
	PauseMenuWidget = CreateWidget<UUserWidget>(GetWorld()->GetGameInstance(), PauseMenuSubClass);
}

void AFallDownCharacter::DestroyPauseMenu()
{
	PauseMenuWidget->RemoveFromParent();
}

void AFallDownCharacter::LaunchWidgetClick()
{
	if (isEquipmentLaunch)
	{
		isEquipmentLaunch = false;
		LaunchEquipment();
	}
	else
	{
		isEquipmentLaunch = true;
		DestroyEquipment();

	}
}

void AFallDownCharacter::LaunchEquipment()
{
	EquipmentWidget = CreateWidget<UUserWidget>(GetWorld()->GetGameInstance(), EquipmentWidgetSubClass);
}

void AFallDownCharacter::DestroyEquipment()
{
	EquipmentWidget->RemoveFromParent();
}