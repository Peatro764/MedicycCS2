INSERT INTO OmronSystem VALUES('VDF');
INSERT INTO OmronSystem VALUES('COOLING');
INSERT INTO OmronSystem VALUES('HF');
INSERT INTO OmronSystem VALUES('SOURCE');

INSERT INTO OmronChannel VALUES('C On Alimentations', 'HF', 3, 'CIO', 146, 0, true);
INSERT INTO OmronChannel VALUES('C Off Alimentations', 'HF', 3, 'CIO', 146, 1, true);
INSERT INTO OmronChannel VALUES('C On Signal', 'HF', 3, 'CIO', 131, 1, true);
INSERT INTO OmronChannel VALUES('C Off Signal', 'HF', 3, 'CIO', 131, 2, true);
INSERT INTO OmronChannel VALUES('C On PreAmpli', 'HF', 3, 'CIO', 130, 12, true);
INSERT INTO OmronChannel VALUES('C Off Partiel PreAmpli', 'HF', 3, 'CIO', 130, 13, true);
INSERT INTO OmronChannel VALUES('C Off Totale PreAmpli', 'HF', 3, 'CIO', 130, 14, true);
INSERT INTO OmronChannel VALUES('C On Ampli', 'HF', 3, 'CIO', 130, 15, true);
INSERT INTO OmronChannel VALUES('C Off Ampli', 'HF', 3, 'CIO', 131, 0, true);

INSERT INTO OmronChannel VALUES('Q HF1 PreAmplis On', 'HF', 3, 'CIO', 140, 13, false);
INSERT INTO OmronChannel VALUES('Q HF1 PreAmplis Off', 'HF', 3, 'CIO', 141, 3, false);
INSERT INTO OmronChannel VALUES('Q HF2 PreAmplis On', 'HF', 3, 'CIO', 140, 14, false);
INSERT INTO OmronChannel VALUES('Q HF2 PreAmplis Off', 'HF', 3, 'CIO', 141, 4, false);
INSERT INTO OmronChannel VALUES('Q HF1-HF2 PreAmplis Defaut', 'HF', 3, 'HR', 3, 1, false);

INSERT INTO OmronChannel VALUES('Q HF1-HF2 Amplis On', 'HF', 3, 'CIO', 142, 10, false);
INSERT INTO OmronChannel VALUES('Q HF1 Amplis Off', 'HF', 3, 'CIO', 142, 3, false);
INSERT INTO OmronChannel VALUES('Q HF1 Alims Ampli Distance', 'HF', 3, 'CIO', 2, 12, false);
INSERT INTO OmronChannel VALUES('Q HF1 Alims Ampli Defaut', 'HF', 3, 'CIO', 2, 13, false);
INSERT INTO OmronChannel VALUES('Q HF2 Amplis Off', 'HF', 3, 'CIO', 142, 4, false);
INSERT INTO OmronChannel VALUES('Q HF2 Alims Ampli Distance', 'HF', 3, 'CIO', 4, 12, false);
INSERT INTO OmronChannel VALUES('Q HF2 Alims Ampli Defaut', 'HF', 3, 'CIO', 4, 13, false);

INSERT INTO OmronChannel VALUES('Q Demarrage Alims en Cours', 'HF', 3, 'CIO', 147, 0, false);
INSERT INTO OmronChannel VALUES('Q Demarrage Alims Termine', 'HF', 3, 'CIO', 147, 1, false);

