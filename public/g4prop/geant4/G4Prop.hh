#ifndef G4PROP_HH
#define G4PROP_HH

// #include "G4PropParticleTree.hh"
// Geant4 Includes
#include "QGSP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"
#include "globals.hh"
#include "G4RunManager.hh"

#include "g4prop/geant4/PropTrackingAction.hh"
#include "g4prop/geant4/PropSteppingAction.hh"
#include "g4prop/geant4/PropStackingAction.hh"
#include "g4prop/geant4/PropRunAction.hh"
#include "g4prop/geant4/PropDetectorConstruction.hh"
#include "g4prop/geant4/PropPrimaryGeneratorAction.hh"
#include "g4prop/geant4/PropEventAction.hh"
#include "G4ParticleGun.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include <boost/foreach.hpp>

using namespace std;

namespace G4Prop
{
	G4RunManager *runManager = nullptr;
	G4UImanager *uiManager = nullptr;
	G4VisManager *visManager = nullptr;
	CLHEP::HepRandomEngine *randomEngine = nullptr;
	G4double relativeCutoff = 0;

	/// @note @ref ConfigureGeant4() needs to be called before initializing or running Geant4.
	/// @param particleGun The G4ParticleGun to have shot.
	/// @param commandList The UI commands to pass to Geant4.
	/// @param storeTrajectory A G4int specifying how trajectories should be stored. The default indicates the G4PropTrajectory should be used.
	void RunGeant4(G4ParticleGun *particleGun, std::vector<std::string> commandList, G4int storeTrajectory = 3)
	{
		// // For now ignore all args

		// G4VModularPhysicsList *p = new FTFP_BERT(0);
		// p->ReplacePhysics(new G4EmStandardPhysics_option4());

		// auto *o = new G4OpticalPhysics();
		// //	o->SetMaxNumPhotonsPerStep(40);
		// //	o->SetMaxBetaChangePerStep(0.1);
		// //	o->SetTrackSecondariesFirst(kCerenkov,true);

		// p->RegisterPhysics(o);

		// // Unsure if this should be some other engine/seed combo
		// G4Random::setTheEngine(new CLHEP::RanecuEngine);
		// G4Random::setTheSeed(12904);

		// auto *runManager = new G4RunManager;
		// runManager->SetUserInitialization(new PropDetectorConstruction());
		// runManager->SetUserInitialization(p);
		// runManager->SetUserAction(new PropSteppingAction(particleTree));
		// runManager->SetUserAction(new PropStackingAction(particleTree));
		// runManager->SetUserAction(new PropTrackingAction(particleTree));
		// runManager->SetUserAction(new PropRunAction(particleTree));

		runManager->SetUserAction(new PropPrimaryGeneratorAction(particleGun));
		runManager->SetUserAction(new PropSteppingAction(particleGun->GetParticleEnergy() * relativeCutoff));
		runManager->SetUserAction(new PropStackingAction());
		runManager->SetUserAction(new PropTrackingAction(particleGun->GetParticleEnergy() * relativeCutoff, storeTrajectory));
		runManager->SetUserAction(new PropRunAction());
		runManager->SetUserAction(new PropEventAction());
		runManager->SetNumberOfEventsToBeStored(1);

		BOOST_FOREACH (std::string &command, commandList)
		{
			uiManager->ApplyCommand(G4String(command));
			G4cout << command << G4endl;
		}
		// uiManager->ApplyCommand("/run/verbose 3");
		// uiManager->ApplyCommand("/event/verbose 1");
		// uiManager->ApplyCommand("/tracking/verbose 2");
		// uiManager->ApplyCommand("/process/setVerbose 0 all");
		// uiManager->ApplyCommand("/process/verbose 0");
		// uiManager->ApplyCommand("/process/had/verbose 0");
		uiManager->ApplyCommand("/run/initialize");
		uiManager->ApplyCommand("/run/beamOn");
	}

