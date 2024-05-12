INSERT INTO ProcessMeta VALUES('/home/hofverberg/gitlab/build-MedicycCS2', '/tmp/');

INSERT INTO Library VALUES('ClientConnection',  '/Hardware/ClientConnection/src/', 'libClientConnection.so');
INSERT INTO Library VALUES('NSingle',  '/Hardware/NSingle/src/', 'libNSingle.so');
INSERT INTO Library VALUES('Omron',  '/Hardware/Omron/src/', 'libOmron.so');
INSERT INTO Library VALUES('ADC',  '/Hardware/ADC/src/', 'libADC.so');
INSERT INTO Library VALUES('IOR',  '/Hardware/IOR/src/', 'libIOR.so');
INSERT INTO Library VALUES('SignalGenerator',  '/Hardware/SignalGenerator/src/', 'libSignalGenerator.so');
INSERT INTO Library VALUES('Electrometer',  '/Hardware/Electrometer/src/', 'libElectrometer.so');
INSERT INTO Library VALUES('RadiationMonitor',  '/Hardware/RadiationMonitor/src/', 'libRadiationMonitor.so');
INSERT INTO Library VALUES('SourcePowerSupply', '/Hardware/SourcePowerSupply/src', 'libSourcePowerSupply.so');
INSERT INTO Library VALUES('HFPowerSupplies', '/Hardware/HFPowerSupplies/src', 'libHFPowerSupplies.so');
INSERT INTO Library VALUES('Global',  '/Global/src/', 'libGlobal.so');
INSERT INTO Library VALUES('StandardNSingleSimulator',  '/Simulators/nsingle/standard/', 'libStandardNSingleSimulator.so');
INSERT INTO Library VALUES('MultiplexedNSingleSimulator',  '/Simulators/nsingle/multiplexed/', 'libMultiplexedNSingleSimulator.so');
INSERT INTO Library VALUES('StandardNSingleGroup',  '/MiddleLayer/NSingleGroup/standard/', 'libStandardNSingleGroup.so');
INSERT INTO Library VALUES('InjectionNSingleGroup',  '/MiddleLayer/NSingleGroup/injection/', 'libInjectionNSingleGroup.so');
INSERT INTO Library VALUES('BobinePrincipaleCycler',  '/MiddleLayer/BobinePrincipaleCycler/', 'libBobinePrincipaleCycler.so');
INSERT INTO Library VALUES('MiddleLayerUtils', '/MiddleLayer/Utils', 'libUtils.so');
INSERT INTO Library VALUES('Source', '/MiddleLayer/Source', 'libSource.so');
INSERT INTO Library VALUES('CoolingSystem', '/MiddleLayer/CoolingSystem', 'libCoolingSystem.so');
INSERT INTO Library VALUES('HF', '/MiddleLayer/HF/src/', 'libHF.so');
INSERT INTO Library VALUES('VacuumSystem', '/MiddleLayer/VacuumSystem', 'libVacuumSystem.so');
INSERT INTO Library VALUES('BeamLineState', '/MiddleLayer/BeamLineState', 'libBeamLineState.so');

INSERT INTO Process VALUES('Extraction', '/Launcher/exe/sourcepowersupply/', 'SourcePowerSupply', ARRAY['Extraction'], True, 1, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Extraction', 'SourcePowerSupply');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Extraction', 'Global');

INSERT INTO Process VALUES('Arc', '/Launcher/exe/sourcepowersupply/', 'SourcePowerSupply', ARRAY['Arc'], True, 1, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Arc', 'SourcePowerSupply');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Arc', 'Global');

INSERT INTO Process VALUES('Filament', '/Launcher/exe/sourcepowersupply/', 'SourcePowerSupply', ARRAY['Filament'], True, 1, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Filament', 'SourcePowerSupply');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Filament', 'Global');

