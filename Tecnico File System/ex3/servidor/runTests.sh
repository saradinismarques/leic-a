#!/usr/bin/env bash
inputdir=$1
outputdir=$2
maxthreads=$3


if [ $# \< 3 ]; then
  echo "Error: Must provide 3 arguments: inputdir outpudir maxthreads."
else
  if [ -d "$inputdir" ]; then
    if [ -d "$outputdir" ]; then
      for i in $inputdir/*; do
        output=$(echo $i- \
        | sed "s@$inputdir@$outputdir/@" \
        | sed 's/.txt//')
          for j in $(seq 1 $maxthreads); do
              echo -e "\e[36mInputFile = $i NumThreads= $j\e[0m "
              print=$(./tecnicofs $i $output$j.txt $j)
              echo $print | sed 's/.*\(TecnicoFS.*\)/\1/'
          done
      done
    else
      echo "Error: outputdir:${outputdir} is not a valid directory. Can not continue."
    fi
  else
    echo "Error: inputdir:${inputdir} is not a valid directory. Can not continue."
  fi
fi
