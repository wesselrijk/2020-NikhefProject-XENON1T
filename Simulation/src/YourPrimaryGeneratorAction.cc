
#include "globals.hh"

#include "YourPrimaryGeneratorAction.hh"
#include "YourDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4ParticleGun.hh"

#include "G4OpticalPhoton.hh"

YourPrimaryGeneratorAction::YourPrimaryGeneratorAction(YourDetectorConstruction* det)
: G4VUserPrimaryGeneratorAction() {
  fYourDetector = det;
  // create particle gun and set default properties:
  fParticleGun  = new G4ParticleGun(1);
  // particle type: optical photon
  G4ParticleDefinition* part = G4OpticalPhoton::Definition();
  fParticleGun->SetParticleDefinition(part);
  // direction: 0,0,1 i.e. to the target 
  fParticleGun->SetParticleMomentumDirection( G4ThreeVector(0.0, 0.0, 1.0) );
  // kinetic energy:  2 MeV
  fParticleGun->SetParticleEnergy(2.0*CLHEP::MeV);
  // set postion
  fParticleGun->SetParticlePosition(G4ThreeVector(0.0, 0.0, 0.0) ); 
}

YourPrimaryGeneratorAction::~YourPrimaryGeneratorAction(){
  delete fParticleGun;
}

void YourPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
  fParticleGun->GeneratePrimaryVertex(anEvent);
}
  