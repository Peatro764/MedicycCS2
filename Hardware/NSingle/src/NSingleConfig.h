#ifndef COMMEQUIPMENT_NSINGLECONFIG_H
#define COMMEQUIPMENT_NSINGLECONFIG_H

#include "NSingle_global.h"

#include <QString>

#include "StateCommand.h"
#include "Measurement.h"
#include "StateReply.h"
#include "Config.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class NSINGLE_EXPORT NSingleConfig
{

public:
    NSingleConfig(QString name, QString ip, int port,
                  MeasurementConfig ch1_setpoint_config,
                  MeasurementConfig ch1_value_config,
                  StateReplyConfig state_reply_config,
                  StateCommandConfig state_command_config,
                  Config low_level_config,
                  bool multiplexed,
                  bool multichannel,
                  int near_zero_limit,
                  int near_target_limit,
                  bool polarity_change_allowed,
                  bool hot_polarity_change,
                  bool reset_at_on,
                  bool switchoff_on_shutdown,
                  bool verify_setpoints,
                  bool active_compensation,
                  int signal_variation,
                  double upper_limit,
                  int read_value_interval,
                  int read_state_interval,
                  int regulation_buffer_size,
                  int timeout_command_factor,
                  int timeout_checkstate,
                  int timeout_startup,
                  int timeout_roughpositioning,
                  int timeout_finepositioning,
                  int timeout_shutdown,
                  double ramp_cmd_inc_time,
                  double ramp_max_delta,
                  int ramp_power_delta,
                  bool ramp_activated);

public:
    QString name() const { return name_; }
    QString ip() const { return ip_; }
    int port() const { return port_; }
    MeasurementConfig GetChannel1SetPointConfig() const { return ch1_setpoint_config_; }
    MeasurementConfig GetChannel1ValueConfig() const { return ch1_value_config_; }
    StateReplyConfig GetStateReplyConfig() const { return state_reply_config_; }
    StateCommandConfig GetStateCommandConfig() const { return state_command_config_; }
    Config GetLowLevelConfig() const { return low_level_config_; }
    bool multiplexed() const { return multiplexed_; }
    bool multichannel() const { return multichannel_; }
    int near_zero_limit() const { return near_zero_limit_; }
    int near_target_limit() const { return near_target_limit_; }
    bool polarity_change_allowed() const { return polarity_change_allowed_; }
    bool hot_polarity_change() const { return hot_polarity_change_; }
    bool reset_at_on() const { return reset_at_on_; }
    bool switchoff_on_shutdown() const { return switchoff_on_shutdown_; }
    bool verify_setpoints() const { return verify_setpoints_; }
    bool active_compensation() const { return active_compensation_; }
    int signal_variation() const { return signal_variation_; }
    double upper_limit() const { return upper_limit_; }
    int read_value_interval() const { return read_value_interval_; }
    int read_state_interval() const { return read_state_interval_; }
    int regulation_buffer_size() const { return regulation_buffer_size_; }
    int timeout_command_factor() const { return timeout_command_factor_; }
    int timeout_checkstate() const { return timeout_checkstate_; }
    int timeout_startup() const { return timeout_startup_; }
    int timeout_roughpositioning() const { return timeout_roughpositioning_; }
    int timeout_finepositioning() const { return timeout_finepositioning_; }
    int timeout_shutdown() const { return timeout_shutdown_; }
    double ramp_cmd_inc_time() const { return ramp_cmd_inc_time_; }
    double ramp_max_delta() const { return ramp_max_delta_; }
    int ramp_power_delta() const { return ramp_power_delta_; }
    bool ramp_activated() const { return ramp_activated_; }

private:
    friend bool operator==(const NSingleConfig &c1, const NSingleConfig& c2);
    QString name_;
    QString ip_;
    int port_;
    MeasurementConfig ch1_setpoint_config_;
    MeasurementConfig ch1_value_config_;
    StateReplyConfig state_reply_config_;
    StateCommandConfig state_command_config_;
    Config low_level_config_;
    bool multiplexed_;
    bool multichannel_;
    int near_zero_limit_;
    int near_target_limit_;
    bool polarity_change_allowed_;
    bool hot_polarity_change_;
    bool reset_at_on_;
    bool switchoff_on_shutdown_;
    bool verify_setpoints_;
    bool active_compensation_;
    int signal_variation_;
    double upper_limit_;
    int read_value_interval_;
    int read_state_interval_;
    int regulation_buffer_size_;
    int timeout_command_factor_;
    int timeout_checkstate_;
    int timeout_startup_;
    int timeout_roughpositioning_;
    int timeout_finepositioning_;
    int timeout_shutdown_;
    double ramp_cmd_inc_time_;
    double ramp_max_delta_;
    int ramp_power_delta_;
    bool ramp_activated_;
};

bool operator==(const NSingleConfig &c1, const NSingleConfig& c2);
bool operator!=(const NSingleConfig &c1, const NSingleConfig& c2);

} // namespace

#endif
