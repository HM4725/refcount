#!/bin/bash

make > /dev/null;
TIMEFORMAT='%R';

# Initialize variables
count=10
nthreads=(1 2 4 8 16 32)

for c in ${nthreads[@]}; do
	echo "#threads: $c"
	gcc -o fadvise -DNTHREADS=$c main.c -lpthread > /dev/null

	total_time=0
	# Loop 10 times to run the command and accumulate elapsed times
	for ((i=1; i<=$count; i++)); do
		# Run the command and capture the output
		./fadvise > /dev/null
		elapsed_time=$(time (echo 1 > /proc/sys/vm/drop_caches) 2>&1)
    
		# Check if the measurement is valid (non-empty)
		if [ -n "$elapsed_time" ]; then
			total_time=$(awk "BEGIN {print $total_time + $elapsed_time}")
			printf "%s, " $elapsed_time
		else
			echo "Error measuring elapsed time in iteration $i."
		fi
	done
	echo ""

	# Calculate and display the average elapsed time
	average_time=$(awk "BEGIN {print $total_time / $count}")
	echo "Average elapsed time: $average_time seconds"
done

make clean > /dev/null;
