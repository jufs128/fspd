import sys
import threading

from concurrent import futures
import grpc
import servidor_central_pb2_grpc
import servidor_central_pb2

#Classe com funções exercidas pelo servidor central
class ServidorCentralServicer(servidor_central_pb2_grpc.ServidorCentralServicer):
    def __init__(self, stop_event):
        self.dict_id_chs = {} #Dicionario com a chave e o id do servidor responsável
        self._stop_event = stop_event #Evento que sinaliza a terminação do servidor
    
    #Coloca as chaves recebidas no dicionário com a string identificadora do servidor que a mandou por último
    def Registro(self, request, context):
        str_id = request.id.str_id
        ch_list = request.ch.chave

        for ch in ch_list: #Cada chave na lista está associada a string recebida
            self.dict_id_chs[ch] = str_id
        
        return servidor_central_pb2.Inteiro(v=len(ch_list)) #Retorna o número de chaves registradas na função
    
    #Retorna o identificador associado à chave recebida no dicionário
    def Mapeamento(self, request, context):
        ch = request.chave
        if ch in self.dict_id_chs:
            return servidor_central_pb2.Identificador(str_id=self.dict_id_chs[ch])
        else:
            return servidor_central_pb2.Identificador(str_id=None)
    
    #Finaliza o servidor
    def Termino(self, request, context):
        self._stop_event.set()  #Aciona o evento de terminação
        int_ret = len(list(self.dict_id_chs.keys()))
        return servidor_central_pb2.Inteiro(v=int_ret) #Retorna o número de chaves no dicionário

#Preparando o servidor
def serve():
    if len(sys.argv) < 2:
        print("Erro! Sem argumentos suficientes")
        return
    port_server = sys.argv[1] #Recebe a porta como argumento

    stop_event = threading.Event() #Evento que sinaliza a terminação do servidor

    #Inicializando o servidor
    server = grpc.server(futures.ThreadPoolExecutor())
    servidor_central_pb2_grpc.add_ServidorCentralServicer_to_server(ServidorCentralServicer(stop_event), server)
    server.add_insecure_port('[::]:'+str(port_server))
    server.start()

    #Esperando algum cliente finalizar o servidor
    stop_event.wait()
    server.stop(grace=None)

if __name__ == '__main__':
    serve()