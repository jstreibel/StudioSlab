#ifndef STUDIOSLAB_GRAPH_SUBSTRATE_V2_H
#define STUDIOSLAB_GRAPH_SUBSTRATE_V2_H

#include "ReflectionTypesV2.h"
#include "SemanticTypesV1.h"

#include <cstddef>

namespace Slab::Core::Reflection::V2 {

    enum class EGraphModeV2 : unsigned char {
        Semantic,
        Template,
        Runtime,
        Routing,
        SchemesBlueprint
    };

    enum class EGraphNodeKindV2 : unsigned char {
        Interface,
        Parameter,
        Operation,
        SemanticOperator,
        Coercion,
        RoutingEndpoint,
        Other
    };

    enum class EGraphPortDirectionV2 : unsigned char {
        Input,
        Output
    };

    enum class EGraphEdgeKindV2 : unsigned char {
        Containment,
        ValueFlow,
        SemanticCoercion,
        HandleBinding,
        StreamSubscription,
        ControlDependency,
        Other
    };

    struct FGraphPointV2 {
        float x = 0.0f;
        float y = 0.0f;
    };

    struct FGraphMetadataV2 {
        Str Description;
        StrVector Tags;
        std::map<Str, FReflectionValueV2> Attrs;
    };

    struct FGraphPolicySetV2 {
        std::optional<EParameterMutability> ParameterMutability;
        std::optional<EParameterExposure> ParameterExposure;
        std::optional<EThreadAffinity> ThreadAffinity;
        std::optional<ERunStatePolicy> RunStatePolicy;
        std::optional<ESideEffectClass> SideEffectClass;
    };

    struct FGraphPortHandleV2 {
        Str HandleId;
        Str TypeId;
        Str SpaceId;
    };

    struct FGraphPortV2 {
        Str PortId;
        Str DisplayName;
        EGraphPortDirectionV2 Direction = EGraphPortDirectionV2::Input;
        bool bRequired = true;
        FGraphPortHandleV2 Handle;
        FGraphMetadataV2 Metadata;
    };

    struct FGraphMemberV2 {
        Str MemberId;
        Str DisplayName;
        Str TypeId;
        FGraphMetadataV2 Metadata;
        FGraphPolicySetV2 Policies;
    };

    struct FGraphNodeV2 {
        Str NodeId;
        Str DisplayName;
        EGraphNodeKindV2 Kind = EGraphNodeKindV2::Other;
        Str SemanticOperatorId;
        FGraphPointV2 Position = FGraphPointV2{120.0f, 120.0f};
        Str SourceInterfaceId;
        Str SourceOperationId;
        Str SourceParameterId;
        Vector<FGraphPortV2> Ports;
        Vector<FGraphMemberV2> Members;
        FGraphMetadataV2 Metadata;
        FGraphPolicySetV2 Policies;
    };

    struct FGraphEdgeV2 {
        Str EdgeId;
        Str FromNodeId;
        Str FromPortId;
        Str ToNodeId;
        Str ToPortId;
        EGraphEdgeKindV2 Kind = EGraphEdgeKindV2::Other;
        Str MatchReason;
        StrVector Diagnostics;
        Vector<Str> SuggestedCoercionOperatorIds;
        FGraphMetadataV2 Metadata;
    };

    struct FGraphCanvasStateV2 {
        DevFloat PanX = 0.0f;
        DevFloat PanY = 0.0f;
        bool bShowGrid = true;
        bool bShowMinimap = false;
    };

    struct FGraphDocumentV2 {
        EGraphModeV2 Mode = EGraphModeV2::Template;
        Vector<FGraphNodeV2> Nodes;
        Vector<FGraphEdgeV2> Edges;
        FGraphCanvasStateV2 Canvas;
    };

    inline auto MakeGraphPortFromOperationField(const FOperationFieldSchemaV2 &field, const EGraphPortDirectionV2 direction)
        -> FGraphPortV2 {
        FGraphPortV2 port;
        port.PortId = field.FieldId;
        port.DisplayName = field.DisplayName;
        port.Direction = direction;
        port.bRequired = field.bRequired;
        port.Handle.TypeId = field.TypeId;
        port.Metadata.Description = field.Description;
        return port;
    }

    inline auto MakeGraphNodeFromParameterSchema(const Str &interfaceId, const FParameterSchemaV2 &parameter)
        -> FGraphNodeV2 {
        FGraphNodeV2 node;
        node.NodeId = interfaceId + "::param:" + parameter.ParameterId;
        node.DisplayName = parameter.DisplayName;
        node.Kind = EGraphNodeKindV2::Parameter;
        node.SourceInterfaceId = interfaceId;
        node.SourceParameterId = parameter.ParameterId;
        node.Metadata.Description = parameter.Description;
        node.Policies.ParameterMutability = parameter.Mutability;
        node.Policies.ParameterExposure = parameter.Exposure;
        return node;
    }

    inline auto MakeGraphNodeFromOperationSchema(const Str &interfaceId, const FOperationSchemaV2 &operation)
        -> FGraphNodeV2 {
        FGraphNodeV2 node;
        node.NodeId = interfaceId + "::op:" + operation.OperationId;
        node.DisplayName = operation.DisplayName;
        node.Kind = EGraphNodeKindV2::Operation;
        node.SourceInterfaceId = interfaceId;
        node.SourceOperationId = operation.OperationId;
        node.Metadata.Description = operation.Description;
        node.Policies.ThreadAffinity = operation.ThreadAffinity;
        node.Policies.RunStatePolicy = operation.RunStatePolicy;
        node.Policies.SideEffectClass = operation.SideEffectClass;

        node.Ports.reserve(operation.Inputs.size() + operation.Outputs.size());
        for (const auto &input : operation.Inputs) {
            node.Ports.push_back(MakeGraphPortFromOperationField(input, EGraphPortDirectionV2::Input));
        }
        for (const auto &output : operation.Outputs) {
            node.Ports.push_back(MakeGraphPortFromOperationField(output, EGraphPortDirectionV2::Output));
        }

        return node;
    }

