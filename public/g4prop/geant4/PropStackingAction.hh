#include "G4UserStackingAction.hh"
#include "G4OpticalPhoton.hh"

#ifndef PROPSTACKINGACTION_H
#define PROPSTACKINGACTION_H

namespace G4Prop
{
	class PropStackingAction : public G4UserStackingAction
	{
	public:
		PropStackingAction();
		~PropStackingAction() override;

		G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track *track) override;
		void NewStage() override
		{
			// ?
		}

		void PrepareNewEvent() override
		{
			fCerenkovCount = 0;
			fTrackCount = 0;
		}

	private:
		G4int fCerenkovCount;
		G4int fTrackCount;
	};
}
#endif // PROPSTACKINGACTION_H