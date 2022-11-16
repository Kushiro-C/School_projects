(* Get Position Error while Parsing *)
let print_position outx lexbuf =
  Lexing.(
    let pos = lexbuf.lex_curr_p in
    Printf.fprintf outx "Ligne %d Col %d"
      pos.pos_lnum
      (pos.pos_cnum - pos.pos_bol + 1)
  )

let _ =
  let lb = Lexing.from_channel stdin
  in
  try
    let ast =
      Parser.input Lexer.main lb
    in
      (* Interpreter.print_program ast *)
      Interpreter.exec_program ast
  with
  | Parser.Error ->
      Printf.fprintf stderr "%a: Syntax error\n" print_position lb;
      exit (-1)
  | Division_by_zero ->
      Printf.fprintf stderr "Error: Division by zero\n";
      exit (-1)
  | Not_found ->
      Printf.fprintf stderr "Error: Variable not set\n";
      exit (-1)
  | Interpreter.UnexpectedVar ->
      Printf.fprintf stderr "Error: Unexpected Variable type (Interpretation) received\n";
      exit (-1)
  | Interface.UnexpectedVar ->
      Printf.fprintf stderr "Error: Unexpected Variable type (Interface) received\n";
      exit (-1)
  | Interface.OutOfBounds ->
      Printf.fprintf stderr "Error: Drawing Out of Bounds\n";
      exit (-1)
  | Interface.UnexpectedColor ->
      Printf.fprintf stderr "Error: Unexpected color\n
          Available colors: [noir, blanc, rouge, vert, bleu, jaune, cyan, magenta]\n";
      exit (-1)