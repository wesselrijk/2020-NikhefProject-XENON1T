#include "MyHit.hh"
#include "G4ios.hh"

MyHit::MyHit()
:   G4VHit(),
    _energy(0.)  
{}


void MyHit::SetEdep(G4double edep)
    {_energy = edep; }

G4double MyHit::GetEdep(){
    return _energy;
    }
void MyHit::SetPos(G4ThreeVector pos){
    _pos = pos;
    }
G4ThreeVector MyHit::GetPos(){
    return _pos;
    }// const
void MyHit::SetTime (G4double time){
    _time = time;
    }
G4double MyHit::GetTime(){// const;
    return _time;
    }

void MyHit::SetPMT (std::string pmt){
    _PMT = pmt;
    }
std::string MyHit::GetPMT(){// const;
    return _PMT;
    }


void MyHit::Draw() // Mainly used for debugging
{   
    G4cout << "MyHit::Draw has been called" << G4endl;
    G4cout << "energy : " << _energy << " time :" << _time << " positions : " << _pos << G4endl;
}

