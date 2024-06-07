#include "PropSteppingAction.hh"
#include "G4Step.hh"
#include "G4TrackStatus.hh"
#include "G4SteppingManager.hh"

PropSteppingAction::PropSteppingAction() : G4UserSteppingAction(), fKineticEnergyMin(0), fCulledEnergy(0) {}

PropSteppingAction::~PropSteppingAction() {}

void PropSteppingAction::UserSteppingAction {const G4Step* step) {
	G4Track* track = step->GetTrack();
	G4double kineticEnergy = track->GetKineticEnergy();
	
	// There was a reason why I excluded kineticEnergy = 0, maybe because it killed decays?
	if (kineticEnergy < fKineticEnergyMin && kineticEnergy > 0) {
		fCulledEnergy += kineticEnergy;
		track->SetTrackStatus(fStopAndKill);
		
		// I3MCTree goes here
	}
}
