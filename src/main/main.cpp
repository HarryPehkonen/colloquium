#include "translator/openai_translator.hpp"
#include "http_client/ihttp_client.hpp"
#include "http_client/curl_http_client.hpp"
#include "exceptions/llm_exceptions.hpp"
#include "core/message.hpp"
#include "core/tool.hpp"
#include "core/source.hpp"
#include "core/streamsource.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <async_deque/async_deque.hpp>

#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <future>
#include <cstdlib>

#include <numeric>
#include <optional>

std::string vectorToString(const std::vector<std::string>& vec, const std::string& delimiter = "") {
    if (vec.empty()) return "";
    return std::accumulate(std::next(vec.begin()), vec.end(), vec[0],
        [&delimiter](const std::string& a, const std::string& b) {
            return a + delimiter + b;
        });
}

std::string theWeatherToolCallback(std::string paramsJson) {
    std::cout << "The tool is being called" << std::endl;
    nlohmann::json params = nlohmann::json::parse(paramsJson);
    return "Sunny and 75 degrees fahrenheit";
}

std::string fahrenheitToCelsius(std::string paramsJson) {
    nlohmann::json params = nlohmann::json::parse(paramsJson);
    if (params.find("fahrenheit") == params.end()) {
        return "Missing fahrenheit parameter";
    }
    double fahrenheit = params["fahrenheit"];
    double celsius = (fahrenheit - 32) * 5 / 9;
    return std::to_string(celsius);
}

