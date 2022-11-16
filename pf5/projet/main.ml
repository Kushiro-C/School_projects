open Lsystems (* Librairie regroupant le reste du code. Cf. fichier dune *)
open Systems (* Par exemple *)
open Turtle
open Graphics

(** Gestion des arguments de la ligne de commande.
    Nous suggérons l'utilisation du module Arg
    http://caml.inria.fr/pub/docs/manual-ocaml/libref/Arg.html
*)

let usage = (* Entete du message d'aide pour --help *)
  "Interpretation de L-systemes et dessins fractals"

let action_what () = Printf.printf "%s\n" usage; exit 0

(*************************************)

(* loop_action : command list list -> int -> unit
    Loop from the drawn graph, waiting for the next action
    Can show the next graph (through another substitution),
    the previous one, or leave.
    Resets current action if we do an action while
    the animation is drawing *)
let rec loop_action (cmd_list_list : command list list)
    i size (subs : 's word list) (lsys : 's Systems.system )=

  try
    let ev = wait_next_event [Key_pressed] in
    if ev.keypressed then
      match ev.key with
      | 'q' -> close_graph(); (* Quit *)
      | 'n' -> clear_graph(); (* Apply and draw next substitution *)
        if (i + 1) >= size then
          let sub = at subs i in
          let sub2 = next_substitution sub lsys.rules in
          let cmd2 = word_to_cmd_list sub2 lsys.interp in
          let new_cmd_list = re_dimension cmd2 in
          let t = start_point new_cmd_list in
          let _ = action_list new_cmd_list t in
          loop_action (cmd_list_list @ [new_cmd_list])
            (i+1) (size+1) (subs @ [sub2]) lsys

        else
          let cmd_list = at cmd_list_list (i+1) in
          let t = start_point cmd_list in
          let _ = action_list cmd_list t in
          loop_action cmd_list_list (i+1) size subs lsys;
      | 'b' ->    (* Back : previous substitution *)
        if i > 0 then
          let cmd_list = at cmd_list_list (i-1) in
          clear_graph();
          let t = start_point cmd_list in
          let _ = action_list cmd_list t in
          loop_action cmd_list_list (i-1) size subs lsys;

        else
          loop_action cmd_list_list i size subs lsys;
      | _ -> loop_action cmd_list_list i size subs lsys;

  with _ -> loop_action cmd_list_list i size subs lsys
;;

(* loop_menu : turtle -> command list -> int -> command list list -> int -> unit
    Loop the menu, change color and then start drawing *)
let rec loop_menu color
    (cmd_list_list : command list list) (subs : 's word list) lsys =

  moveto (600-160) (500);
  draw_string "Press c to change color and then press s to Start.";
  moveto (600-240) (400);
  draw_string "When the figure is drawn press n to move to the
   next subtitution, and b to go back.";
  let ev = wait_next_event [Key_pressed] in

  if ev.keypressed then
    match ev.key with
    | 's'  -> clear_graph(); (* change bottom line to t *)
      let cmd_list = re_dimension (at cmd_list_list 0)  in
      let t = start_point cmd_list in
      let _ = action_list cmd_list t in
      loop_action [cmd_list] 0 1 subs lsys;
    | 'q'  -> close_graph();
    | 'c'  -> set_color colors.((color + 1) mod 8);
      loop_menu ((color +1) mod 8) cmd_list_list subs lsys
    |  _   -> loop_menu color cmd_list_list subs lsys
;;

(* read_file_and_draw : string s -> unit
    Lis un fichier au suffixe ".sys" donné en paramètre
    On suppose que les ".sys" ont un bon format Lsystèmes
    Ensuite affiche graphiquement l'image attendue *)
let read_file_and_draw filename =
  if not (Systems.is_suffix_sys filename)
  then
    Printf.printf "Le fichier %s n'a pas pour suffixe .sys\n" filename
  else
    let file_contents = Systems.read_file_contents filename in (* Full file *)
    let file_no_comments = Systems.remove_comments file_contents in

    (* Get axiome *)
    let axiom_str = Systems.get_line_axiom file_no_comments in

    (* Get règles *)
    let rules_str_arr = Systems.get_lines_rules file_no_comments in

    (* Get interprétations *)
    let interp_str_arr = Systems.get_lines_interp file_no_comments in

    (* Get System : axiom, rules, interp; pour un fichier *)
    let ax = Systems.get_axiom file_no_comments in
    let ru = Systems.get_rules file_no_comments in
    let inter = Systems.get_interp file_no_comments in

    let lsys : 's Systems.system =
      {
        axiom = ax;
        rules = ru;
        interp = inter
      }
    in

    let cmd = Systems.word_to_cmd_list ax lsys.interp in

    let call_turtle_gui (cmd_list_list : command list list) i =
      match cmd_list_list with
      | [] -> Printf.printf "erreur aucune commande\n"
      | x :: y -> 
        open_graph " 1200x800";
        set_color (rgb 255 105 180);
        let new_cmd_list = re_dimension x in
        let t = start_point new_cmd_list in
        loop_menu 0 [new_cmd_list] [ax] lsys
    in

    call_turtle_gui [cmd] 0;
;;



let cmdline_options = [
  ("--what" , Arg.Unit action_what, "Description");
  ("--sys" , Arg.String read_file, "Lecture d'un fichier .sys");
  ("--t_gui" ,Arg.Unit test_gui, "Lance un test pour un graphe prédésigné");
  ("--lsys" , Arg.String read_file_and_draw, 
   "Lecture d'un fichier .sys et le dessine");
]

let extra_arg_action = fun s -> failwith ("Argument inconnu :"^s)

let main () =
  Arg.parse cmdline_options extra_arg_action usage

(** On ne lance ce main que dans le cas d'un programme autonome
    (c'est-à-dire que l'on est pas dans un "toplevel" ocaml interactif).
    Sinon c'est au programmeur de lancer ce qu'il veut *)

let () = if not !Sys.interactive then main ()