INSERT INTO OmronChannel VALUES('Q HF1 Alim Grille PreAmpli Pret', 'HF', 3, 'CIO', 2, 2, false);
INSERT INTO OmronChannel VALUES('Q HF1 Alim Grille PreAmpli On', 'HF', 3, 'CIO', 2, 3, false);
INSERT INTO OmronChannel VALUES('Q HF1 Alim Grille PreAmpli Defaut', 'HF', 3, 'CIO', 2, 4, false);
INSERT INTO OmronChannel VALUES('Q HF1 Anode PreAmpli On', 'HF', 3, 'CIO', 2, 6, false);
INSERT INTO OmronChannel VALUES('Q HF1 Anode PreAmpli Defaut', 'HF', 3, 'CIO', 2, 7, false);
INSERT INTO OmronChannel VALUES('Q HF1 Ecran PreAmpli Pret', 'HF', 3, 'CIO', 2, 8, false);
INSERT INTO OmronChannel VALUES('Q HF1 Ecran PreAmpli On', 'HF', 3, 'CIO', 2, 9, false);
INSERT INTO OmronChannel VALUES('Q HF1 Ecran PreAmpli Defaut', 'HF', 3, 'CIO', 2, 10, false);
INSERT INTO OmronChannel VALUES('Q HF1 Alim Grille Ampli Pret', 'HF', 3, 'CIO', 2, 14, false);
INSERT INTO OmronChannel VALUES('Q HF1 Alim Grille Ampli On', 'HF', 3, 'CIO', 2, 15, false);
INSERT INTO OmronChannel VALUES('Q HF1 Alim Grille Ampli Defaut', 'HF', 3, 'CIO', 3, 0, false);
INSERT INTO OmronChannel VALUES('Q HF1 Alim Ecran Ampli Pret', 'HF', 3, 'CIO', 3, 1, false);
INSERT INTO OmronChannel VALUES('Q HF1 Alim Ecran Ampli On', 'HF', 3, 'CIO', 3, 2, false);
INSERT INTO OmronChannel VALUES('Q HF1 Alim Ecran Ampli Defaut', 'HF', 3, 'CIO', 3, 3, false);
INSERT INTO OmronChannel VALUES('Q HF1 Filament Preampli On', 'HF', 3, 'CIO', 3, 4, false);
INSERT INTO OmronChannel VALUES('Q HF1 Filament PreAmpli Defaut', 'HF', 3, 'HR', 2, 3, false);
INSERT INTO OmronChannel VALUES('Q HF1 Filament Ampli On', 'HF', 3, 'CIO', 3, 5, false);
INSERT INTO OmronChannel VALUES('Q HF1 Filament Ampli Defaut', 'HF', 3, 'HR', 2, 5, false);
INSERT INTO OmronChannel VALUES('Q HF2 Filament Preampli On', 'HF', 3, 'CIO', 3, 6, false);
INSERT INTO OmronChannel VALUES('Q HF2 Filament Ampli On', 'HF', 3, 'CIO', 3, 7, false);
INSERT INTO OmronChannel VALUES('Q HF2 Filament PreAmpli Defaut', 'HF', 3, 'HR', 2, 4, false);
INSERT INTO OmronChannel VALUES('Q HF2 Filament Ampli Defaut', 'HF', 3, 'HR', 2, 6, false);

INSERT INTO OmronChannel VALUES('Q Alim THT On', 'HF', 3, 'CIO', 3, 9, false);
INSERT INTO OmronChannel VALUES('Q Alim THT Aux On', 'HF', 3, 'CIO', 3, 8, false);
INSERT INTO OmronChannel VALUES('Q Presence THT sur Ampli1', 'HF', 3, 'CIO', 3, 12, false);
INSERT INTO OmronChannel VALUES('Q Surcharge THT sur Ampli1', 'HF', 3, 'CIO', 3, 13, false);
INSERT INTO OmronChannel VALUES('Q Presence THT sur Ampli2', 'HF', 3, 'CIO', 3, 14, false);
INSERT INTO OmronChannel VALUES('Q Surcharge THT sur Ampli2', 'HF', 3, 'CIO', 3, 15, false);
INSERT INTO OmronChannel VALUES('Q Alim THT Defaut', 'HF', 3, 'HR', 3, 15, false);
INSERT INTO OmronChannel VALUES('Q Alim THT Aux Defaut', 'HF', 3, 'HR', 3, 14, false);

