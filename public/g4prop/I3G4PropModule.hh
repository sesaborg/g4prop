#ifndef I3G4PROPMODULE_HH
#define I3G4PROPMODULE_HH

#include "G4ParticleDefinition.hh"
#include "icetray/I3ConditionalModule.h"
#include "icetray/I3Module.h"
#include "g4prop/G4Prop.hh"
#include <boost/bimap.hpp>

class I3G4PropModule : public I3ConditionalModule
{
public:
    I3G4PropModule(const I3Context &context) : I3ConditionalModule(context)
    {
        uiCommands_ = std::vector<std::string>({
            "/process/optical/cerenkov/setMaxPhotons 20",
            "/process/optical/cerenkov/setMaxBetaChange 40",
            "/tracking/verbose 0",
            "/process/optical/cerenkov/setTrackSecondariesFirst false", // Tracking secondaries first introduces splits tracks into multiple parts with identical IDs, not ideal.
        });
        AddParameter("Geant4Commands",
                     "Commands to pass to the Geant4 UI manager.",
                     uiCommands_);

        reserveLength_ = 10000;
        AddParameter("ReserveLength",
                     "Number of Tracks/Particles to allocate space for at a time in their respective vectors.",
                     reserveLength_);
    }

    virtual ~I3G4PropModule();

    virtual void Configure();

    virtual void Process();

    virtual void Finish();

    void RunGeant4(G4ParticleDefinition[]);

    // Will want an overload that lets users provide a custom physics list.
    void ConfigureGeant4();

    void DAQ(I3FramePtr frame);

    bool ShouldDoProcess(I3FramePtr frame);

private:
    I3G4PropModule();
    I3G4PropModule(const I3G4PropModule &);
    I3G4PropModule &operator=(const I3G4PropModule &);
    bool configured = false;
    std::vector<std::string> uiCommands_;
    size_t reserveLength_;
    // void InsertToTree(boost::shared_ptr<I3MCTree> &, const std::map<G4int, std::vector<G4int>> *, const boost::bimap<I3ParticleID, G4int> *, const std::map<I3ParticleID, I3Particle> *, const I3ParticleID &);
};

#endif // I3G4PROPMODULE_HH
