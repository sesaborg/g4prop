#ifndef PROPSTEPPINGACTION_H
#define PROPSTEPPINGACTION_H
#include "G4UserSteppingAction.hh"
#include "globals.hh"
// #include "boost/graph/graph_traits.hpp"
// #include "boost/graph/undirected_graph.hpp"

namespace G4Prop
{
	/// @brief A G4UserSteppingAction that terminates tracks mid-step if their kinetic energy falls below a cutoff.
	class PropSteppingAction : public G4UserSteppingAction
	{
	public:
		PropSteppingAction();

		/// @brief An alternative constructor that sets the minimum kinetic energy threshold.
		/// @param kineticEnergyMin 
		PropSteppingAction(G4double kineticEnergyMin);
		~PropSteppingAction() override;

	public:
		/// @brief Kills tracks if their kinetic energy falls below the set kinetic energy cutoff.
		/// @param step 
		void UserSteppingAction(const G4Step *step) override;

		/// @brief Sets the minimum kinetic energy to terminate tracks based off.
		/// @param kineticEnergyMin The minimum kinetic energy cutoff.
		void SetMinimumKineticEnergy(G4double kineticEnergyMin) { fKineticEnergyMin = kineticEnergyMin; }
		
		/// @brief Gets the cumulative energy culled by artificially killing all the tracks of an event.
		/// @return The cumulative culled energy.
		G4double GetCulledEnergy() { return fCulledEnergy; }

	private:
		G4double fKineticEnergyMin;
		G4double fCulledEnergy;
	};
}
#endif // PROPSTEPPINGACTION_H