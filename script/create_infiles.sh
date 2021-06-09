#! /bin/bash


#setting arguments 
inputFile=$1
input_dir=$2
numFilesPerDirectory=$3

#checking if i have 3 arguments 
if [ $# -ne 3 ];
then
    echo "Give 3 arguments"
    exit
fi
#checking if the inputFile exists and is not empty
if [ ! -s $inputFile ];
then
    echo "InputFile does not exist or has no data"
    exit
fi
# checking if the directory already exists
if [ -d $input_dir ];
then 
    echo "input_dir already exists"
    exit
fi   

#checking if the last arguments is a valid arguments, it must be a number and positive

if   [[ ! $numFilesPerDirectory =~ ^-?[0-9]+$ ]] || [ $numFilesPerDirectory -lt 0 ];
then
    echo "numFilesPerDirectory must be an integer and a positive number "
    exit
fi

#create directory

mkdir -p $input_dir
declare -a countries

cd $input_dir
 #taking the countries from inputFile 
 
#creatind directories and .txt inside
 while IFS= read -r line;
 do
    country=$(echo $line | awk '{print $4}')
    if [ ! -d $country ]; then
        countries+=("$country")
        mkdir $country 
        cd $country
        for (( numberOfTxt=0; numberOfTxt<$numFilesPerDirectory; numberOfTxt++ ));
        do
            touch "${country}-$((${numberOfTxt}+1)).txt"
        done
        
        # echo $country

        cd ../
        
    fi

done < ../$inputFile
declare -a eachLine;

for(( i=0; i<${#countries[@]}; i++ ));
do
    valueOftxt=0
    country=${countries[$i]}
    cd $country
 
    while IFS= read -r inner;
    do 
        
        # innerCountry=$(echo $inner | awk '{print $4}')
        IFS=' ' read -r -a eachLine <<< $inner
        innerCountry=${eachLine[3]}
        if [ $innerCountry == $country ];
        then
            if [ $(($valueOftxt % $numFilesPerDirectory)) -eq 0 ];
            then
               
                valueOftxt=0
            fi
            valueOftxt=$((valueOftxt+1))
            echo $inner >> "${country}-${valueOftxt}.txt"
        fi

    done < ../../$inputFile
     cd ../
done



# 