#pragma once

#include <optional>

namespace dlms
{

struct ProtocolVersion
{
    uint8_t value = 0;
};

struct ApplicationContextName
{

};

struct ApTitle
{

};

struct AeQualifier
{

};

struct ApInvocationIdentifier
{

};

struct AeInvocationIdentifier
{

};

struct AcseRequirements
{

};

struct MechanismName
{

};

struct AuthenticationValue
{

};

struct ImplementationData
{

};

struct AssociationInformation
{

};

struct ApplicationAssociationRequest
{
    ProtocolVersion protocol_version;
    ApplicationContextName application_context_name;
    std::optional<ApTitle> called_ap_title;
    std::optional<AeQualifier> called_ae_qualifier;
    std::optional<ApInvocationIdentifier> called_ap_invocation_id;
    std::optional<AeInvocationIdentifier> called_ae_invocation_id;
    std::optional<ApTitle> calling_ap_title;
    std::optional<AeQualifier> calling_ae_qualifier;
    std::optional<ApInvocationIdentifier> calling_ap_invocation_id;
    std::optional<AeInvocationIdentifier> calling_ae_invocation_id;
    std::optional<AcseRequirements> sender_acse_requirements;
    std::optional<MechanismName> mechanism_name;
    std::optional<AuthenticationValue> calling_authentication_value;
    std::optional<ImplementationData> implementation_information;
    std::optional<AssociationInformation> user_information;
};

class AarqBuilder
{
public:
    ApplicationAssociationRequest build() {
        return aarq_;
    }

    void set_protocol_version(uint8_t version) {
        aarq_.protocol_version.value = version;
    }

private:
    ApplicationAssociationRequest aarq_;
};

}
