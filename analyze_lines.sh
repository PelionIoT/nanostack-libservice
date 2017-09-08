#!/bin/bash

nbr=0

filename=$1

declare -A perm_arr
declare -A p_arr
declare -A temp_arr

interest="sw_mac.c"

while read -r line || [ -n "$line" ]
do
	((nbr+=1))
	if [[ "$line" == *"MEMMEM"* ]]
	then
		if [[ "$line" == *"temp allocated"* ]]
		then
			addr=$(echo "$line" | sed 's/.* - 0x//')
			addr=$(echo "$addr" | sed 's/ temp .*//')
			temp_arr[$addr]="$nbr - $line"
		elif [[ "$line" == *'allocated'* ]]
		then
			addr=$(echo "$line" | sed 's/.* - 0x//')
			addr=$(echo "$addr" | sed 's/ allocated .*//')
			p_arr[$addr]="$nbr - $line"
		else
			addr=$(echo "$line" | sed 's/.* -- Block: 0x//')
			addr=$(echo "$addr" | sed 's/ freed.*//')
			unset temp_arr[$addr]
			unset p_arr[$addr]
		fi
		#TODO: change this to some sensible info
		#echo "$nbr - $line"
	fi
done < $filename
#done < "node_0.log"

for K in "${!p_arr[@]}"; do 
	log_line=${p_arr[$K]}
	file=$(echo "$log_line" | sed 's/.*File: //')
	file=$(echo "$file" | sed 's/, l:.*//')
	size=$(echo "$log_line" | sed 's/.*(size: //')
	size=$(echo "$size" | sed 's/ bytes).*//')

	if [[ $file == *$interest* ]]; then
		echo "$log_line"
	fi

	if [[ ${perm_arr[$file]+t} ]]
	then
		perm_arr[$file]=$((${perm_arr[$file]}+$size))
	else
		perm_arr[$file]=$size
	fi
done

echo "Permanent allocations:"
echo ""
for K in "${!perm_arr[@]}"; do echo $K --- ${perm_arr[$K]};done
echo ""
echo ""
echo ""
echo ""
echo "unfreed temporaries"
echo ""
echo ""
for K in "${!temp_arr[@]}"; do echo $K --- ${temp_arr[$K]};done
