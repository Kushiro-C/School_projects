{
open Lexing
open Parser

let next_line lexbuf =
  let pos = lexbuf.lex_curr_p in
  lexbuf.lex_curr_p <-
    { pos with pos_bol = lexbuf.lex_curr_pos;
                pos_lnum = pos.pos_lnum + 1
    }
}

let layout = [ ' ' '\t' ]
let ident_char = [ 'a'-'z' 'A'-'Z' ]
let digit = [ '0'-'9' ]



rule main = parse
  | layout		        { main lexbuf }
  | '\n'              { next_line lexbuf; main lexbuf }
  | "Var"             { VAR }
  | "Avance"          { AVANCE }
  | "Tourne"          { TOURNE }
  | "Debut"           { DEBUT }
  | "Fin"             { FIN }
  | "BasPinceau"      { BASPINCEAU }
  | "HautPinceau"     { HAUTPINCEAU }
  | "ChangeEpaisseur" { CHANGEEPAISSEUR }
  | "ChangeCouleur"   { CHANGECOULEUR }
  | "Si"              { IF }
  | "Alors"           { THEN }
  | "Sinon"           { ELSE }
  | "Tant que"        { WHILE }
  | "Faire"           { DO }
  | '='			          { EQUAL }
  | '+'			          { PLUS }
  | '-'			          { MOINS }
  | '/'			          { DIV }
  | '*'			          { MULT }
  | ';'			          { SEMICOLON }
  | ident_char+		    { IDENTI (Lexing.lexeme lexbuf) }
  | digit+    		    { NOMBRE (int_of_string (Lexing.lexeme lexbuf)) }
  | eof			          { EOF }
  | _			            { failwith "unexpected character" }