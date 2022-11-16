type op = Equal


type identificateur =
  string

(*expression*)
type def = string * expression 
and expression=
  | Plus of expression * expression (*expressionSuite*)
  | Moins of expression * expression (*expressionSuite*)
  | Ident of identificateur * expression
  | Let of def * expression
  (*| epsilon*)

type instr = string * instruction
and instruction=
  | Avance of expression
  | Tourne of expression
  | BasPinceau
  | HautPinceau
  | Ident of identificateur * op * expression
  | Bloc of instruction list(*blocInstruction*)
  (*| epsilon*)

(*Pragramme*)
type programme = declarations * instr
and declarations =
  | Var of identificateur
  | Decl of declarations