INSERT INTO Process VALUES('HFController', '/Launcher/exe/hf/controller/', 'HFController', ARRAY[]::text[], True, 1, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('HFController', 'HF');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('HFController', 'Global');

INSERT INTO Process VALUES('HFPowerSupplies', '/Launcher/exe/hf/powersupplies/', 'HFPowerSupplies', ARRAY[]::text[], True, 1, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('HFPowerSupplies', 'HFPowerSupplies');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('HFPowerSupplies', 'Global');

INSERT INTO Process VALUES('MessageLogger', '/MessageLogger/src/', 'MessageLoggerServer', ARRAY[]::text[], True, 1, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('MessageLogger', 'Global');

INSERT INTO Process VALUES('DataLogger', '/DataLogger/', 'DataLogger', ARRAY[]::text[], True, 1, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('DataLogger', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('DataLogger', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('DataLogger', 'RadiationMonitor');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('DataLogger', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('DataLogger', 'Omron');

INSERT INTO Process VALUES('Barco', '/Launcher/exe/omron/barco/', 'Barco', ARRAY[]::text[], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Barco', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Barco', 'Omron');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Barco', 'Global');

INSERT INTO Process VALUES('OmronVDF', '/Launcher/exe/omron/standard/', 'Omron', ARRAY ['VDF'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('OmronVDF', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('OmronVDF', 'Omron');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('OmronVDF', 'Global');

INSERT INTO Process VALUES('OmronCooling', '/Launcher/exe/omron/standard/', 'Omron', ARRAY ['Cooling'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('OmronCooling', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('OmronCooling', 'Omron');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('OmronCooling', 'Global');

INSERT INTO Process VALUES('OmronSource', '/Launcher/exe/omron/standard/', 'Omron', ARRAY ['Source'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('OmronSource', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('OmronSource', 'Omron');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('OmronSource', 'Global');

INSERT INTO Process VALUES('OmronHF', '/Launcher/exe/omron/standard/', 'Omron', ARRAY ['HF'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('OmronHF', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('OmronHF', 'Omron');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('OmronHF', 'Global');

INSERT INTO Process VALUES('SignalGenerator', '/Launcher/exe/signalgenerator/', 'SignalGenerator', ARRAY[]::text[], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('SignalGenerator', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('SignalGenerator', 'SignalGenerator');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('SignalGenerator', 'Global');

INSERT INTO Process VALUES('ElectrometerStripper', '/Launcher/exe/electrometer/', 'Electrometer', ARRAY ['Stripper'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('ElectrometerStripper', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('ElectrometerStripper', 'Electrometer');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('ElectrometerStripper', 'Global');

INSERT INTO Process VALUES('ADC', '/Launcher/exe/adc/', 'ADC', ARRAY[]::text[], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('ADC', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('ADC', 'ADC');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('ADC', 'Global');

INSERT INTO Process VALUES('IOR', '/Launcher/exe/ior/', 'IOR', ARRAY[]::text[], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('IOR', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('IOR', 'IOR');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('IOR', 'Global');

INSERT INTO Process VALUES('Multiplexed_1', '/Launcher/exe/nsingle/multiplexed/', 'MultiplexedNSingle', ARRAY ['Multiplexed_1'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Multiplexed_1', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Multiplexed_1', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Multiplexed_1', 'Global');

INSERT INTO Process VALUES('Multiplexed_2', '/Launcher/exe/nsingle/multiplexed/', 'MultiplexedNSingle', ARRAY ['Multiplexed_2'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Multiplexed_2', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Multiplexed_2', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Multiplexed_2', 'Global');

INSERT INTO Process VALUES('Dipole_M1', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Dipole_M1'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Dipole_M1', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Dipole_M1', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Dipole_M1', 'Global');

INSERT INTO Process VALUES('Dipole_M2', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Dipole_M2'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Dipole_M2', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Dipole_M2', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Dipole_M2', 'Global');

INSERT INTO Process VALUES('Dipole_M3', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Dipole_M3'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Dipole_M3', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Dipole_M3', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Dipole_M3', 'Global');

INSERT INTO Process VALUES('Dipole_M4', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Dipole_M4'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Dipole_M4', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Dipole_M4', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Dipole_M4', 'Global');

INSERT INTO Process VALUES('Bobine_Harmonique_1', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Bobine_Harmonique_1'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_Harmonique_1', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_Harmonique_1', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_Harmonique_1', 'Global');

INSERT INTO Process VALUES('Bobine_Harmonique_2', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Bobine_Harmonique_2'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_Harmonique_2', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_Harmonique_2', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_Harmonique_2', 'Global');

INSERT INTO Process VALUES('Bobine_Harmonique_3', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Bobine_Harmonique_3'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_Harmonique_3', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_Harmonique_3', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_Harmonique_3', 'Global');

INSERT INTO Process VALUES('Bobine_Harmonique_4', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Bobine_Harmonique_4'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_Harmonique_4', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_Harmonique_4', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_Harmonique_4', 'Global');

INSERT INTO Process VALUES('InflecteurNeg', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['InflecteurNeg'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('InflecteurNeg', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('InflecteurNeg', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('InflecteurNeg', 'Global');

INSERT INTO Process VALUES('InflecteurPos', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['InflecteurPos'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('InflecteurPos', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('InflecteurPos', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('InflecteurPos', 'Global');

INSERT INTO Process VALUES('Injection_33kV', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Injection_33kV'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Injection_33kV', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Injection_33kV', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Injection_33kV', 'Global');

INSERT INTO Process VALUES('Lentille_Inferieure', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Lentille_Inferieure'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Lentille_Inferieure', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Lentille_Inferieure', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Lentille_Inferieure', 'Global');

INSERT INTO Process VALUES('Lentille_Superieure', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Lentille_Superieure'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Lentille_Superieure', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Lentille_Superieure', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Lentille_Superieure', 'Global');

INSERT INTO Process VALUES('Quadrupole_2-H', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Quadrupole_2-H'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_2-H', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_2-H', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_2-H', 'Global');

INSERT INTO Process VALUES('Quadrupole_3-5-H', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Quadrupole_3-5-H'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_3-5-H', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_3-5-H', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_3-5-H', 'Global');

INSERT INTO Process VALUES('Quadrupole_7-9-H', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Quadrupole_7-9-H'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_7-9-H', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_7-9-H', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_7-9-H', 'Global');

INSERT INTO Process VALUES('Quadrupole_1-V', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Quadrupole_1-V'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_1-V', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_1-V', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_1-V', 'Global');

INSERT INTO Process VALUES('Quadrupole_4-V', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Quadrupole_4-V'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_4-V', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_4-V', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_4-V', 'Global');

INSERT INTO Process VALUES('Quadrupole_6-8-V', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Quadrupole_6-8-V'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_6-8-V', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_6-8-V', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Quadrupole_6-8-V', 'Global');

INSERT INTO Process VALUES('Bobine_Principale', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Bobine_Principale'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_Principale', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_Principale', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_Principale', 'Global');

INSERT INTO Process VALUES('Bobine_de_Correction_C09', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Bobine_de_Correction_C09'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_de_Correction_C09', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_de_Correction_C09', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_de_Correction_C09', 'Global');

INSERT INTO Process VALUES('Bobine_de_Correction_C10', '/Launcher/exe/nsingle/standard/', 'StandardNSingle', ARRAY ['Bobine_de_Correction_C10'], True, 2, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_de_Correction_C10', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_de_Correction_C10', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Bobine_de_Correction_C10', 'Global');

INSERT INTO Process VALUES('RadiationMonitor_Salle_RD', '/Launcher/exe/radiationmonitor/', 'RadiationMonitor', ARRAY ['RadiationMonitor_Salle_RD'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('RadiationMonitor_Salle_RD', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('RadiationMonitor_Salle_RD', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('RadiationMonitor_Salle_RD', 'RadiationMonitor');

INSERT INTO Process VALUES('RadiationMonitor_Salle_Clinic', '/Launcher/exe/radiationmonitor/', 'RadiationMonitor', ARRAY ['RadiationMonitor_Salle_Clinic'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('RadiationMonitor_Salle_Clinic', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('RadiationMonitor_Salle_Clinic', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('RadiationMonitor_Salle_Clinic', 'RadiationMonitor');

INSERT INTO Process VALUES('VDF', '/Launcher/exe/nsinglegroup/standard/', 'StandardNSingleGroup', ARRAY['VDF', '3000'], True, 3, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('VDF', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('VDF', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('VDF', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('VDF', 'StandardNSingleGroup');

INSERT INTO Process VALUES('Injection', '/Launcher/exe/nsinglegroup/injection/', 'InjectionNSingleGroup', ARRAY['2000'], True, 3, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Injection', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Injection', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Injection', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Injection', 'StandardNSingleGroup');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Injection', 'InjectionNSingleGroup');

INSERT INTO Process VALUES('Ejection', '/Launcher/exe/nsinglegroup/standard/', 'StandardNSingleGroup', ARRAY['EJECTION', '3000'], True, 3, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Ejection', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Ejection', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Ejection', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Ejection', 'StandardNSingleGroup');

INSERT INTO Process VALUES('BobinesDeCorrection', '/Launcher/exe/nsinglegroup/standard/', 'StandardNSingleGroup', ARRAY['BOBINESDECORRECTION', '10000'], True, 3, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('BobinesDeCorrection', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('BobinesDeCorrection', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('BobinesDeCorrection', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('BobinesDeCorrection', 'StandardNSingleGroup');

INSERT INTO Process VALUES('BobinePrincipaleCycler', '/Launcher/exe/bobineprincipalecycler/', 'BobinePrincipaleCycler', ARRAY[]::text[], True, 3, True);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('BobinePrincipaleCycler', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('BobinePrincipaleCycler', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('BobinePrincipaleCycler', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('BobinePrincipaleCycler', 'BobinePrincipaleCycler');

INSERT INTO Process VALUES('CoolingSystem', '/Launcher/exe/coolingsystem/', 'CoolingSystem', ARRAY[]::text[], True, 3, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('CoolingSystem', 'CoolingSystem');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('CoolingSystem', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('CoolingSystem', 'MiddleLayerUtils');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('CoolingSystem', 'Omron');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('CoolingSystem', 'ClientConnection');

INSERT INTO Process VALUES('BeamLineState', '/Launcher/exe/beamlinestate/', 'BeamLineState', ARRAY[]::text[], True, 3, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('BeamLineState', 'BeamLineState');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('BeamLineState', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('BeamLineState', 'MiddleLayerUtils');

INSERT INTO Process VALUES('VacuumSystem', '/Launcher/exe/vacuumsystem/', 'VacuumSystem', ARRAY[]::text[], True, 3, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('VacuumSystem', 'VacuumSystem');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('VacuumSystem', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('VacuumSystem', 'MiddleLayerUtils');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('VacuumSystem', 'IOR');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('VacuumSystem', 'ClientConnection');

INSERT INTO Process VALUES('Source', '/Launcher/exe/source/', 'Source', ARRAY[]::text[], True, 3, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Source', 'Source');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Source', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Source', 'SourcePowerSupply');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Source', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Source', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Source', 'MiddleLayerUtils');

INSERT INTO Process VALUES('Sim_Injection_33kV', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['Injection_33kV'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Injection_33kV', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Injection_33kV', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Injection_33kV', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Injection_33kV', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_Dipole_M1', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['Dipole_M1'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Dipole_M1', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Dipole_M1', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Dipole_M1', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Dipole_M1', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_Dipole_M2', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['Dipole_M2'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Dipole_M2', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Dipole_M2', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Dipole_M2', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Dipole_M2', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_Dipole_M4', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['Dipole_M4'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Dipole_M4', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Dipole_M4', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Dipole_M4', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Dipole_M4', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_Bobine_Harmonique_1', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['Bobine_Harmonique_1'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_Harmonique_1', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_Harmonique_1', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_Harmonique_1', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_Harmonique_1', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_Bobine_Harmonique_2', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['Bobine_Harmonique_2'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_Harmonique_2', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_Harmonique_2', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_Harmonique_2', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_Harmonique_2', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_InflecteurNeg', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['InflecteurNeg'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_InflecteurNeg', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_InflecteurNeg', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_InflecteurNeg', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_InflecteurNeg', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_InflecteurPos', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['InflecteurPos'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_InflecteurPos', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_InflecteurPos', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_InflecteurPos', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_InflecteurPos', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_Lentille_Inferieure', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['Lentille_Inferieure'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Lentille_Inferieure', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Lentille_Inferieure', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Lentille_Inferieure', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Lentille_Inferieure', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_Lentille_Superieure', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['Lentille_Superieure'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Lentille_Superieure', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Lentille_Superieure', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Lentille_Superieure', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Lentille_Superieure', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_Quadrupole_2-H', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['Quadrupole_2-H'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_2-H', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_2-H', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_2-H', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_2-H', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_Quadrupole_3-5-H', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['Quadrupole_3-5-H'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_3-5-H', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_3-5-H', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_3-5-H', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_3-5-H', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_Quadrupole_7-9-H', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['Quadrupole_7-9-H'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_7-9-H', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_7-9-H', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_7-9-H', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_7-9-H', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_Quadrupole_1-V', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['Quadrupole_1-V'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_1-V', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_1-V', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_1-V', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_1-V', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_Quadrupole_4-V', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['Quadrupole_4-V'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_4-V', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_4-V', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_4-V', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_4-V', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_Quadrupole_6-8-V', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['Quadrupole_6-8-V'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_6-8-V', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_6-8-V', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_6-8-V', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Quadrupole_6-8-V', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_Bobine_Principale', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['Bobine_Principale'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_Principale', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_Principale', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_Principale', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_Principale', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_Bobine_de_Correction_C09', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['Bobine_de_Correction_C09'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_de_Correction_C09', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_de_Correction_C09', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_de_Correction_C09', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_de_Correction_C09', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_Bobine_de_Correction_C10', '/Launcher/exe/nsingle/simulator/standard/', 'StandardNSingleSimulator', ARRAY ['Bobine_de_Correction_C10'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_de_Correction_C10', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_de_Correction_C10', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_de_Correction_C10', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Bobine_de_Correction_C10', 'StandardNSingleSimulator');

INSERT INTO Process VALUES('Sim_Multiplexed_1', '/Launcher/exe/nsingle/simulator/multiplexed/', 'MultiplexedNSingleSimulator', ARRAY ['Multiplexed_1'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Multiplexed_1', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Multiplexed_1', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Multiplexed_1', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Multiplexed_1', 'MultiplexedNSingleSimulator');

INSERT INTO Process VALUES('Sim_Multiplexed_2', '/Launcher/exe/nsingle/simulator/multiplexed/', 'MultiplexedNSingleSimulator', ARRAY ['Multiplexed_2'], True, 2, False);
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Multiplexed_2', 'ClientConnection');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Multiplexed_2', 'NSingle');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Multiplexed_2', 'Global');
INSERT INTO ProcessLibrary(process_id, library_id) VALUES ('Sim_Multiplexed_2', 'MultiplexedNSingleSimulator');

