orig = propcalc.formula("(((((en<->t)&edge))|((en<->t)|(reset<->t)))<->t)->(edge<->t)");
orig.truthtable()
=> 
  en     t      edge   reset  value
  False  False  False  False  True   
  False  False  False  True   True   
  False  False  True   False  True   
  False  False  True   True   True   
  False  True   False  False  True   
  False  True   False  True   False  
  False  True   True   False  True   
  False  True   True   True   True   
  True   False  False  False  True   
  True   False  False  True   True   
  True   False  True   False  True   
  True   False  True   True   False  
  True   True   False  False  False  
  True   True   False  True   False  
  True   True   True   False  True   
  True   True   True   True   True   

latex(orig.truthtable())
=> 
  \\\begin{tabular}{lllll}reset & edge & t & en & value \\\hline False & False & False & False & True \\False & False & False & True & True \\False & False & True & False & True \\False & False & True & True & True \\False & True & False & False & True \\False & True & False & True & False \\False & True & True & False & True \\False & True & True & True & True \\True & False & False & False & True \\True & False & False & True & True \\True & False & True & False & True \\True & False & True & True & False \\True & True & False & False & False \\True & True & False & True & False \\True & True & True & False & True \\True & True & True & True & True\end{tabular}


