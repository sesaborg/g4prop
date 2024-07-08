#ifndef I3G4PROPMODULE_HH
#define I3G4PROPMODULE_HH

#include "icetray/I3ConditionalModule.h"

class I3G4PropModule : public I3ConditionalModule {
public:
    I3G4PropModule(const I3Context& context) : I3ConditionalModule(context) {

    }

    virtual ~I3G4PropModule();
    
    virtual void Configure();

    virtual void Process();

    virtual void Finish();
    
//    I3G4PropModule(const I3G4PropModule& prop) {
//
//    }
private:
    I3G4PropModule();
    I3G4PropModule(const I3G4PropModule&);
    I3G4PropModule& operator=(const I3G4PropModule&);
};

#endif //I3G4PROPMODULE_HH
