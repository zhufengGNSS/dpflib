# We must start 0,...,NUM_NODES instances of dpf simultaneously.
# This is one way to do it, using GNU screen. 
# Although note that in this case files will be written to the $HOME directory!
# The results will be written to $HOME/temp.dat
if [ $# -ge 1 ]; then
	seed=$1
else
	seed=0
fi
screen -X screen -t dpf0 $PWD/dpf 0 $seed
screen -X screen -t dpf1 $PWD/dpf 1 $seed
screen -X screen -t dpf2 $PWD/dpf 2 $seed
screen -X screen -t dpf3 $PWD/dpf 3 $seed
cat $HOME/dat/temp.dat