    inline auto MakeGraphNodeFromSemanticOperatorSchema(const FSemanticOperatorSchemaV1 &semanticOperator) -> FGraphNodeV2 {
        FGraphNodeV2 node;
        node.NodeId = semanticOperator.OperatorId;
        node.DisplayName = semanticOperator.DisplayName;
        node.Kind = semanticOperator.Kind == ESemanticOperatorKindV1::Coercion
            ? EGraphNodeKindV2::Coercion
            : EGraphNodeKindV2::SemanticOperator;
        node.SemanticOperatorId = semanticOperator.OperatorId;
        node.Metadata.Description = semanticOperator.Description;
        node.Metadata.Tags = semanticOperator.Tags;

        node.Ports.reserve(semanticOperator.DomainPorts.size() + semanticOperator.CodomainPorts.size());
        for (const auto &inputPort : semanticOperator.DomainPorts) {
            FGraphPortV2 port;
            port.PortId = inputPort.PortId;
            port.DisplayName = inputPort.DisplayName;
            port.Direction = EGraphPortDirectionV2::Input;
            port.bRequired = inputPort.bRequired;
            port.Handle.SpaceId = inputPort.SpaceId;
            port.Metadata.Description = inputPort.Latex;
            node.Ports.push_back(std::move(port));
        }

        for (const auto &outputPort : semanticOperator.CodomainPorts) {
            FGraphPortV2 port;
            port.PortId = outputPort.PortId;
            port.DisplayName = outputPort.DisplayName;
            port.Direction = EGraphPortDirectionV2::Output;
            port.bRequired = outputPort.bRequired;
            port.Handle.SpaceId = outputPort.SpaceId;
            port.Metadata.Description = outputPort.Latex;
            node.Ports.push_back(std::move(port));
        }

        return node;
    }

    inline auto MakeGraphNodeFromInterfaceSchema(const FInterfaceSchemaV2 &interfaceSchema) -> FGraphNodeV2 {
        FGraphNodeV2 node;
        node.NodeId = interfaceSchema.InterfaceId;
        node.DisplayName = interfaceSchema.DisplayName;
        node.Kind = EGraphNodeKindV2::Interface;
        node.SourceInterfaceId = interfaceSchema.InterfaceId;
        node.Metadata.Description = interfaceSchema.Description;
        node.Metadata.Tags = interfaceSchema.Tags;

        node.Members.reserve(interfaceSchema.Parameters.size() + interfaceSchema.Operations.size());
        for (const auto &parameter : interfaceSchema.Parameters) {
            FGraphMemberV2 member;
            member.MemberId = parameter.ParameterId;
            member.DisplayName = parameter.DisplayName;
            member.TypeId = parameter.TypeId;
            member.Metadata.Description = parameter.Description;
            member.Policies.ParameterMutability = parameter.Mutability;
            member.Policies.ParameterExposure = parameter.Exposure;
            node.Members.push_back(std::move(member));
        }
        for (const auto &operation : interfaceSchema.Operations) {
            FGraphMemberV2 member;
            member.MemberId = operation.OperationId;
            member.DisplayName = operation.DisplayName;
            member.TypeId = ToString(operation.Kind);
            member.Metadata.Description = operation.Description;
            member.Policies.ThreadAffinity = operation.ThreadAffinity;
            member.Policies.RunStatePolicy = operation.RunStatePolicy;
            member.Policies.SideEffectClass = operation.SideEffectClass;
            node.Members.push_back(std::move(member));
        }

        return node;
    }

    inline auto MakeGraphDocumentFromReflectionCatalog(const FReflectionCatalogV2 &catalog, const EGraphModeV2 mode)
        -> FGraphDocumentV2 {
        FGraphDocumentV2 document;
        document.Mode = mode;

        for (const auto &interfaceSchema : catalog.Interfaces) {
            document.Nodes.push_back(MakeGraphNodeFromInterfaceSchema(interfaceSchema));
            const auto interfaceNodeId = interfaceSchema.InterfaceId;

            for (const auto &parameter : interfaceSchema.Parameters) {
                const auto parameterNode = MakeGraphNodeFromParameterSchema(interfaceSchema.InterfaceId, parameter);
                document.Edges.push_back(FGraphEdgeV2{
                    .EdgeId = interfaceNodeId + "::contains::param:" + parameter.ParameterId,
                    .FromNodeId = interfaceNodeId,
                    .ToNodeId = parameterNode.NodeId,
                    .Kind = EGraphEdgeKindV2::Containment
                });
                document.Nodes.push_back(parameterNode);
            }

            for (const auto &operation : interfaceSchema.Operations) {
                const auto operationNode = MakeGraphNodeFromOperationSchema(interfaceSchema.InterfaceId, operation);
                document.Edges.push_back(FGraphEdgeV2{
                    .EdgeId = interfaceNodeId + "::contains::op:" + operation.OperationId,
                    .FromNodeId = interfaceNodeId,
                    .ToNodeId = operationNode.NodeId,
                    .Kind = EGraphEdgeKindV2::Containment
                });
                document.Nodes.push_back(operationNode);
            }
        }

        return document;
    }

    inline auto CountGraphMembers(const FGraphDocumentV2 &document) -> std::size_t {
        std::size_t count = 0;
        for (const auto &node : document.Nodes) {
            count += node.Members.size();
        }
        return count;
    }
}

#endif
