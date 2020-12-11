#!/bin/zsh

CLASSPATH=po-uuilib/po-uuilib.jar:project/woo-core/woo-core.jar:project/woo-app/woo-app.jar

failures=0
successes=0
total=0

for x in auto-tests/*.in; do
    if [ -e ${x%.in}.import ]; then
        java -cp $CLASSPATH -Dimport=${x%.in}.import -Din=$x -Dout=${x%.in}.outhyp woo.app.App
    else
        java -cp $CLASSPATH                      -Din=$x -Dout=${x%.in}.outhyp woo.app.App
    fi

    diff -cBb -w ${x%.in}.out ${x%.in}.outhyp > ${x%.in}.diff
    if [ -s ${x%.in}.diff ]; then
        echo "\e[91mFAIL: \e[0m$x. See file $x:r.diff"
        let "failures += 1"
        let "total += 1"
    else
        echo -n "\e[92mSUCCESS: \e[0m$x\n"
        rm -f ${x%.in}.diff ${x%.in}.outhyp 
        let "successes += 1"
        let "total += 1"
    fi
done

echo "\e[36mTOTAL: \e[0m$total"
echo "\e[36mFAILURES: \e[0m$failures"
echo "\e[36mSUCCESSES: \e[0m$successes"
if [ ${failures} -eq 0 ]; then
    echo "\e[95mALL TESTS PASSED SUCCESSFULLY!"
fi



