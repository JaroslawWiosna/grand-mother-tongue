[![Build Status](https://github.com/JaroslawWiosna/grand-mother-tongue/workflows/CI/badge.svg)](https://github.com/JaroslawWiosna/grand-mother-tongue/actions)

# 👪 grand-mother-tongue  👵   👴

> Provides percentage composition of one's blood in terms of nationality/native language

<p align="center">
  <img src="https://raw.githubusercontent.com/JaroslawWiosna/grand-mother-tongue/pics/grand-mother-400-400.png" />
</p>


Have you ever wonder how many languages does your blood speak?
`grand-mother-tongue` can tell you, if only you know your *Wikidata item*.

Most likely you don't have your own *Wikidata item*. Too bad.
You can always check out the blood of your favourite nobleman instead...

## This is (0.y.z), so The public API should not be considered stable.

According to https://semver.org/spec/v2.0.0-rc.2.html 

`4. Major version zero (0.y.z) is for initial development. Anything may change at any time. The public API should not be considered stable.`

## Usage

```console
usage: ./grand-mother-tongue [--version] [--help] [-i <WIKIDATA ITEM>] 
                             [--db <path>] [--dump-db <path>]          
                             [--blood-pie-chart <path>] 
```

## Example

For example, for: `make -B && ./grand-mother-tongue Q454337` and `MAX_CURL_CALLS = 2000;` (based on 2dd4f45 )

```
<< ORIGIN BY BLOOD >>
François d'Orléans, Prince of Joinville
  de 0.213623
  fr 0.0979004
  da 0.012207
  nl 0.0443115
  oc 0.0321045
  pl 0.0340576
  es 0.0513916
  sv 0.000976562
  it 0.013916

```

## Emojis (just to see how they look in github's page

👑
🤴
👸
🩸
🇫🇷
🇩🇪



## references

https://stackoverflow.com/questions/34815172/how-to-get-all-property-values-labels-of-an-wikidata-item
https://www.wikidata.org/w/api.php?action=help&modules=wbgetclaims
https://youtu.be/15Uce4fG4R0 <-- Is Everyone a Descendant of Royalty? by UsefulCharts
https://youtu.be/Fm0hOex4psA <-- EVERY baby is a ROYAL baby - Numberphile


