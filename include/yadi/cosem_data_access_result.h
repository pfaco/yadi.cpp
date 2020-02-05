#pragma once

#include <cstdint>

namespace dlms
{

enum class DataAccessResult : uint8_t {
    SUCCESS = 0,
    HARDWARE_FAULT = 1,
    TEMPORARY_FAILURE = 2,
    READ_WRITE_DENIED = 3,
    OBJECT_UNDEFINED = 4,
    OBJECT_CLASS_INCONSISTENT = 9,
    OBJECT_UNAVAILABLE = 11,
    TYPE_UNMATCHED = 12,
    SCOPE_OF_ACCESS_VIOLATED = 13,
    DATA_BLOCK_UNAVAILABLE = 14,
    LONG_GET_ABORTED = 15,
    NO_LONG_GET_IN_PROGRESS = 16,
    LONG_SET_ABORTED = 17,
    NO_LONG_SET_IN_PROGRESS = 18,
    DATA_BLOCK_NUMBER_INVALID = 19,
    OTHER_REASON = 250,
    UNKNOWN_REASON = 255
};

enum class ActionAccessResult : uint8_t {
    SUCCESS = 0,
    HARDWARE_FAULT = 1,
    TEMPORARY_FAILURE = 2,
    READ_WRITE_DENIED = 3,
    OBJECT_UNDEFINED = 4,
    OBJECT_CLASS_INCONSISTENT = 9,
    OBJECT_UNAVAILABLE = 11,
    TYPE_UNMATCHED = 12,
    SCOPE_OF_ACCESS_VIOLATED = 13,
    DATA_BLOCK_UNAVAILABLE = 14,
    LONG_ACTION_ABORTED = 15,
    NO_LONG_ACTION_IN_PROGRESS = 16,
    OTHER_REASON = 250,
    UNKNOWN_REASON = 255
};

static inline DataAccessResult make_data_access_result(uint8_t value) {
    if (value > 4 && value < 9) {
        return DataAccessResult::UNKNOWN_REASON;
    }

    if (value == 10) {
        return DataAccessResult::UNKNOWN_REASON;
    }

    if (value > 20 && value < 250) {
        return DataAccessResult::UNKNOWN_REASON;
    }

    if (value > 250) {
        return DataAccessResult::UNKNOWN_REASON;
    }

    return static_cast<DataAccessResult>(value);
}

static inline ActionAccessResult make_action_access_result(uint8_t value) {
    if (value > 4 && value < 9) {
        return ActionAccessResult::UNKNOWN_REASON;
    }

    if (value == 10) {
        return ActionAccessResult::UNKNOWN_REASON;
    }

    if (value > 16 && value < 250) {
        return ActionAccessResult::UNKNOWN_REASON;
    }

    if (value > 250) {
        return ActionAccessResult::UNKNOWN_REASON;
    }

    return static_cast<ActionAccessResult>(value);
}

static inline const char * data_access_result_to_string(DataAccessResult result) {
    return "data access result";
}

static inline const char * action_access_result_to_string(ActionAccessResult result) {
    return "action access result";
}

}
