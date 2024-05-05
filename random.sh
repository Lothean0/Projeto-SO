execute_task(){
	local task="$1"
	local teste=$task*100
	echo "./client execute teste -u "../../Progs_exemplo/hello $task"" >> scripts.txt
	echo "./client execute teste -u "../../Progs_exemplo/void $task"" >> scripts.txt
}

if [ "$#" -ne 1 ]; then
	echo "Erro nos argumentos"
	exit 1
fi

quantas=$1
max_time=10

for ((i=1;i<= quantas; i++)); do
	random_time=$((RANDOM % max_time + 1))

	execute_task "$random_time"
done