void run(Source& source, std::vector<std::unique_ptr<Message>>& conversation) {
    try {

        // const std::string chatUri = "https://api.venice.ai/api/v1/chat/completions";
        // const std::string apiKeyName = "VENICE_API_KEY";

        // image:
        // const std::string modelName = "fluently-xl";
        // const std::string modelName = "flux-dev";
        // const std::string modelName = "flux-dev-uncensored";

        // text:
        // const std::string modelName = "nous-theta-8b";
        // const std::string modelName = "llama-3.2-3b";
        // const std::string modelName = "dolphin-2.9.2-qwen2-72b";

        const std::string chatUri = "https://api.mistral.ai/v1/chat/completions";
        const std::string apiKeyName = "MISTRAL_API_KEY";

        // const std::string modelName = "codestral-2405";
        // const std::string modelName = "codestral-latest";
        // const std::string modelName = "codestral-mamba-2407";
        // const std::string modelName = "codestral-mamba-latest";
        // const std::string modelName = "ministral-3b-2410";
        // const std::string modelName = "ministral-3b-latest";
        // const std::string modelName = "ministral-8b-2410";
        const std::string modelName = "ministral-8b-latest";
        // const std::string modelName = "mistral-embed";
        // const std::string modelName = "mistral-large-2402";
        // const std::string modelName = "mistral-large-2407";
        // const std::string modelName = "mistral-large-latest";
        // const std::string modelName = "mistral-medium";
        // const std::string modelName = "mistral-medium-2312";
        // const std::string modelName = "mistral-medium-latest";
        // const std::string modelName = "mistral-small";
        // const std::string modelName = "mistral-small-2312";
        // const std::string modelName = "mistral-small-2402";
        // const std::string modelName = "mistral-small-2409";
        // const std::string modelName = "mistral-small-latest";
        // const std::string modelName = "mistral-tiny";
        // const std::string modelName = "mistral-tiny-2312";
        // const std::string modelName = "mistral-tiny-2407";
        // const std::string modelName = "mistral-tiny-latest";
        // const std::string modelName = "open-codestral-mamba";
        // const std::string modelName = "open-mistral-7b";
        // const std::string modelName = "open-mistral-nemo";
        // const std::string modelName = "open-mistral-nemo-2407";
        // const std::string modelName = "open-mixtral-8x22b";
        // const std::string modelName = "open-mixtral-8x22b-2404";
        // const std::string modelName = "open-mixtral-8x7b";
        // const std::string modelName = "pixtral-12b";
        // const std::string modelName = "pixtral-12b-2409";
        // const std::string modelName = "pixtral-12b-latest";


        // Get API key from environment variable
        const char* apiKey = std::getenv(apiKeyName.c_str());
        if (apiKey == nullptr) {
            spdlog::error(apiKeyName + " environment variable not set");
            throw llm::LLMException(apiKeyName + " environment variable not set");
        }

        auto httpClient = std::make_unique<http_client::CurlHTTPClient>();
        auto translator = std::make_unique<OpenAITranslator>();

        spdlog::info("Creating weather tool");
        Tool weatherTool;
        weatherTool.name = "get_weather";
        weatherTool.description = "Get the current weather for a location";
        weatherTool.function = theWeatherToolCallback;

        Parameter locationParam;
        locationParam.name = "location";
        locationParam.type = "string";
        locationParam.description = "The city and state, e.g. San Francisco, CA";
        locationParam.required = true;

        weatherTool.parameters.push_back(locationParam);

        Tool fahrenheitToCelsiusTool;
        fahrenheitToCelsiusTool.name = "fahrenheit_to_celsius";
        fahrenheitToCelsiusTool.description = "Converts a temperature in Fahrenheit to Celsius";
        fahrenheitToCelsiusTool.function = fahrenheitToCelsius;

        Parameter fahrenheitParam;
        fahrenheitParam.name = "fahrenheit";
        fahrenheitParam.type = "number";
        fahrenheitParam.description = "The temperature in Fahrenheit";
        fahrenheitParam.required = true;

        fahrenheitToCelsiusTool.parameters.push_back(fahrenheitParam);

        std::vector<Tool> tools = {weatherTool, fahrenheitToCelsiusTool};

        async_deque::AsyncDeque<std::unique_ptr<Message>> cache;

        do {
            std::cerr << "Looping..." << std::endl;

            // we need the next message.  we will either get that from
            // the cache, or create a new one from a prompt from the
            // source.
            std::unique_ptr<Message> message{nullptr};

            {
                std::optional<std::string> prompt{std::nullopt};

                // do we have any cached messages to process before getting
                // the next prompt?
                if (!cache.empty()) {
                    message = std::move(*(cache.pop_front()));
                    std::cerr << "Got a message from the cache: " << message->content << std::endl;
                } else {

                    // if there was nothing in the cache. get the next prompt
                    prompt = source.get();

                    if (!prompt) {

                        // no next prompt, so we must be done
                        std::cerr << "script is done." << std::endl;
                        break;
                    }

                    // if prompt starts with #system, then it is a system
                    // message, otherwise it is a user message
                    if (prompt->find("#system") == 0) {
                        message = std::make_unique<Message>(Message::Type::System);
                        message->content = prompt->substr(7);
                        std::cerr << "System message: " << message->content << std::endl;

                        // system messages are not sent to the API alone
                        // so we continue to the next iteration
                        conversation.push_back(std::move(message));
                        continue;
                    } else {
                        message = std::make_unique<Message>(Message::Type::User);
                        message->content = *prompt;
                        std::cerr << "User message: " << message->content << std::endl;
                    }
                    message->model = modelName;
                }
            }

            conversation.push_back(std::move(message));

            std::string requestBody = translator->createRequest(conversation, tools);
            std::string authHeader = "Authorization: Bearer " + std::string(apiKey);
            std::vector<std::string> headers = {
                "Content-Type: application/json",
                "Accept: application/json",
                authHeader
            };

            std::cerr << "Sending request" << std::endl;
            std::future<http_client::HTTPResponse> responseFuture =
                httpClient->Post(chatUri, requestBody, headers);

            http_client::HTTPResponse response = responseFuture.get();

            if (response.statusCode != 200) {
                std::cerr << "Received error status code: " << response.statusCode << std::endl;
                std::cerr << "Response body:\n" << response.body << std::endl;
                return;
            }

            auto responseMessage = translator->responseToMessage(response.body);
            if (!responseMessage) {
                std::cerr << "Failed to parse the response." << std::endl;
                return;
            }

            {
                // make a copy for the conversation
                auto responseMessageCopy = std::make_unique<Message>(*responseMessage);
                conversation.push_back(std::move(responseMessageCopy));
            }

            if (responseMessage->tool_calls.size() == 0) {
                std::cout << responseMessage->content << std::endl;
            }
            for (const std::map<std::string, std::string>& tool_call : responseMessage->tool_calls) {

                // which tool name is being called?
                auto toolNameIt = tool_call.find("name");
                if (toolNameIt == tool_call.end()) {
                    throw llm::LLMException("Tool call does not have a name");
                }
                std::string toolName = toolNameIt->second;

                auto toolCallIdIt = tool_call.find("id");
                if (toolCallIdIt == tool_call.end()) {
                    throw llm::LLMException("Tool call does not have an id");
                }
                std::string toolCallId = toolCallIdIt->second;

                // find the tool by name
                auto toolIt = std::find_if(tools.begin(), tools.end(),
                    [&toolName](const Tool& tool) { return tool.name == toolName; }
                );
                if (toolIt == tools.end()) {
                    throw llm::LLMException("Didn't find the tool");
                }
                Tool tool = *toolIt;

                // get the arguments
                auto toolArgsIt = tool_call.find("arguments");
                if (toolArgsIt == tool_call.end()) {
                    spdlog::error("Tool call does not have arguments");
                }
                std::string toolArgsJson = toolArgsIt->second;

                // call the tool
                std::string toolResult = tool.function(toolArgsJson);

                // create a new message with the tool result
                auto toolResultMessage = std::make_unique<Message>(Message::Type::ToolResult);
                toolResultMessage->content = toolResult;
                toolResultMessage->name = toolName;
                toolResultMessage->tool_call_id = toolCallId;

                toolResultMessage->model = modelName;


                // add the tool result message to the conversation
                cache.push_front(std::move(toolResultMessage));
                spdlog::info("tool result saved in conversation");
            }
        } while (true);
    } catch (const llm::HTTPException& e) {
        std::cerr << "HTTP Error: " << e.what() << std::endl;
    } catch (const llm::TranslationException& e) {
        std::cerr << "Translation Error: " << e.what() << std::endl;
    } catch (const llm::LLMException& e) {
        std::cerr << "LLM Error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {

    std::vector<std::string> filenames;
    std::optional<std::string> logLevel = std::nullopt;
    enum class ArgvState {
        None,
        Filename,
        LogLevel
    } argvState = ArgvState::None;
    for (int i = 1; i < argc; i++) {
        if (argvState == ArgvState::None) {
            if (std::string(argv[i]) == "--filename") {
                argvState = ArgvState::Filename;
            } else if (std::string(argv[i]) == "--log-level") {
                argvState = ArgvState::LogLevel;
            } else {
                std::cerr << "Unknown argument: " << argv[i] << std::endl;
                return 1;
            }
        } else if (argvState == ArgvState::Filename) {
            std::string filename = std::string(argv[i]);
            filenames.push_back(filename);
            argvState = ArgvState::None;
        } else if (argvState == ArgvState::LogLevel) {
            logLevel = std::string(argv[i]);
            argvState = ArgvState::None;
        }
    }
    if (argvState != ArgvState::None) {
        std::cerr << "Missing argument value" << std::endl;
        return 1;
    }

    // Set up logging
    auto console = spdlog::stdout_color_mt("console");
    spdlog::set_default_logger(console);

    // Set log level based on command line argument
    if (logLevel) {
        if (logLevel == "--debug") {
            spdlog::set_level(spdlog::level::debug);
            spdlog::debug("Debug logging enabled");
        }
    }

    std::ifstream file(filenames[0]);

    // Check if file opened successfully
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filenames[0]);
    }

    StreamSource source(file);

    std::vector<std::unique_ptr<Message>> conversation;

    run(source, conversation);
    return 0;
}
