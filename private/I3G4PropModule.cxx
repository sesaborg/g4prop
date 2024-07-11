#include "dataclasses/physics/I3MCTree.h"
#include "dataclasses/physics/I3MCTreeUtils.h"
#include "g4prop/G4Prop.hh"
#include "icetray/I3Module.h"
#include "g4prop/I3G4PropModule.hh"
#include "G4ParticleDefinition.hh"
#include <boost/foreach.hpp>
#include "G4SystemOfUnits.hh"
#include "g4prop/I3CLSimI3ParticleGeantConverter.hh"

typedef I3G4PropModule g4prop;
namespace
{
    I3_MODULE(g4prop);
};

I3G4PropModule::~I3G4PropModule()
{
    log_info("Destroying g4prop.");
}

void I3G4PropModule::Configure()
{
    log_info("Configuring g4prop.");
    if (!configured)
    {
        ConfigureGeant4();
        configured = true;
        log_info("g4prop's Geant4 has been configured.");
    }
    else
    {
        log_warn("g4prop's Geant4 has already been configured!");
    }
}

void I3G4PropModule::Finish()
{
    log_info("g4prop has finished");
}

void I3G4PropModule::Process()
{
    log_info("g4prop Process()");
    I3FramePtr frame = PopFrame();
    if (!frame)
        I3FramePtr frame(new I3Frame(I3Frame::DAQ));

    log_info("I3Vector<double> test");
    auto test = new I3VectorDouble(std::size_t(4));
    (*test)[0] = 214.2;
    (*test)[1] = 242.0;
    (*test)[2] = 04.2;
    (*test)[3] = 1.1;
    log_info("I3Vector<double> test Put");
    frame->Put("TestVector", I3VectorDoublePtr(test));

    if (frame->GetStop() == I3Frame::DAQ)
    {
        DAQ(frame);
        return;
    }

    PushFrame(frame);
    return;
}

void I3G4PropModule::ConfigureGeant4()
{
    log_info("Configuring Geant4");
    G4Prop::ConfigureGeant4(0.001);
}

void I3G4PropModule::RunGeant4(G4ParticleDefinition particleList[])
{
    log_info("Trying to run Geant4");
}

void I3G4PropModule::DAQ(I3FramePtr frame)
{
    log_info("g4prop DAQ");
    I3MCTreeConstPtr i3MCTree = frame->Get<I3MCTreeConstPtr>();
    if (!i3MCTree)
    {
        PushFrame(frame);
        return;
    }

    std::vector<G4Prop::G4PropParticleProperties> particlesToShoot;

    log_info("I3MCTree has %zu nodes. It's children are:", i3MCTree->size());
    auto iter = i3MCTree->begin();
    for (size_t i = 0; i < i3MCTree->size(); i++)
    {
        auto &particle = *iter;
        log_info(" -- %i  %s  (%f m, %f m, %f m)  (%f deg, %f deg)  %f ns  %f GeV  %f m  %f MeV/c^2  %i",
                 particle.GetMinorID(),
                 particle.GetTypeString().c_str(),
                 particle.GetPos().GetX() / I3Units::mm,
                 particle.GetPos().GetY() / I3Units::mm,
                 particle.GetPos().GetZ() / I3Units::mm,
                 particle.GetAzimuth() / I3Units::deg,
                 particle.GetZenith() / I3Units::deg,
                 particle.GetTime() / I3Units::ns,
                 particle.GetEnergy() / I3Units::GeV,
                 particle.GetLength() / I3Units::m,
                 particle.HasMass() ? particle.GetMass() / (I3Units::MeV / (I3Constants::c * I3Constants::c)) : NAN,
                 particle.GetPdgEncoding());

        if (configured)
        {
            G4ParticleDefinition *geant4Particle = G4ParticleTable::GetParticleTable()->FindParticle(particle.GetPdgEncoding());
            if (geant4Particle != 0)
            {
                log_info(" -- In Geant4: %s", geant4Particle->GetParticleName().c_str());
                particlesToShoot.insert(particlesToShoot.end(), (G4Prop::G4PropParticleProperties){
                                                                    particle.GetMinorID(),
                                                                    particle.GetPdgEncoding(),
                                                                    particle.GetEnergy() / I3Units::GeV * CLHEP::GeV,
                                                                    G4ThreeVector(particle.GetPos().GetX() / I3Units::mm * CLHEP::mm,
                                                                                  particle.GetPos().GetY() / I3Units::mm * CLHEP::mm,
                                                                                  particle.GetPos().GetZ() / I3Units::mm * CLHEP::mm),
                                                                    G4ThreeVector(cos(particle.GetAzimuth() / I3Units::rad) * cos(particle.GetZenith() / I3Units::rad),
                                                                                  sin(particle.GetAzimuth() / I3Units::rad) * cos(particle.GetZenith() / I3Units::rad),
                                                                                  sin(particle.GetZenith() / I3Units::rad)),
                                                                    particle.GetAzimuth() / I3Units::deg * CLHEP::deg,
                                                                    particle.GetZenith(),
                                                                    particle.GetTime()});
            }
            else
            {
                log_warn(" -- In Geant4: Unsupported PDG!");
            }
        }

        iter++;
    }

    std::vector<G4ParticleGun *> particleGuns;
    G4Prop::LoadParticleGuns(particlesToShoot, particleGuns);

    for (size_t i = 0; i < particleGuns.size(); i++)
    {
        G4Prop::RunGeant4(particleGuns[i]);
        G4Prop::runManager->GetPreviousEvent()
    }

    log_info("Pushing DAQ Frame.");
    PushFrame(frame);
    return;
}

bool I3G4PropModule::ShouldDoProcess(I3FramePtr frame)
{
    return true;
}