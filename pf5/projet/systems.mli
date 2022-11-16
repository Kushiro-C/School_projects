(** Words, rewrite systems, and rewriting *)

type 's word =
  | Symb of 's
  | Seq of 's word list
  | Branch of 's word

type 's rewrite_rules = 's -> 's word

type 's system = {
  axiom : 's word;
  rules : 's rewrite_rules;
  interp : 's -> Turtle.command list }

(** Put here any type and function interfaces concerning systems *)
val is_suffix_sys : string -> bool
val read_file_contents : string -> string list
val remove_comments : string list -> string list
val print_interp : Turtle.command list -> unit
val print_cmd_list_list : Turtle.command list list -> unit
val get_line_axiom : string list -> string
val get_lines_rules : string list -> string list
val get_lines_interp : string list -> string list
val get_axiom : string list -> char word
val get_rules : string list -> char rewrite_rules
val get_interp : string list -> char -> Turtle.command list
val next_substitution : 's word -> 's rewrite_rules -> 's word
val word_to_cmd_list : 's word -> ('s -> Turtle.command list)
  -> Turtle.command list
val read_file : string -> unit