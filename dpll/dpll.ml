open List

(* fonctions utilitaires *********************************************)
(* filter_map : ('a -> 'b option) -> 'a list -> 'b list
   disponible depuis la version 4.08.0 de OCaml dans le module List :
   pour chaque élément de `list', appliquer `filter' :
   - si le résultat est `Some e', ajouter `e' au résultat ;
   - si le résultat est `None', ne rien ajouter au résultat.
   Attention, cette implémentation inverse l'ordre de la liste *)
let filter_map filter list =
  let rec aux list ret =
    match list with
    | []   -> ret
    | h::t -> match (filter h) with
      | None   -> aux t ret
      | Some e -> aux t (e::ret)
  in aux list []


let rec filter p l =
  match l with
  |[] -> l
  |[x] -> if p x then [x] else []
  |a :: b -> if p a then a :: filter p b
    else filter p b;;

exception Clause_vide;;

(* print_modele : int list option -> unit
   affichage du résultat *)
let print_modele: int list option -> unit = function
  | None   -> print_string "UNSAT\n"
  | Some modele -> print_string "SAT\n";
     let modele2 = sort (fun i j -> (abs i) - (abs j)) modele in
     List.iter (fun i -> print_int i; print_string " ") modele2;
     print_string "0\n"

(* ensembles de clauses de test *)
let exemple_3_12 = [[1;2;-3];[2;3];[-1;-2;3];[-1;-3];[1;-2]]
let exemple_7_2 = [[1;-1;-3];[-2;3];[-2]]
let exemple_7_4 = [[1;2;3];[-1;2;3];[3];[1;-2;-3];[-1;-2;-3];[-3]]
let exemple_7_8 = [[1;-2;3];[1;-3];[2;3];[1;-2]]
let systeme = [[-1;2];[1;-2];[1;-3];[1;2;3];[-1;-2]]
let dependances = [[1];[-1;2];[-1;3;4;5];[-2;6];[-3;7];[-4;8;9];[-4;9];[-9;-6];[-9;-7];[-4;-5];[-8;-9];[-6;-7]]
let coloriage = [[1;2;3];[4;5;6];[7;8;9];[10;11;12];[13;14;15];[16;17;18];[19;20;21];[-1;-2];[-1;-3];[-2;-3];[-4;-5];[-4;-6];[-5;-6];[-7;-8];[-7;-9];[-8;-9];[-10;-11];[-10;-12];[-11;-12];[-13;-14];[-13;-15];[-14;-15];[-16;-17];[-16;-18];[-17;-18];[-19;-20];[-19;-21];[-20;-21];[-1;-4];[-2;-5];[-3;-6];[-1;-7];[-2;-8];[-3;-9];[-4;-7];[-5;-8];[-6;-9];[-4;-10];[-5;-11];[-6;-12];[-7;-10];[-8;-11];[-9;-12];[-7;-13];[-8;-14];[-9;-15];[-7;-16];[-8;-17];[-9;-18];[-10;-13];[-11;-14];[-12;-15];[-13;-16];[-14;-17];[-15;-18]]

(********************************************************************)



(* simplifie : int -> int list list -> int list list
    applique la simplification de l'ensemble des clauses en mettant
    le littéral i à vrai *)
  let rec simplifie i clauses =
    match clauses with
    | [] -> clauses
    | hd :: tl -> if List.mem i hd then simplifie i tl                         (* Retire la clause satisfiée *)
                  else (filter (function x -> x <> -i) hd) ::simplifie i tl    (* Retire -i de la clause *)
;;

(* solveur_split : int list list -> int list -> int list option
   exemple d'utilisation de `simplifie' *)
let rec solveur_split clauses interpretation =
  (* l'ensemble vide de clauses est satisfiable *)
  if clauses = [] then Some interpretation else
  (* un clause vide est insatisfiable *)
  if mem [] clauses then None else
  (* branchement *)
  let l = hd (hd clauses) in
  let branche = solveur_split (simplifie l clauses) (l::interpretation) in
  match branche with
  | None -> solveur_split (simplifie (-l) clauses) ((-l)::interpretation)
  | _    -> branche
;;

(* solveur dpll récursif *)


(* unitaire_aux : int list -> int
    -si 'clause' est une clause unitaire,
     retourne son littéral ;
     sinon, lève une exception 'Not_found' *)
let unitaire_aux clause =
  match clause with
  | []     -> raise Not_found
  | [x]    -> x
  | _ :: _ -> raise Not_found
;;

(* unitaire : int list list -> int
    - si `clauses' contient au moins une clause unitaire, retourne
      le littéral de cette clause unitaire ;
    - sinon, lève une exception `Not_found' *)
let rec unitaire clauses =
  match clauses with
  | []    -> raise Not_found
  | [[x]] -> x
  (* Cherche une clause par une si elle est unitaire *)
  | h :: t -> try unitaire_aux h with Not_found -> unitaire t
;;


(* pur_aux : int list list -> int list -> int
      si 'clauses' ne contient pas la négation d'un littéral de 'reste',
      retourne ce littéral;
    - sinon, lève une exception `Failure "pas de littéral pur"' *)
let rec pur_aux clauses reste =
  match reste with
  | []  -> raise (Failure "Pas de littéral pur")
  | [x] -> if List.mem (-x) clauses then raise (Failure "Pas de littéral pur")
             else x
  (* Cherche is la 'négation de h' est dans l'ensemble des littéraux des 'clauses' *)
  | h :: t -> if List.mem (-h) clauses
              then pur_aux clauses t
              else h
;;

(* pur : int list list -> int
    - si `clauses' contient au moins un littéral pur, retourne
      ce littéral ;
    - sinon, lève une exception `Failure "pas de littéral pur"' *)
let pur clauses =
  (* Ajoute 'e' dans 'liste' que si il n'y est pas déjà *)
  let unique_ajout e liste = if List.mem e liste then liste else e :: liste in
  (* Enlève les doublons de la 'liste' *)
  let retire_doublons liste = List.fold_right unique_ajout liste [] in

  (* Passage de plusieurs 'clauses' séparées à une grande 'clause' sans doublons*)
  let liste_clauses = retire_doublons (List.flatten clauses) in

  (* Cherche si il existe un littéral dans 'clauses' qui ne contient sa négation nul part *)
  pur_aux liste_clauses liste_clauses
;;


(* h : int list list -> int
      Si la liste n'est pas vide, retourne
      le premier élément de la liste;
      sinon lève une erreur avec failwith *)
let h liste =
  let liste_flat = List.flatten liste in
  match liste_flat with
  | [] -> failwith("Empty")
  | h :: t -> h
;;


(* solveur_dpll_rec : int list list -> int list -> int list option *)
let rec solveur_dpll_rec clauses interpretation =
  if clauses = [] then Some interpretation else
  if List.mem [] clauses then None else

(* Essaye en premier 'unit' *)
  try
    let unit_l = unitaire clauses in
    solveur_dpll_rec (simplifie unit_l clauses) (unit_l :: interpretation)
  with
  | Not_found ->

(* Essaye 'pur' si 'unit' échoue *)
    try
      solveur_dpll_rec (simplifie (pur clauses) clauses) (pur clauses :: interpretation)
    with
    | Failure s ->


(* Fait 'split p' ou 'split not p', si 'unit' et 'pur' échouent *)
    let p = h clauses in
    let simpl a = simplifie a clauses in

    (* split p *)
      let split_p = solveur_dpll_rec (simpl p) (p :: interpretation) in
      match split_p with
      (* Si split p échoue, fait split not p *)
      | None -> solveur_dpll_rec (simpl (-p)) (-p :: interpretation)
      | _    -> split_p
;;


(* tests *)

(* UNSAT *)
(* let () = print_modele (solveur_dpll_rec exemple_3_12 []) *)
(* let () = print_modele (solveur_dpll_rec exemple_7_4 []) *)
(* let () = print_modele (solveur_dpll_rec systeme []) *)

(* SAT *)
(* let () = print_modele (solveur_dpll_rec coloriage []) *)
(* let () = print_modele (solveur_dpll_rec exemple_7_2 []) *)
(* let () = print_modele (solveur_dpll_rec exemple_7_8 []) *)
(* let () = print_modele (solveur_dpll_rec dependances []) *)

let () =
  let clauses = Dimacs.parse Sys.argv.(1) in
  print_modele (solveur_dpll_rec clauses [])