INSERT INTO OmronChannel VALUES('Q HF2 Alims Preampli Distance', 'HF', 3, 'CIO', 4, 0, false);
INSERT INTO OmronChannel VALUES('Q HF2 Alim Grille Preampli Pret', 'HF', 3, 'CIO', 4, 2, false);
INSERT INTO OmronChannel VALUES('Q HF2 Alim Grille Preampli On', 'HF', 3, 'CIO', 4, 3, false);
INSERT INTO OmronChannel VALUES('Q HF2 Alim Grille Preampli Defaut', 'HF', 3, 'CIO', 4, 4, false);
INSERT INTO OmronChannel VALUES('Q HF2 Alim Anode Preampli Pret', 'HF', 3, 'CIO', 4, 5, false);
INSERT INTO OmronChannel VALUES('Q HF2 Alim Anode Preampli On', 'HF', 3, 'CIO', 4, 6, false);
INSERT INTO OmronChannel VALUES('Q HF2 Alim Anode Preampli Defaut', 'HF', 3, 'CIO', 4, 7, false);
INSERT INTO OmronChannel VALUES('Q HF2 Alim Ecran Preampli Pret', 'HF', 3, 'CIO', 4, 8, false);
INSERT INTO OmronChannel VALUES('Q HF2 Alim Ecran Preampli On', 'HF', 3, 'CIO', 4, 9, false);
INSERT INTO OmronChannel VALUES('Q HF2 Alim Ecran Preampli Defaut', 'HF', 3, 'CIO', 4, 10, false);
INSERT INTO OmronChannel VALUES('Q HF2 Alim Grille Ampli Pret', 'HF', 3, 'CIO', 4, 14, false);
INSERT INTO OmronChannel VALUES('Q HF2 Alim Grille Ampli On', 'HF', 3, 'CIO', 4, 15, false);
INSERT INTO OmronChannel VALUES('Q HF2 Alim Grille Ampli Defaut', 'HF', 3, 'CIO', 5, 0, false);
INSERT INTO OmronChannel VALUES('Q HF2 Alim Ecran Ampli Pret', 'HF', 3, 'CIO', 5, 1, false);
INSERT INTO OmronChannel VALUES('Q HF2 Alim Ecran Ampli On', 'HF', 3, 'CIO', 5, 2, false);
INSERT INTO OmronChannel VALUES('Q HF2 Alim Ecran Ampli Defaut', 'HF', 3, 'CIO', 5, 3, false);
INSERT INTO OmronChannel VALUES('Q HF1 Presence V.Grille Preampli', 'HF', 3, 'CIO', 5, 4, false);
INSERT INTO OmronChannel VALUES('Q HF1 Presence V.Grille Ampli', 'HF', 3, 'CIO', 5, 5, false);
INSERT INTO OmronChannel VALUES('Q HF2 Presence V.Grille Preampli', 'HF', 3, 'CIO', 5, 6, false);
INSERT INTO OmronChannel VALUES('Q HF2 Presence V.Grille Ampli', 'HF', 3, 'CIO', 5, 7, false);

INSERT INTO OmronChannel VALUES('Q Dephasage HF1-HF2 ', 'HF', 3, 'CIO', 2, 11, false);

INSERT INTO OmronChannel VALUES('Q Autorisation Niveau On', 'HF', 3, 'CIO', 144, 13, false);
INSERT INTO OmronChannel VALUES('Q HF1 Niveau On', 'HF', 3, 'CIO', 7, 0, false);
INSERT INTO OmronChannel VALUES('Q HF1 Regulation On', 'HF', 3, 'CIO', 7, 1, false);
INSERT INTO OmronChannel VALUES('Q HF1 Pret', 'HF', 3, 'CIO', 7, 2, false);
INSERT INTO OmronChannel VALUES('Q Surveillance Niveau Off', 'HF', 3, 'CIO', 5, 8, false);
INSERT INTO OmronChannel VALUES('C HF1 Niveau On', 'HF', 3, 'CIO', 18, 0, false);
INSERT INTO OmronChannel VALUES('C HF1 Niveau Off', 'HF', 3, 'CIO', 18, 1, false);
INSERT INTO OmronChannel VALUES('C HF1 Up Niveau', 'HF', 3, 'CIO', 18, 2, false);
INSERT INTO OmronChannel VALUES('C HF1 Down Niveau', 'HF', 3, 'CIO', 18, 3, false);
INSERT INTO OmronChannel VALUES('C HF1 On Regulation', 'HF', 3, 'CIO', 18, 4, false);
INSERT INTO OmronChannel VALUES('C HF1 Off Regulation', 'HF', 3, 'CIO', 18, 5, false);
INSERT INTO OmronChannel VALUES('Q HF1 Autorisation Niveau', 'HF', 3, 'CIO', 18, 10, false);
INSERT INTO OmronChannel VALUES('C HF2 Niveau On', 'HF', 3, 'CIO', 19, 0, false);
INSERT INTO OmronChannel VALUES('C HF2 Niveau Off', 'HF', 3, 'CIO', 19, 1, false);
INSERT INTO OmronChannel VALUES('C HF2 Up Niveau', 'HF', 3, 'CIO', 19, 2, false);
INSERT INTO OmronChannel VALUES('C HF2 Down Niveau', 'HF', 3, 'CIO', 19, 3, false);
INSERT INTO OmronChannel VALUES('C HF2 On Regulation', 'HF', 3, 'CIO', 19, 4, false);
INSERT INTO OmronChannel VALUES('C HF2 Off Regulation', 'HF', 3, 'CIO', 19, 5, false);
INSERT INTO OmronChannel VALUES('Q HF2 Autorisation Niveau', 'HF', 3, 'CIO', 19, 10, false);
INSERT INTO OmronChannel VALUES('Q HF1 Pulsation On', 'HF', 3, 'CIO', 7, 4, false);
INSERT INTO OmronChannel VALUES('Q HF1 fc phase A1-G1', 'HF', 3, 'CIO', 7, 5, false);
INSERT INTO OmronChannel VALUES('Q HF2 Niveau On', 'HF', 3, 'CIO', 7, 6, false);
INSERT INTO OmronChannel VALUES('Q HF2 Regulation On', 'HF', 3, 'CIO', 7, 7, false);
INSERT INTO OmronChannel VALUES('Q HF2 Pret', 'HF', 3, 'CIO', 7, 8, false);
INSERT INTO OmronChannel VALUES('Q HF2 Pulsation On', 'HF', 3, 'CIO', 7, 10, false);
INSERT INTO OmronChannel VALUES('Q Pupitre HF distance', 'HF', 3, 'CIO', 7, 14, false);
INSERT INTO OmronChannel VALUES('Q Dephasage 0 deg', 'HF', 3, 'CIO', 7, 15, false);
INSERT INTO OmronChannel VALUES('Q HF2 fc phase A2-G2', 'HF', 3, 'CIO', 7, 11, false);

