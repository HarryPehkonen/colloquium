// src/main/main.cpp
#include "session/llm_session.hpp"
#include "translator/openai_translator.hpp"
#include "http_client/curl_http_client.hpp"
#include "core/streamsource.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

Tool createWeatherTool() {
    Tool tool;
    tool.name = "get_weather";
    tool.description = "Get the current weather for a location";
    tool.function = [](std::string paramsJson) {
        spdlog::debug("Weather tool called with: {}", paramsJson);
        std::string result{"Sunny and 75 degrees fahrenheit"};
        spdlog::debug("Weather tool result: {}", result);
        return result;
    };

    Parameter location;
    location.name = "location";
    location.type = "string";
    location.description = "The city and state, e.g. San Francisco, CA";
    location.required = true;

    tool.parameters.push_back(location);
    return tool;
}

Tool createTemperatureConverterTool() {
    Tool tool;
    tool.name = "fahrenheit_to_celsius";
    tool.description = "Converts a temperature in Fahrenheit to Celsius";
    tool.function = [](std::string paramsJson) -> std::string {
        spdlog::debug("Temperature converter tool called with: {}", paramsJson);
        nlohmann::json params = nlohmann::json::parse(paramsJson);
        if (params.find("fahrenheit") == params.end()) {
            return "Missing fahrenheit parameter";
        }
        double fahrenheit = params["fahrenheit"];
        double celsius = (fahrenheit - 32) * 5 / 9;
        spdlog::debug("Temperature converter tool result: {}", celsius);
        return std::to_string(celsius);
    };

    Parameter param;
    param.name = "fahrenheit";
    param.type = "number";
    param.description = "The temperature in Fahrenheit";
    param.required = true;

    tool.parameters.push_back(param);
    return tool;
}

void setupLogging(const std::optional<std::string>& logLevel) {
    auto console = spdlog::stdout_color_mt("console");
    spdlog::set_default_logger(console);
    
    if (logLevel && *logLevel == "debug") {
        spdlog::set_level(spdlog::level::debug);
        spdlog::debug("Debug logging enabled");
    }
}

std::vector<std::string> parseCommandLineArgs(int argc, char* argv[]) {
    std::vector<std::string> filenames;
    
    for (int i = 1; i < argc; i += 2) {
        if (i + 1 >= argc) {
            throw std::runtime_error("Missing argument value");
        }
        
        std::string arg = argv[i];
        std::string value = argv[i + 1];
        
        if (arg == "--filename") {
            filenames.push_back(value);
        } else if (arg == "--log-level") {
            setupLogging(value);
        } else {
            throw std::runtime_error("Unknown argument: " + arg);
        }
    }
    
    if (filenames.empty()) {
        throw std::runtime_error("No input file specified");
    }
    
    return filenames;
}

int main(int argc, char* argv[]) {
    try {
        auto filenames = parseCommandLineArgs(argc, argv);
        
        std::ifstream file(filenames[0]);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + filenames[0]);
        }
        StreamSource source(file);

        // Create session with basic dependencies
        auto session = LLMSession(
            std::make_unique<http_client::CurlHTTPClient>()
        );

        // Add available tools
        session.addTool(createWeatherTool());
        session.addTool(createTemperatureConverterTool());

        // Process all messages from the source
        session.processMessages(source);
        
        return 0;
    } 
    catch (const std::exception& e) {
        spdlog::error("Error: {}", e.what());
        return 1;
    }
}
