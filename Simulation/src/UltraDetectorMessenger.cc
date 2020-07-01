// Copied and modified by us

// Code developed by:
//  B. Tomé
//
//    *********************************
//    *                               *
//    *    UltraDetectorMessenger.cc *
//    *                               *
//    *********************************
//
//
//
// 

#include "UltraDetectorMessenger.hh"
#include "YourDetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"


UltraDetectorMessenger::UltraDetectorMessenger( YourDetectorConstruction* Det): fDetector(Det)
{ 
  fDetectorDir = new G4UIdirectory("/detector/");
  fDetectorDir -> SetGuidance("Configuration of Ultra setup.");
  
  fMirrorCmd = new G4UIcmdWithAString("/detector/reflection",this);
  fMirrorCmd -> SetGuidance("Define the type of reflecting surface. Default is none."); 
  fMirrorCmd -> SetParameterName("choice",true);
  fMirrorCmd -> SetDefaultValue("none");
  fMirrorCmd -> SetCandidates("none specular ground");
  fMirrorCmd -> AvailableForStates(G4State_Idle); 
 }

UltraDetectorMessenger::~UltraDetectorMessenger()
{
  delete fMirrorCmd;
  delete fDetectorDir;
}

void UltraDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 
  // Select the type of reflection at ground
  if( command == fMirrorCmd ) {
    fDetector -> SetReflectionType(newValue); 
  }
}

