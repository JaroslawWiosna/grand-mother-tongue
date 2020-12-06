#!/usr/bin/env bash

mkdir -p locale/pl/LC_MESSAGES

#xgettext --keyword=_ --language=C --add-comments --sort-output -o po/grand-mother-tongue.pot src/main.cpp
#msginit --input=po/grand-mother-tongue.pot --locale=pl --output=po/pl/grand-mother-tongue.po
#msgfmt --output-file=locale/pl/LC_MESSAGES/grand-mother-tongue.mo po/pl/grand-mother-tongue.po

#TODO: Not sure if fuzzy translation is needed

xgettext --keyword=_ --language=C --add-comments --sort-output -o po/grand-mother-tongue.pot src/main.cpp
msgmerge --update po/pl/grand-mother-tongue.po po/grand-mother-tongue.pot
msgfmt --output-file=locale/pl/LC_MESSAGES/grand-mother-tongue.mo po/pl/grand-mother-tongue.po


