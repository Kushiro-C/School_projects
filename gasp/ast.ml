type op = Equal

type identificateur =
  string

(* expressionSuite: recursive expression *)
type expression =
  | Plus of expression * expression
  | Moins of expression * expression
  | Div of expression * expression
  | Mult of expression * expression
  | Identi of identificateur
  | Nombre of int

type instruction =
  | Avance of expression
  | Tourne of expression
  | BasPinceau
  | HautPinceau
  | Ident of identificateur * op * expression
  | BlocInstruction of instruction list
  | ChangeEpaisseur of expression
  | ChangeCouleur of identificateur
  | Cond of expression * instruction * instruction (* IF...THEN...ELSE... *)
  | Cond2 of expression * instruction (* IF...THEN... *)
  | Loop of expression * instruction (* WHILE...DO... *)

(* Programme *)
type programme = declarations list * instruction
and declarations =
  Var of identificateur
