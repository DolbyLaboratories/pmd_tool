#!/bin/bash
#
# simple script to create language codes from lanugage .csv file
# all use the language-codes.csv file downloaded from"
#      https://datahub.io/core/language-codes
#
# where the codes are published with public domain license at:
#   https://opendatacommons.org/licenses/pddl/1.0/
#
# fields are ISO 639-2/B, ISO 639-2/T, ISO 639-1, English name ,French name
#
# usage: ./mklang.sh <outputfilename>

OUTPUT=$1
if [ "$#" -eq "0" ]
then
    echo "Error - expected output filename"
    exit
fi    

# remove 'local use' codes
sed -e '/qaa-qtz/d' language-codes.csv > tmp

# 1. extract ISO 639-2/B codes
awk -F, '{ n=split($1,array,""); printf("    MAKE_ISO_639_CODE("); for (i=1;i<=n;++i) { printf "\47"array[i]"\47"}; print "),     /* " $4 "*/"}' tmp | sort | uniq > t1

# 2. extract ISO 639-2/T codes
awk -F, '{ n=split($2,array,""); printf("    MAKE_ISO_639_CODE("); for (i=1;i<=n;++i) { printf "\47"array[i]"\47"}; print "),     /* " $4 "*/"}' tmp | sort | uniq | sed -e '/()/d' > t2

# 3. concatenate the two
cat t1 >> t2
cat t2 | sort | uniq > t1

# 4. extract ISO 639-1 codes
awk -F, '{ n=split($3,array,""); printf("    MAKE_ISO_639_CODE("); for (i=1;i<=n;++i) { printf "\47"array[i]"\47"}; print "\47\1340\47),    /* " $4 "*/"}' tmp | sort | uniq | sed -e '/(\x27\\0\x27)/d' > t2

# 5. add it to master list
cat t2 >> t1
cat t1 | sort | uniq > t2

# 4. replace !! with ',' and ! with ' (\x27 escapes single quote)
sed -e 's!\x27\x27!\x27,\x27!g' t2 > $OUTPUT

# 5. now dump supplementary files to use as part of testing
awk -F, '{print "    \"" $1 "\","}' tmp > iso-639-2b-codes.c
awk -F, '{print "    \"" $2 "\","}' tmp | sed -e '/\"\"/d' > iso-639-2t-codes.c
awk -F, '{print "    \"" $3 "\","}' tmp | sed -e '/\"\"/d' > iso-639-1-codes.c





