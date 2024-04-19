
if [ "$#" -ne 1 ]; then
	echo "Erro nos argumentos"
	exit 1
fi

ficheiro="$1"

if [ ! -f "$ficheiro" ]; then
	echo "Ficheiro provido nao existe :("
	exit 1
fi

mapfile -t scripts < "$ficheiro"

for scrpt in "${scripts[@]}"; do
	eval "$scrpt" &
	wait
done