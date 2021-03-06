
#include "globals.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4UIcommand.hh"
#include "G4RunManager.hh"
#include "G4ScoringManager.hh"
#include "G4EventManager.hh"

#include "G4PhysListFactory.hh"

#include "FTFP_BERT.hh" // physics list, double for now, since already obtained from the factory
#include "G4OpticalPhysics.hh" // for optical physics (includes optical photon?)
#include "G4EmStandardPhysics_option1.hh" // physics list

#include "YourDetectorConstruction.hh"
#include "YourPrimaryGeneratorAction.hh"
#include "YourActionInitialization.hh"

int main(int argc, char** argv) {

//(lets use C++ implementation of Jame's RANLUX generator)
  G4Random::setTheEngine(new CLHEP::RanluxEngine);

  //1. create the RunManager object 
  G4RunManager* runManager = new G4RunManager();
 
  //2. Create an object from YourDetectorConstruction and register in the Run-Manager
  YourDetectorConstruction* detector = new YourDetectorConstruction();
  runManager->SetUserInitialization( detector );
   
  //3. Create/obtain an Physics List and register it in the Run-Manager
  // This physics list is obtained from the Geant4 LXe example
  G4PhysListFactory physListFactory;
  const G4String plName = "FTFP_BERT";
  G4VModularPhysicsList* physicsList = physListFactory.GetReferencePhysList( plName );
  
  physicsList->ReplacePhysics(new G4EmStandardPhysics_option1());
  G4OpticalPhysics* opticalPhysics = new G4OpticalPhysics();
  opticalPhysics->SetWLSTimeProfile("delta");

  opticalPhysics->SetScintillationYieldFactor(1.0);
  opticalPhysics->SetScintillationExcitationRatio(1.0);

  opticalPhysics->SetMaxNumPhotonsPerStep(100);
  opticalPhysics->SetMaxBetaChangePerStep(10.0);
  
  opticalPhysics->SetTrackSecondariesFirst(kCerenkov, true);
  opticalPhysics->SetTrackSecondariesFirst(kScintillation, true);

  physicsList->RegisterPhysics(opticalPhysics);
  runManager->SetUserInitialization(physicsList);

  //5. Create your action initialization and register in the Run-Manager
  runManager->SetUserInitialization( new YourActionInitialization( detector ) );
  
  //VisManager goes here:
  G4VisManager* visManager = new G4VisExecutive();
  visManager->Initialize();
  G4UIExecutive* ui = 0;
  if (argc == 1) {
    ui =new G4UIExecutive(argc, argv);
  }
  
  // initialization of the run
  runManager->Initialize();

  auto UImanager = G4UImanager::GetUIpointer();
 
  if (! ui) {
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  } else {
    UImanager->ApplyCommand("/control/execute opt_phot.mac");
    ui->SessionStart();
    delete ui;
  }

  // delete the Run-manager
  delete runManager;
  delete visManager;
  
  return 0;
}
