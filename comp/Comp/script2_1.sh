#!/usr/bin/bash
total=0
count=0

for d in auto-tests/*.l22
do
    l22/l22 --target asm $d
    test_name=${d#auto-tests/}
    test_name=${test_name%.l22}
    yasm -felf32 auto-tests/$test_name.asm
    mv $test_name.o auto-tests/
    ld -melf_i386 -o main auto-tests/$test_name.o -L ${HOME}/compiladores/root/usr/lib/ -lrts
    mv main auto-tests/
    total=$(( $total + 1 ))
    diff -u auto-tests/expected/$test_name.out <(./auto-tests/main)
    diff=$?
    if [ $diff != 0 ]; then
        echo "$d"
        count=$(( $count + 1 ))
    fi
    rm auto-tests/$test_name.o
    rm auto-tests/main
done

echo "total" $total
echo "failing" $count
echo "output on" $(($total-$count))
