(* Ce mini-projet porte sur l'apprentissage d'automates séparateurs.
   La lecture de la Section 16.3 des notes de cours est fortement
   recommandée. Le code que vous devez écrire prend en entrée deux
   listes de mots I et E, lues à partir d'un fichier passé en argument
   et renvoie sur la sortie standard le code SMT-LIB à faire passer
   dans une solveur SMT, comme Z3. 
 *)

open Printf
open List

(* ensembles de test : ensemble I *)
(* let li = ["";"ab"] *)

(* ensembles de test : ensemble E *)
(* let le = ["aa";"b"] *)



(* ======================================================================= *)
(* EXERCICE 1 : extraire  l'alphabet de l'automate.
   La fonction alphabet_from_list prend en entrée une liste de
   chaînes de caractères et retourne une liste triée de
   caractères sans duplication. 
 *)
(* explode : string -> char list 
   prend une chaîne de caractères et renvoie la liste de ses caractères 
 *)
let explode s =
  let rec exp i l =
    if i < 0 then l else exp (i - 1) (s.[i] :: l) in
  exp (String.length s - 1) []

(* alphabet_from_list : string list -> char list  
   - prend en entrée une liste l de chaînes de caractères 
   - renvoie la liste triée et sans duplication de caractères
     apparaissant dans l
 *)
let alphabet_from_list l =
  let rec toChar l =
    match l with
    | [] -> []
    | [x] -> explode x
    | h :: t -> explode h @ toChar t
    in
  List.sort_uniq Char.compare (toChar l)
;;

(* test *)
(* let a = alphabet_from_list (li @ le) *)

(* ======================================================================= *)
(* EXERCICE 2 : définition de l'alphabet A et de l'arbre préfixe T en
   SMT-LIB Pour les listes données en exemple, la sortie de la
   fonction declare_types doit être la chaîne de caractères
   "(declare-datatypes () ((A a b) (T e ea eaa eab eb)))" *)

(* prefixes : string -> string list
   renvoie la liste des préfixes d'une chaîne de caractères 
   Nous allons ajouter à chaque préfixe le caractère 'e'.
   Par exemple, prefixes "aba" = ["e"; "ea"; "eab"; "eaba"] *)
let prefixes s =
  let n = String.length s in
  List.init (n + 1) (fun i -> ("e" ^ (String.sub s 0 i)) )
;;

