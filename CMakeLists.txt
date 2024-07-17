i3_project(g4prop
        PYTHON_DIR python
        DOCS_DIR resources/docs
)

SET(LIB_${PROJECT_NAME}_SOURCEFILES

        # Geant4 Files
        private/g4prop/PropDetectorConstruction.cxx
        private/g4prop/PropRunAction.cxx
        private/g4prop/PropStackingAction.cxx
        private/g4prop/PropSteppingAction.cxx
        # private/g4prop/PropTrackingAction.cxx

        # Icetray-specific bindings
        private/I3G4PropParticleTree.cxx
        private/I3G4PropParticleTranslator.cxx
        private/I3G4PropModule.cxx

        # Core g4prop

        # Public Includes
)

SET(LIB_${PROJECT_NAME}_PYBINDINGS_SOURCEFILES
        private/pybindings/module.cxx
)

SET(LIB_${PROJECT_NAME}_PROJECTS
        serialization icetray dataclasses phys-services simclasses clsim
)

SET(LIB_${PROJECT_NAME}_TOOLS python boost clhep geant4)

i3_add_library(${PROJECT_NAME}
        ${LIB_${PROJECT_NAME}_SOURCEFILES}
        USE_TOOLS ${LIB_${PROJECT_NAME}_TOOLS}
        USE_PROJECTS ${LIB_${PROJECT_NAME}_PROJECTS}
)

#i3_add_pybindings(g4prop
#        ${LIB_${PROJECT_NAME}_PYBINDINGS_SOURCEFILES}
#        USE_TOOLS boost python
#        USE_PROJECTS serialization icetray dataclasses g4prop
#)