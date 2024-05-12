include(../MedicycCS2.pri)

SUBDIRS = NSingleGroup BobinePrincipaleCycler Source CoolingSystem Utils SondePrincipale HF VacuumSystem BeamLineState

NSingleGroup.depends = Utils
BobinePrincipaleCycler.depends = Utils
Source.depends = Utils
CoolingSystem.depends = Utils
SondePrincipale.depends = Utils
VacuumSystem.depends = Utils
HF.depends = Utils
BeamLineState.depends = Utils

