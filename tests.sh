executar_server(){
	tarefas="$1"
	cmds_client="$2"
	politica="$3"
	fich_out="$4"
	./server "$fich_out" "$tarefas" "$politica" &
	server_pid=$!

	sleep 1 # esperar o server ligar

	eval "bash run.sh $cmds_client" &
	wait

	sleep 5

	./client status

	kill "$server_pid"
	wait "$server_pid"

}

if [ "$#" -ne 2 ]; then
	echo "Erro nos argumentos"
	exit 1
fi

ficheiro="$1"
tarefas="$2"

if [ ! -f "$ficheiro" ]; then
	echo "Ficheiro provido nao existe :("
	exit 1
fi

executar_server "$tarefas" "$ficheiro" "SJF" "outputSJF"
executar_server "$tarefas" "$ficheiro" "LJF" "outputLJF"
executar_server "$tarefas" "$ficheiro" "FCFS" "outputFCFS"
