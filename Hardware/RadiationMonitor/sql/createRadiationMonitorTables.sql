CREATE TABLE RadiationMonitorConfiguration
(
    name TEXT PRIMARY KEY,
    timestamp TIMESTAMP NOT NULL,
    ip TEXT NOT NULL,
    port INTEGER NOT NULL CHECK (port > 0),
    connection_timeout INTEGER NOT NULL,
    read_timeout INTEGER NOT NULL CHECK (read_timeout > 0),
    write_timeout INTEGER NOT NULL CHECK (write_timeout > 0),
    mode_functionnement INTEGER NOT NULL CHECK (mode_functionnement >= 0 AND mode_functionnement <= 4),
    preampli_type INTEGER NOT NULL CHECK (preampli_type >= 0 AND preampli_type <= 1),
    preampli_gamme INTEGER NOT NULL CHECK (preampli_gamme >= 0 AND preampli_gamme <= 4),
    instantaneous_lam BOOLEAN NOT NULL,
    integrated1_lam BOOLEAN NOT NULL,
    integrated2_lam BOOLEAN NOT NULL,
    command_confirmation BOOLEAN NOT NULL,
    include_date_in_measurement BOOLEAN NOT NULL,
    include_raw_in_measurement BOOLEAN NOT NULL,
    instantaneous_time INTEGER NOT NULL CHECK (instantaneous_time > 0 AND instantaneous_time <= 10),
    instantaneous_elements INTEGER NOT NULL CHECK (instantaneous_elements > 0),
    integrated1_time INTEGER NOT NULL CHECK (integrated1_time > 0 AND integrated1_time <= 60),
    integrated2_time INTEGER NOT NULL CHECK (integrated2_time > 0 AND integrated2_time <= 24),
    number_of_measurements INTEGER NOT NULL CHECK (number_of_measurements >= 0),
    instantaneous_conversion_coefficient NUMERIC NOT NULL CHECK (instantaneous_conversion_coefficient > 0.0),
    integrated_conversion_coefficient NUMERIC NOT NULL CHECK (integrated_conversion_coefficient > 0.0),
    integrated_threshold NUMERIC NOT NULL CHECK (integrated_threshold > 0.0),
    threshold_A NUMERIC NOT NULL CHECK (threshold_A > 0.0),
    threshold_B NUMERIC NOT NULL CHECK (threshold_A > 0.0),
    threshold_C NUMERIC NOT NULL CHECK (threshold_A > 0.0)
);

COMMENT ON COLUMN RadiationMonitorConfiguration.mode_functionnement IS 'PR0 0 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.preampli_type IS 'PR0 1 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.preampli_gamme IS 'PR0 2 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.instantaneous_lam IS 'PR0 4 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.integrated1_lam IS 'PR0 5 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.integrated2_lam IS 'PR0 6 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.command_confirmation IS 'REP X';
COMMENT ON COLUMN RadiationMonitorConfiguration.include_date_in_measurement IS 'PR0 7 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.include_raw_in_measurement IS 'PR0 8 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.instantaneous_time IS 'PR1 15 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.instantaneous_elements IS 'PR1 16 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.integrated1_time IS 'PR1 18 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.integrated2_time IS 'PR1 19 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.number_of_measurements IS 'PR1 20 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.instantaneous_conversion_coefficient IS 'PR1 21 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.integrated_conversion_coefficient IS 'PR1 22 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.integrated_threshold IS 'PR1 23 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.threshold_A IS 'PR1 24 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.threshold_B IS 'PR1 25 X';
COMMENT ON COLUMN RadiationMonitorConfiguration.threshold_C IS 'PR1 26 X';