	/// @brief (Re)initializes the G4RunManager and G4VisManager, with a new cutoff.
	/// @param cutoff The fractional cutoff, daughter particles are killed when their kinetic energy fall below the initial particle's kinetic energy times the cutoff value.
	void ConfigureGeant4(G4double cutoff)
	{
		relativeCutoff = cutoff;

		G4cout << "Deleting runManager" << G4endl;
		delete runManager;

		G4cout << "Deleting visManager" << G4endl;
		delete visManager;

		delete randomEngine;

		G4VModularPhysicsList *p = new QGSP_BERT();
		p->ReplacePhysics(new G4EmStandardPhysics_option4());

		G4cout << "Making Optical Physics" << G4endl;

		auto *o = new G4OpticalPhysics();
		G4cout << "Registering Optical Physics" << G4endl;
		// p->RegisterPhysics(o);
		G4cout << "Setting Random Engine" << G4endl;
		// Unsure if this should be some other engine/seed combo

		randomEngine = new CLHEP::RanecuEngine;
		G4Random::setTheEngine(randomEngine);
		G4Random::setTheSeed(12904);

		G4cout << "Making New G4RunManager" << G4endl;
		runManager = new G4RunManager;
		G4cout << "Setting User Initialization" << G4endl;
		runManager->SetUserInitialization(new PropDetectorConstruction());
		runManager->SetUserInitialization(p);
		runManager->Initialize();

		uiManager = G4UImanager::GetUIpointer();

		visManager = new G4VisExecutive;
		visManager->Initialize();
	}

	/// @brief Gets all tracks from an event (if stored).
	/// @return Vector of track pointers, which are deleted when the runManager is deleted
	std::vector<G4Track *> GetTracks()
	{
		return ((PropTrackingAction *)(runManager->GetUserTrackingAction()))->GetTracks();
	}

	/// @brief Gets a reference to the map between particle IDs and the IDs of their secondaries.
	/// @return Map which maps particle IDs to a vector (pointer) containing child particle IDs
	std::map<G4int, boost::shared_ptr<std::vector<G4int>>> &GetTrackToSecondariesMap()
	{
		return ((PropTrackingAction *)(runManager->GetUserTrackingAction()))->GetTrackToSecondariesMap();
	}

	/// @brief Gets all the trajectories (if saved).
	/// @return A vector of pointers to the g4prop-specific trajectory class from the most recent event.
	std::vector<boost::shared_ptr<PropTrajectory>> &GetTrajectories()
	{
		return ((PropEventAction *)(runManager->GetUserEventAction()))->GetTrajectories();
	}
};
namespace G4Prop
{
	// Construct this with the specified CHLEP units
	struct G4PropParticleProperties
	{
		int32_t Id;
		int32_t EncodingPDG;
		double Energy;					 // CLHEP::GeV
		G4ThreeVector Position;			 // CLHEP::mm
		G4ThreeVector MomentumDirection; // Unitless
		double Azimuth;					 // CLHEP::deg
		double Zenith;					 // CLHEP::deg
		double Time;					 // CHLEP::ns
	};

	/// @brief A function that configures a particle to be fired from a Geant4 particle gun.
	/// @param particleToShoot The particle to be shot.
	/// @param particleGun The G4ParticleGun to be loaded.
	void LoadParticleGun(const G4PropParticleProperties &particleToShoot, G4ParticleGun *particleGun)
	{
		auto particle = particleToShoot;
		G4cout << particleToShoot.Id << G4endl;
		G4cout << G4ParticleTable::GetParticleTable()->FindParticle(particleToShoot.EncodingPDG)->GetParticleName() << G4endl;

		G4ParticleGun *source = new G4ParticleGun();
		source->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(particle.EncodingPDG));
		source->SetNumberOfParticles(1);
		source->SetParticleEnergy(particle.Energy);
		source->SetParticleTime(particle.Time);
		// Instead of setting the particle where it should belong w.r.t to the external environment, just put it in the middle of a generic volume.
		source->SetParticlePosition(G4ThreeVector(0, 0, 0));
		source->SetParticleMomentumDirection(particle.MomentumDirection);

		// Assuming particle polarization doesn't matter...
		source->SetParticlePolarization(G4RandomDirection());

		particleGun = source;
	}

	/// @brief A convenient function that configures a list of particles to be fired from Geant4 particle guns.
	/// @param particlesToShoot The list of particles to be shot (note you must construct G4PropParticleProperties yourself),
	/// @param particleGuns A reference to your vector of G4ParticleGun pointers. The new particle guns are appended to this list.
	void LoadParticleGuns(const std::vector<G4PropParticleProperties> particlesToShoot, std::vector<G4ParticleGun *> &particleGuns)
	{
		for (size_t i = 0; i < particlesToShoot.size(); i++)
		{
			G4ParticleGun *source;
			LoadParticleGun(particlesToShoot[i], source);
			particleGuns.insert(particleGuns.end(), source);
		}
	}
}
#endif // G4PROP_HH