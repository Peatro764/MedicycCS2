INSERT INTO SourcePowerSupply VALUES(
'Extraction', 'V.Extraction Source', 3000,
0.2, 0.5, 0.5, 0.0, 4.5, 4,
0.2, 0.5, 0.5, 0.0, 4.5, 4,
'kV', 'mA');
INSERT INTO SourcePowerSupply VALUES(
'Arc', 'V.Arc Source', 3000,
10, 30, 40, 0, 200, 4,
10, 30, 40, 0, 200, 4,
'V', 'A');
INSERT INTO SourcePowerSupply VALUES(
'Filament', 'I.Arc Source', 25000,
0.05, 0.3, 0.3, 0.0, 7, 4,
0.05, 0.3, 0.3, 0.0, 6.0, 4,
'V', 'A');

INSERT INTO SourcePowerSupplySetPoint VALUES('Extraction', 'VoieMedicale-1', 4.0, 'V');
INSERT INTO SourcePowerSupplySetPoint VALUES('Injection', 'VoieMedicale-1', 33.0, 'V');
INSERT INTO SourcePowerSupplySetPoint VALUES('Arc', 'VoieMedicale-1', 160, 'V');
INSERT INTO SourcePowerSupplySetPoint VALUES('Arc', 'VoieMedicale-1', 1.0, 'I');

INSERT INTO SourcePowerSupplyDBusAddress VALUES('Extraction', 'medicyc.cyclotron.hardware.sourcepowersupply.extraction', '/PowerSupply');
INSERT INTO SourcePowerSupplyDBusAddress VALUES('Filament', 'medicyc.cyclotron.hardware.sourcepowersupply.filament', '/PowerSupply');
INSERT INTO SourcePowerSupplyDBusAddress VALUES('Arc', 'medicyc.cyclotron.hardware.sourcepowersupply.arc', '/PowerSupply');
