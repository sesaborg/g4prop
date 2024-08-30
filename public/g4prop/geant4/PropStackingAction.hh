#include "G4UserStackingAction.hh"
#include "G4OpticalPhoton.hh"

#ifndef PROPSTACKINGACTION_H
#define PROPSTACKINGACTION_H

namespace G4Prop
{
	/// @brief A G4UserStackingAction that is currently empty.
	class PropStackingAction : public G4UserStackingAction
	{
	public:
		PropStackingAction();
		~PropStackingAction() override;

		/// @brief Custom Classification, if that's useful eventually.
		/// @param track 
		/// @return 
		G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track *track) override;

		void NewStage() override
		{
			// You can put stacking logic in here (maybe to deal with Cerenkov photons)
		}

		void PrepareNewEvent() override
		{
			// Configure cool stacking variables here!
		}
	private:
		// Variables go here
	};
}
#endif // PROPSTACKINGACTION_H