{
open Parser
}
(*transfomr les lettres en id(un token avec un identif qui le nomme)
, true et false et parenthese ne sont pas des id mais des tokens*)
let layout = [ ' ' '\t' '\n' ]
let ident_char = [ 'a'-'z' ]
let n1a9 = ['1'-'9']
let n1a9 = ['0'-'9']

(*true et false avant id*)
rule main = parse
  | layout		{ main lexbuf }
  | ')'			{ RPAREN }
  | '('			{ LPAREN }
  | "\\/"		{ OR }
  | "/\\"		{ AND }
  | "false"		{ FALSE } 
  | "true"		{ TRUE }
  | ident_char+		{ ID (Lexing.lexeme lexbuf) }
  | eof			{ EOF }
  | "let"   {LET}
  | _			{ failwith "unexpected character" }
