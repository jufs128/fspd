import sys
import socket

import grpc
import servidor_central_pb2_grpc
import servidor_central_pb2

import armazem_chave_valor_pb2
import armazem_chave_valor_pb2_grpc

#Função do cliente central
def run():
    if len(sys.argv) < 2:
        print("Erro! Sem argumentos suficientes")
        return
    str_id = sys.argv[1] #Recebe string identificadora
    
    with grpc.insecure_channel(str_id) as channel: #Abre o canal
        stub = servidor_central_pb2_grpc.ServidorCentralStub(channel) #Stub do servidor central

        #Lendo os comandos até que receba T ou finalize com Ctrl-D
        for comando in sys.stdin:
            comando = comando.strip('\n\r')
            
            #Terminação
            if comando[0] == 'T':
                param_vazio = servidor_central_pb2.Param_Vazio() #Cria um parâmetro vazio
                int_ret = stub.Termino(param_vazio) #Chama a terminação do servidor
                print(int_ret.v)
                break #Sai do programa

            #Consulta
            elif comando[0] == 'C':
                _, ch = comando.split(',') #Lê chave
                chave = servidor_central_pb2.Chave(chave=int(ch)) #Cria a estrutura
                valor_ret = stub.Mapeamento(chave) #Chama Mapeamento e recebe string identificadora associada à chave

                if len(valor_ret.str_id) != 0: #Se recebeu string adequada
                    output = valor_ret.str_id + ':' #Imprime string identificadora
                else: #Se recebeu string vazia, printa vazio e lê próximo comando
                    print()
                    continue
                
                #Pega endereço do servidor responsável pela chave
                str_serv, port_serv = valor_ret.str_id.split(':')
                addr_serv = socket.gethostbyname(str_serv)
                
                #Conecta com o servidor para fazer consulta
                with grpc.insecure_channel(addr_serv + ':' + port_serv) as channel_serv: #Abre o canal com servidor de pares
                    stub_serv = armazem_chave_valor_pb2_grpc.ArmazemChaveValorStub(channel_serv) #Stub do servidor de pares
                    ch = armazem_chave_valor_pb2.Chave(chave=int(ch)) #Cria estrutura
                    valor_ret = stub_serv.Consulta(ch) #Chama Consulta e recebe valor associado à chave no servidor responsável
                    print(output + valor_ret.valor)

if __name__ == '__main__':
    run()