```
./get.sh Q533449 P22
./extract_P22.sh Q53449_P22.json 
```

(#48) for `./grand-mother-tongue search 'Elizabeth II'
https://www.wikidata.org/w/api.php?action=wbsearchentities&search=Elizabeth%20II&language=en&format=json
is downloaded as `search-Elizabeth-II.json`
by using command:

`wget "https://www.wikidata.org/w/api.php?action=wbsearchentities&search=Elizabeth%20II&language=en&format=json" -O tmp && cat tmp | jq . > search-Elizabeth-II.json && rm -f tmp;`

also:

`wget "https://www.wikidata.org/w/api.php?action=wbsearchentities&search=Napoleon&language=en&format=json -O tmp && cat tmp | jq . > search-Napoleon.json && rm -f tmp;`