(* prefixes_of_list : string list -> string list
   renvoie la liste triée et sans doublons des préfixes des chaînes 
   de caractères d'une liste donnée *)
let prefixes_of_list l =
  let pref = List.map prefixes l in
  let flat = List.flatten pref in
  List.sort_uniq String.compare flat
;;

(* declare_types_alphabet : char list -> string
   prend une liste de caractères [c_1; ...; c_n] et renvoie une chaîne 
   de la forme "(A c_1... c_n)" *)
let declare_types_alphabet cl =
  let rec aux cl =
    match cl with
    | [] -> ""
    | [x] -> String.make 1 x
    | h :: t -> ((String.make 1 h)  ^ " " ^ aux t)
  in
  "(A " ^ aux cl ^ ")"
;;


(* declare_types_trie : string list -> string
   prend une liste l de chaînes de caractères et 
   renvoie une chaîne de la forme "(T es_1 ... es_n)" où 
   s_1... s_n est une énumération de tous les 
   prefixes des chaînes apparaissant dans l *)
let declare_types_trie l =
  let pref = prefixes_of_list l in
  let rec aux l =
    match l with
    | [] -> ""
    | [x] -> x
    | h :: t -> h  ^ " " ^ aux t
  in
  "(T " ^ aux pref ^ ")"
;;

(* declare_types : string list -> char list -> string
    la sortie de la fonction declare_types doit être
    la chaîne de caractères :
   "(declare-datatypes () ((A a b) (T e ea eaa eab eb)))" *)
let declare_types l cl =
  let decA = declare_types_alphabet cl in
  let decT = declare_types_trie l in
  "(declare-datatypes () (" ^ decA ^ " " ^ decT ^ "))\n"
;;

(* test *)
(* Printf.printf "%s\n" (declare_types (li @ le) a) *)


(* ======================================================================= *)
(* EXERCICE 3 : définir une chaîne de caractères pour les définitions
   en SMT-LIB de
   - l'ensemble d'états Q,
   - la fonction delta de transition de l'automate,
   - l'ensemble final des états acceptants et
   - la fonction f,
   ainsi que les assertions associées.
   Ces définitions ne dépendent pas des listes de mots acceptés ou rejetés. *)

let define_sorts_and_functions  =
  (* L'ensemble d'états Q *)
  "(define-sort Q () Int)\n" ^
  "(declare-const n Q)\n" ^
  "(assert (> n 0))\n" ^

  (* La fonction delta transmise à l'automate *)
  "\n(declare-fun delta (Q A) Q)\n" ^
  "(assert (forall ((q Q) (a A))\n" ^
  "  (and (>= (delta q a) 0) (< (delta q a) n))))\n" ^

  (* L'ensemble final des états acceptants *)
  "\n(declare-fun final (Q) Bool)\n" ^

  (* La fonction f *)
  "\n(declare-fun f (T) Q)\n" ^
  "(assert (forall ((x T))\n" ^
  "  (and (>= (f x) 0) (< (f x) n))))\n\n" ^
  "(assert (= 0 (f e)))\n\n" (* Contrainte état initial (55) *)
;;


(* ======================================================================= *)
(* EXERCICE 4 : contraintes sur les transitions
   La fonction assert_transition_constraints prend en entrée une trie 
   et retourne une chaîne qui modélise les contraintes sur les transitions 
   de l'automate décrites par la formule (56). *)
  
(* eq_trans_constr : string -> char -> string
   renvoie une chaîne de caractères qui correspond à une formule atomique pour 
   la transition étiquetée avec 'a' à partir de l'état s
   Par exemple, pour s = "abc" et  c = 'd' on a 
   eq_trans_constr outputs "(= (f abcd)  (delta (f abc)  d))" *)
let eq_trans_constr s a =
  let str_a = String.make 1 a in
  "(= (f " ^ s ^ str_a ^ ")  (delta (f " ^ s ^ ")  " ^ str_a ^ "))"
;;

(* list_transition_constraints : string list -> string list
   prend une liste de chaînes de caractères et génère une liste 
   de formules atomiques ou de chaînes vides comme suit
   - pour une chaîne non vide de la forme sa on obtient
     une chaine correspondant à l'équation f(sa) = delta (fs) a
   - pour la chaîne vide on obtient la chaîne vide *)
let rec list_transition_constraints l =
  match l with
  | [] -> []
  | h :: t ->
    let n = String.length h in
    if n < 2 then
      list_transition_constraints t
    else
      ((eq_trans_constr (String.sub h 0 (n-1)) (h.[n-1]))) ::
      list_transition_constraints t
;;


(* assert_transition_constraints : string list -> string
   prend en entrée une liste de mots et renvoie une chaîne qui modélise 
   les contraintes sur les transitions de l'automate décrit par la 
   formule (56).
   Par exemple, pour la liste [""; "ab"; "aa"; "b"] on obtient la chaîne
   "(assert (and 
               (= (f ea)  (delta (f e)  a))
               (= (f eaa)  (delta (f ea)  a))
               (= (f eab)  (delta (f ea)  b))
               (= (f eb)  (delta (f e)  b))))"
 *)
let assert_transition_constraints l =
  let pfx_list = prefixes_of_list l in
  let a = "(assert (and \n           " in
  let b = String.concat
    "\n           " (list_transition_constraints pfx_list) in

  a ^ b ^ "))\n"
;;

(* test *)
(* Printf.printf "%s" (assert_transition_constraints (li @ le)) *)

(* ======================================================================= *)
(* EXERCICE 5 : contraintes sur les états acceptants La fonction
   assert_acceptance prend en entrée deux listes de mots et retourne
   une chaîne de caractères qui modélise les contraintes sur les états
   acceptants décrites par la formule (57). *)

(* eq_accept : string -> string 
   - prend une chaîne de caractères s et renvoie une chaîne de caractères 
   qui modélise l'appartenance de s à l'ensemble final des états acceptants *)
let eq_accept s =
  "(final (f " ^ s ^ "))"
;;

(* eq_non_accept : string -> string 
   - prend une chaîne de caractères s et renvoie une chaîne de caractères 
   qui modélise la non-appartenance de s à l'ensemble final des états acceptants 
 *)
let eq_non_accept s =
  "(not(final (f " ^ s ^ ")))"
;;

(* assert_acceptance : string list -> string list > string
   prend deux listes de chaînes de caractères, li et le, et renvoie une
   chaine qui modélise les contraintes sur les états acceptants
   décrites par la formule (52). 
   Les mots dans li sont acceptés et les mots dans le ne le sont pas. *)
let assert_acceptance li le  =
  let pfx_li = List.map (fun s -> "e" ^ s) li in
  let pfx_le = List.map (fun s -> "e" ^ s) le in
  let constraints_li = List.map (fun s -> eq_accept s) pfx_li in
  let constraints_le = List.map (fun s -> eq_non_accept s) pfx_le in

  let a = "(assert (and \n           " in
  let b_li = String.concat "\n           " (constraints_li) in
  let b_le = String.concat "\n           " (constraints_le) in

  a ^ b_li ^ "\n           " ^ b_le ^ "))\n"
;;

(* test *)
(* Printf.printf "%s" (assert_acceptance li le) *)
  
(* ======================================================================= *)
(* EXERCICE 6 :
   La fonction smt_code prend en entrée deux listes de mots
   et retourne une chaîne de caractères qui donne l'implémentation 
   en SMT-LIB. *)

(* smt_code : string list -> string list -> string 
   prend deux listes de chaînes de caractères, li et le, et renvoie une chaîne 
   de caractères qui donne l'implémentation en SMT-LIB.
   Les mots dans li sont acceptés et les mots dans le ne le sont pas. 
   Pour vérifier votre algorithme, vous pouvez essayer le code SMT-LIB 
   que vous obtenez dans le solveur Z3: https://rise4fun.com/z3 *)
let smt_code li le =
  let l = li @ le in
  let al = alphabet_from_list l in
  let dec_data = declare_types l al in
  let const_transi = assert_transition_constraints l in
  let const_accept = assert_acceptance li le in
  let exit =
    "(check-sat-using (then qe smt))\n\n" ^
    "(get-model)\n\n" ^
    "(exit)\n"
  in

  dec_data ^ "\n" ^ define_sorts_and_functions ^
  const_transi ^ "\n" ^ const_accept ^ "\n" ^ exit
;;

(* test *)
(* Printf.printf "%s" (smt_code li le) *)


(* ======================================================================= *)
(* lire deux listes de chaîne de caractères I et E d'un fichier *)
(* Ne pas modifier cette partie *)

let input_line_opt ic =
  try Some (input_line ic)
  with End_of_file -> None
                    
let read_lines ic =
  let rec aux acc =
    match input_line_opt ic with
    | Some line -> aux (line::acc)
    | None -> (List.rev acc)
  in
  aux []
  
let lines_of_file filename =
  let ic = open_in filename in
  let lines = read_lines ic in
  close_in ic;
  (lines)

let read_lists_from_file (filename: string): ((string list) * (string list))  =
  let lines = lines_of_file filename in
  (String.split_on_char ' ' (List.nth lines 0),
   String.split_on_char ' ' (List.nth lines 1))
  
let () =
  let (li,le) = (read_lists_from_file Sys.argv.(1)) in
  Printf.printf "%s" (smt_code li le)
