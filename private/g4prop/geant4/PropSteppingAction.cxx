#include "g4prop/geant4/PropSteppingAction.hh"
#include "G4Step.hh"
#include "G4TrackStatus.hh"
#include "G4SteppingManager.hh"

using namespace G4Prop;

PropSteppingAction::PropSteppingAction() : G4UserSteppingAction(), fCulledEnergy(0) {}

PropSteppingAction::PropSteppingAction(G4double kineticEnergyMin) : PropSteppingAction()
{
	fKineticEnergyMin = kineticEnergyMin;
}

PropSteppingAction::~PropSteppingAction() = default;

void PropSteppingAction::UserSteppingAction(const G4Step *step)
{
	G4Track *track = step->GetTrack();
	G4double kineticEnergy = track->GetKineticEnergy();

	// There was a reason why I excluded kineticEnergy = 0, I think it was killing decays?
	if (kineticEnergy < fKineticEnergyMin && kineticEnergy > 0)
	{
		fCulledEnergy += kineticEnergy;
		track->SetTrackStatus(fStopAndKill);
	}
};