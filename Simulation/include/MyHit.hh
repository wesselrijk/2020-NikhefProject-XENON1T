#ifndef MyHit_h
#define MyHit_h 1
#include <list>
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4ThreeVector.hh"

/// PMT hit class
///
/// It defines data members to store: the energy of the particle, the position it entered the PMT
/// the time of detection and the PMT number in which the hit is detected: _E, _pos, _time and _PMT
///

class MyHit : public G4VHit
{
public:
    MyHit();
    // set/get methods; eg.
    void SetEdep (G4double edep);
    G4double GetEdep();// const;
    void SetPos(G4ThreeVector pos);
    G4ThreeVector GetPos();// const
    void SetTime (G4double time);
    G4double GetTime();// const;
    void SetPMT (std::string PMT);
    std::string GetPMT();// const;

    virtual void Draw();
    


private:
    // some data members; eg.
    G4double _energy; // energy
    G4ThreeVector _pos; // position
    G4double _time; // time of detection
    std::string _PMT; // PMT number


};
typedef G4THitsCollection<MyHit> MyHitsCollection;

#endif
