#ifndef PROPTRACKINGACTION_H
#define PROPTRACKINGACTION_H
#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include "globals.hh"
#include "G4Types.hh"
#include "boost/shared_ptr.hpp"
#include "g4prop/geant4/PropTrajectory.hh"

namespace G4Prop
{
	/// @brief A G4UserTrackingAction that kills secondary particles if their kinetic energy falls below a cutoff.
	/// Also saves tracks or tracjectories according to how it is configured.
	class PropTrackingAction : public G4UserTrackingAction
	{
	public:
		PropTrackingAction(G4double kineticEnergyMin, G4int storeTrajectory);

		~PropTrackingAction() override;

	public:
		/// @brief Configures the tracking manager to store the trajectory of the track as a PropTrajectory.
		/// @param track
		void PreUserTrackingAction(const G4Track *track) override;

		/// @brief Evaluates whether to kill the secondaries of the track, then copies track information if trajectory information isn't saved.
		/// @param track
		void PostUserTrackingAction(const G4Track *track) override;

		/// @brief Sets the minimum kinetic energy to terminate tracks based off.
		/// @param kineticEnergyMin The minimum kinetic energy cutoff.
		void SetMinimumKineticEnergy(G4double KineticEnergyMin) { fKineticEnergyMin = KineticEnergyMin; }

		/// @brief Gets the cumulative energy culled by artificially killing all the tracks of an event.
		/// @return The cumulative culled energy.
		G4double GetCulledEnergy() { return fCulledEnergy; }
		std::vector<G4Track *> GetTracks() { return fTracks; }
		std::map<G4int, boost::shared_ptr<std::vector<G4int>>> &GetTrackToSecondariesMap() { return fTrackToSecondariesMap; }

	private:
		G4int fStoreTrajectory;
		G4double fKineticEnergyMin;
		G4double fCulledEnergy;
		std::vector<G4Track *> fTracks;
		std::map<G4int, boost::shared_ptr<std::vector<G4int>>> fTrackToSecondariesMap; // Map between Tracks and their secondaries
	};
}
#endif // PROPTRACKINGACTION_H