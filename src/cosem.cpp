/*
 * This file is part of the yadi.cpp project.
 *
 * Copyright (C) 2017 Paulo Faco <paulofaco@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <yadi/cosem.h>
#include <yadi/parser.h>
#include "security.h"

namespace dlms
{

static auto serialize_request_normal(Request const& req, uint8_t tag) -> std::vector<uint8_t>;

/*
 * Application Association Request - AARQ
 *
 * AARQ is the message sent from the client to the server in order to establish an application association.
 * In the AARQ frame the client sends all the proposed services that it wants to use, like get, set and action.
 *
 * The format of the AARQ frame is shown below (from Green Book edition 8.1)
 *
 * AARQ ::= [APPLICATION 0] IMPLICIT SEQUENCE
 * {
 * -- [APPLICATION 0] == [ 60H ] = [ 96 ]
 *     protocol-version             [0] IMPLICIT BIT STRING {version1 (0)} DEFAULT {version1},
 *     application-context-name     [1]          Application-context-name,
 *     called-AP-title              [2]          AP-title OPTIONAL,
 *     called-AE-qualifier          [3]          AE-qualifier OPTIONAL,
 *     called-AP-invocation-id      [4]          AP-invocation-identifier OPTIONAL,
 *     called-AE-invocation-id      [5]          AE-invocation-identifier OPTIONAL,
 *     calling-AP-title             [6]          AP-title OPTIONAL,
 *     calling-AE-qualifier         [7]          AE-qualifier OPTIONAL,
 *     calling-AP-invocation-id     [8]          AP-invocation-identifier OPTIONAL,
 *     calling-AE-invocation-id     [9]          AE-invocation-identifier OPTIONAL,
 *
 * -- The following field shall not be present if only the kernel is used.
 *     sender-acse-requirements     [10] IMPLICIT ACSE-requirements OPTIONAL,
 *
 * -- The following field shall only be present if the authentication functional unit is selected.
 *     mechanism-name               [11] IMPLICIT Mechanism-name OPTIONAL,
 *
 * -- The following field shall only be present if the authentication functional unit is selected.
 *     calling-authentication-value [12] EXPLICIT Authentication-value OPTIONAL,
 *     implementation-information   [29] IMPLICIT Implementation-data OPTIONAL,
 *     user-information             [30] EXPLICIT Association-information OPTIONAL
 * }
 *
 * @return a std::vector<uint8_t> containing the encoded AARQ frame, based on the CosemParameters of this object
 */
