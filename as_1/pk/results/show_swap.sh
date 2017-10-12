
a=3
while [ "$a" -le 9 ]
do

grep -r '\bavg_swap_length:\s' "thread_test_sp${a}_${1}.txt"

a=`expr $a + 2`
done

