#include "g4prop/I3G4PropParticleTranslator.h"
#include "dataclasses/physics/I3Particle.h"

I3Particle I3G4PropParticleTranslator::TranslateParticle(const G4Track &track) {
	I3Particle i3Particle;
	i3Particle.SetPdgEncoding(track.GetDefinition()->GetPDGEncoding());
	return i3Particle;
}
