run_cli_pares: armazem_chave_valor_proto
	@python3 armazem_client.py $(arg)

run_serv_pares_1: armazem_chave_valor_proto servidor_central_proto
	@python3 armazem_server.py $(arg)

run_serv_pares_2: armazem_chave_valor_proto servidor_central_proto
	@python3 armazem_server.py $(arg) flag

run_serv_central: servidor_central_proto
	@python3 servidor_central.py $(arg)

run_cli_central:
	@python3 central_client.py $(arg)

armazem_chave_valor_proto: protos/armazem_chave_valor.proto
	@python3 -m grpc_tools.protoc -Iprotos --python_out=. --grpc_python_out=. protos/armazem_chave_valor.proto

servidor_central_proto: protos/servidor_central.proto
	@python3 -m grpc_tools.protoc -Iprotos --python_out=. --grpc_python_out=. protos/servidor_central.proto

.PHONY: clean
clean:
	rm -rf __pycache__
	rm -rf *pb2*
