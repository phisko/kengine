#pragma once

#include <unordered_map>
#include <string>

namespace kengine::meta {
    struct Attributes {
        struct AttributeInfo;
        using AttributeMap = std::unordered_map<std::string, AttributeInfo>;
        
        struct AttributeInfo {
            std::ptrdiff_t offset;
            size_t size;
            AttributeMap attributes;
        };

        AttributeMap attributes;
    };
}
