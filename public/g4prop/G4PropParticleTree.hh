// #ifndef G4PROPPARTICLETREE_H
// #define G4PROPPARTICLETREE_H

// // An abstract template that (when implemented) lets you insert particles (of your choice) into an iterator-type tree (also of your choice).
// // This abstraction helps g4prop pass and receive cascades of particles without caring about the original class of the particle (hence the G4PropParticleTranslator helper class)

// #include "G4PropParticleTranslator.hh"

// class G4PropParticleTree
// {
// public:
//     virtual G4PropParticleTree();
//     virtual ~G4PropParticleTree();
//     virtual void AppendChild(const G4int parentId, const G4Track *child) = 0;
// };
// #endif // G4PROPPARTICLETREE_H