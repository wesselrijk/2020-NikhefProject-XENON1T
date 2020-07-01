#include "MySD.hh"
#include "MyHit.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"


#include <fstream>
#include <iostream>
using namespace std;

// -------------------------------------------------------------------------
//  The sensitive detector class.
//
// -------------------------------------------------------------------------
MySD::MySD(const G4String& name,
const G4String& hitsCollectionName) :
G4VSensitiveDetector(name), fHitsCollection(0)
{
collectionName.insert(hitsCollectionName);
}
MySD::~MySD()
{
}

void MySD::Initialize(G4HCofThisEvent* hce)
{
fHitsCollection = new MyHitsCollection(SensitiveDetectorName, collectionName[0]); 
auto hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
hce->AddHitsCollection( hcID, fHitsCollection ); 
}

G4bool MySD::ProcessHits(G4Step* step, G4TouchableHistory* /*history*/)
{
fHitsCollection = new MyHitsCollection(SensitiveDetectorName, collectionName[0]); 
MyHit* newHit = new MyHit();

// I use the pre point to get the parameters of the particle
G4StepPoint* prePoint = step->GetPreStepPoint();
if (prePoint->GetMass() != 0) {
    return false;
}
G4double edep = prePoint->GetKineticEnergy();
G4double rel_time = prePoint->GetGlobalTime();
G4ThreeVector postion = prePoint->GetPosition();
std::string pmt = prePoint->GetPhysicalVolume()->GetName();

// store the information in the hit
newHit->SetEdep(edep);
newHit->SetTime(rel_time);
newHit->SetPos(postion);
newHit->SetPMT(pmt);

// add the hit to our hitlist _hits
if (edep > 0){
    _hits.push_back(newHit);
}   

//uncomment to print the number of hits
//G4cout << _hits.size() << G4endl;

// save the hitlist if 200000 hits have been detected
if (_hits.size() == 200000) {
    fstream ofile("data.dat", ios::out);
    int counter = 1;
    for (auto iterator=_hits.begin(); iterator!=_hits.end(); ++iterator, ++counter) 

    // format in which we store is : #hit, #pmt, energy, time, position(x, y, z)
    {
    ofile << counter << ";" <<(*iterator)->GetPMT()<< ";" <<(*iterator)->GetEdep() << ";" << (*iterator)->GetTime() << ";" << (*iterator)->GetPos() << "\n";
    }
    ofile.close();
}

return true;
}


void MySD::EndOfEvent(G4HCofThisEvent* hce) //
{
    G4cout
    << G4endl 
    << "-------->Hits Collection: in this event they are "  
    << " hits in the tracker chambers: " 
    << G4endl;    
}