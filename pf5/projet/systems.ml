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

(** Put here any type and function implementations concerning systems *)


(* is_suffix_sys : string s -> bool
    Vérifie que le nom du fichier termine par .sys *)
let is_suffix_sys filename =
  let len = String.length(filename) in
  if len < 5 then (* On ne peut pas avoir .sys en suffixe *)
    false
  else
    try
      if (String.sub filename (len-4) 4) = ".sys" then
        true
      else
        false
    with Invalid_argument _ -> false
;;

(* read_file_contents : string -> a list
    Ouvre un fichier si possible, et récupère
    chaque ligne du fichier sous la forme d'une liste
    Sinon renvoie une erreur *)
let read_file_contents filename =
  let lines_list = ref [] in
  let fchan =
    try
      open_in filename
    with Sys_error _ -> failwith "Echec lors de l'ouverture du fichier"
  in

  try
    while true; do
      lines_list := input_line fchan :: !lines_list
    done;
    !lines_list
  with End_of_file ->
    close_in fchan;
    List.rev !lines_list
;;

(* print_list : string list -> unit
    Affiche le contenu d'une liste de strings
    On peut l'utiliser pour afficher le contenu d'un fichier *)
let rec print_list file_lines =
  match file_lines with
  | [] -> ()
  | line :: l -> Printf.printf "%s\n" line; print_list l
;;

(* print_word : 's word -> unit
    Affiche un type 's word
    Les 'Branch' sont interprétés par des '[]' *)
let rec print_word word =
  let rec print_seq seq =
    match seq with
    | [] -> ()
    | h :: t -> print_word h; print_seq t
  in

  match word with
  | Symb s -> Printf.printf "%c" s
  | Seq seq -> print_seq seq
  | Branch b ->
    Printf.printf "["; print_word b; Printf.printf "]"
;;

(* print_cmd : Turtle.command -> unit
    Affiche une commande de Turtle *)
let print_cmd cmd =
  match cmd with
  | Turtle.Line i -> Printf.printf "Line %d " i
  | Turtle.Move i -> Printf.printf "Move %d " i
  | Turtle.Turn i -> Printf.printf "Turn %d " i
  | Turtle.Store  -> Printf.printf "Store "
  | Turtle.Restore -> Printf.printf "Restore "
;;

(* print_interp : Turtle.command list -> unit
    Affiche une interprétation, qui consiste
    en une liste de commandes de Turtle *)
let print_interp cmd_list =
  let rec print_aux cmd_list =
    match cmd_list with
    | [] -> ()
    | h :: t -> print_cmd h; print_aux t
  in
  Printf.printf "[ ";
  print_aux cmd_list;
  Printf.printf "]\n";
;;

(* print_cmd_list_list : command list list -> unit
    Affiche une interprétation, qui consiste
    en une liste de liste de commandes de Turtle *)
let print_cmd_list_list l =
  let rec print_aux l =
    match l with
    | [] -> ()
    | h :: t -> print_interp h; print_aux t
  in
  Printf.printf "[ ";
  print_aux l;
  Printf.printf "]\n";
;;