auto Cosem::serialize_aarq() -> std::vector<uint8_t>
{
    std::vector<uint8_t> buffer;
    CosemSerializer serializer(buffer);

    

    buffer.push_back(BER_CONSTRUCTED | BER_CLASS_APPLICATION);

    if (params_.authentication == AuthenticationMechanism::LOWEST) {
        buffer.insert(buffer.end(), {0x80, 0x02, 0x07, 0x80});
    }

    //application-context-name [1]
    buffer.push_back(BER_CLASS_CONTEXT | BER_CONSTRUCTED | AARQ_APPLICATION_CONTEXT_NAME);
    buffer.push_back(9);
    buffer.push_back(BER_OBJECT_IDENTIFIER);
    buffer.push_back(7);
    if (params_.security == SecurityContext::NONE) {
        buffer.insert(buffer.end(), {0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01}); //LNAME_NO_CIPHERING
    } else {
        buffer.insert(buffer.end(), {0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x03}); //LNAME_WITH_CIPHERING
    }

    //calling-AP-title [6]
    if (params_.security != SecurityContext::NONE || params_.authentication == AuthenticationMechanism::HLS_GMAC) {
        buffer.push_back(BER_CLASS_CONTEXT | BER_CONSTRUCTED | AARQ_CALLING_AP_TITLE);
        buffer.push_back(params_.system_title.size() + 2);
        buffer.push_back(BER_OBJECT_IDENTIFIER);
        buffer.push_back(params_.system_title.size());
        buffer.insert(buffer.end(), params_.system_title.begin(), params_.system_title.end());
    }

    if (params_.authentication != AuthenticationMechanism::LOWEST) {
        //sender-acse-requirements [10]
        buffer.push_back(BER_CLASS_CONTEXT | AARQ_SENDER_ACSE_REQUIREMENTS);
        buffer.push_back(2);
        buffer.push_back(BER_BIT_STRING | BER_OCTET_STRING);
        buffer.push_back(0x80);

       //mechanism-name [11]
        buffer.push_back(BER_CLASS_CONTEXT | AARQ_MECHANISM_NAME);
        buffer.push_back(7);
        buffer.insert(buffer.end(), {0x60, 0x85, 0x74, 0x05, 0x08, 0x02});
        buffer.push_back(static_cast<uint8_t>(params_.authentication));

        //calling-authentication-value [12]
        if (params_.authentication == AuthenticationMechanism::LLS) {
            buffer.push_back(BER_CLASS_CONTEXT | BER_CONSTRUCTED | AARQ_CALLING_AUTHENTICATION_VALUE);
            buffer.push_back(params_.secret.size() + 2);
            buffer.push_back(BER_CLASS_CONTEXT);
            buffer.push_back(params_.secret.size());
            buffer.insert(buffer.end(), params_.secret.begin(), params_.secret.end());
        } else {
            buffer.push_back(BER_CLASS_CONTEXT | BER_CONSTRUCTED | AARQ_CALLING_AUTHENTICATION_VALUE);
            buffer.push_back(static_cast<uint8_t>(params_.challenger_size + 2));
            buffer.push_back(BER_CLASS_CONTEXT);
            buffer.push_back(static_cast<uint8_t>(params_.challenger_size));
            Security::generate_challenger(params_.challenger_size, buffer);
        }
    }

    //Conformance block
    uint32_t conformance_block = 0;
    conformance_block |= ConformanceBlock::GET;
    conformance_block |= ConformanceBlock::SET;
    conformance_block |= ConformanceBlock::ACTION;
    conformance_block |= ConformanceBlock::SELECTIVE_ACCESS;
    conformance_block |= ConformanceBlock::BLOCK_TRANSFER_WITH_GET_OR_READ;
    conformance_block |= ConformanceBlock::BLOCK_TRANSFER_WITH_SET_OR_WRITE;
    conformance_block |= ConformanceBlock::BLOCK_TRANSFER_WITH_ACTION;

    //user-information [30]
    buffer.push_back(BER_CONSTRUCTED | AARQ_USER_INFORMATION);
    buffer.push_back(13);
    buffer.push_back(BER_OCTET_STRING);
    buffer.push_back(11);
    buffer.push_back(XDLMS_NO_CIPHERING_INITIATE_REQUEST);
    buffer.push_back(0); //Dedicated key
    buffer.push_back(0); //Response-allowed
    buffer.push_back(0); //Proposed quality of service
    buffer.push_back(XDLMS_VERSION);
    buffer.push_back(ConformanceBlock::TAG);
    buffer.push_back(4); //conformance block size
    buffer.push_back((uint8_t)conformance_block);
    buffer.push_back((uint8_t)(conformance_block >> 8u));
    buffer.push_back((uint8_t)(conformance_block >> 16u));
    buffer.push_back((uint8_t)(conformance_block >> 24u));

    // Add correct frame size
    buffer[1] = (uint8_t)(buffer.size() - 2);

    return buffer;
}

/**
 * A Get-Request is sent from the client to read some client attribute.
 *
 * Get-Request ::= CHOICE
 * {
 *     get-request-normal      [1] IMPLICIT Get-Request-Normal,
 *     get-request-next        [2] IMPLICIT Get-Request-Next,
 *     get-request-with-list   [3] IMPLICIT Get-Request-With-List
 * }
 *
 * Get-Request-Normal ::= SEQUENCE
 * {
 *     invoke-id-and-priority      Invoke-Id-And-Priority,
 *     cosem-attribute-descriptor  Cosem-Attribute-Descriptor,
 *     access-selection            Selective-Access-Descriptor OPTIONAL
 * }
 *
 * Get-Request-Next ::= SEQUENCE
 * {
 *     invoke-id-and-priority      Invoke-Id-And-Priority,
 *     block-number                Unsigned32
 * }
 *
 * Get-Request-With-List ::= SEQUENCE
 * {
 *     invoke-id-and-priority          Invoke-Id-And-Priority,
 *     attribute-descriptor-list       SEQUENCE OF Cosem-Attribute-Descriptor-With-Selection
 * }
 *
 * @param req
 * @return
 */
auto Cosem::serialize_get(Request const& req) -> std::vector<uint8_t>
{
    return serialize_request_normal(req, XDLMS_NO_CIPHERING_GET_REQUEST);
}

