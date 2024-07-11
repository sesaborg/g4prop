#ifndef G4PROPPARTICLETREE_H
#define G4PROPPARTICLETREE_H

// An abstract template that (when implemented) lets you insert particles (of your choice) into an iterator-type tree (also of your choice).
// This abstraction helps g4prop pass and receive cascades of particles without caring about the original class of the particle (hence the G4PropParticleTranslator helper class)

#include "G4PropParticleTranslator.hh"

template <typename TreeIterator, typename Particle>
class G4PropParticleTree
{
public:
    virtual void InsertChild(const TreeIterator &parentNode, const Particle &child) = 0;

    // I don't know what class corresponds to a tree in Geant4, for now these are pointless.
    //	virtual void TranslateTreeToG4(const TreeIterator &rootNode) = 0;
    //	virtual TreeIterator TranslateTreeFromG4() = 0;
};
#endif // G4PROPPARTICLETREE_H