\c timeseries

CREATE TYPE MESSAGETYPE AS ENUM('ERROR', 'WARNING', 'INFO', 'DEBUG');

CREATE EXTENSION IF NOT EXISTS timescaledb CASCADE;

CREATE TABLE Messages (
  time TIMESTAMP      NOT NULL,
  system    TEXT 	   NOT NULL,
  type	    MESSAGETYPE	   NOT NULL,
  content   TEXT           NOT NULL
);

SELECT create_hypertable('Messages', 'time', chunk_time_interval => INTERVAL '7 days');


-- ALTER TYPE MESSAGETYPE ADD VALUE 'DEBUG';
