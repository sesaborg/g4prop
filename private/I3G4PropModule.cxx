#include "g4prop/G4Prop.hh"
#include "icetray/I3Module.h"
#include "g4prop/I3G4PropModule.hh"

typedef I3G4PropModule g4prop;
namespace { ::I3Registrator<I3Module, g4prop, StandardCreate> const& BOOST_PP_CAT(I3Registrator_, __LINE__) = I3::Singleton<I3Registrator<I3Module, g4prop, StandardCreate> > ::get_mutable_instance().key_register(BOOST_PP_STRINGIZE(g4prop), BOOST_PP_STRINGIZE(PROJECT)); };

I3G4PropModule::~I3G4PropModule() {

}

void I3G4PropModule::Configure() {

}

void I3G4PropModule::Finish() {

}

void I3G4PropModule::Process() {

}
