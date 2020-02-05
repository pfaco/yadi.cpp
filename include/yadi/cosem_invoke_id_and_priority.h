#pragma once

#include <cstdint>

namespace dlms
{

enum class Priority : uint8_t
{
    NORMAL = 0x00,
    HIGH = 0x80
};

enum class ServiceClass : uint8_t
{
    UNCONFIRMED = 0x00,
    CONFIRMED = 0x40
};

class InvokeIdAndPriority
{
public:
    InvokeIdAndPriority() : InvokeIdAndPriority(0, ServiceClass::CONFIRMED, Priority::HIGH) {
    }

    explicit InvokeIdAndPriority(uint8_t invoke_id_and_priority) {
        invoke_id_ = invoke_id_and_priority & 0x0F;
        service_class_ = (invoke_id_and_priority & 0x40) == 0x40 ? ServiceClass::CONFIRMED : ServiceClass::UNCONFIRMED;
        priority_ = (invoke_id_and_priority & 0x80) == 0x80 ? Priority::HIGH : Priority::NORMAL;
    }

    InvokeIdAndPriority(uint8_t invoke_id, ServiceClass service_class, Priority priority)
        : invoke_id_{invoke_id}, service_class_{service_class}, priority_{priority} {
    }

    uint8_t invoke_id() const { 
        return invoke_id_; 
    }

    ServiceClass service_class() const { 
        return service_class_; 
    }

    Priority priority() const { 
        return priority_;
    }

    uint8_t value() const {
        uint8_t retval = invoke_id_ & 0x0F;
        retval |= static_cast<uint8_t>(service_class_);
        retval |= static_cast<uint8_t>(priority_);
        return retval;
    }

private:
    uint8_t invoke_id_;
    ServiceClass service_class_;
    Priority priority_;
};

}