/**
 * A Set-Request is sent from the client to write some client attribute.
 *
 * Set-Request ::= CHOICE
 * {
 *     set-request-normal                          [1] IMPLICIT Set-Request-Normal,
 *     set-request-with-first-datablock            [2] IMPLICIT Set-Request-With-First-Datablock,
 *     set-request-with-datablock                  [3] IMPLICIT Set-Request-With-Datablock,
 *     set-request-with-list                       [4] IMPLICIT Set-Request-With-List,
 *     set-request-with-list-and-first-datablock   [5] IMPLICIT Set-Request-With-List-And-First-Datablock
 * }
 *
 * Set-Request-Normal ::= SEQUENCE
 * {
 *     invoke-id-and-priority      Invoke-Id-And-Priority,
 *     cosem-attribute-descriptor  Cosem-Attribute-Descriptor,
 *     access-selection            Selective-Access-Descriptor OPTIONAL,
 *     value Data
 * }
 *
 * Set-Request-With-First-Datablock ::= SEQUENCE
 * {
 *     invoke-id-and-priority      Invoke-Id-And-Priority,
 *     cosem-attribute-descriptor  Cosem-Attribute-Descriptor,
 *     access-selection            [0] IMPLICIT Selective-Access-Descriptor OPTIONAL,
 *     datablock DataBlock-SA
 * }
 *
 * Set-Request-With-Datablock ::= SEQUENCE
 * {
 *     invoke-id-and-priority      Invoke-Id-And-Priority,
 *     datablock                   DataBlock-SA
 * }
 *
 * Set-Request-With-List ::= SEQUENCE
 * {
 *     invoke-id-and-priority Invoke-Id-And-Priority,
 *     attribute-descriptor-list   SEQUENCE OF Cosem-Attribute-Descriptor-With-Selection,
 *     value-list                  SEQUENCE OF Data
 * }
 *
 * Set-Request-With-List-And-First-Datablock ::= SEQUENCE
 * {
 *     invoke-id-and-priority Invoke-Id-And-Priority,
 *     attribute-descriptor-list   SEQUENCE OF Cosem-Attribute-Descriptor-With-Selection,
 *     datablock                   DataBlock-SA
 * }
 *
 * @param req
 * @return
 */
auto Cosem::serialize_set(Request const& req) -> std::vector<uint8_t>
{
    return serialize_request_normal(req, XDLMS_NO_CIPHERING_SET_REQUEST);
}

/**
 * An Action-Request is sent from the client to execute some client method.
 *
 * Action-Request ::= CHOICE
 * {
 *     action-request-normal                       [1] IMPLICIT Action-Request-Normal,
 *     action-request-next-pblock                  [2] IMPLICIT Action-Request-Next-Pblock,
 *     action-request-with-list                    [3] IMPLICIT Action-Request-With-List,
 *     action-request-with-first-pblock            [4] IMPLICIT Action-Request-With-First-Pblock,
 *     action-request-with-list-and-first-pblock   [5] IMPLICIT Action-Request-With-List-And-First-Pblock,
 *     action-request-with-pblock                  [6] IMPLICIT Action-Request-With-Pblock
 * }
 *
 * Action-Request-Normal ::= SEQUENCE
 * {
 *     invoke-id-and-priority          Invoke-Id-And-Priority,
 *     cosem-method-descriptor         Cosem-Method-Descriptor,
 *     method-invocation-parameters    Data OPTIONAL
 * }
 *
 * Action-Request-Next-Pblock ::= SEQUENCE
 * {
 *     invoke-id-and-priority  Invoke-Id-And-Priority,
 *     block-number            Unsigned32
 * }
 *
 * Action-Request-With-List ::= SEQUENCE
 * {
 *     invoke-id-and-priority          Invoke-Id-And-Priority,
 *     cosem-method-descriptor-list    SEQUENCE OF Cosem-Method-Descriptor,
 *     method-invocation-parameters    SEQUENCE OF Data
 * }
 *
 * Action-Request-With-First-Pblock ::= SEQUENCE
 * {
 *     invoke-id-and-priority      Invoke-Id-And-Priority,
 *     cosem-method-descriptor     Cosem-Method-Descriptor,
 *     pblock                      DataBlock-SA
 * }
 *
 * Action-Request-With-List-And-First-Pblock ::= SEQUENCE
 * {
 *     invoke-id-and-priority          Invoke-Id-And-Priority,
 *     cosem-method-descriptor-list    SEQUENCE OF Cosem-Method-Descriptor,
 *     pblock                          DataBlock-SA
 * }
 *
 * Action-Request-With-Pblock ::= SEQUENCE
 * {
 *     invoke-id-and-priority  Invoke-Id-And-Priority,
 *     pblock                  DataBlock-SA
 * }
 *
 * @param req
 * @return
 */
 auto Cosem::serialize_act(Request const& req) -> std::vector<uint8_t>
 {
     return serialize_request_normal(req, XDLMS_NO_CIPHERING_ACTION_REQUEST);
 }

