#include "NSingleConfig.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

NSingleConfig::NSingleConfig(QString name, QString ip, int port,
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
                             bool ramp_activated)
    : name_(name), ip_(ip), port_(port), ch1_setpoint_config_(ch1_setpoint_config),
      ch1_value_config_(ch1_value_config), state_reply_config_(state_reply_config),
      state_command_config_(state_command_config), low_level_config_(low_level_config),
      multiplexed_(multiplexed),
      multichannel_(multichannel),
      near_zero_limit_(near_zero_limit),
      near_target_limit_(near_target_limit),
      polarity_change_allowed_(polarity_change_allowed),
      hot_polarity_change_(hot_polarity_change), reset_at_on_(reset_at_on),
      switchoff_on_shutdown_(switchoff_on_shutdown),
      verify_setpoints_(verify_setpoints),
      active_compensation_(active_compensation),
      signal_variation_(signal_variation), upper_limit_(upper_limit),
      read_value_interval_(read_value_interval), read_state_interval_(read_state_interval),
      regulation_buffer_size_(regulation_buffer_size),
      timeout_command_factor_(timeout_command_factor),
      timeout_checkstate_(timeout_checkstate),
      timeout_startup_(timeout_startup),
      timeout_roughpositioning_(timeout_roughpositioning),
      timeout_finepositioning_(timeout_finepositioning),
      timeout_shutdown_(timeout_shutdown),
      ramp_cmd_inc_time_(ramp_cmd_inc_time),
      ramp_max_delta_(ramp_max_delta),
      ramp_power_delta_(ramp_power_delta),
      ramp_activated_(ramp_activated) {}

bool operator==(const NSingleConfig &c1, const NSingleConfig& c2) {
    return (c1.name_ == c2.name_ &&
            c1.ip_ == c2.ip_ &&
            c1.port_ == c2.port_ &&
            c1.ch1_setpoint_config_ == c2.ch1_setpoint_config_ &&
            c1.ch1_value_config_ == c2.ch1_value_config_ &&
            c1.state_reply_config_ == c2.state_reply_config_ &&
            c1.state_command_config_ == c2.state_command_config_ &&
            c1.low_level_config_ == c2.low_level_config_ &&
            c1.multiplexed_ == c2.multiplexed_ &&
            c1.polarity_change_allowed_ == c2.polarity_change_allowed_ &&
            c1.hot_polarity_change_ == c2.hot_polarity_change_ &&
            c1.reset_at_on_ == c2.reset_at_on_ &&
            c1.switchoff_on_shutdown_ == c2.switchoff_on_shutdown_ &&
            c1.verify_setpoints_ == c2.verify_setpoints_ &&
            c1.active_compensation_ == c2.active_compensation_ &&
            c1.signal_variation_ == c2.signal_variation_ &&
            (c1.upper_limit() - c2.upper_limit()) < 0.001 &&
            c1.read_value_interval_ == c2.read_value_interval_ &&
            c1.read_state_interval_ == c2.read_state_interval_ &&
            c1.regulation_buffer_size_ == c2.regulation_buffer_size_ &&
            c1.timeout_command_factor_ == c2.timeout_command_factor_ &&
            c1.timeout_checkstate_ == c2.timeout_checkstate_ &&
            c1.timeout_startup_ == c2.timeout_startup_ &&
            c1.timeout_roughpositioning_ == c2.timeout_roughpositioning_ &&
            c1.timeout_finepositioning_ == c2.timeout_finepositioning_ &&
            c1.timeout_shutdown_ == c2.timeout_shutdown_ &&
            std::abs(c1.ramp_cmd_inc_time_ - c2.ramp_cmd_inc_time_) < 0.001 &&
            std::abs(c1.ramp_max_delta_ - c2.ramp_max_delta_) < 0.001 &&
            c1.ramp_power_delta_ == c2.ramp_power_delta_ &&
            c1.ramp_activated_ == c2.ramp_activated_);
}

bool operator!=(const NSingleConfig &c1, const NSingleConfig& c2) {
    return !(c1 == c2);
}

} // ns
