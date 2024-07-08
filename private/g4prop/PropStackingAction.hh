#include "G4UserStackingAction.hh"
#include "G4OpticalPhoton.hh"

#ifndef PROPSTACKINGACTION_H
#define PROPSTACKINGACTION_H

class PropStackingAction : public G4UserStackingAction {
	PropStackingAction();
	~PropStackingAction() override;

	G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* track) override;
	void NewStage() override {
		// ?
	}

	void PrepareNewEvent() override {
		fCerenkovCount = 0;
		fTrackCount = 0;
	}	

private:
	G4int fCerenkovCount;
	G4int fTrackCount;
};
#endif //PROPSTACKINGACTION_H