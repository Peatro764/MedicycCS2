INSERT INTO CycloConfig VALUES('Proton-1', '2019-04-08');
INSERT INTO CycloConfig VALUES('Proton-2', '2020-03-08');

INSERT INTO EquipmentConfig VALUES('Proton-1', 'VDF', 'Dipole_M1', 13.0, '+', TRUE);
INSERT INTO EquipmentConfig VALUES('Proton-1', 'VDF', 'Dipole_M4', 23.0, '+', TRUE);

INSERT INTO EquipmentConfig VALUES('Proton-2', 'VDF', 'Dipole_M1', 28.0, '-', FALSE);
INSERT INTO EquipmentConfig VALUES('Proton-2', 'VDF', 'Dipole_M2', 8.44, '-', TRUE);
INSERT INTO EquipmentConfig VALUES('Proton-2', 'VDF', 'Dipole_M4', 38.0, '+', TRUE);

INSERT INTO BobinePrincipaleLevels VALUES ('Proton-2', 1, 1190, 40);
INSERT INTO BobinePrincipaleLevels VALUES ('Proton-2', 2, 1310, 30);
INSERT INTO BobinePrincipaleLevels VALUES ('Proton-2', 3, 1290, 60);
