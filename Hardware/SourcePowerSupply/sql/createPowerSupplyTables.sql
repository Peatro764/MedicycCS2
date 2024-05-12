CREATE TABLE SourcePowerSupply
(
  name TEXT NOT NULL PRIMARY KEY,
  regulated_param TEXT NOT NULL, -- which parameter is actively regulated
  heat_up_time INTEGER NOT NULL, -- the delay between switched on and the next action
  reg_tolerance NUMERIC NOT NULL, -- tolerance of the regulated parameter before offtarget
  reg_near_zero NUMERIC NOT NULL, -- when is the powsersupply sufficiently close to zero to switch off
  reg_near_target NUMERIC NOT NULL, -- when can use the fineregulation algo instead of the roughregulation algo
  reg_lower_limit NUMERIC NOT NULL, -- lower allowed limit
  reg_upper_limit NUMERIC NOT NULL, -- upper allowed limit
  reg_buffer_size INTEGER NOT NULL, -- the number of measurements used to derive mean and standard deviation
  act_tolerance NUMERIC NOT NULL,
  act_near_zero NUMERIC NOT NULL,
  act_near_target NUMERIC NOT NULL,
  act_lower_limit NUMERIC NOT NULL,
  act_upper_limit NUMERIC NOT NULL,
  act_buffer_size INTEGER NOT NULL,
  voltage_unit TEXT NOT NULL,
  current_unit TEXT NOT NULL
 );

CREATE TABLE SourcePowerSupplySetPoint
(
   power_supply_name TEXT NOT NULL ,
   cyclo_config TEXT NOT NULL REFERENCES CycloConfig(name) ON UPDATE CASCADE,
   value NUMERIC NOT NULL CHECK (value >= 0.0),
   unit TEXT NOT NULL, 
   PRIMARY KEY(power_supply_name, unit, cyclo_config)
);

CREATE TABLE SourcePowerSupplyDBusAddress
(
  name TEXT NOT NULL PRIMARY KEY REFERENCES SourcePowerSupply(name) ON UPDATE CASCADE,
  interface_address TEXT NOT NULL,
  object_name TEXT NOT NULL
);
