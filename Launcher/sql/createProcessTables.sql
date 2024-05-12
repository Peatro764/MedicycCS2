CREATE TABLE ProcessMeta
(
 topdir TEXT NOT NULL,
 logdir TEXT NOT NULL
);

CREATE TABLE Library
(
 id TEXT NOT NULL PRIMARY KEY,
 path TEXT NOT NULL,
 name TEXT NOT NULL
);

CREATE TABLE Process
(
 id TEXT NOT NULL PRIMARY KEY,
 path TEXT NOT NULL,
 name TEXT NOT NULL,
 args TEXT [], enabled Boolean,
 priority INTEGER NOT NULL,
 required BOOL NOT NULL
);

CREATE TABLE ProcessLibrary
(
 process_id TEXT NOT NULL REFERENCES Process(id) ON UPDATE CASCADE,
 library_id TEXT NOT NULL REFERENCES Library(id) ON UPDATE CASCADE,
 PRIMARY KEY(process_id, library_id)
);
