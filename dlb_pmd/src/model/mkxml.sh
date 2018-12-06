#!/bin/bash
#
# simple script to create language codes for XML schema from lanugage .csv file
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
awk -F, '{ if (length($1)) print("      <xs:enumeration value=\""$1"\"/>        <!-- " $4 " -->");}' tmp | sort | uniq > t1

# 2. extract ISO 639-2/T codes
awk -F, '{ if (length($2)) print("      <xs:enumeration value=\""$2"\"/>        <!-- " $4 " -->");}' tmp | sort | uniq > t2

# 3. concatenate the two
cat t1 >> t2
cat t2 | sort | uniq > t1

# 4. extract ISO 639-1 codes
awk -F, '{ if (length($3)) print("      <xs:enumeration value=\""$3"\" />        <!-- " $4 " -->");}' tmp | sort | uniq > t2

# 5. add it to master list
cat t2 >> t1
cat t1 | sort | uniq > t2

echo "  <xs:simpleType name=\"CountryCode\">" > ${OUTPUT}
echo "    <xs:restriction base=\"xs:string\">" >> ${OUTPUT}
cat t2 >> ${OUTPUT}
echo "    </xs:restriction>" >> ${OUTPUT}
echo "  </xs:simpleType>" >> ${OUTPUT}



