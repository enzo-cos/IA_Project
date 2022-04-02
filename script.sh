#!/bin/bash
port=$1

./serveur $port &

./joueur 127.0.0.1 $port & 

./joueur2 127.0.0.1 $port & 