%{
open Ast
%}

%token<int> NOMBRE
%token<string> IDENTI
%token PLUS MOINS DIV MULT EQUAL
%token AVANCE TOURNE BASPINCEAU HAUTPINCEAU
%token CHANGEEPAISSEUR CHANGECOULEUR
%token VAR DEBUT FIN SEMICOLON EOF
%token IF THEN ELSE WHILE DO

(* Priority of operations *)
%left PLUS
%left MOINS
%left DIV MULT

%nonassoc THEN
%left ELSE

%start<Ast.programme> input

%%

input: p=programme EOF { p }

expression:
  | l=expression MOINS r=expression { Moins (l, r) }
  | MOINS e=expression              { Moins (Nombre 0, e) }
  | l=expression PLUS r=expression  { Plus (l, r) }
  | l=expression DIV r=expression   { Div (l, r) }
  | l=expression MULT r=expression  { Mult (l, r) }
  | s=IDENTI                        { Identi s }
  | n=NOMBRE                        { Nombre n }

instruction:
  | AVANCE e=expression           { Avance e }
  | TOURNE e=expression           { Tourne e }
  | BASPINCEAU                    { BasPinceau }
  | HAUTPINCEAU                   { HautPinceau }
  | s=IDENTI EQUAL e=expression   { Ident (s, Equal, e) }
  | CHANGEEPAISSEUR e=expression  { ChangeEpaisseur e }
  | CHANGECOULEUR s=IDENTI        { ChangeCouleur s }
  | IF e=expression
    THEN i=instruction
    ELSE i2=instruction           { Cond (e, i, i2) }
  | IF e=expression
    THEN i=instruction            { Cond2 (e, i) }
  | WHILE e=expression
    DO i=instruction              { Loop (e, i) }
  | DEBUT bi=instr* FIN           { BlocInstruction bi }

instr:
  i=instruction SEMICOLON { i }

declarations:
  | VAR s=IDENTI SEMICOLON  { Var s }

programme:
  d=declarations* i=instruction { (d, i) }
