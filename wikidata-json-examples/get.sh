


wget "https://www.wikidata.org/w/api.php?action=wbgetclaims&format=json&entity=$1&property=$2" -O tmp && cat tmp | jq . > $1_$2.json && rm -f tmp;