INSERT INTO OmronChannel VALUES('C HF1 Auto Membrane', 'HF', 3, 'CIO', 18, 6, true);
INSERT INTO OmronChannel VALUES('C HF1 Manu Membrane', 'HF', 3, 'CIO', 18, 7, true);
INSERT INTO OmronChannel VALUES('C HF1 + Membrane', 'HF', 3, 'CIO', 18, 8, false);
INSERT INTO OmronChannel VALUES('C HF1 - Membrane', 'HF', 3, 'CIO', 18, 9, false);
INSERT INTO OmronChannel VALUES('C HF2 Auto Membrane', 'HF', 3, 'CIO', 19, 6, true);
INSERT INTO OmronChannel VALUES('C HF2 Manu Membrane', 'HF', 3, 'CIO', 19, 7, true);
INSERT INTO OmronChannel VALUES('C HF2 + Membrane', 'HF', 3, 'CIO', 19, 8, false);
INSERT INTO OmronChannel VALUES('C HF2 - Membrane', 'HF', 3, 'CIO', 19, 9, false);
INSERT INTO OmronChannel VALUES('Q HF1 Membrane Auto', 'HF', 3, 'CIO', 7, 3, false);
INSERT INTO OmronChannel VALUES('Q HF2 Membrane Auto', 'HF', 3, 'CIO', 7, 9, false);
INSERT INTO OmronChannel VALUES('Q Fin de course membranes', 'HF', 3, 'CIO', 7, 13, false);

INSERT INTO OmronChannel VALUES('Alim Filament ON', 'Source', 3, 'CIO', 5, 9, false);
INSERT INTO OmronChannel VALUES('Alim Arc ON', 'Source', 3, 'CIO', 5, 10, false);
INSERT INTO OmronChannel VALUES('Alim Extraction ON', 'Source', 3, 'CIO', 5, 11, false);
INSERT INTO OmronChannel VALUES('Alims en distance', 'Source', 3, 'CIO', 5, 12, false);

INSERT INTO OmronChannel VALUES('On Filament', 'Source', 3, 'CIO', 131, 3, false);
INSERT INTO OmronChannel VALUES('Off Filament', 'Source', 3, 'CIO', 131, 4, false);
INSERT INTO OmronChannel VALUES('Inc Filament', 'Source', 3, 'CIO', 131, 5, false);
INSERT INTO OmronChannel VALUES('Dec Filament', 'Source', 3, 'CIO', 131, 6, false);
INSERT INTO OmronChannel VALUES('On Arc', 'Source', 3, 'CIO', 131, 7, false);
INSERT INTO OmronChannel VALUES('Off Arc', 'Source', 3, 'CIO', 131, 8, false);
INSERT INTO OmronChannel VALUES('Inc Arc', 'Source', 3, 'CIO', 131, 9, false);
INSERT INTO OmronChannel VALUES('Dec Arc', 'Source', 3, 'CIO', 131, 10, false);
INSERT INTO OmronChannel VALUES('On Extraction', 'Source', 3, 'CIO', 131, 11, false);
INSERT INTO OmronChannel VALUES('Off Extraction', 'Source', 3, 'CIO', 131, 12, false);
INSERT INTO OmronChannel VALUES('Inc Extraction', 'Source', 3, 'CIO', 131, 13, false);
INSERT INTO OmronChannel VALUES('Dec Extraction', 'Source', 3, 'CIO', 131, 14, false);

