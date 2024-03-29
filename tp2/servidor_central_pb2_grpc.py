# Generated by the gRPC Python protocol compiler plugin. DO NOT EDIT!
"""Client and server classes corresponding to protobuf-defined services."""
import grpc

import servidor_central_pb2 as servidor__central__pb2


class ServidorCentralStub(object):
    """Missing associated documentation comment in .proto file."""

    def __init__(self, channel):
        """Constructor.

        Args:
            channel: A grpc.Channel.
        """
        self.Registro = channel.unary_unary(
                '/servidor.ServidorCentral/Registro',
                request_serializer=servidor__central__pb2.Id_Chave.SerializeToString,
                response_deserializer=servidor__central__pb2.Inteiro.FromString,
                )
        self.Mapeamento = channel.unary_unary(
                '/servidor.ServidorCentral/Mapeamento',
                request_serializer=servidor__central__pb2.Chave.SerializeToString,
                response_deserializer=servidor__central__pb2.Identificador.FromString,
                )
        self.Termino = channel.unary_unary(
                '/servidor.ServidorCentral/Termino',
                request_serializer=servidor__central__pb2.Param_Vazio.SerializeToString,
                response_deserializer=servidor__central__pb2.Inteiro.FromString,
                )


class ServidorCentralServicer(object):
    """Missing associated documentation comment in .proto file."""

    def Registro(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def Mapeamento(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def Termino(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')


def add_ServidorCentralServicer_to_server(servicer, server):
    rpc_method_handlers = {
            'Registro': grpc.unary_unary_rpc_method_handler(
                    servicer.Registro,
                    request_deserializer=servidor__central__pb2.Id_Chave.FromString,
                    response_serializer=servidor__central__pb2.Inteiro.SerializeToString,
            ),
            'Mapeamento': grpc.unary_unary_rpc_method_handler(
                    servicer.Mapeamento,
                    request_deserializer=servidor__central__pb2.Chave.FromString,
                    response_serializer=servidor__central__pb2.Identificador.SerializeToString,
            ),
            'Termino': grpc.unary_unary_rpc_method_handler(
                    servicer.Termino,
                    request_deserializer=servidor__central__pb2.Param_Vazio.FromString,
                    response_serializer=servidor__central__pb2.Inteiro.SerializeToString,
            ),
    }
    generic_handler = grpc.method_handlers_generic_handler(
            'servidor.ServidorCentral', rpc_method_handlers)
    server.add_generic_rpc_handlers((generic_handler,))


 # This class is part of an EXPERIMENTAL API.
class ServidorCentral(object):
    """Missing associated documentation comment in .proto file."""

    @staticmethod
    def Registro(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/servidor.ServidorCentral/Registro',
            servidor__central__pb2.Id_Chave.SerializeToString,
            servidor__central__pb2.Inteiro.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def Mapeamento(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/servidor.ServidorCentral/Mapeamento',
            servidor__central__pb2.Chave.SerializeToString,
            servidor__central__pb2.Identificador.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def Termino(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/servidor.ServidorCentral/Termino',
            servidor__central__pb2.Param_Vazio.SerializeToString,
            servidor__central__pb2.Inteiro.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)
