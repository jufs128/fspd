import sys
import threading
import socket

from concurrent import futures

import grpc
import armazem_chave_valor_pb2
import armazem_chave_valor_pb2_grpc
import servidor_central_pb2_grpc
import servidor_central_pb2

#Classe com funções exercidas pelo servidor de pares
class ArmazemChaveValorServicer(armazem_chave_valor_pb2_grpc.ArmazemChaveValorServicer):
    def __init__(self, stop_event, flag, porta) -> None:
        self.dict_chave_valor = {} #Dicionário
        self._stop_event = stop_event #Evento que sinaliza a terminação do servidor
        self.flag = flag #Flag que indica se há conexão com o servidor central
        self.port = porta #Port do servidor
    
    #Insere chave e valor no dicionário, se chave não estava antes
    def Insercao(self, request, context):
        if request.chave_j.chave in self.dict_chave_valor:
            return armazem_chave_valor_pb2.Inteiro(v=-1)
        else:
            self.dict_chave_valor[request.chave_j.chave] = request.valor_j.valor
            return armazem_chave_valor_pb2.Inteiro(v=0)
    
    #Retorna o valor atribuído à chave no dicionário
    def Consulta(self, request, context):
        if request.chave in self.dict_chave_valor:
            return armazem_chave_valor_pb2.Valor(valor=self.dict_chave_valor[request.chave])
        else:
            return armazem_chave_valor_pb2.Valor(valor=None)
    
    #Não faz nada se o flag é falso; registra chaves no servidor central se o flag é verdadeiro
    def Ativacao(self, request, context):
        if self.flag == False:
            return armazem_chave_valor_pb2.Inteiro(v=0)
        
        with grpc.insecure_channel(request.valor) as channel:
            stub = servidor_central_pb2_grpc.ServidorCentralStub(channel)
            
            server_str = f'{socket.getfqdn()}:{self.port}'
            server_id = servidor_central_pb2.Identificador(str_id=server_str)
            server_chs = servidor_central_pb2.Lista_Chaves(chave=list(self.dict_chave_valor.keys()))
            server_id_chs = servidor_central_pb2.Id_Chave(id=server_id, ch=server_chs)

            int_ret = stub.Registro(server_id_chs)
            return armazem_chave_valor_pb2.Inteiro(v=int_ret.v)
    
    #Finaliza o servidor
    def Termino(self, request, context):
        self._stop_event.set() #Aciona o evento de terminação
        return armazem_chave_valor_pb2.Inteiro(v=0)

#Preparando o servidor
def serve():
    if len(sys.argv) < 2:
        print("Erro! Sem argumentos suficientes")
        return
    port_server = sys.argv[1] #Recebe a porta como argumento

    flag = False
    if len(sys.argv) >= 3: #Verifica se houve um segundo argumento que ativa a flag
        flag = True

    stop_event = threading.Event() #Evento que sinaliza a terminação do servidor

    #Inicializando o servidor
    server = grpc.server(futures.ThreadPoolExecutor())
    armazem_chave_valor_pb2_grpc.add_ArmazemChaveValorServicer_to_server(ArmazemChaveValorServicer(stop_event, flag, port_server), server)
    server.add_insecure_port('[::]:'+str(port_server))
    server.start()

    #Esperando algum cliente finalizar o servidor
    stop_event.wait()
    server.stop(grace=None)

if __name__ == '__main__':
    serve()