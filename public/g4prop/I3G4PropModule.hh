#ifndef I3G4PROPMODULE_HH
#define I3G4PROPMODULE_HH

#include "G4ParticleDefinition.hh"
#include "icetray/I3ConditionalModule.h"
#include "icetray/I3Module.h"
#include "g4prop/G4Prop.hh"

class I3G4PropModule : public I3ConditionalModule
{
public:
    I3G4PropModule(const I3Context &context) : I3ConditionalModule(context)
    {
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
};

#endif // I3G4PROPMODULE_HH
