#include "PropStackingAction.hh"
#include "G4ClassificationOfNewTrack.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

G4ClassificationOfNewTrack PropStackingAction::ClassifyNewTrack(const G4Track* track) {
	if (track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition() && track->GetParentID() > 0 && track->GetCreatorProcess()->GetProcessName() == "Cerenkov") {
	fCerenkovCount++;
	}
	fTrackCount++;
	return fUrgent;
}

PropStackingAction::PropStackingAction() : G4UserStackingAction(), fCerenkovCount(0), fTrackCount(0) {}
PropStackingAction::~PropStackingAction() = default;
