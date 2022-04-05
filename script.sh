#!/bin/bash
port=$1

./serveur $port &

./truePlayer j1 $port &

./truePlayer j2 $port &