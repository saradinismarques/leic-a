#!/usr/bin/bash

FAIL_C='\033[0;35m'
SUC_C='\033[0;36m'
TMT_C='\033[0;31m'
NC='\033[0m'

total=0
tmtout=0
comp_fail=0
fail=0
success=0

cd auto-tests

for d in ./*.l22
do
    ../l22/l22 --target asm $d &> /dev/null || echo asm
    #../l22/l22 --target xml $d &> /dev/null
    yasm -felf32 ${d: : -4}.asm &> /dev/null || echo yasm
    total=$(( $total + 1 ))
    ld -melf_i386 -o ${d: : -4}.exe ${d: : -4}.o -L ${HOME}/compiladores/root/usr/lib/ -lrts &> /dev/null
    
    if [ $? != 0 ]; then
        comp_fail=$(( $comp_fail + 1 ))
        echo -e ${d: : -4}
        echo -e "${FAIL_C}Failed Compilation${NC}"
    fi
done

for d in ./*.exe
do
    echo ${d: : -4}
    timeout 0.1 ./$d > ${d: : -4}.out
    if [ ! $? -eq 124 ]; then
        diff -w ${d: : -4}.out expected/${d: : -4}.out 

        if [ $? != 0 ]; then
            fail=$(( $fail + 1 ))
            echo -e "${FAIL_C}FAIL${NC}"

        else
            success=$(( $success + 1 ))
            echo -e "${SUC_C}SUCksASS${NC}"
        fi
    else
        tmtout=$(( $tmtout + 1 ))
        echo -e "${TMT_C}TIMEOUT${NC}"
    fi
    echo
done

cd ..

echo "TOTAL" $total
echo "|- ${TMT_C}FAILED COMPILATION:${NC} $comp_fail"
echo "|- ${TMT_C}TIMEOUT:${NC} $tmtout"
echo "|- ${FAIL_C}FAILING:${NC} $fail"
echo "|- ${SUC_C}SUCCESS:${NC} $success"