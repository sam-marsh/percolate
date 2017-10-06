#/bin/bash

k=$1
p=0.592746
if [ "$k" == "b" ]
then
	p=0.5
fi

for e in {1..15}
do
	let n=1000*e # n = 1000, 2000, 3000 ...
	for c in {1..5} # number of times to execute
	do
		for t in {2..4} # number of threads
		do
			r=$(./main $n $p $k 2 $t) # execute percolation program
            f=$(echo ${r##*=} | sed 's/.$//')
			echo $n $t $f # print time taken
		done
	done
done