INSERT INTO OmronChannel VALUES('Q CF9-2 IN', 'VDF', 1, 'CIO', 15, 14, false);
INSERT INTO OmronChannel VALUES('Q CF9-2 OUT', 'VDF', 1, 'CIO', 15, 15, false);
INSERT INTO OmronChannel VALUES('Q QZ10 IN', 'VDF', 1, 'CIO', 15, 8, false);
INSERT INTO OmronChannel VALUES('Q QZ10 OUT', 'VDF', 1, 'CIO', 15, 9, false);
INSERT INTO OmronChannel VALUES('Q CF10 IN', 'VDF', 1, 'CIO', 9, 14, false);
INSERT INTO OmronChannel VALUES('Q CF10 OUT', 'VDF', 1, 'CIO', 9, 15, false);
INSERT INTO OmronChannel VALUES('Q Guillotine Ouverte', 'VDF', 1, 'CIO', 19, 8, false);
INSERT INTO OmronChannel VALUES('Q Diaphragme Proton', 'VDF', 1, 'CIO', 15, 1, false);

INSERT INTO OmronChannel VALUES('Q Eletta Dee HF1', 'Cooling', 3, 'CIO', 0, 5, false);
INSERT INTO OmronChannel VALUES('Q Eletta BC Superieures', 'Cooling', 3, 'CIO', 0, 3, false);
INSERT INTO OmronChannel VALUES('Q Eletta Claviers HF2', 'Cooling', 3, 'CIO', 0, 9, false);
INSERT INTO OmronChannel VALUES('Q Eletta Tubes HF2', 'Cooling', 3, 'CIO', 0, 10, false);
INSERT INTO OmronChannel VALUES('Q Eletta Commun HF', 'Cooling', 3, 'CIO', 0, 11, false);
INSERT INTO OmronChannel VALUES('Q Eletta Source-Injax', 'Cooling', 3, 'CIO', 0, 12, false);
INSERT INTO OmronChannel VALUES('Q Eletta BC Inferieures', 'Cooling', 3, 'CIO', 0, 4, false);
INSERT INTO OmronChannel VALUES('Q Eletta BP Inferieure', 'Cooling', 3, 'CIO', 0, 2, false);
INSERT INTO OmronChannel VALUES('Q Eletta BP Superieure', 'Cooling', 3, 'CIO', 0, 1, false);
INSERT INTO OmronChannel VALUES('Q Eletta Claviers HF1', 'Cooling', 3, 'CIO', 0, 6, false);
INSERT INTO OmronChannel VALUES('Q Eletta Tubes HF1', 'Cooling', 3, 'CIO', 0, 7, false);
INSERT INTO OmronChannel VALUES('Q Eletta Dee HF2', 'Cooling', 3, 'CIO', 0, 8, false);
INSERT INTO OmronChannel VALUES('Q Eletta Alims BP-BC-BH', 'Cooling', 3, 'CIO', 0, 14, false);
INSERT INTO OmronChannel VALUES('Q Eletta Alim Lentille', 'Cooling', 3, 'CIO', 0, 15, false);
INSERT INTO OmronChannel VALUES('Q Eletta Ejection Sonde Princ.', 'Cooling', 3, 'CIO', 0, 13, false);
INSERT INTO OmronChannel VALUES('Q Eletta Eau VIDE', 'Cooling', 3, 'CIO', 1, 3, false);
INSERT INTO OmronChannel VALUES('Q Eletta Diag-Aimants VdF', 'Cooling', 3, 'CIO', 1, 4, false);
INSERT INTO OmronChannel VALUES('Q Eletta Alims VdF', 'Cooling', 3, 'CIO', 1, 5, false);





