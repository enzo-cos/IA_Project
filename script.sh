#!/bin/bash
port=$1

./serveur $port &

./joueur 127.0.0.1 $port j1 0 &

./joueur 127.0.0.1 $port j2 1 &