CREATE TABLE IORChannel
(
  name TEXT NOT NULL PRIMARY KEY,
  card INTEGER NOT NULL CHECK (card >= 0),
  block INTEGER NOT NULL CHECK (block >= 0 AND block <= 3),
  address INTEGER NOT NULL CHECK (address >= 0)
 );

