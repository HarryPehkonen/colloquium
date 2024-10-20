#include "translator/openai_translator.hpp"
#include "http_client/include/http_client/ihttp_client.hpp"
#include "http_client/src/curl_http_client.hpp"
#include "http_client/include/http_client/http_client_exception.hpp"
#include "core/message.hpp"
#include "core/tool.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <iostream>
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

std::string theTool(std::string paramsJson) {
    std::cout << "The tool is being called" << std::endl;
    nlohmann::json params = nlohmann::json::parse(paramsJson);
    // return "The weather in " + params["location"].get<std::string>() + " is sunny.";
    return "Sunny";
}

int main(int argc, char* argv[]) {
    // Set up logging
    auto console = spdlog::stdout_color_mt("console");
    spdlog::set_default_logger(console);

    // Set log level based on command line argument
    if (argc > 1 && std::string(argv[1]) == "--debug") {
        spdlog::set_level(spdlog::level::debug);
        spdlog::debug("Debug logging enabled");
    } else {
        spdlog::set_level(spdlog::level::info);
    }

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
            throw std::runtime_error(apiKeyName + " environment variable not set");
        }

        spdlog::info("Creating HTTP client");
        auto httpClient = std::make_unique<http_client::CurlHTTPClient>();

        spdlog::info("Creating translator");
        auto translator = std::make_unique<OpenAITranslator>();

        spdlog::info("Creating the conversation");
        std::vector<std::unique_ptr<Message>> conversation;

        auto systemMessage = std::make_unique<Message>(Message::Type::System);
        systemMessage->content = R"(You are a helpful assistant.  If you are asked for the weather, please use the "get_weather" tool to get the current weather for any given location.)";
        conversation.push_back(std::move(systemMessage));

        auto userMessage = std::make_unique<Message>(Message::Type::User);
        userMessage->content = "What's the weather like today?  I'm in Vancouver, BC, Canada.";
        userMessage->model = modelName;

        // not for venice.ai
        // userMessage->tool_choice = "required";
        conversation.push_back(std::move(userMessage));

        spdlog::info("Creating weather tool");
        Tool weatherTool;
        weatherTool.name = "get_weather";
        weatherTool.description = "Get the current weather for a location";
        weatherTool.function = theTool;

        Parameter locationParam;
        locationParam.name = "location";
        locationParam.type = "string";
        locationParam.description = "The city and state, e.g. San Francisco, CA";
        locationParam.required = true;

        weatherTool.parameters.push_back(locationParam);

        std::vector<Tool> tools = {weatherTool};

        do {
            spdlog::info("Creating request using the translator");
            std::string requestBody = translator->createRequest(conversation, tools);
            spdlog::debug("Request body: {}", requestBody);

            spdlog::info("Setting up headers");
            std::string authHeader = "Authorization: Bearer " + std::string(apiKey);
            std::vector<std::string> headers = {
                "Content-Type: application/json",
                "Accept: application/json",
                authHeader
            };
            spdlog::debug("Headers: {}", vectorToString(headers, "\n"));

            spdlog::info("Sending request asynchronously");
            std::future<http_client::HTTPResponse> responseFuture = 
                httpClient->Post(chatUri, requestBody, headers);

            spdlog::info("Waiting for the response");
            http_client::HTTPResponse response = responseFuture.get();

            if (response.statusCode != 200) {
                spdlog::error("Received error status code: {}", response.statusCode);
                spdlog::error("Response body: {}", response.body);
                return 1;
            }
            spdlog::info("Received successful response");
            spdlog::debug("Response body: {}", response.body);

            auto responseMessage = translator->responseToMessage(response.body);
            if (!responseMessage) {
                spdlog::error("Failed to parse the response.");
                return 1;
            }
            // make a copy for the conversation
            auto responseMessageCopy = std::make_unique<Message>(*responseMessage);
            conversation.push_back(std::move(responseMessageCopy));

            std::cout << "************************************************************************" << std::endl;
            for (const auto& message : conversation) {
                std::cout << "Message:\n" << message->to_string() << std::endl;
            }

            if (responseMessage->tool_calls.size() == 0) {
                std::cout << "Done!" << std::endl;
                break;
            }
            for (const std::map<std::string, std::string>& tool_call : responseMessage->tool_calls) {
                std::cout << "Tool Call: " << std::endl;

                // print out all fields for the tool call from the
                // request
                for (const auto& [key, value] : tool_call) {
                    std::cout << "\t" << key << ": " << value << std::endl;
                }

                // which tool name is being called?
                auto toolNameIt = tool_call.find("name");
                if (toolNameIt == tool_call.end()) {
                    spdlog::error("Tool call does not have a name");
                    continue;
                }
                std::string toolName = toolNameIt->second;
                std::cout << "we are looking for " << toolName << std::endl;

                auto toolCallIdIt = tool_call.find("id");
                if (toolCallIdIt == tool_call.end()) {
                    spdlog::error("Tool call does not have an id");
                }
                std::string toolCallId = toolCallIdIt->second;

                // find the tool by name
                auto toolIt = std::find_if(tools.begin(), tools.end(), 
                    [&toolName](const Tool& tool) { return tool.name == toolName; }
                ); 
                if (toolIt == tools.end()) {
                    spdlog::error("Didn't find the tool");
                    continue;
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

                // segfault:
                // toolResultMessage->model = userMessage->model;
                toolResultMessage->model = modelName;


                // add the tool result message to the conversation
                conversation.push_back(std::move(toolResultMessage));

                // print out the conversation so far
                std::cout << "************************************************************************" << std::endl;
                std::cout << "should we go again?" << std::endl;
                for (const auto& message : conversation) {
                    std::cout << "Message:\n" << message->to_string() << std::endl;
                }
            }
        } while (true);
    } catch (const http_client::HTTPClientException& e) {
        spdlog::error("HTTP Client error: {}", e.what());
    } catch (const std::exception& e) {
        spdlog::error("Error: {}", e.what());
    }

    return 0;
}
