#include "G4UserStackingAction.hh"
#include "G4OpticalPhoton.hh"
#include ""

class PropStackingAction : public G4UserStackingAction {
	PropStackingAction();
	virtual ~PropStackingAction();

	virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* track); 
	virtual void NewStage() {
		// ?
	}

	virtual void PrepareNewEvent() {
		fCerenkovCount = 0;
		fTrackCount = 0;
	}	

private:
	G4int fCerenkovCount;
	G4int fTrackCount;
}
