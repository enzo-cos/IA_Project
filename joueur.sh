host=$1
port=$2
nom=$3
portIA=$4


./truePlayer $host $port $nom $portIA &
sleep 2
java IA "127.0.0.1" $portIA 


