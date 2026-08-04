#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/callback_sink.h>
#include <glim/util/logging.hpp>
#include <glim/util/extension_module.hpp>

namespace glim {

/// @brief A demo extension module that hooks into all loggers and prints log messages to the console.
/// @note  This module is assumed to be loaded after all other modules, so that it can hook into all loggers.
class LoggerHookDemo : public ExtensionModule {
public:
  LoggerHookDemo() : logger(create_module_logger("loghook")) {
    // Message callback sink to be added to all loggers
    auto sink = std::make_shared<spdlog::sinks::callback_sink_mt>([this](const auto& msg) { msg_callback(msg); });

    // Add the callback sink to all existing loggers
    spdlog::apply_all([this, sink](std::shared_ptr<spdlog::logger> logger) {
      if (logger->name() == "loghook") {
        return;
      }

      this->logger->info("Adding callback sink to logger: {}", logger->name());
      logger->sinks().push_back(sink);
    });
  }

  ~LoggerHookDemo() {}

  /// @brief Log message callback function
  /// @param msg  Log message
  void msg_callback(const spdlog::details::log_msg& msg) {  //
    const std::string_view logger_name(msg.logger_name.data(), msg.logger_name.size());
    const std::string level = fmt::format(spdlog::level::to_string_view(msg.level));
    const std::string_view payload(msg.payload.data(), msg.payload.size());
    std::cout << "[LoggerHookDemo] " << logger_name << " [" << level << "] " << payload << std::endl;
  }

private:
  // Logger
  std::shared_ptr<spdlog::logger> logger;
};
}  // namespace glim

extern "C" glim::ExtensionModule* create_extension_module() {
  return new glim::LoggerHookDemo();
}