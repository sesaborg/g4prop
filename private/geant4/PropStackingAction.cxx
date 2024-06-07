#include "PropStackingAction.hh"

PropStackingAction::PropStackingAction() : G4UserStackingAction(), fCerenkovCount(0), fTrackCount(0) {}
virtual ~PropStackingAction() {}

G4ClassificationOfNewTrack PropStackingAction::ClassifyNewTrack(const G4Track* track) { 
	if (track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition() && track->GetParentId() > 0 && track->GetCreatorProcess()->GetProcessName() == "Cerenkov") {
	fCerenkovCount++;
	}
	fTrackCount++;
	return fUrgent;
}
