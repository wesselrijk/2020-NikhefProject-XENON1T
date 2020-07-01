
#ifndef UltraDetectorMessenger_h
#define UltraDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class YourDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;


class UltraDetectorMessenger: public G4UImessenger
{
	friend class YourDetectorConstruction;

public:
 
  void SetNewValue(G4UIcommand*, G4String);
  
private:
	
	UltraDetectorMessenger(YourDetectorConstruction* );
	~UltraDetectorMessenger();
	YourDetectorConstruction*   fDetector;// pointer to detector
	G4UIdirectory*               fDetectorDir; 
	G4UIcmdWithAString*          fMirrorCmd; // change surface reflective properties
};

#endif

