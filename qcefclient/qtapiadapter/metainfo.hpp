#pragma once


struct ObjectProperty
{
    std::string name;
    std::string value;
    std::uint32_t type;
};

JSON_AUTO(ObjectProperty, name, value, type);

struct ObjectSignal
{
    std::string sig;
    std::string name;
    std::vector<std::uint32_t> paramTypes;
};

JSON_AUTO(ObjectSignal, sig, name, paramTypes);

struct ObjectMethod
{
    std::string sig;
    std::string name;
    std::vector<std::uint32_t> paramTypes;
    std::uint32_t returnType;
};

JSON_AUTO(ObjectMethod, sig, name, paramTypes, returnType);

struct ObjectMetaInfo
{
    std::string name;
    std::vector<ObjectProperty> properties;
    std::vector<ObjectSignal> events;
    std::vector<ObjectMethod> methods;
};
JSON_AUTO(ObjectMetaInfo, name, properties, events, methods);
