#pragma once

#include <vector>
#include <string>
#include <functional>
#include <yadi/cosem_response.h>
#include <unordered_map>
#include <memory>

namespace dlms
{

class CosemParser
{
public:
    explicit CosemParser(const std::vector<uint8_t> &buffer);
    virtual ~CosemParser();

    int8_t int8();
    int16_t int16();
    int32_t int32();
    int64_t int64();

    uint8_t uint8();
    uint16_t uint16();
    uint32_t uint32();
    uint64_t uint64();

    float float32();
    double float64();

    std::string visible_string();
    std::vector<uint8_t> octet_string();
    std::vector<bool> bit_string();

    size_t array_size();
    size_t struct_size();

    bool boolean();

    uint8_t enumeration();

    uint8_t raw_uint8();
    void all_available_raw_data(std::vector<uint8_t> &buffer);

    void check_response(ResponseTag tag, ResponseType type);

    bool optional();

    void parse_by_index(const std::unordered_map<int, std::function<void(void)>> &callback);

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

struct CosemParserError : std::exception {
    const char * what() const noexcept override {
        return "COSEM parser error";
    }
};

static inline void parse(CosemParser &parser, int8_t &value) { value = parser.int8(); }
static inline void parse(CosemParser &parser, int16_t &value) { value = parser.int16(); }
static inline void parse(CosemParser &parser, int32_t &value) { value = parser.int32(); }
static inline void parse(CosemParser &parser, int64_t &value) { value = parser.int64(); }
static inline void parse(CosemParser &parser, uint8_t &value) { value = parser.uint8(); }
static inline void parse(CosemParser &parser, uint16_t &value) { value = parser.uint16(); }
static inline void parse(CosemParser &parser, uint32_t &value) { value = parser.uint32(); }
static inline void parse(CosemParser &parser, uint64_t &value) { value = parser.uint64(); }
static inline void parse(CosemParser &parser, float &value) { value = parser.float32(); }
static inline void parse(CosemParser &parser, double &value) { value = parser.float64(); }
static inline void parse(CosemParser &parser, bool &value) { value = parser.boolean(); }
static inline void parse(CosemParser &parser, std::string &value) { value = parser.visible_string(); }
static inline void parse(CosemParser &parser, std::vector<uint8_t> &value) { value = parser.octet_string(); }
static inline void parse(CosemParser &parser, std::vector<bool> &value) { value = parser.bit_string(); }

static inline void parse(CosemParser &parser, RawResponseBody &value) {
    parser.all_available_raw_data(value.value);
}

template<typename Body = RawResponseBody>
GetResponse<Body> parse_get_normal(CosemParser &parser) {
    parser.check_response(ResponseTag::GET_RESPONSE, ResponseType::NORMAL);
    GetResponse<Body> resp{InvokeIdAndPriority(parser.raw_uint8()), {}, {}};
    parser.parse_by_index({
        {0x00, [&] {
            resp.result = DataAccessResult::SUCCESS;
            resp.body = Body{};
            ::dlms::parse(parser, resp.body);
        }},
        {0x01, [&] {
            resp.result = ::dlms::make_data_access_result(parser.raw_uint8());
        }}
    });
    return resp;
}

static inline SetResponse parse_set_normal(CosemParser &parser) {
    parser.check_response(ResponseTag::SET_RESPONSE, ResponseType::NORMAL);
    return SetResponse{
        InvokeIdAndPriority(parser.raw_uint8()),
        ::dlms::make_data_access_result(parser.raw_uint8())
    };
}

template<typename Body = RawResponseBody>
ActionResponse<Body>  parse_action_normal(CosemParser &parser) {
    parser.check_response(ResponseTag::ACTION_RESPONSE, ResponseType::NORMAL);
    ActionResponse<Body> resp;
    resp.invoke_id_and_priority = InvokeIdAndPriority(parser.raw_uint8());
    resp.result = ::dlms::make_action_access_result(parser.raw_uint8());
    if (parser.optional()) {
        parser.parse_by_index({
            {0x00, [&](){
                resp.data_access_result = DataAccessResult::SUCCESS;
                ::dlms::parse(parser, resp.body);
            }},
            {0x01, [&](){
                resp.data_access_result = ::dlms::make_data_access_result(parser.raw_uint8());
            }}
        });
    }
    return resp;
}

}
