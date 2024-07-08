#ifndef I3G4PROPPARTICLETREE_H
#define I3G4PROPPARTICLETREE_H
#include "G4PropParticleTree.hh"

#include "dataclasses/physics/I3MCTree.h"
#include "dataclasses/physics/I3Particle.h"

class I3G4PropParticleTree : public G4PropParticleTree<I3MCTree, I3Particle> {
public:
    I3G4PropParticleTree();
    ~I3G4PropParticleTree();

    void InsertChild(const TreeBase::Tree<I3Particle, I3ParticleID> &parentNode, const I3Particle &child) override;
private:
	I3MCTree fTree;
};
#endif //I3G4PROPPARTICLETREE_H