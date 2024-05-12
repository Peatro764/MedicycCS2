INSERT INTO NSingle VALUES('Bobine_Principale', 'BP', true, 'Digital', 'BP', 'BOBINEPRINCIPALE', 'Uni', 'Low', 'AbsoluteValues', 'ON', '172.20.216.131', 1470, false, false, 0, 14, 15, 0.0367779, 1, 13, 14, 0.05, 100, 30, false, false, false, true, true, false, 14, 1200.0, 1000, 3000, 6, 2, 30000, 30000, 220000, 40000, 220000, 1000, 20, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_Principale', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_Principale', '+0', '+1', '+2', '+3', '-3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_Principale', 'medicyc.cyclotron.nsingle.Bobine_Principale', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Bobine_de_Correction_C09', 'BC09', true, 'Digital', 'VDF', 'BOBINESDECORRECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.134', 1470, false, false, 0, 11, 15, 0.07323975, 0, 11, 15, 0.07326032, 30, 15, true, false, false, true, true, false, 3, 300.0, 1000, 3000, 6, 1, 30000, 30000, 30000, 30000, 30000, 1000, 20, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_de_Correction_C09', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_de_Correction_C09', '+0', '', '+2', '+3', '-3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_de_Correction_C09', 'medicyc.cyclotron.nsingle.Bobine_de_Correction_C09', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Multiplexed_1', 'Multiplexed_1', true, 'Digital', 'Basic', 'OTHER', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.114', 1470, false, false, 5, 14, 15, 0.003052, 5, 14, 15, 0.003052, 300, 200, true, false, false, true, true, false, 100, 100.0, 10000, 10000, 5, 2, 30000, 30000, 30000, 30000, 30000, 1000, 20, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Multiplexed_1', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Multiplexed_1', '+0', '', '+2', '-3', '+3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Multiplexed_1', 'medicyc.cyclotron.nsingle.Multiplexed_1', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Bobine_de_Correction_C00', 'BC 00', true, 'Digital', 'Basic', 'BOBINESDECORRECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.114:0', 1470, true, false, 5, 14, 15, 0.0031125, 5, 14, 15, 0.003051948, 300, 200, true, false, false, true, true, false, 100, 30.0, 10000, 10000, 5, 2, 30000, 30000, 30000, 30000, 30000, 1000, 20, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_de_Correction_C00', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_de_Correction_C00', '+0', '', '+2', '-3', '+3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_de_Correction_C00', 'medicyc.cyclotron.nsingle.Multiplexed_1', '/Controller/Bobine_de_Correction_C00', '/NSingle/Multiplexed_1');

INSERT INTO NSingle VALUES('Bobine_de_Correction_C01', 'BC 01', true, 'Digital', 'Basic', 'BOBINESDECORRECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.114:1', 1470, true, false, 5, 14, 15, 0.0031439, 5, 14, 15, 0.00305147, 300, 200, true, false, false, true, true, false, 100, 30.0, 10000, 10000, 5, 2, 30000, 30000, 30000, 30000, 30000, 1000, 20, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_de_Correction_C01', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_de_Correction_C01', '+0', '', '+2', '-3', '+3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_de_Correction_C01', 'medicyc.cyclotron.nsingle.Multiplexed_1', '/Controller/Bobine_de_Correction_C01', '/NSingle/Multiplexed_1');

INSERT INTO NSingle VALUES('Bobine_de_Correction_C02', 'BC 02', false, 'Digital', 'Basic', 'BOBINESDECORRECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.114:2', 1470, true, false, 5, 14, 15, 0.0031439, 5, 14, 15, 0.00305147, 300, 200, true, false, false, true, true, false, 100, 30.0, 10000, 10000, 5, 2, 30000, 30000, 30000, 30000, 30000, 1000, 20, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_de_Correction_C02', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_de_Correction_C02', '+0', '', '+2', '-3', '+3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_de_Correction_C02', 'medicyc.cyclotron.nsingle.Multiplexed_1', '/Controller/Bobine_de_Correction_C02', '/NSingle/Multiplexed_1');

INSERT INTO NSingleMultiplexConfig VALUES('Bobine_de_Correction_C00', 'Multiplexed_1', 1);
INSERT INTO NSingleMultiplexConfig VALUES('Bobine_de_Correction_C01', 'Multiplexed_1', 2);
INSERT INTO NSingleMultiplexConfig VALUES('Bobine_de_Correction_C02', 'Multiplexed_1', 2);

INSERT INTO CycloConfig VALUES('Proton-1', '2019-04-08');
INSERT INTO CycloConfig VALUES('Proton-2', '2020-04-08');
INSERT INTO CycloConfig VALUES('Proton-3', '2018-04-08');

INSERT INTO NSingleSetPoint VALUES('Bobine_Principale', 'Proton-1', TRUE, '+', 1130.0);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C00', 'Proton-1', TRUE, '+', 13.0);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C01', 'Proton-1', TRUE, '+', 23.0);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C09', 'Proton-1', TRUE, '+', 887.0);

INSERT INTO NSingleSetPoint VALUES('Bobine_Principale', 'Proton-2', TRUE, '+', 1130.0);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C00', 'Proton-2', TRUE, '-',28.0);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C02', 'Proton-2', FALSE, '-', 0.0);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C01', 'Proton-2', TRUE, '-', 8.44);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C09', 'Proton-2', TRUE, '+', 38.0);

INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C09', 'Proton-3', TRUE, '+', 38.0);

INSERT INTO BPCycleLevel VALUES (1, 1190, 40);
INSERT INTO BPCycleLevel VALUES (2, 1310, 30);
INSERT INTO BPCycleLevel VALUES (3, 1290, 60);
