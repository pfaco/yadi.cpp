#include "hdlc.h"
#include "wrapper.h"
#include "cosem.h"

namespace dlms
{

template<typename T>
struct CosemHdlcClient {
    Cosem cosem;
    hdlc::HdlcContext hdlc_ctx;
    hdlc::HdlcParameters hdlc_params;

    bool connect(T& serial) {
        serial.write(hdlc::serialize_snrm(hdlc_params));
        if (!hdlc::parse_snrm_response(hdlc_params, hdlc_ctx, serial.read())) {
            return false;
        }
        serial.write(hdlc::serialize(hdlc_params, hdlc_ctx, serialize_aarq(cosem)));
        return AssociationResult::ACCEPTED == parse_aare(cosem, hdlc::parse(hdlc_params, hdlc_ctx, serial.read()));
    }

    bool disconnect(T& serial) {
        serial.write(hdlc::serialize_disc(hdlc_params));
        return hdlc::parse_disc_response(serial.read());
    }

    bool authenticate(T& serial) {
        serial.write(hdlc::serialize(hdlc_params, hdlc_ctx, serialize_action_request(cosem, {})));
        return true;
    }

    Response get_request(T& serial, const Request &req) {
        serial.write(hdlc::serialize(hdlc_params, hdlc_ctx, serialize_get_request(cosem, req)));
        return parse_get_response(cosem, hdlc::parse(hdlc_params, hdlc_ctx, serial.read()));
    }

    Response set_request(T& serial, const Request &req) {
        serial.write(hdlc::serialize(hdlc_params, hdlc_ctx, serialize_set_request(cosem, req)));
        return parse_set_response(cosem, hdlc::parse(hdlc_params, hdlc_ctx, serial.read()));
    }
};

template<typename T>
struct CosemWrapperClient {
    Cosem cosem;
    wrapper::WrapperParameters wrapper_params;

    bool connect(T& serial) {
        serial.write(wrapper::serialize(wrapper_params, serialize_aarq(cosem)));
        return AssociationResult::ACCEPTED == parse_aare(cosem, wrapper::parse(wrapper_params, serial.read()));
    }

    Response get_request(T& serial, const Request &req) {
        serial.write(wrapper::serialize(wrapper_params, serialize_get_request(cosem, req)));
        return parse_get_response(cosem, wrapper::parse(wrapper_params, serial.read()));
    }

    Response set_request(T& serial, const Request &req) {
        serial.write(wrapper::serialize(wrapper_params, serialize_set_request(cosem, req)));
        return parse_set_response(cosem, wrapper::parse(wrapper_params, serial.read()));
    }
};

}

