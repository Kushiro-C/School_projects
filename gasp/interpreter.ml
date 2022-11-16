open Ast

(* Get Value of a declaration *)
let get_decl_name = function
  Var x -> x

(* Initialize stored variables list as a List of (string, int) *)
let init_decl_list decls =
  let l = List.map ( function d -> (get_decl_name d) ) decls in
  List.map ( function name -> (name, 0)) l

(* Updates the value in the List *)
let updateList name value var_list =
  (name, value)::(List.remove_assoc name var_list)

(* Evaluates the result of an expression *)
let rec evalExp var_list = function
| Nombre x -> x
| Identi s -> List.assoc s var_list
| Plus (l, r) -> evalExp var_list l + evalExp var_list r
| Moins (l, r) -> evalExp var_list l - evalExp var_list r
| Mult (l, r) -> evalExp var_list l * evalExp var_list r
| Div (l, r) -> try evalExp var_list l / evalExp var_list r
                with Division_by_zero -> raise Division_by_zero

(* Prints a list of declarations *)
let rec print_decl_list = function
  | h::t -> print_decl h; print_decl_list t
  | [] -> Printf.printf "\n"
and print_decl = function
  Var x -> Printf.printf "Var %s;\n" x

(* Prints Instructions while updating *)
let rec print_instr var_list = function
  | Avance e -> Printf.printf "Avance %d;\n" (evalExp var_list e); var_list
  | Tourne e -> Printf.printf "Tourne %d;\n" (evalExp var_list e); var_list
  | BasPinceau -> Printf.printf "BasPinceau;\n"; var_list
  | HautPinceau -> Printf.printf "HautPinceau;\n"; var_list
  | Ident (s, eg, e) -> let eq = function Equal -> "=" in
                      let value = evalExp var_list e in
                      Printf.printf "%s %s %d;\n" s (eq eg) value;
                      updateList s value var_list
  | ChangeEpaisseur e -> Printf.printf "ChangeEpaisseur %d;\n" (evalExp var_list e); var_list
  | ChangeCouleur s -> Printf.printf "ChangeCouleur %s;\n" s; var_list
  | Cond (e, i, i2) -> Printf.printf "IF exp THEN instr ELSE instr :\n";
                      if (evalExp var_list e) == 0
                      then print_instr var_list i
                      else print_instr var_list i2
  | Cond2 (e, i) -> Printf.printf "IF exp THEN instr :\n";
                      if (evalExp var_list e) == 0
                      then print_instr var_list i
                      else (Printf.printf "\n"; var_list) (* No Else *)
  | Loop (e, i) -> Printf.printf "WHILE exp DO instr :\n";
                let rec lp ex instr var_list =
                  if (evalExp var_list ex) != 0
                  then lp ex instr (print_instr var_list i)
                  else var_list
                in
                lp e i var_list
  | BlocInstruction bi -> match bi with
                          | [] -> var_list
                          | h::t -> let res = print_instr var_list h in
                                print_instr res (BlocInstruction t)

(* Print the program *)
let print_program (d, i) =
  print_decl_list d;
  print_instr (init_decl_list d) i

exception UnexpectedVar

(* Matches the instruction, and returns the instruction
    with the expression solved as a Nombre
    Usage in newInstrList *)
let getDrawInstr var_list = function
  | Avance e -> Avance (Nombre (evalExp var_list e))
  | Tourne e -> Tourne (Nombre (evalExp var_list e))
  | BasPinceau -> BasPinceau
  | HautPinceau -> HautPinceau
  | ChangeEpaisseur e -> ChangeEpaisseur (Nombre (evalExp var_list e))
  | ChangeCouleur s -> ChangeCouleur s
  | _ -> raise UnexpectedVar

(* Create a new instruction list by replacing all variables
    with its Number expression and removes Ident instructions *)
let rec newInstrList var_list = function
  | [] -> []
  | h::t ->
      match h with
        | Ident (s, eg, e) ->
                      newInstrList (updateList s (evalExp var_list e) var_list) t
        | BlocInstruction bi -> newInstrList var_list (bi@t)
        | Cond (e, i, i2) ->
                      if (evalExp var_list e) == 0
                      then newInstrList var_list (i::t)
                      else newInstrList var_list (i2::t)
        | Cond2 (e, i) ->
                      if (evalExp var_list e) == 0
                      then newInstrList var_list (i::t)
                      else newInstrList var_list t
        | Loop (e, i) ->
                      let newLoop = Loop (e, i) in
                      if (evalExp var_list e) == 0
                      then newInstrList var_list t
                      else newInstrList var_list (i :: newLoop :: t)
        | _ -> (getDrawInstr var_list h) :: (newInstrList var_list t)

(* Execute the program with the graphic interface *)
let exec_program (d, i) =
  Interface.create_graph (newInstrList (init_decl_list d) [i])
