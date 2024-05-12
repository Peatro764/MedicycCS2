INSERT INTO NSingle VALUES('Bobine_Principale', 'BP', true, 'Digital', 'BP', 'BOBINEPRINCIPALE', 'Uni', 'Low', 'AbsoluteValues', 'ON', '172.20.216.131', 1470, false, false, 0, 14, 15, 0.0367779, 0, 14, 15, 0.0367169, 100, 30, false, false, false, true, true, false, 18, 1200.0, 1000, 3000, 6, 2, 30000, 30000, 220000, 40000, 220000, 1, 30, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_Principale', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_Principale', '+0', '+1', '+2', '+3', '-3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_Principale', 'medicyc.cyclotron.nsingle.Bobine_Principale', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Bobine_de_Correction_C09', 'BC09', true, 'Digital', 'VDF', 'BOBINESDECORRECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.134', 1470, false, false, 0, 11, 15, 0.07323975, 0, 11, 15, 0.07326032, 30, 15, true, false, false, true, true, false, 3, 300.0, 1000, 3000, 6, 1, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_de_Correction_C09', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_de_Correction_C09', '+0', '', '+2', '+3', '-3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_de_Correction_C09', 'medicyc.cyclotron.nsingle.Bobine_de_Correction_C09', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Bobine_de_Correction_C10', 'BC10', true, 'Digital', 'VDF', 'BOBINESDECORRECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.135', 1470, false, false, 0, 11, 15, 0.07333564, 0, 11, 15, 0.0732847,  30, 15, true, false, false, true, true, false, 3, 300.0, 1000, 3000, 6, 1, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_de_Correction_C10', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_de_Correction_C10', '+0', '', '+2', '+3', '-3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_de_Correction_C10', 'medicyc.cyclotron.nsingle.Bobine_de_Correction_C10', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Dipole_M4', 'Dipole M4', true, 'Digital', 'VDF', 'VDF', 'Bi', 'Low', 'AbsoluteValues', 'ON', '172.20.216.111', 1470, false, false, 0, 11, 15, 0.07335453, 0, 11, 15, 0.0732846, 30, 15, true, false, false, true, true, false, 3, 300.0, 1000, 3000, 6, 1, 60000, 60000, 30000, 60000, 30000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Dipole_M4', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Dipole_M4', '+0', '', '+2', '+3', '-3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Dipole_M4', 'medicyc.cyclotron.nsingle.Dipole_M4', '/Controller', '/NSingle');

/*
INSERT INTO NSingle VALUES('Dipole_M3', 'Dipole M3', false, 'Digital', 'VDF', 'VDF', 'Bi', 'Low', 'AbsoluteValues', 'ON', '172.20.216.119', 1470, false, false, 0, 11, 15, 0.07326, 0, 11, 15, 0.07326, 30, 15, true, false, false, true, true, false, 3, 300.0, 1000, 3000, 6, 1, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Dipole_M3', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Dipole_M3', '+0', '', '+2', '+3', '-3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Dipole_M3', 'medicyc.cyclotron.nsingle.Dipole_M3', '/Controller', '/NSingle');
*/

INSERT INTO NSingle VALUES('Dipole_M2', 'Dipole M2', false, 'Digital', 'VDF', 'VDF', 'Bi', 'Low', 'AbsoluteValues', 'ON', '172.20.216.118', 1470, false, false, 0, 11, 15, 0.07326, 0, 11, 15, 0.07326, 30, 15, true, false, false, true, true, false, 3, 300.0, 1000, 3000, 6, 1, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Dipole_M2', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Dipole_M2', '+0', '', '+2', '+3', '-3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Dipole_M2', 'medicyc.cyclotron.nsingle.Dipole_M2', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Dipole_M1', 'Dipole M1', true, 'Digital', 'VDF', 'VDF', 'Bi', 'Low', 'AbsoluteValues', 'ON', '172.20.216.110', 1470, false, false, 0, 11, 15, 0.07335220, 0, 11, 15, 0.07326005, 30, 15, true, false, false, true, true, false, 3, 300.0, 1000, 3000, 6, 1, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Dipole_M1', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Dipole_M1', '+0', '', '+2', '+3', '-3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Dipole_M1', 'medicyc.cyclotron.nsingle.Dipole_M1', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Multiplexed_1', 'Multiplexed_1', true, 'Digital', 'Basic', 'OTHER', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.114', 1470, false, false, 5, 14, 15, 0.003052, 5, 14, 15, 0.003052, 300, 200, true, false, false, true, true, false, 100, 100.0, 10000, 10000, 5, 2, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Multiplexed_1', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Multiplexed_1', '+0', '', '+2', '-3', '+3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Multiplexed_1', 'medicyc.cyclotron.nsingle.Multiplexed_1', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Bobine_de_Correction_C00', 'BC 00', true, 'Digital', 'Basic', 'BOBINESDECORRECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.114:0', 1470, true, false, 5, 14, 15, 0.0031125, 5, 14, 15, 0.003051948, 300, 200, true, false, false, true, true, false, 100, 30.0, 10000, 10000, 5, 2, 40000, 80000, 60000, 40000, 60000, 1, 5, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_de_Correction_C00', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_de_Correction_C00', '+0', '', '+2', '-3', '+3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_de_Correction_C00', 'medicyc.cyclotron.nsingle.Multiplexed_1', '/Controller/Bobine_de_Correction_C00', '/NSingle/Bobine_de_Correction_C00');

INSERT INTO NSingle VALUES('Bobine_de_Correction_C01', 'BC 01', true, 'Digital', 'Basic', 'BOBINESDECORRECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.114:1', 1470, true, false, 5, 14, 15, 0.0031439, 5, 14, 15, 0.00305147, 300, 200, true, false, false, true, true, false, 100, 30.0, 10000, 10000, 5, 2, 40000, 80000, 60000, 40000, 60000, 1, 5, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_de_Correction_C01', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_de_Correction_C01', '+0', '', '+2', '-3', '+3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_de_Correction_C01', 'medicyc.cyclotron.nsingle.Multiplexed_1', '/Controller/Bobine_de_Correction_C01', '/NSingle/Bobine_de_Correction_C01');

INSERT INTO NSingle VALUES('Bobine_de_Correction_C02', 'BC 02', true, 'Digital', 'Basic', 'BOBINESDECORRECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.114:2', 1470, true, false, 5, 14, 15, 0.002968, 5, 14, 15, 0.003053571, 300, 200, true, false, false, true, true, false, 100, 30.0, 10000, 10000, 5, 2, 40000, 80000, 60000, 40000, 60000, 1, 5, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_de_Correction_C02', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_de_Correction_C02', '+0', '', '+2', '-3', '+3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_de_Correction_C02', 'medicyc.cyclotron.nsingle.Multiplexed_1', '/Controller/Bobine_de_Correction_C02', '/NSingle/Bobine_de_Correction_C02');

INSERT INTO NSingle VALUES('Bobine_de_Correction_C03', 'BC 03', true, 'Digital', 'Basic', 'BOBINESDECORRECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.114:3', 1470, true, false, 5, 14, 15, 0.0030695, 5, 14, 15, 0.003051470, 300, 200, true, false, false, true, true, false, 100, 50.0, 10000, 10000, 5, 2, 40000, 80000, 60000, 40000, 60000, 1, 5, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_de_Correction_C03', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_de_Correction_C03', '+0', '', '+2', '-3', '+3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_de_Correction_C03', 'medicyc.cyclotron.nsingle.Multiplexed_1', '/Controller/Bobine_de_Correction_C03', '/NSingle/Bobine_de_Correction_C03');

INSERT INTO NSingle VALUES('Bobine_de_Correction_C04', 'BC 04', true, 'Digital', 'Basic', 'BOBINESDECORRECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.114:4', 1470, true, false, 5, 14, 15, 0.003105, 5, 14, 15, 0.003052226, 300, 200, true, false, false, true, true, false, 100, 50.0, 10000, 10000, 5, 2, 40000, 80000, 60000, 40000, 60000, 1, 5, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_de_Correction_C04', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_de_Correction_C04', '+0', '', '+2', '-3', '+3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_de_Correction_C04', 'medicyc.cyclotron.nsingle.Multiplexed_1', '/Controller/Bobine_de_Correction_C04', '/NSingle/Bobine_de_Correction_C04');

INSERT INTO NSingle VALUES('Bobine_de_Correction_C05', 'BC 05', true, 'Digital', 'Basic', 'BOBINESDECORRECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.114:5', 1470, true, false, 5, 14, 15, 0.003098, 5, 14, 15, 0.003051609, 300, 200, true, false, false, true, true, false, 100, 50.0, 10000, 10000, 5, 2, 40000, 80000, 60000, 40000, 60000, 1, 5, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_de_Correction_C05', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_de_Correction_C05', '+0', '', '+2', '-3', '+3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_de_Correction_C05', 'medicyc.cyclotron.nsingle.Multiplexed_1', '/Controller/Bobine_de_Correction_C05', '/NSingle/Bobine_de_Correction_C05');

INSERT INTO NSingle VALUES('Bobine_de_Correction_C06', 'BC 06', true, 'Digital', 'Basic', 'BOBINESDECORRECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.114:6', 1470, true, false, 5, 14, 15, 0.00308668, 5, 14, 15, 0.0030516098, 300, 200, true, false, false, true, true, false, 140, 50.0, 10000, 10000, 5, 2, 40000, 80000, 60000, 40000, 60000, 1, 5, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_de_Correction_C06', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_de_Correction_C06', '+0', '', '+2', '-3', '+3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_de_Correction_C06', 'medicyc.cyclotron.nsingle.Multiplexed_1', '/Controller/Bobine_de_Correction_C06', '/NSingle/Bobine_de_Correction_C06');

INSERT INTO NSingle VALUES('Bobine_de_Correction_C07', 'BC 07', true, 'Digital', 'Basic', 'BOBINESDECORRECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.114:7', 1470, true, false, 5, 14, 15, 0.003105, 5, 14, 15, 0.0030522, 300, 200, true, false, false, true, true, false, 120, 50.0, 10000, 10000, 5, 2, 40000, 80000, 60000, 40000, 60000, 1, 5, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_de_Correction_C07', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_de_Correction_C07', '+0', '', '+2', '-3', '+3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_de_Correction_C07', 'medicyc.cyclotron.nsingle.Multiplexed_1', '/Controller/Bobine_de_Correction_C07', '/NSingle/Bobine_de_Correction_C07');

INSERT INTO NSingle VALUES('Bobine_de_Correction_C08', 'BC 08', true, 'Digital', 'Basic', 'BOBINESDECORRECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.114:8', 1470, true, false, 5, 14, 15, 0.0030702, 5, 14, 15, 0.00305222, 300, 200, true, false, false, true, true, false, 100, 100.0, 10000, 10000, 5, 2, 40000, 80000, 60000, 40000, 60000, 1, 5, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_de_Correction_C08', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_de_Correction_C08', '+0', '', '+2', '-3', '+3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_de_Correction_C08', 'medicyc.cyclotron.nsingle.Multiplexed_1', '/Controller/Bobine_de_Correction_C08', '/NSingle/Bobine_de_Correction_C08');

INSERT INTO NSingle VALUES('Steerer_X', 'Steerer X', true, 'Digital',  'Basic', 'VDF', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.114:9', 1470, true, false, 5, 14, 15, 0.0031289, 5, 14, 15, 0.00305205, 300, 200, true, false, false, true, true, false, 100, 100.0, 10000, 10000, 5, 2, 40000, 80000, 60000, 40000, 60000, 1, 10, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Steerer_X', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Steerer_X', '+0', '', '+2', '-3', '+3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Steerer_X', 'medicyc.cyclotron.nsingle.Multiplexed_1', '/Controller/Steerer_X', '/NSingle/Steerer_X');

INSERT INTO NSingle VALUES('Steerer_Y', 'Steerer Y', true, 'Digital', 'Basic', 'VDF', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.114:10', 1470, true, false, 5, 14, 15, 0.0030357, 5, 14, 15, 0.0030357, 300, 200, true, false, false, true, true, false, 50, 5.0, 10000, 10000, 5, 2, 40000, 80000, 60000, 40000, 60000, 1, 10, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Steerer_Y', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Steerer_Y', '+0', '', '+2', '-3', '+3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Steerer_Y', 'medicyc.cyclotron.nsingle.Multiplexed_1', '/Controller/Steerer_Y', '/NSingle/Steerer_Y');

INSERT INTO NSingle VALUES('Multiplexed_2', 'Multiplexed 2', true, 'Digital', 'Basic', 'INJECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.130', 1470, false, false, 4, 10, 15, 0.0026, 4, 10, 15, 0.0026, 50, 50, true, true, true, false, true, false, 50, 5.0, 6000, 6000, 5, 2, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Multiplexed_2', false, '+1', '+2', '-2', '');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Multiplexed_2', '+0', '+1', '+2', '', '', '-5', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Multiplexed_2', 'medicyc.cyclotron.nsingle.Multiplexed_2', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Steerer_Bx', 'Steerer Bx', true, 'Digital', 'Basic', 'INJECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.130:1', 1470, true, true, 4, 10, 15, 0.0026, 4, 10, 15, 0.0026, 50, 50, true, true, true, false, true, false, 50, 5.0, 6000, 6000, 5, 2, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Steerer_Bx', false, '+1', '+2', '-2', '');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Steerer_Bx', '+0', '+1', '+2', '', '', '-5', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Steerer_Bx', 'medicyc.cyclotron.nsingle.Multiplexed_2', '/Controller/Steerer_Bx', '/NSingle/Steerer_Bx');

INSERT INTO NSingle VALUES('Steerer_By', 'Steerer By', true, 'Digital', 'Basic', 'INJECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.130:2', 1470, true, true, 4, 10, 15, 0.0026, 4, 10, 15, 0.0026, 50, 50, true, true, true, false, true, false, 60, 5.0, 6000, 6000, 5, 2, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Steerer_By', false, '+1', '+2', '-2', '');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Steerer_By', '+0', '+1', '+2', '', '', '-5', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Steerer_By', 'medicyc.cyclotron.nsingle.Multiplexed_2', '/Controller/Steerer_By', '/NSingle/Steerer_By');

INSERT INTO NSingle VALUES('Steerer_Hx', 'Steerer Hx', true, 'Digital', 'Basic', 'INJECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.130:3', 1470, true, true, 4, 10, 15, 0.0026, 4, 10, 15, 0.0026, 50, 50, true, true, true, false, true, false, 50, 5.0, 6000, 6000, 5, 2, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Steerer_Hx', false, '+1', '+2', '-2', '');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Steerer_Hx', '+0', '+1', '+2', '', '', '-5', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Steerer_Hx', 'medicyc.cyclotron.nsingle.Multiplexed_2', '/Controller/Steerer_Hx', '/NSingle/Steerer_Hx');

INSERT INTO NSingle VALUES('Steerer_Hy', 'Steerer Hy', true, 'Digital', 'Basic', 'INJECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.130:4', 1470, true, true, 4, 10, 15, 0.0026, 4, 10, 15, 0.0026, 50, 50, true, true, true, false, true, false, 50, 5.0, 6000, 6000, 5, 2, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Steerer_Hy', false, '+1', '+2', '-2', '');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Steerer_Hy', '+0', '+1', '+2', '', '', '-5', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Steerer_Hy', 'medicyc.cyclotron.nsingle.Multiplexed_2', '/Controller/Steerer_Hy', '/NSingle/Steerer_Hy');

INSERT INTO NSingle VALUES('Bobine_Harmonique_1', 'BH 01', true, 'Digital', 'BC_ISR', 'EJECTION', 'Bi', 'Low', 'AbsoluteValues', 'ON', '172.20.216.108', 1470, false, false, 2, 14, 15, 0.00912613, 2, 14, 15, 0.0093245341, 100, 100, true, false, true, true, true, false, 32, 100.0, 1000, 3000, 6, 1, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_Harmonique_1', true, '', '+2', '+0', '');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_Harmonique_1', '+0', '', '+2', '-7', '+7', '-5', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_Harmonique_1', 'medicyc.cyclotron.nsingle.Bobine_Harmonique_1', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Bobine_Harmonique_2', 'BH 02', true, 'Digital', 'BC_ISR', 'EJECTION', 'Bi', 'Low', 'AbsoluteValues', 'ON', '172.20.216.106', 1470, false, false, 1, 14, 15, 0.0092733990, 1, 14, 15, 0.00934243176, 100, 100, true, false,true, true,true, false, 20, 100.0, 1000, 3000, 6, 1, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_Harmonique_2', true, '', '+2', '+0', '');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_Harmonique_2', '+0', '', '+2', '-7', '+7', '-5', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_Harmonique_2', 'medicyc.cyclotron.nsingle.Bobine_Harmonique_2', '/Controller', '/NSingle');
/*
INSERT INTO NSingle VALUES('Bobine_Harmonique_3', 'BH 03', false, 'Digital', 'BC_ISR', 'EJECTION', 'Bi', 'Low', 'AbsoluteValues', 'ON', '172.20.216.109', 1470, false, false, 1, 14, 15, 0.009325, 1, 14, 15, 0.009325, 100, 100, true, false, true,true,true, false, 20, 100.0, 1000, 3000, 6, 1, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_Harmonique_3', true, '', '+2', '+0', '');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_Harmonique_3', '+0', '', '+2', '-7', '+7', '-5', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_Harmonique_3', 'medicyc.cyclotron.nsingle.Bobine_Harmonique_3', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Bobine_Harmonique_4', 'BH 04', false, 'Digital', 'BC_ISR', 'EJECTION', 'Bi', 'Low', 'AbsoluteValues', 'ON', '172.20.216.133', 1470, false, false, 1, 14, 15, 0.008948, 1, 14, 15, 0.008948, 100, 100, true, false, true,true,true, false, 20, 100.0, 1000, 3000, 6, 1, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Bobine_Harmonique_4', true, '', '+2', '+0', '');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Bobine_Harmonique_4', '+0', '', '+2', '-7', '+7', '-5', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Bobine_Harmonique_4', 'medicyc.cyclotron.nsingle.Bobine_Harmonique_4', '/Controller', '/NSingle');
*/
INSERT INTO NSingle VALUES('Stripper', 'Stripper', true, 'Digital', 'Basic', 'OTHER', 'Uni', 'None', 'AbsoluteValues', 'ON', '172.20.216.123', 1470, false, false, 0, 11, 15, 0.002441, 0, 11, 15, 0.002441, 15, 15, false, false, false,true,true, false, 3, 20.0, 1000, 3000, 6, 1, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Stripper', false, '+1', '+2', '-2', '');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Stripper', '-2', '+1', '+2', '+3', '-3', '+4', '-4', '', '+7');
INSERT INTO NSingleDBusAddress VALUES('Stripper', 'medicyc.cyclotron.nsingle.Stripper', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Sonde_Principale', 'SP', true, 'Digital', 'Basic', 'OTHER', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.121', 1470, false, false, 0, 6, 15, 1.0, 0, 13, 15, 0.05, 15, 15, false, false,false, true,true, false, 3, 20.0, 1000, 3000, 6, 1, 30000, 30000, 60000, 30000, 30000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Sonde_Principale', false, '', '+2', '-2', '');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Sonde_Principale', '-2', '', '+2', '', '', '-5', '+5', '+6' , '+7');
INSERT INTO NSingleDBusAddress VALUES('Sonde_Principale', 'medicyc.cyclotron.nsingle.Sonde_Principale', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('InflecteurNeg', 'Inf Neg', true, 'Hybrid', 'BC_ISR', 'INJECTION', 'Uni', 'Low', 'SignedValues', 'ON', '172.20.216.100', 1470, false, false, 2, 14, 15, 0.30501598, 0, 14, 15, 0.6083906327, 150, 50, false, false,true,true, false, false, 15, 7000.0, 1000, 3000, 10, 1, 30000, 180000, 180000, 30000, 180000, 1, 300, 3, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('InflecteurNeg', false, '+1', '+2', '-2', '');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('InflecteurNeg', '-2', '', '+2', '', '', '', '', '' , '');
INSERT INTO NSingleDBusAddress VALUES('InflecteurNeg', 'medicyc.cyclotron.nsingle.InflecteurNeg', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('InflecteurPos', 'Inf Pos', true, 'Hybrid', 'BC_ISR', 'INJECTION', 'Uni', 'Low', 'SignedValues', 'ON', '172.20.216.103', 1470, false, false, 2, 14, 15, 0.3046504, 0, 14, 15, 0.608818523, 150, 50, false, false,true, true,false, false, 15, 7000.0, 1000, 3000, 10, 1, 30000, 180000, 180000, 30000, 180000, 1, 300, 3, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('InflecteurPos', false, '+1', '+2', '-2', '');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('InflecteurPos', '-2', '', '+2', '', '', '', '', '' , '');
INSERT INTO NSingleDBusAddress VALUES('InflecteurPos', 'medicyc.cyclotron.nsingle.InflecteurPos', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Injection_33kV', 'Inj 33kV', true, 'Hybrid', 'BC_ISR', 'SOURCE', 'Uni', 'Medium', 'SignedValues', 'ON', '172.20.216.136', 1470, false, false, 2, 14, 15, 1.22078, 2, 14, 15, 1.24778, 100, 100, false, false, true,true,false, false, 10, 400000.0, 1000, 3000, 6, 1, 30000, 180000, 180000, 30000, 180000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Injection_33kV', false, '', '+2', '-2', '');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Injection_33kV', '+0', '', '+2', '', '', '', '', '' , '');
INSERT INTO NSingleDBusAddress VALUES('Injection_33kV', 'medicyc.cyclotron.nsingle.Injection_33kV', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Quadrupole_2-H', 'Quad 2-H', true, 'Digital', 'VDF', 'VDF', 'Bi', 'Low', 'AbsoluteValues', 'ON', '172.20.216.125', 1470, false, false, 0, 11, 15, 0.072876, 0, 11, 15, 0.07320178, 30, 15, true, false,false, true,true, false, 3, 200.0, 1000, 3000, 6, 1, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Quadrupole_2-H', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Quadrupole_2-H', '+0', '', '+2', '+3', '-3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Quadrupole_2-H', 'medicyc.cyclotron.nsingle.Quadrupole_2-H', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Quadrupole_3-5-H', 'Quad 3-5-H', true, 'Digital', 'VDF', 'VDF', 'Bi', 'Low', 'AbsoluteValues', 'ON', '172.20.216.115', 1470, false, false, 0, 11, 15, 0.070338461, 0, 11, 15, 0.0732692307, 30, 15, true, false,false, true,true, false, 3, 200.0, 1000, 3000, 6, 1, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Quadrupole_3-5-H', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Quadrupole_3-5-H', '+0', '', '+2', '+3', '-3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Quadrupole_3-5-H', 'medicyc.cyclotron.nsingle.Quadrupole_3-5-H', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Quadrupole_7-9-H', 'Quad 7-9-H', true, 'Digital', 'VDF', 'VDF', 'Bi', 'Low', 'AbsoluteValues', 'ON', '172.20.216.117', 1470, false, false, 0, 11, 15, 0.072697495, 0, 11, 15, 0.073262135, 30, 15, true, false, false,true,true, false, 3, 200.0, 1000, 3000, 6, 1, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Quadrupole_7-9-H', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Quadrupole_7-9-H', '+0', '', '+2', '+3', '-3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Quadrupole_7-9-H', 'medicyc.cyclotron.nsingle.Quadrupole_7-9-H', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Quadrupole_1-V', 'Quad 1-V', true, 'Digital', 'VDF', 'VDF', 'Bi', 'Low', 'AbsoluteValues', 'ON', '172.20.216.113', 1470, false, false, 0, 11, 15, 0.073070458, 0, 11, 15, 0.073259883, 30, 15, true, false,false, true,true, false, 3, 200.0, 1000, 3000, 6, 1, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Quadrupole_1-V', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Quadrupole_1-V', '+0', '', '+2', '+3', '-3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Quadrupole_1-V', 'medicyc.cyclotron.nsingle.Quadrupole_1-V', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Quadrupole_4-V', 'Quad 4-V', true, 'Digital', 'VDF', 'VDF', 'Bi', 'Low', 'AbsoluteValues', 'ON', '172.20.216.116', 1470, false, false, 0, 11, 15, 0.072752941, 0, 11, 15, 0.073270142, 30, 15, true, false, false,true,true, false, 3, 200.0, 1000, 3000, 6, 1, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Quadrupole_4-V', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Quadrupole_4-V', '+0', '', '+2', '+3', '-3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Quadrupole_4-V', 'medicyc.cyclotron.nsingle.Quadrupole_4-V', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Quadrupole_6-8-V', 'Quad 6-8-V', true, 'Digital', 'VDF', 'VDF', 'Bi', 'Low', 'AbsoluteValues', 'ON', '172.20.216.112', 1470, false, false, 0, 11, 15, 0.0729644268, 0, 11, 15, 0.073399602, 30, 15, true, false,false, true,true, false, 3, 200.0, 1000, 3000, 6, 1, 60000, 30000, 60000, 30000, 60000, 1, 10, 2, false);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Quadrupole_6-8-V', true, '', '+2', '+0', '+3');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Quadrupole_6-8-V', '+0', '', '+2', '+3', '-3', '+4', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Quadrupole_6-8-V', 'medicyc.cyclotron.nsingle.Quadrupole_6-8-V', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Lentille_Inferieure', 'Lent Inf', true, 'Digital', 'BC_ISR', 'INJECTION', 'Bi', 'None', 'AbsoluteValues', 'ON', '172.20.216.129', 1470, false, false, 0, 14, 15, 0.009102524, 0, 14, 15, 0.009108797,  300, 100, true, false, true,true, true, false, 20, 300.0, 1000, 3000, 6, 1, 60000, 30000, 60000, 30000, 60000, 1, 10, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Lentille_Inferieure', true, '', '+2', '+0', '');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Lentille_Inferieure', '+0', '', '+2', '-7', '+7', '-5', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Lentille_Inferieure', 'medicyc.cyclotron.nsingle.Lentille_Inferieure', '/Controller', '/NSingle');

INSERT INTO NSingle VALUES('Lentille_Superieure', 'Lent Sup', true, 'Digital', 'BC_ISR', 'INJECTION', 'Bi', 'Low', 'AbsoluteValues', 'ON', '172.20.216.132', 1470, false, false, 0, 14, 15, 0.008939573, 0, 14, 15, 0.00894832, 300, 100, true, false, true, true,true, false, 30, 300.0, 1000, 3000, 6, 1, 30000, 60000, 60000, 30000, 60000, 1, 10, 2, true);
INSERT INTO NSingleStateCommand(nsingle_name, impuls, stdby, on_, off_, reset) VALUES('Lentille_Superieure', true, '', '+2', '+0', '');
INSERT INTO NSingleStateReply(nsingle_name, off_, stdby, on_, ok, error, local_, remote, switch1, switch2) VALUES('Lentille_Superieure', '+0', '', '+2', '-7', '+7', '-5', '+5', '', '');
INSERT INTO NSingleDBusAddress VALUES('Lentille_Superieure', 'medicyc.cyclotron.nsingle.Lentille_Superieure', '/Controller', '/NSingle');

INSERT INTO NSingleMultiplexConfig VALUES('Bobine_de_Correction_C00', 'Multiplexed_1', 1);
INSERT INTO NSingleMultiplexConfig VALUES('Bobine_de_Correction_C01', 'Multiplexed_1', 2);
INSERT INTO NSingleMultiplexConfig VALUES('Bobine_de_Correction_C02', 'Multiplexed_1', 3);
INSERT INTO NSingleMultiplexConfig VALUES('Bobine_de_Correction_C03', 'Multiplexed_1', 4);
INSERT INTO NSingleMultiplexConfig VALUES('Bobine_de_Correction_C04', 'Multiplexed_1', 5);
INSERT INTO NSingleMultiplexConfig VALUES('Bobine_de_Correction_C05', 'Multiplexed_1', 6);
INSERT INTO NSingleMultiplexConfig VALUES('Bobine_de_Correction_C06', 'Multiplexed_1', 7);
INSERT INTO NSingleMultiplexConfig VALUES('Bobine_de_Correction_C07', 'Multiplexed_1', 8);
INSERT INTO NSingleMultiplexConfig VALUES('Bobine_de_Correction_C08', 'Multiplexed_1', 9);
INSERT INTO NSingleMultiplexConfig VALUES('Steerer_X', 'Multiplexed_1', 10);
INSERT INTO NSingleMultiplexConfig VALUES('Steerer_Y', 'Multiplexed_1', 11);

INSERT INTO NSingleMultiplexConfig VALUES('Steerer_Bx', 'Multiplexed_2', 1);
INSERT INTO NSingleMultiplexConfig VALUES('Steerer_By', 'Multiplexed_2', 2);
INSERT INTO NSingleMultiplexConfig VALUES('Steerer_Hx', 'Multiplexed_2', 3);
INSERT INTO NSingleMultiplexConfig VALUES('Steerer_Hy', 'Multiplexed_2', 4);

INSERT INTO NSingleComments(nsingle_name, comment) VALUES('Bobine_Principale', 'Hej vad heter du jag heter Dirk Diggler');

INSERT INTO BPCycleLevel VALUES(1, 1190.00, 300);
INSERT INTO BPCycleLevel VALUES(2, 1029.41, 300);
INSERT INTO BPCycleLevel VALUES(3, 1137.77, 300);
INSERT INTO BPCycleLevel VALUES(4, 1084.59, 30);

INSERT INTO NSingleSetPoint VALUES('Dipole_M1', 'VoieMedicale-1', TRUE, '+', 233.26);
INSERT INTO NSingleSetPoint VALUES('Dipole_M2', 'VoieMedicale-1', FALSE, '+', 0.0);
/*
INSERT INTO NSingleSetPoint VALUES('Dipole_M3', 'VoieMedicale-1', FALSE, '+', 0.0);
*/
INSERT INTO NSingleSetPoint VALUES('Dipole_M4', 'VoieMedicale-1', TRUE, '+', 230.70);

INSERT INTO NSingleSetPoint VALUES('Quadrupole_1-V', 'VoieMedicale-1', TRUE, '-', 113.04);
INSERT INTO NSingleSetPoint VALUES('Quadrupole_2-H', 'VoieMedicale-1', TRUE, '+', 98.31);
INSERT INTO NSingleSetPoint VALUES('Quadrupole_3-5-H', 'VoieMedicale-1', TRUE, '+', 22.86);
INSERT INTO NSingleSetPoint VALUES('Quadrupole_4-V', 'VoieMedicale-1', TRUE, '-', 30.92);
INSERT INTO NSingleSetPoint VALUES('Quadrupole_6-8-V', 'VoieMedicale-1', TRUE, '-', 36.92);
INSERT INTO NSingleSetPoint VALUES('Quadrupole_7-9-H', 'VoieMedicale-1', TRUE, '+', 37.73);
INSERT INTO NSingleSetPoint VALUES('Steerer_X', 'VoieMedicale-1', TRUE, '-', 39.75);
INSERT INTO NSingleSetPoint VALUES('Steerer_Y', 'VoieMedicale-1', TRUE, '-', 0.68);

INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C00', 'VoieMedicale-1', TRUE, '+', 7.52);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C01', 'VoieMedicale-1', TRUE, '+', 1.66);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C02', 'VoieMedicale-1', TRUE, '+', 3.42);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C03', 'VoieMedicale-1', TRUE, '+', 16.60);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C04', 'VoieMedicale-1', TRUE, '+', 14.26);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C05', 'VoieMedicale-1', TRUE, '-', 12.89);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C06', 'VoieMedicale-1', TRUE, '-', 12.89);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C07', 'VoieMedicale-1', TRUE, '-', 11.33);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C08', 'VoieMedicale-1', TRUE, '-', 49.91);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C09', 'VoieMedicale-1', TRUE, '+', 260.88);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C10', 'VoieMedicale-1', TRUE, '+', 211.06);

INSERT INTO NSingleSetPoint VALUES('Lentille_Inferieure', 'VoieMedicale-1', TRUE, '-', 118.97);
INSERT INTO NSingleSetPoint VALUES('Lentille_Superieure', 'VoieMedicale-1', TRUE, '+', 127.97);
INSERT INTO NSingleSetPoint VALUES('InflecteurPos', 'VoieMedicale-1', TRUE, '+', 6152.77);
INSERT INTO NSingleSetPoint VALUES('InflecteurNeg', 'VoieMedicale-1', TRUE, '+', 6105.22);
INSERT INTO NSingleSetPoint VALUES('Steerer_Bx', 'VoieMedicale-1', TRUE, '+', 0.0);
INSERT INTO NSingleSetPoint VALUES('Steerer_By', 'VoieMedicale-1', TRUE, '+', 1.62);
INSERT INTO NSingleSetPoint VALUES('Steerer_Hx', 'VoieMedicale-1', TRUE, '-', 0.01);
INSERT INTO NSingleSetPoint VALUES('Steerer_Hy', 'VoieMedicale-1', TRUE, '+', 0.08);

INSERT INTO NSingleSetPoint VALUES('Bobine_Harmonique_1', 'VoieMedicale-1', TRUE, '+', 36.03);
INSERT INTO NSingleSetPoint VALUES('Bobine_Harmonique_2', 'VoieMedicale-1', TRUE, '-', 7.53);


INSERT INTO NSingleSetPoint VALUES('Dipole_M1', 'VoieR&D-1', TRUE, '+', 232.67);
INSERT INTO NSingleSetPoint VALUES('Dipole_M2', 'VoieR&D-1', FALSE, '+', 0.0);
/*
INSERT INTO NSingleSetPoint VALUES('Dipole_M3', 'VoieR&D-1', FALSE, '+', 0.0);
*/
INSERT INTO NSingleSetPoint VALUES('Dipole_M4', 'VoieR&D-1', FALSE, '+', 0.0);
INSERT INTO NSingleSetPoint VALUES('Quadrupole_1-V', 'VoieR&D-1', TRUE, '-', 121.61);
INSERT INTO NSingleSetPoint VALUES('Quadrupole_2-H', 'VoieR&D-1', TRUE, '+', 101.9);
INSERT INTO NSingleSetPoint VALUES('Quadrupole_3-5-H', 'VoieR&D-1', TRUE, '+', 36.92);
INSERT INTO NSingleSetPoint VALUES('Quadrupole_4-V', 'VoieR&D-1', TRUE, '-', 31.36);
INSERT INTO NSingleSetPoint VALUES('Quadrupole_6-8-V', 'VoieR&D-1', TRUE, '-', 37.14);
INSERT INTO NSingleSetPoint VALUES('Quadrupole_7-9-H', 'VoieR&D-1', TRUE, '+', 49.67);
INSERT INTO NSingleSetPoint VALUES('Steerer_X', 'VoieR&D-1', TRUE, '-', 43.66);
INSERT INTO NSingleSetPoint VALUES('Steerer_Y', 'VoieR&D-1', TRUE, '-', 1.07);

INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C00', 'VoieR&D-1', TRUE, '+', 7.52);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C01', 'VoieR&D-1', TRUE, '+', 1.66);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C02', 'VoieR&D-1', TRUE, '+', 3.42);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C03', 'VoieR&D-1', TRUE, '+', 16.60);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C04', 'VoieR&D-1', TRUE, '+', 14.26);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C05', 'VoieR&D-1', TRUE, '-', 12.89);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C06', 'VoieR&D-1', TRUE, '-', 12.89);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C07', 'VoieR&D-1', TRUE, '-', 11.33);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C08', 'VoieR&D-1', TRUE, '-', 49.91);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C09', 'VoieR&D-1', TRUE, '+', 260.88);
INSERT INTO NSingleSetPoint VALUES('Bobine_de_Correction_C10', 'VoieR&D-1', TRUE, '+', 211.06);

INSERT INTO NSingleSetPoint VALUES('Lentille_Inferieure', 'VoieR&D-1', TRUE, '-', 118.97);
INSERT INTO NSingleSetPoint VALUES('Lentille_Superieure', 'VoieR&D-1', TRUE, '+', 127.97);
INSERT INTO NSingleSetPoint VALUES('InflecteurPos', 'VoieR&D-1', TRUE, '+', 6152.77);
INSERT INTO NSingleSetPoint VALUES('InflecteurNeg', 'VoieR&D-1', TRUE, '+', 6105.22);
INSERT INTO NSingleSetPoint VALUES('Steerer_Bx', 'VoieR&D-1', TRUE, '+', 0.0);
INSERT INTO NSingleSetPoint VALUES('Steerer_By', 'VoieR&D-1', TRUE, '+', 1.62);
INSERT INTO NSingleSetPoint VALUES('Steerer_Hx', 'VoieR&D-1', TRUE, '-', 0.01);
INSERT INTO NSingleSetPoint VALUES('Steerer_Hy', 'VoieR&D-1', TRUE, '+', 0.08);

INSERT INTO NSingleSetPoint VALUES('Bobine_Harmonique_1', 'VoieR&D-1', TRUE, '+', 36.03);
INSERT INTO NSingleSetPoint VALUES('Bobine_Harmonique_2', 'VoieR&D-1', TRUE, '-', 7.53);



