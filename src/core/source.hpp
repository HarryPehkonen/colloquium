#pragma once

#include <string>
#include <memory>
#include <optional>

// Forward declaration of context class
class SourceContext;

/**
 * @brief Abstract interface for data sources
 * 
 * Defines a common interface for objects that can provide data as strings.
 * Each implementation can have its own specific way of acquiring the data.
 */
class Source {
public:
    virtual ~Source() = default;

    /**
     * @brief Get the next piece of data from the source
     * 
     * @param context Optional context object providing access to external resources
     * @return std::optional<std::string> The next data string, or std::nullopt if no more data
     */
    virtual std::optional<std::string> get(const std::shared_ptr<SourceContext>& context = nullptr) = 0;

protected:
    Source() = default;
    
    // Prevent copying and moving
    Source(const Source&) = delete;
    Source& operator=(const Source&) = delete;
    Source(Source&&) = delete;
    Source& operator=(Source&&) = delete;
};

/**
 * @brief Context object for providing resources to Source implementations
 * 
 * This class can be extended to provide whatever context/resources
 * are needed by specific Source implementations.
 */
class SourceContext {
public:
    virtual ~SourceContext() = default;

protected:
    SourceContext() = default;
};
