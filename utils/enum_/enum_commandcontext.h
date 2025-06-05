#ifndef ENUM_COMMAND_CONTEXT_H
#define ENUM_COMMAND_CONTEXT_H
namespace utils::enum_ {
enum class CommandContext { 
    GENERAL, 
    NETWORK, 
    SYSTEM, 
    SECURITY,
    DEVICE_CONTROL,
    HARDWARE };
}

#endif//ENUM_COMMAND_CONTEXT_H