%{
open Ast
%}

%token OR AND LPAREN RPAREN TRUE FALSE EOF
%token<string> ID 
%left OR (*priorité croissant AND est prioritaire a OR*)
%left AND
(*make puis ./parser true \/ true \/ true puis ctrk +D*)

%start<Ast.expression> input

%%

(*le parser produit un arbre*)
input: c=expression EOF { c }

expression:
x=ID  { Var x }
| l=expression OR r=expression { Or (l, r) }
| l=expression AND r=expression { And (l, r) }
| TRUE { True}
| FALSE { False }
| LPAREN c=expression RPAREN { c }

(*expression:
LET d=def IN e = expression {Let(d,e)}
def: x=ID EQUALS e = expression {(x,e)}*)
