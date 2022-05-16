#!/bin/bash

if [ "$#" -ne 4 ]; then
    echo "Usage: $0 <host> <port> <nom_joueur> <portIA>"
    exit 1
else
    host=$1
    port=$2
    nom=$3
    portIA=$4
fi


./truePlayer $host $port $nom $portIA &
sleep 1
java IA $1 $portIA 


