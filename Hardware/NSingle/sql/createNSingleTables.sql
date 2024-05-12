CREATE TYPE NSINGLECLASS AS ENUM('Digital', 'Hybrid');
CREATE TYPE NSINGLETYPE AS ENUM('Basic', 'BP', 'BC_ISR', 'VDF');
CREATE TYPE NSINGLEPOLARITY AS ENUM('Bi', 'Uni');
CREATE TYPE NSINGLEFILTERING AS ENUM('None', 'Low', 'Medium', 'High');
CREATE TYPE NSINGLEAVERAGING AS ENUM('AbsoluteValues', 'SignedValues');
CREATE TYPE NSINGLESTATEMONITORING AS ENUM('OFF', 'ON');
CREATE TYPE NSINGLESTATES AS ENUM('Off', 'On', 'Ok', 'Error', 'Local', 'Remote');

CREATE TABLE NSingle
(
  name TEXT NOT NULL PRIMARY KEY,
  short_name TEXT NOT NULL,
  available BOOLEAN NOT NULL, 
  class NSINGLECLASS NOT NULL,
  type NSINGLETYPE NOT NULL,
  subsystem SUBSYSTEM NOT NULL,
  polarity NSINGLEPOLARITY NOT NULL,
  filtering NSINGLEFILTERING NOT NULL,
  averaging NSINGLEAVERAGING NOT NULL,
  state_monitoring NSINGLESTATEMONITORING NOT NULL,
  ip TEXT NOT NULL UNIQUE,
  port INTEGER NOT NULL,
  multiplexed BOOLEAN NOT NULL,
  multichannel BOOLEAN NOT NULL,
  setpoint_lsb INTEGER NOT NULL CHECK (setpoint_lsb >= 0),
  setpoint_msb INTEGER NOT NULL CHECK (setpoint_msb >= 0),
  setpoint_sign_bit INTEGER NOT NULL CHECK (setpoint_sign_bit >= 0),
  setpoint_lsb_conversion NUMERIC NOT NULL CHECK (setpoint_lsb_conversion > 0.0),
  value_lsb INTEGER NOT NULL CHECK (value_lsb >= 0),
  value_msb INTEGER NOT NULL CHECK (value_msb >= 0),
  value_sign_bit INTEGER NOT NULL CHECK (value_sign_bit >= 0),
  value_lsb_conversion NUMERIC NOT NULL,
  near_zero_limit INTEGER NOT NULL,
  near_target_limit INTEGER NOT NULL,
  polarity_change_allowed BOOLEAN NOT NULL,
  hot_polarity_change BOOLEAN NOT NULL,
  reset_at_on BOOLEAN NOT NULL,
  switchoff_on_shutdown BOOLEAN NOT NULL,
  verify_setpoints BOOLEAN NOT NULL,
  active_compensation BOOLEAN NOT NULL,
  signal_variation INTEGER NOT NULL,
  upper_limit NUMERIC NOT NULL CHECK (upper_limit > 0),
  read_value_interval INTEGER NOT NULL CHECK (read_value_interval > 0),
  read_state_interval INTEGER NOT NULL CHECK (read_state_interval > 0),
  regulation_buffer_size INTEGER NOT NULL CHECK (regulation_buffer_size > 0),
  timeout_command_factor INTEGER NOT NULL CHECK (timeout_command_factor > 0),
  timeout_checkstate INTEGER NOT NULL CHECK (timeout_checkstate > 0),
  timeout_startup INTEGER NOT NULL CHECK (timeout_startup > 0),
  timeout_roughpositioning INTEGER NOT NULL CHECK (timeout_roughpositioning > 0),
  timeout_finepositioning INTEGER NOT NULL CHECK (timeout_finepositioning > 0),
  timeout_shutdown INTEGER NOT NULL CHECK (timeout_shutdown > 0),
  ramp_cmd_inc_time NUMERIC NOT NULL CHECK (ramp_cmd_inc_time > 0),
  ramp_max_delta NUMERIC NOT NULL CHECK (ramp_max_delta > 0.1),
  ramp_power_delta INTEGER NOT NULL CHECK (ramp_power_delta > 0),
  ramp_activated BOOLEAN NOT NULL
);

CREATE TABLE NSingleSetPoint
(
   name TEXT NOT NULL REFERENCES NSingle(name) ON UPDATE CASCADE,
   cyclo_config TEXT NOT NULL REFERENCES CycloConfig(name) ON UPDATE CASCADE,
   powered_up BOOLEAN NOT NULL, /* powered_up = false -> power supply will be switched off on Startup.*/
   sign SIGN NOT NULL,
   value NUMERIC NOT NULL CHECK (value >= 0.0),
   PRIMARY KEY(name, cyclo_config)
);

CREATE TABLE BPCycleLevel
(
  id INTEGER NOT NULL CHECK (id >= 0),
  setpoint NUMERIC NOT NULL CHECK (setpoint > 0),
  duration INTEGER NOT NULL CHECK (duration > 0),
  PRIMARY KEY(id)
);


CREATE TABLE NSingleDBusAddress
(
  name TEXT NOT NULL PRIMARY KEY REFERENCES NSingle(name) ON UPDATE CASCADE,
  interface_address TEXT NOT NULL,
  controller_object_name TEXT NOT NULL,
  nsingle_object_name TEXT NOT NULL
);

CREATE TABLE NSingleComments
(
  id SERIAL PRIMARY KEY,
  nsingle_name TEXT NOT NULL REFERENCES NSingle(name) ON UPDATE CASCADE,
  comment TEXT
);

CREATE TABLE NSingleStateCommand
(
  id SERIAL PRIMARY KEY,
  nsingle_name TEXT NOT NULL REFERENCES NSingle(name) ON UPDATE CASCADE,
  impuls BOOLEAN NOT NULL,
  stdby CHARACTER(2),
  on_ CHARACTER(2),
  off_ character(2),
  reset character(2)
);

CREATE TABLE NSingleStateReply
(
  id SERIAL PRIMARY KEY,
  nsingle_name TEXT NOT NULL REFERENCES NSingle(name) ON UPDATE CASCADE,
  off_ CHARACTER(2),
  stdby CHARACTER(2),
  on_ CHARACTER(2),
  ok CHARACTER(2),
  error CHARACTER(2),
  local_ CHARACTER(2),
  remote CHARACTER(2),
  switch1 CHARACTER(2),
  switch2 CHARACTER(2)
);

CREATE TABLE NSingleMultiplexConfig
(
  channel TEXT NOT NULL,
  nsingle_name TEXT NOT NULL REFERENCES NSingle(name) ON UPDATE CASCADE,  
  address INTEGER NOT NULL CHECK (address > 0 AND address < 16),
  PRIMARY KEY(channel, nsingle_name)
);
