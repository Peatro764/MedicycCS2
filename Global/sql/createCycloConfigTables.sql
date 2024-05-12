CREATE TABLE CycloConfig
(
  name TEXT NOT NULL PRIMARY KEY,
  timestamp TIMESTAMP NOT NULL,
  comments TEXT
);

CREATE TABLE ActiveConfig
(
  timestamp TIMESTAMP NOT NULL,
  subsystem SUBSYSTEM NOT NULL,
  config TEXT NOT NULL REFERENCES CycloConfig(name) ON UPDATE CASCADE,
  PRIMARY KEY(timestamp, subsystem)
);
