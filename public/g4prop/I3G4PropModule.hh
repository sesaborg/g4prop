#ifndef I3G4PROPMODULE_HH
#define I3G4PROPMODULE_HH

#include "icetray/I3ConditionalModule.h"
#include "icetray/I3Module.h"
#include "g4prop/geant4/G4Prop.hh"
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

        relativeCutoff_ = 0.001;
        AddParameter("RelativeCutoff",
                     "The ratio between the particle cutoff energy and the initial particle energy.",
                     relativeCutoff_);

        reserveLength_ = 10000;
        AddParameter("ReserveLength",
                     "Number of Tracks/Particles to allocate space for at a time in their respective vectors.",
                     reserveLength_);

        AddOutBox("OutBox");
    }

    virtual ~I3G4PropModule();

    virtual void Configure();

    // virtual void Process();

    // virtual void Finish();

    // Will want an overload that lets users provide a custom physics list.
    // void ConfigureGeant4();

    void DAQ(I3FramePtr frame);

    bool ShouldDoProcess(I3FramePtr frame);

private:
    I3G4PropModule();
    I3G4PropModule(const I3G4PropModule &);
    I3G4PropModule &operator=(const I3G4PropModule &);
    bool configured = false;
    std::vector<std::string> uiCommands_;
    size_t reserveLength_;
    double relativeCutoff_;
    // void InsertToTree(boost::shared_ptr<I3MCTree> &, const std::map<G4int, std::vector<G4int>> *, const boost::bimap<I3ParticleID, G4int> *, const std::map<I3ParticleID, I3Particle> *, const I3ParticleID &);
};

#endif // I3G4PROPMODULE_HH
