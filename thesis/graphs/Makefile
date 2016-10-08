# classDiagRtlil: classDiagRtlil.dot
# 	dot2tex --codeonly \
# 	--figpreamble '\resizebox{\textwidth}{!}{\begin{tikzpicture}[line join=bevel,anchor=base]' \
# 	--figpostamble '\end{tikzpicture}}' \
# 	-t raw -f pgf --valignmode=dot --nominsize \
# 	--graphstyle="anchor=base" genericTrav.dot > genericTrav.tex
#
# genericTrav: genericTrav.dot
# 	dot2tex \
# 	--codeonly \
# 	--figpreamble '\resizebox{\textwidth}{!}{\begin{tikzpicture}[line join=bevel,anchor=base]' \
# 	--figpostamble '\end{tikzpicture}}' \
# 	-t raw -f pgf --valignmode=dot --nominsize \
# 	--graphstyle="anchor=base" genericTrav.dot > genericTrav.tex
#
# genericTravStateFour: genericTravStateFour.dot
# 	dot2tex \
# 	--codeonly \
# 	--figpreamble '\resizebox{0.6\textwidth}{!}{\begin{tikzpicture}[line join=bevel,anchor=base]' \
# 	--figpostamble '\end{tikzpicture}}' \
# 	-t raw -f pgf --valignmode=dot --nominsize \
# 	--graphstyle="anchor=base" genericTravStateFour.dot > genericTravStateFour.tex


%.tex: *.dot
	dot2tex --codeonly \
	--figpreamble '\resizebox{\textwidth}{!}{\begin{tikzpicture}[line join=bevel,anchor=base]' \
	--figpostamble '\end{tikzpicture}}' \
	-t raw -f pgf --valignmode=dot --nominsize \
	--graphstyle="anchor=base" $*.dot > $*.dot.tex

%.tex.nonraw: *.dot
	dot2tex --codeonly \
	--figpreamble '\resizebox{\textwidth}{!}{\begin{tikzpicture}[line join=bevel,anchor=base]' \
	--figpostamble '\end{tikzpicture}}' \
	-f pgf --valignmode=dot --nominsize \
	--graphstyle="anchor=base" $*.dot > $*.dot.tex

%.tex.neato: *.dot
	dot2tex --codeonly --prog neato --autosize \
	--figpreamble '\resizebox{\textwidth}{!}{\begin{tikzpicture}' \
	--figpostamble '\end{tikzpicture}}' \
	-f tikz --nominsize $*.dot > $*.dot.tex
