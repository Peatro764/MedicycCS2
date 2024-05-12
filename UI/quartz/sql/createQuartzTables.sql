CREATE TABLE BeamAxesCalibration
(
  quartz TEXT NOT NULL PRIMARY KEY,
  x_min NUMERIC NOT NULL,
  x_max NUMERIC NOT NULL,
  y_min NUMERIC NOT NULL,
  y_max NUMERIC NOT NULL
);

CREATE TABLE BeamImageSet
(
  name TEXT NOT NULL PRIMARY KEY,
  timestamp TIMESTAMP NOT NULL,
  comments TEXT
);

CREATE TABLE BeamImage
(
   quartz TEXT NOT NULL,
   beam_image_set TEXT NOT NULL REFERENCES BeamImageSet(name) ON UPDATE CASCADE,
   x_min NUMERIC NOT NULL,
   x_max NUMERIC NOT NULL,
   y_min NUMERIC NOT NULL,
   y_max NUMERIC NOT NULL,
   zoom_x NUMERIC NOT NULL,
   zoom_y NUMERIC NOT NULL,
   zoom_width NUMERIC NOT NULL,
   zoom_height NUMERIC NOT NULL,
   data NUMERIC[] NOT NULL,
   ncol INTEGER NOT NULL CHECK(ncol > 0),
   PRIMARY KEY(quartz, beam_image_set)
);
