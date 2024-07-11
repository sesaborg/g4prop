#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4RandomDirection.hh"

#ifndef PROPPRIMARYGENERATORACTION_H
#define PROPPRIMARYGENERATORACTION_H

class PropPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PropPrimaryGeneratorAction(G4ParticleGun *particleGun) : G4VUserPrimaryGeneratorAction()
    {
        source = particleGun;
    }
    virtual ~PropPrimaryGeneratorAction()
    {
        // Source is kept independent of the G4ParticleGun* vector
        delete source;
    }
    virtual void GeneratePrimaries(G4Event *event)
    {
        event->SetEventID(eventID);
        eventID++;
        // CLHEP::HepRandom::setTheSeed(eventID);
        source->SetParticlePolarization(G4RandomDirection());
        // source->SetParticleTime(0);
        source->GeneratePrimaryVertex(event);
    }

    void SetEventID(G4int ID) { eventID = ID; }
    // void SetParticleEnergy(G4double Energy) { energy = Energy; }
    // void SetParticleType(G4int Type) { type = Type; }

private:
    G4ParticleGun *source;
    G4int eventID;
    // G4double energy;
    // G4int type;
};

#endif // PROPPRIMARYGENERATORACTION_H