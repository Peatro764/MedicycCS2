CREATE TABLE ADCChannel
(
  name TEXT NOT NULL PRIMARY KEY,
  card INTEGER NOT NULL CHECK (card >= 0),
  address INTEGER NOT NULL CHECK (address >= 0),
  conversion_factor NUMERIC NOT NULL,
  unit TEXT NOT NULL,
  sim_value NUMERIC NOT NULL,
  sim_step NUMERIC NOT NULL
 );

