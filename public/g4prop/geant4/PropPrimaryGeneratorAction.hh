#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4RandomDirection.hh"

#ifndef PROPPRIMARYGENERATORACTION_H
#define PROPPRIMARYGENERATORACTION_H
namespace G4Prop{
/// @brief A G4VUserPrimaryGeneratorAction that takes a G4ParticleGun pointer to generate a primary vertex. 
class PropPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PropPrimaryGeneratorAction(G4ParticleGun *particleGun) : G4VUserPrimaryGeneratorAction()
    {
        source = particleGun;
    }
    virtual ~PropPrimaryGeneratorAction()
    {
    }

    /// @brief Uses the loaded G4ParticleGun to generate the primary vertex.
    /// @param event 
    virtual void GeneratePrimaries(G4Event *event)
    {
        event->SetEventID(eventID);
        eventID++;
        source->GeneratePrimaryVertex(event);
    }

    void SetEventID(G4int ID) { eventID = ID; }

private:
    G4ParticleGun *source;
    G4int eventID;
};
}
#endif // PROPPRIMARYGENERATORACTION_H