/*
 * File:   main.cpp
 *
 * Main entry point for the simulation, reads the CLI and starts the model.
 */
#include <fmt/format.h>

#include <args.hxx>
#include <iostream>

#include "easylogging++.h"

// need to have execinfo lib
// #include "error_handler.hxx"
#include "Helpers/OSHelpers.h"
#include "Model.h"

// Set this flag to disable Linux / Unix specific code, this should be provided
// via CMake automatically
#define __DISABLE_CRIT_ERR

#ifndef __DISABLE_CRIT_ERR
namespace {
// invoke set_terminate as part of global constant initialization
static const bool SET_TERMINATE = std::set_terminate(crit_err_terminate);
}  // namespace
#endif

// Settings read from the CLI
int job_number = 0;
std::string path(".");

INITIALIZE_EASYLOGGINGPP

void handle_cli(Model *model, int argc, char **argv);

void config_logger() {
  const std::string OUTPUT_FORMAT = "[%level] [%logger] [%host] [%func] [%loc] %msg";

  // Create the default configuration
  el::Configurations default_conf;
  default_conf.setToDefault();
  default_conf.set(el::Level::Debug, el::ConfigurationType::Format, OUTPUT_FORMAT);
  default_conf.set(el::Level::Error, el::ConfigurationType::Format, OUTPUT_FORMAT);
  default_conf.set(el::Level::Fatal, el::ConfigurationType::Format, OUTPUT_FORMAT);
  default_conf.set(el::Level::Trace, el::ConfigurationType::Format, OUTPUT_FORMAT);
  default_conf.set(el::Level::Info, el::ConfigurationType::Format, "[%level] [%logger] %msg");
  default_conf.set(el::Level::Warning, el::ConfigurationType::Format, "[%level] [%logger] %msg");
  default_conf.set(el::Level::Verbose, el::ConfigurationType::Format, "[%level-%vlevel] [%logger] %msg");
  default_conf.setGlobally(el::ConfigurationType::ToFile, "false");
  default_conf.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
  default_conf.setGlobally(el::ConfigurationType::LogFlushThreshold, "100");
  el::Loggers::reconfigureLogger("default", default_conf);
}

int main(const int argc, char **argv) {
#ifndef __DISABLE_CRIT_ERR
  // Set the last chance error handler
  struct sigaction sigact;
  sigact.sa_sigaction = crit_err_hdlr;
  sigact.sa_flags = SA_RESTART | SA_SIGINFO;
  if (sigaction(SIGABRT, &sigact, (struct sigaction *)NULL) != 0) {
    std::cerr << "error setting handler for signal " << SIGABRT << " (" << strsignal(SIGABRT) << ")\n";
    exit(EXIT_FAILURE);
  }
#endif

  auto start = std::chrono::high_resolution_clock::now();

  // Parse the CLI
  auto *m = new Model();
  handle_cli(m, argc, argv);

  // Prepare the logger
  config_logger();
  START_EASYLOGGINGPP(argc, argv);

  // Run the model
  m->initialize();
  m->run();

  // Clean-up and return
  delete m;

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);

  LOG(INFO) << fmt::format("Running times: {}s", duration.count());
  exit(EXIT_SUCCESS);
}

void handle_cli(Model *model, int argc, char **argv) {
  args::ArgumentParser parser("Individual-based simulation for malaria.", "uut47@psu.edu");

  args::Group commands(parser, "commands");
  args::HelpFlag help(commands, "help", "Display this help menu", { 'h', "help" });
  args::ValueFlag<std::string> input_file(commands, "string", "The config file (YAML format). \nEx: MaSim -i input.yml",
                                          { 'i', 'c', "input", "config" });
  args::ValueFlag<int> cluster_job_number(commands, "int", "Cluster job number. \nEx: MaSim -j 1", { 'j' });
  args::ValueFlag<std::string> reporter(commands, "string", "Reporter Type. \nEx: MaSim -r mmc", { 'r' });
  args::ValueFlag<std::string> output_path(
      commands, "string", "Path for output files, default is current directory. \nEx: MaSim -p out", { 'o' });

  // Allow the --v=[int] flag to be processed by START_EASYLOGGINGPP
  args::Group arguments(parser, "verbosity", args::Group::Validators::DontCare, args::Options::Global);
  args::ValueFlag<int> verbosity(arguments, "int", "Sets the current verbosity of the logging, default zero", { "v" });

  try {
    parser.ParseCLI(argc, argv);
  } catch (const args::Help &e) {
    std::cout << e.what() << parser;
    exit(EXIT_SUCCESS);
  } catch (const args::ParseError &e) {
    LOG(ERROR) << fmt::format("{0} {1}", e.what(), parser);
    exit(EXIT_FAILURE);
  } catch (const args::ValidationError &e) {
    LOG(ERROR) << fmt::format("{0} {1}", e.what(), parser);
    exit(EXIT_FAILURE);
  }

  // Check for the existence of the input file, exit if it doesn't exist.
  const auto input = input_file ? args::get(input_file) : "input.yml";
  if (!OsHelpers::file_exists(input)) {
    LOG(ERROR) << fmt::format("File {0} does not exists. Rerun with -h or --help for help.", input);
    exit(EXIT_FAILURE);
  }
  model->set_config_filename(input);

  // Set the remaining values if given
  path = output_path ? args::get(output_path) : path;
  Model::MODEL->set_output_path(path);
  job_number = cluster_job_number ? args::get(cluster_job_number) : 0;
  model->set_cluster_job_number(job_number);
  const auto reporter_type = reporter ? args::get(reporter) : "";
  model->set_reporter_type(reporter_type);
}
