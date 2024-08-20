#pragma once
#include "../pch.hpp"

namespace JFMService
{
    template <class T, class ConstructingObject>
    concept Loader = requires( T &loader, ConstructingObject object, const std::filesystem::path &path) {
        { loader.load(path) } -> std::convertible_to<ConstructingObject>;
        { loader.checkExtensionCompatibility(path) } -> std::same_as<bool>;
        { loader.getLoaderName() } ->std::convertible_to<std::string>;
    };
    template <class T, class SerializingObject>
    concept Serializer = requires(const T &serializer, SerializingObject serializing) {
        { serializer.serialize(serializing) } -> std::convertible_to<std::string>;
    };

}