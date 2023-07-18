for FILE in `ls auto-tests/*.l22`; do

    # Test name
    echo ${FILE}

    ./l22/l22 $FILE 2>&1 -o test.asm | tail -n 1
done