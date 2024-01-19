import sys

import grpc
import armazem_chave_valor_pb2
import armazem_chave_valor_pb2_grpc

#Função do cliente de pares
def run():
    if len(sys.argv) < 2:
        print("Erro! Sem argumentos suficientes")
        return
    str_id = sys.argv[1] #Recebe string identificadora
    
    with grpc.insecure_channel(str_id) as channel: #Abre o canal
        stub = armazem_chave_valor_pb2_grpc.ArmazemChaveValorStub(channel) #Stub de armazém

        #Lendo os comandos até que receba T ou finalize com Ctrl-D
        for comando in sys.stdin:
            comando = comando.strip('\n\r')
            
            #Terminação
            if comando[0] == 'T':
                param_vazio = armazem_chave_valor_pb2.Param_Vazio() #Cria um parâmetro vazio
                int_ret = stub.Termino(param_vazio) #Chama a terminação do servidor
                print(int_ret.v) 
                break #Sai do programa

            #Inserção
            elif comando[0] == 'I':
                _, ch, str_desc = comando.split(sep=',') #Lê chave e valor atribuído
                #Cria estruturas
                chave = armazem_chave_valor_pb2.Chave(chave=int(ch))
                valor = armazem_chave_valor_pb2.Valor(valor=str_desc)
                chave_valor = armazem_chave_valor_pb2.Chave_Valor(chave_j = chave, valor_j=valor)
                
                int_ret = stub.Insercao(chave_valor) #Chama função quee insere no dicionário do servidor
                print(int_ret.v)
            
            #Consulta
            elif comando[0] == 'C':
                _, ch = comando.split(sep=',') #Lê chave
                chave = armazem_chave_valor_pb2.Chave(chave=int(ch)) #Cria estrutura
                valor_ret = stub.Consulta(chave) #Chama função que retorna valor no dicionário do servidor
                print(valor_ret.valor)
            
            #Ativação
            elif comando[0] == 'A':
                _, str_id_a = comando.split(sep=',') #Lê string identificadora
                valor = armazem_chave_valor_pb2.Valor(valor=str_id_a) #Cria estrutura parâmetro
                int_ret = stub.Ativacao(valor) #Chama a função de ativação, cuja função depende do flag do servidor
                print(int_ret.v)
                

if __name__ == '__main__':
    run()