CREATE TABLE HFConfiguration
(
  id SERIAL PRIMARY KEY,
  HF INTEGER NOT NULL CHECK (HF > 0 AND HF < 3), -- HF1 or HF2
  timestamp TIMESTAMPTZ NOT NULL, -- timestamp of insertion
  accord_voltage_lower NUMERIC CHECK (accord_voltage_lower > 0), -- lower bound when the membrane is considered accorded
  accord_voltage_upper NUMERIC CHECK (accord_voltage_upper > 0), -- upper bound when the membrane is considered accorded
  accord_phase_lower NUMERIC CHECK (accord_phase_lower > 0), -- lower bound when the membrane is considered accorded
  accord_phase_upper NUMERIC CHECK (accord_phase_upper > 0), -- upper bound when the membrane is considered accorded
  membrane_min NUMERIC CHECK (membrane_min > 0), -- min allowed membrane position during search accord procedure
  membrane_max NUMERIC CHECK (membrane_max > 0), -- max allowed membrane position during search accord procedure
  membrane_clearance NUMERIC CHECK (membrane_clearance > 0), -- distance from limit (min and max) where the HF frequency can continue to be changed
  niveau_preon_value NUMERIC CHECK (niveau_preon_value > 0), -- the niveau that will be set before switching it on
  niveau_preoff_value NUMERIC CHECK (niveau_preoff_value > 0), -- the niveau that will be set before switching it off
  niveau_startup_final_value NUMERIC CHECK (niveau_startup_final_value > 0), -- the niveau that will be set during the startup process after regul is switched on
  niveau_shutdown_initial_value NUMERIC CHECK (niveau_shutdown_initial_value > 0), -- the niveau that will be set first thing during the shutdown process before increasing the frequency
  niveau_max NUMERIC CHECK (niveau_max > 0), -- max allowed niveau
  voltage_max NUMERIC CHECK (voltage_max > 0), -- max allowed voltage during startup process
  voltage_delta_down NUMERIC CHECK (voltage_delta_down > 0), -- the delta voltage that will be done (by acting on niveau) in shutdown procedure
  voltage_delta_up NUMERIC CHECK (voltage_delta_up > 0) -- the delta voltage that will be done (by acting on niveau) in startup procedure
 );


CREATE OR REPLACE FUNCTION send_notification()
  RETURNS trigger AS $send_notification$
  DECLARE
  msg TEXT;
  BEGIN
  msg := cast(NEW.id as TEXT);
  execute 'notify hfconfiguration_notification, ''' || msg || '''';
  RETURN NEW;
 END;
 $send_notification$ LANGUAGE plpgsql;

CREATE TRIGGER send_notification
  AFTER INSERT ON HFConfiguration
  FOR EACH ROW
  EXECUTE PROCEDURE send_notification();
