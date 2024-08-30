# g4prop
An implementation of Geant4 that is primarily useful for high energy shower development. Currently, this repository has the IceTray-specific project structure and code required to make g4prop an IceTray module. However, the underlying code is completely independent of Icetray, and is planned to be used in the future simulation framework for P-ONE. For more reference, take a look at [resources/docs/index.rst](https://github.com/sesaborg/g4prop/blob/80fbe299c9fa74cda40d27d3a4b07a3cb1c4addf/resources/docs/index.rst).

## Warning
g4prop is still under development and requires a significant amount of work before it is ready to be used in simulations.

## Using g4prop
To used g4prop, you need to have Geant4 installed (and for use in IceTray, you will need to insert this repository into the list of IceTray modules and build everything yourself).

As a standalone project, you will need to call the following functions:

```c++
G4Prop::ConfigureGeant4(G4double cutoff);
G4Prop::RunGeant4(G4ParticleGun *particleGun, std::vector<std::string> commandList, G4int storeTrajectory);
```
Configuring Geant4 through g4prop takes a relative cutoff parameter, which is the fraction between a primary particle's energy and the minimum kinetic energy allowed during tracking before the particle track is stopped and frozen. To run g4prop's Geant4, you need to pass it a G4ParticleGun pointer. You can configure this from an output particle of an event generator, like Pythia, for example. The commandList is a list of commands to be passed to Geant4's ui (see the Geant4 documentation for all the different options). Finally, storeTrajectory is an enumeration that configures g4prop to either store trajectories (recommended, default = 3) or store raw tracks (not recommended, = 0).

If you are using the IceTray framework, adding g4prop as a module can look like this:
```python
tray.AddModule("g4prop",
Geant4Commands = ["/process/optical/cerenkov/setMaxPhotons 5",
            "/process/optical/cerenkov/setMaxBetaChange 20",
            "/tracking/verbose 0",
            "/run/verbose 0",
            "/process/optical/cerenkov/setTrackSecondariesFirst false",
            "/tracking/storeTrajectory 1",
            ],
            RelativeCutoff = 0.01)
```
