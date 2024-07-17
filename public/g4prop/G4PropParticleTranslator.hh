// #ifndef G4PROPPARTICLETRANSLATOR_H
// #define G4PROPPARTICLETRANSLATOR_H
// #include "G4Track.hh"

// // A barebones template for a framework-independent particle translator.
// // To implement a particle translator, declare a class that inherits from G4PropParticleTranslator<Particle>, where Particle is your framework's particle class.
// // Then declare an override to TranslateParticle, like so:
// //
// //    I3Particle TranslateParticle(const G4Track &track) override;
// //
// // All that's left is to define your custom conversion between a G4Track and your framework's particle class.
// template <typename Particle> class G4PropParticleTranslator {
// public:
// 	virtual Particle TranslateParticle(const G4Track &track) = 0;
// };
// #endif //G4PROPPARTICLETRANSLATOR_H