(* is_comment : string -> bool
    Vérifie si la ligne donnée en paramètre commence
    par un '#', donc qui est une ligne commentaire.
    Renvoie true si c'est le cas, false sinon *)
let is_comment line =
  try
    if (String.get line 0) = '#' then
      true
    else
      false
  with Invalid_argument _ -> false
;;

(* remove_comments : string list -> string list
    Prend une liste de lignes (string) et retire
    les lignes qui commencent par un '#',
    correspondant à des commentaires. *)
let remove_comments file_lines =
  List.filter (function line -> not (is_comment line)) file_lines
;;

(* sublist : int -> int -> a list
    Prend une liste, et deux entiers
    (begin, end), pour récupérer une
    sous-liste entre les indices b et e *)
let rec sublist b e l =
  match l with
  | [] -> failwith "Could not get sublist"
  | h :: t ->
    let tail =
      if e = 0 then
        []
      else
        sublist (b-1) (e-1) t
    in

    if b > 0 then
      tail
    else
      h :: tail
;;

(* first_empty_line_index : string list -> int
    Renvoie le premier indice de la liste
    qui correspont à une ligne vide *)
let first_empty_line_index (l : string list) =
  let rec aux_empty_line li idx =
    match li with
    | [] -> failwith "No empty line"
    | h :: t ->
      if (String.length h) = 0 then
        idx
      else
        aux_empty_line t (idx + 1)
  in
  aux_empty_line l 0
;;

(* remove_axiom_from_lines : string list -> string list
    Renvoie une liste de lignes (sans commentaires)
    et sans les lignes du début concernant l'axiome *)
let rec remove_axiom_from_lines lines =
  match lines with
  | [] -> failwith "Empty lines"
  | h :: t ->
    if String.length h = 0 then
      t
    else
      remove_axiom_from_lines t
;;

(* get_line_axiom : string list -> string
    Renvoie la ligne correspondant à l'axiome
    Il s'agit de la première ligne sans commentaires
    Le paramètre ne doit pas contenir de commentaires *)
let get_line_axiom lines =
  match lines with
  | [] -> failwith "Empty lines"
  | first_line :: _ -> first_line
;;

(* get_lines_rules : string list -> string list
    Renvoie une liste de string correspondant
    aux règles des substitutions
    On appelle la fonction avec le contenu
    d'un fichier.sys sans commentaires *)
let get_lines_rules lines =
  let no_axiom = remove_axiom_from_lines lines in
  let idx_sep = first_empty_line_index no_axiom in
  sublist 0 (idx_sep - 1) no_axiom
;;

(* get_lines_interp : string list -> string list
    Renvoie une liste de string correspondant
    aux interprétations
    On appelle la fonction avec le contenu
    d'un fichier.sys sans commentaires *)
let get_lines_interp lines =
  let no_axiom = remove_axiom_from_lines lines in
  let idx_sep = first_empty_line_index no_axiom in
  sublist (idx_sep + 1) (List.length no_axiom - 1) no_axiom
;;

(* str_to_word : string -> 's word
    Prends un String, et renvoie
    sa conversion en type 's word
    Ce str ne doit pas contenir d'espaces *)
let str_to_word str =
  let len = String.length str in
  if len = 0 then
    failwith "Empty String"
  else if len = 1 then
    Symb str.[0] (* len = 1 donc pas de Invalid_Argument pour 0 *)
  else
    let word = ref [] in
    let i = ref 0 in
    while !i < len do
      if (str.[!i] = '[') then (* On crée une Branch *)
        let br = ref [] in
        i := !i + 1;
        while not (str.[!i] = ']') do
          br := !br @ [Symb str.[!i]];
          i := !i + 1
        done;
        word := !word @ [Branch (Seq !br)] (* On ajoute la Branch *)
      else
        word := !word @ [Symb str.[!i]]; (* Pas dans une Branch *)
      i := !i + 1
    done;
    Seq !word
;;

(* get_axiom : string list -> 's word
    A partir du contenu du fichier sans commentaires,
    on crée et renvoie un 's word, pour l'axiome *)
let get_axiom lines =
  let ax = get_line_axiom lines in (* string *)
  str_to_word ax
;;

(* get_rule_of : char -> string list -> string
    Cherche dans une liste 'l' de string, si le
    caractère 'c' existe, et renvoie la règle
    qui correspond en string.
    Sinon renvoie le char sous forme d'un String *)
let rec get_rule_of c l =
  match l with
  | [] -> String.make 1 c
  | h :: t ->
    try
      if h.[0] = c then
        String.sub h 2 (String.length h - 2)
      else
        get_rule_of c t
    with Invalid_argument _ -> "File does not respect .sys format"
;;

(* get_rules : string list -> 's rewrite_rules
    A partir du contenu du fichier sans commentaires,
    on extrait les 'rules' puis on les renvoies
    sous son nouveau type *)
let get_rules lines : 's rewrite_rules =
  let ru = get_lines_rules lines in (* string list *)
  (function c -> str_to_word (get_rule_of c ru))
;;

(* turtle_cmd : string -> Turtle.command
    Prend un string représentant une commande
    d'après le type .sys (L5; T-10; etc...)
    et le transforme en une commande de Turtle *)
let turtle_cmd str =
  try
    let value = int_of_string (String.sub str 1 (String.length str - 1)) in

    match str.[0] with
    | 'L' -> Turtle.Line value
    | 'M' -> Turtle.Move value
    | 'T' -> Turtle.Turn value
    |  _  -> failwith "Could not get Turtle command"

  with _ ->
    if String.length str > 1 then
      failwith "Turtle Command Wrong Format"
    else
      match str.[0] with
      | 'S' -> Turtle.Store
      | 'R' -> Turtle.Restore
      |  _  -> failwith "File does not respect .sys format"
;;

(* str_to_cmd_list : string -> Turtle.command list
    Transforme un string en une liste
    de commande Turtle.
    On traite aussi le cas où on peut avoir plusieurs
    commandes pour un symbole, même si les exemples
    donnés n'ont qu'une commande *)
let str_to_cmd_list str =
  let rec cmd_list_aux l =
    match l with
    | [] -> []
    | h :: t -> turtle_cmd h :: cmd_list_aux t
  in

  let str_list = String.split_on_char ' ' str in
  cmd_list_aux str_list
;;

(* get_cmd_list_of : char -> string list -> Turtle.command list
    Cherche la commande de tortue dans la liste donnée
    et renvoie une liste de commandes *)
let rec get_cmd_of c l =
  match l with
  | [] -> failwith "Command not found"
  | h :: t ->
    try
      if h.[0] = c then
        str_to_cmd_list (String.sub h 2 (String.length h - 2))
      else
        get_cmd_of c t
    with Invalid_argument _ -> failwith "File does not respect .sys format"
;;

(* get_interp : string list -> 's -> Turtle.command list = <fun>
    A partir du contenu du fichier sans commentaires,
    on extrait les interprétations, puis on les
    renvoies sous forme de liste de commandes
    pour la tortue à exécuter *)
let get_interp lines =
  let inter = get_lines_interp lines in
  (function c -> get_cmd_of c inter)
;;


(* next_substitution : 's word -> 's rewrite_rules -> 's word
    Applique une substitution à un mot donné,
    renvoie le mot après une substitution *)
let rec next_substitution word rules =
  let rec sub_seq seq =
    match seq with
    | [] -> []
    | h :: t -> (next_substitution h rules) :: (sub_seq t)
  in

  match word with
  | Symb s -> rules s
  | Seq seq -> Seq (sub_seq seq)
  | Branch b -> Branch (next_substitution b rules)
;;

(* word_to_cmd_list : 's word -> ('s -> Turtle.command list)
   -> Turtle.command list
    Renvoie une liste de commandes pour la Tortue,
    à partir du mot donné, et des interprétations
    pour chaque symboles *)
let rec word_to_cmd_list word interp =
  let rec cmd_seq seq =
    match seq with
    | [] -> []
    | h :: t -> (word_to_cmd_list h interp) :: (cmd_seq t)
  in

  match word with
  | Symb s -> interp s
  | Seq seq -> List.flatten (cmd_seq seq)
  | Branch b -> Turtle.Store :: (word_to_cmd_list b interp) @ [Turtle.Restore]
;;

(* read_file : string s -> unit
    Lis et traite un fichier donné en paramètre
    Le fichier doit avoir un suffixe ".sys"
    Et on suppose que les ".sys" ont un bon format Lsystèmes
    FONCTION POUR TESTER (NON GRAPHIQUE) *)
let read_file filename =
  if not (is_suffix_sys filename) then
    Printf.printf "Le fichier %s n'a pas pour suffixe .sys\n" filename
  else
    let file_contents = read_file_contents filename in (* Full file *)
    let file_no_comments = remove_comments file_contents in

    (* Affiche axiome *)
    let axiom_str = get_line_axiom file_no_comments in
    Printf.printf "%s\n\n" axiom_str;

    (* Affiche règles *)
    let rules_str_arr = get_lines_rules file_no_comments in
    print_list rules_str_arr;
    Printf.printf "\n";

    (* Affiche interprétations *)
    let interp_str_arr = get_lines_interp file_no_comments in
    print_list interp_str_arr;

    (* Affichage : axiom, un rules, une interp; pour un fichier *)
    Printf.printf "\nAffichage type word\n\n";
    let ax = get_axiom file_no_comments in
    let ru = get_rules file_no_comments in
    let inter = get_interp file_no_comments in

    let lsys : 's system =
      {
        axiom = ax;
        rules = ru;
        interp = inter
      }
    in

    (* On affiche à travers un 's system *)
    print_word lsys.axiom;
    Printf.printf "\n";
    print_word (lsys.rules 'A');
    Printf.printf "\n";
    print_interp (lsys.interp 'A');

    (* Test Substitution & Command List*)
    Printf.printf "\nSUBSTITUTIONS ET COMMANDES\n\n";
    let sub = next_substitution lsys.axiom lsys.rules in
    let cmd = word_to_cmd_list sub lsys.interp in
    print_word sub;
    Printf.printf "\n";
    print_interp cmd;
    Printf.printf "\n";

    let sub2 = next_substitution sub lsys.rules in
    let cmd2 = word_to_cmd_list sub2 lsys.interp in
    print_word sub2;
    Printf.printf "\n";
    print_interp cmd2;
    Printf.printf "\n";
;;