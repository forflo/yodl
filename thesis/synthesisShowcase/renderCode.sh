#!/bin/bash

if ./vhdlpp $1; then
    cp ~/schrottplatz/fnord.output.dot ${1%.vhd}.dot
    dot -T png ~/schrottplatz/fnord.output.dot > ${1%.vhd}.png

    dot2tex --codeonly \
            --figpreamble '\resizebox{\textwidth}{!}{\begin{tikzpicture}[line join=bevel,anchor=base]' \
            --figpostamble '\end{tikzpicture}}' \
            -f pgf --valignmode=dot --nominsize ${1%.vhd}.dot > ${1%.vhd}.tex
else
    echo "FAILURE"
fi
