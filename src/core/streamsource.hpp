#include "source.hpp"
#include <istream>

/**
 * @brief Implementation of Source that reads from a stream
 * 
 * This class reads data line by line from any input stream,
 * including files, stdin, or other stream sources.
 */
class StreamSource : public Source {
public:
    /**
     * @brief Construct a new StreamSource
     * 
     * @param stream Reference to an input stream. The stream must remain valid
     *              for the lifetime of the StreamSource object.
     */
    explicit StreamSource(std::istream& stream) : stream_(stream) {}

    /**
     * @brief Get the next line from the stream
     * 
     * @param context Optional context object (unused in this implementation)
     * @return std::optional<std::string> The next line from the stream, or std::nullopt
     *         if end of stream is reached or an error occurs
     */
    std::optional<std::string> get(const std::shared_ptr<SourceContext>& context = nullptr) override {
        std::string line;
        if (std::getline(stream_, line)) {
            return line;
        }
        return std::nullopt;
    }

private:
    std::istream& stream_;
};