/**
 * Application Association Response - AARE
 *
 * AARE is the message sent from the server to the client as a response to a AARQ frame.
 *
 * The format of the AARE frame is shown below (from Green Book edition 8.1)
 *
 * AARE-apdu ::= [APPLICATION 1] IMPLICIT SEQUENCE
 * {
 * -- [APPLICATION 1] == [ 61H ] = [ 97 ]
 *     protocol-version                [0] IMPLICIT    BIT STRING {version1 (0)} DEFAULT {version1},
 *     application-context-name        [1]             Application-context-name,
 *     result                          [2]             Association-result,
 *     result-source-diagnostic        [3]             Associate-source-diagnostic,
 *     responding-AP-title             [4]             AP-title OPTIONAL,
 *     responding-AE-qualifier         [5]             AE-qualifier OPTIONAL,
 *     responding-AP-invocation-id     [6]             AP-invocation-identifier OPTIONAL,
 *     responding-AE-invocation-id     [7]             AE-invocation-identifier OPTIONAL,
 *
 * -- The following field shall not be present if only the kernel is used.
 *     responder-acse-requirements     [8] IMPLICIT    ACSE-requirements OPTIONAL,
 *
 * -- The following field shall only be present if the authentication functional unit is selected.
 *     mechanism-name                  [9] IMPLICIT    Mechanism-name OPTIONAL,
 *
 * -- The following field shall only be present if the authentication functional unit is selected.
 *     responding-authentication-value [10] EXPLICIT   Authentication-value OPTIONAL,
 *     implementation-information      [29] IMPLICIT   Implementation-data OPTIONAL,
 *     user-information                [30] EXPLICIT   Association-information OPTIONAL
 * }
 *
 * @param data : a std::vector<uint8_t> with the octet-string representation of the AARE frame
 * @return an AssociationResult with the result of the propposed association
 */
auto Cosem::parse_aare(std::vector<uint8_t> const& data) -> AssociationResult
{

}

/**
 * Parses a response
 * @param data
 * @return
 */
auto Cosem::parse_response(std::vector<uint8_t> const& data) -> Response
{

}

/**
 *
 * Get-Request-Normal ::= SEQUENCE
 * {
 *     invoke-id-and-priority      Invoke-Id-And-Priority,
 *     cosem-attribute-descriptor  Cosem-Attribute-Descriptor,
 *     access-selection            Selective-Access-Descriptor OPTIONAL
 * }
 *
 * Set-Request-Normal ::= SEQUENCE
 * {
 *     invoke-id-and-priority      Invoke-Id-And-Priority,
 *     cosem-attribute-descriptor  Cosem-Attribute-Descriptor,
 *     access-selection            Selective-Access-Descriptor OPTIONAL,
 *     value Data
 * }
 *
 * Action-Request-Normal ::= SEQUENCE
 * {
 *     invoke-id-and-priority          Invoke-Id-And-Priority,
 *     cosem-method-descriptor         Cosem-Method-Descriptor,
 *     method-invocation-parameters    Data OPTIONAL
 * }
 *
 * @param req
 * @param tag
 * @return
 */
static auto serialize_request_normal(Request const& req, uint8_t tag) -> std::vector<uint8_t>
{
    std::vector<uint8_t> buffer;
    buffer.push_back(tag);
    buffer.push_back(1);
    buffer.push_back(static_cast<uint8_t>(XDLMS_HIGH_PRIORITY | XDLMS_SERVICE_CONFIRMED | XDLMS_INVOKE_ID));
    buffer.push_back(static_cast<uint8_t>(static_cast<uint16_t>(req.class_id) >> 8U));
    buffer.push_back(static_cast<uint8_t>(req.class_id));
    buffer.insert(buffer.end(), req.logical_name.begin(), req.logical_name.end());
    buffer.push_back(req.index);
    buffer.push_back(req.data.empty() ? static_cast<uint8_t>(0U) : static_cast<uint8_t>(1U));
    buffer.insert(buffer.end(), req.data.begin(), req.data.end());
    return buffer;
}

}
