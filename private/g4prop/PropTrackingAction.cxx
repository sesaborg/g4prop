#include "G4HadronicProcess.hh"
#include "PropTrackingAction.hh"
#include "G4TrackStatus.hh"

PropTrackingAction::PropTrackingAction() : G4UserTrackingAction(), fKineticEnergyMin(0), fCulledEnergy(0) {}

void PropTrackingAction::PreUserTrackingAction(const G4Track* track) {

}

void PropTrackingAction::PostUserTrackingAction(const G4Track* track) {
	const G4TrackVector* secondaries = track->GetStep()->GetSecondary();
	
	for (auto secondaryTrack : *secondaries) {
        G4double kineticEnergy = secondaryTrack->GetKineticEnergy();
		
		if (kineticEnergy < fKineticEnergyMin) {
			fCulledEnergy += kineticEnergy;
			secondaryTrack->SetTrackStatus(fStopAndKill);

			// I3MCTree code goes here
		}
	}
}

PropTrackingAction::~PropTrackingAction() = default;
