#!/bin/bash

parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )

cd "$parent_path"

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
cd "$parent_path/Codigo/bin"
for scrpt in "${scripts[@]}"; do
	eval "$scrpt" &
	wait
done