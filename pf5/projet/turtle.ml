open Graphics
open Unix

type command =
  | Line of int
  | Move of int
  | Turn of int
  | Store
  | Restore

type position = {
  x: float;      (** position x *)
  y: float;      (** position y *)
  a: int;        (** angle of the direction *)
}

(* Put here any type and function
   implementations concerning turtle *)
type turtle = {
  location : position;
  save : position list option;
}

(* use it to simulate a graph and find its size *)
type dimension = {
  t : turtle;
  min_x : float;
  min_y : float;
  max_x : float;
  max_y : float;
}

let shocking_pink = rgb 255 105 180;;

let colors = [|black; red; green; blue;
               yellow; cyan; magenta; shocking_pink|];;

let truncatef x = snd (modf x);;

(* sleep : float -> unit
    sleep the program *)
let sleep (sec : float) =
  ignore (Unix.select [] [] [] sec)

(* Degré = Radian * 180 / PI
   Radian = Degré * PI / 180 *)
let pi = 4.0 *. atan 1.0;;

(* degr_of_rad : int -> float
    convert degree to radian *)
let degr_of_rad i = float_of_int(i) *. pi /. 180.

(*absF : float -> float
   Absolute value for a float *)
let absF (f : float) =
  if f > 0. then f else (f *. (-1.0))
;;


(* draw : int -> int -> tutle -> unit
    draw line with animation *)
let rec draw i j t =
  sleep 0.0001;

  if j < i then
    let _ = fill_circle
        (int_of_float
           (t.location.x +. (float_of_int j *. cos(degr_of_rad t.location.a))))
        (int_of_float
           (t.location.y +. (float_of_int j *. sin(degr_of_rad t.location.a))))
        2 in

    draw i (j+1) t
;;

(* line : int -> turtle -> turtle
    draw line and update the turtle with new line *)
let line i t =
  let l = {
    x = (t.location.x +. (float_of_int i *. cos(degr_of_rad t.location.a)));
    y = (t.location.y +. (float_of_int i *. sin(degr_of_rad t.location.a)));
    a = (t.location.a)} in
  let _ = draw i 0 t in
  {location = l; save = t.save}
;;

(* move : int -> turtle -> turtle
    move in window and update turtle *)
let move i t =
  let l = {
    x = (t.location.x +. (float_of_int i *. cos(degr_of_rad t.location.a)));
    y = (t.location.y +. (float_of_int i *. sin(degr_of_rad t.location.a)));
    a = (t.location.a)} in
  moveto (int_of_float l.x)  (int_of_float l.y);
  {location = l; save = t.save}
;;

(* store : turtle -> turtle
    store in turtle a position *)
let store t =
  match t.save with
  | Some a -> {location = t.location; save = Some (t.location :: a)}
  | None -> {location = t.location; save = Some [t.location]}
;;

(* restore : turtle -> turtle
    restore a position that we saved in turtle *)
let restore t =
  match t.save with
  | None -> t
  | Some l ->
    match l with
    | [] -> {location = t.location; save = None}
    | [a] ->
      moveto (int_of_float a.x) (int_of_float a.y);
      {location = a; save = None}
    | a :: b ->
      moveto (int_of_float a.x) (int_of_float a.y);
      {location = a; save = Some b}
;;

(* action : command -> turtle -> turtle
    take action according to the command *)
let action (cmd : command) (t : turtle) : turtle =
  match cmd with
  | Line i -> line i t
  | Move i -> move i t
  | Turn i -> {location = {x = t.location.x; y = t.location.y;
                           a = ((t.location.a + i) mod 360)}; save = t.save}
  | Store -> store t
  | Restore -> restore t
;;

(* action_list : command list -> turtle -> unit
   read a list of command and take actions accordingly *)
let rec action_list (cmd_list : command list) (t : turtle) =
  match cmd_list with
  | [] -> ()
  | [x] -> let result = action x t in
    action_list [] result
  | x :: y -> let result = action x t in
    action_list y result
;;

(* restore_dimension : dimension -> dimension
   part of a function used to simulate a command list to calculate max size *)
let restore_dimension (d : dimension) : dimension =
  match d.t.save with
  | None -> d
  | Some l ->
    match l with
    | [] -> {t = {location = d.t.location; save = None};
             min_x = d.min_x; min_y = d.min_y;
             max_x = d.max_x; max_y = d.max_y}
    | [a] ->
      {t = {location = a; save = None};
       min_x = d.min_x; min_y = d.min_y; max_x = d.max_x; max_y = d.max_y}
    | a :: b ->
      {t ={location = a; save = Some b};
       min_x = d.min_x; min_y = d.min_y; max_x = d.max_x; max_y = d.max_y}
;;

(* store_dimension : dimension -> dimension
   part of a function used to simulate a command list to calculate max size *)
let store_dimension (d : dimension) : dimension =
  match d.t.save with
  | Some a -> {
      t = {location = d.t.location; save = Some (d.t.location :: a)};
      min_x = d.min_x; min_y = d.min_y; max_x = d.max_x; max_y = d.max_y}
  | None -> {
      t = {location = d.t.location; save = Some [d.t.location]};
      min_x = d.min_x; min_y = d.min_y; max_x = d.max_x; max_y = d.max_y}
;;

(* check_dimension_bis : command -> dimension -> dimension
    Simulates the drawing of a graph from its command list
    Returns the max dimension taken *)
let check_dimension_bis (cmd : command) (d : dimension) : dimension =
  match cmd with
  | Turn i -> 
    {t = {location = {x = d.t.location.x; y = d.t.location.y;
                      a = ((d.t.location.a + i) mod 360)}; save = d.t.save};
     min_x = d.min_x; min_y = d.min_y; max_x = d.max_x; max_y = d.max_y}
  | Store -> store_dimension d
  | Restore -> restore_dimension d
  | Line i ->
    let l = {
      x = (d.t.location.x +. (float_of_int i
                              *. cos(degr_of_rad d.t.location.a)));
      y = (d.t.location.y +. (float_of_int i
                              *. sin(degr_of_rad d.t.location.a)));
      a = (d.t.location.a)} in
    { t = {location = l; save = d.t.save};
      min_x = (min l.x d.min_x); min_y = (min l.y d.min_y);
      max_x = (max l.x d.max_x); max_y = (max l.y d.max_y) }
  | Move i ->
    let l = {
      x = (d.t.location.x +. (float_of_int i
                              *. cos(degr_of_rad d.t.location.a)));
      y = (d.t.location.y +. (float_of_int i
                              *. sin(degr_of_rad d.t.location.a)));
      a = (d.t.location.a)} in
    { t = {location = l; save = d.t.save};
      min_x = (min l.x d.min_x); min_y = (min l.y d.min_y);
      max_x = (max l.x d.max_x); max_y = (max l.y d.max_y) }
;;

(* check_dimension : command list -> dimension -> dimension
    Checks how much place a graph will take
    from its command list.
    Returns its max dimensions *)
let rec check_dimension (cmd_list : command list) (d : dimension) =
  match cmd_list with
  | [] -> d
  | [x] -> let result = check_dimension_bis x d in
    check_dimension [] result
  | x :: y -> let result = check_dimension_bis x d in
    check_dimension y result
;;

(* max_size : command list -> position
    Checks the maximum size taken by a list of commands
    and returns a starting position *)
let max_size (cmd_list : command list) =
  let s = check_dimension cmd_list   (* Max size dimensions *)
      {t = {location
            = {x = 0.; y = 0.; a = 0}; save = None};
       min_x = 1200.; min_y = 800.; max_x = 0.; max_y = 0.} in

  (* Get new position *)
  let aux i j =
    if ((i < 0.) && (j < 0.)) then
      ((absF j) -. (absF i))
    else if i < 0. then
      (absF i) +. j
    else
      j
  in
  {x = aux s.min_x s.max_x; y = aux s.min_y s.max_y; a = 0}
;;

(* start_point : command list -> turtle
    return the starting position in a window
    from a list of commands *)
let start_point (cmd_list : command list) =
  let s = check_dimension cmd_list      (* Max size dimensions *)
      {t = {location = {x = 0.; y = 0.; a = 0}; save = None};
       min_x = 1200.; min_y = 800.; max_x = 0.; max_y = 0.} in

  (* Staring position in window *)
  let calculatePos i j size =
    (((size -. ((absF i) +. j)) /. 2.) +. absF i)
  in

  (* Return new position in the window *)
  {location = {
      x = truncatef (calculatePos s.min_x s.max_x 1200.);
      y = truncatef (calculatePos s.min_y s.max_y 800.);
      a = 0};
   save = None}
;;

(* change_size_by_percentage : command list -> int -> command list
   Resize the values of a command list, increases by 'p' percents *)
let rec change_size_by_percentage (cmd_list : command list) p : command list =
  let aux cmd p =
    match cmd with
    | Line i -> if i = 1 then Line 1 else  Line ((i * p) / 100)
    | Move i -> if i = 1 then Move 1 else  Move ((i * p) / 100)
    | Turn i -> Turn i
    | Store -> Store
    | Restore -> Restore
  in
  match cmd_list with
  | [] -> cmd_list
  | [x] -> [aux x p]
  | x :: y -> aux x p :: change_size_by_percentage y p
;;

(* re_dimension : command list -> command list
   return a new list of command with appropriate size *)
let re_dimension (cmd_list : command list) : command list =
  let m = max_size cmd_list in
  let mx_percent = (m.x *. 100.) /. float_of_int (size_x()) in
  let my_percent = (m.y *. 100.) /. float_of_int (size_y()) in
  let max_percent = (* Biggest dimension (width or height) *)
    if mx_percent > my_percent then
      mx_percent
    else my_percent
  in
  (* 75% of max window size *)
  let new_size_percent = int_of_float (75. /. max_percent *. 100.) in

  change_size_by_percentage cmd_list new_size_percent
;;

(* test_loop_quit : 'a -> unit
   Loop after the graph is displayed
   We wait for the user to press Q to quit *)
let rec test_loop_quit t =
  let ev = wait_next_event [Key_pressed] in
  if ev.keypressed
  then match ev.key with
    | 'q' -> close_graph();
    | _ -> test_loop_quit t
;;

(* test_loop_menu : turtle -> command list -> color -> unit
    Loops the menu, change color and then start drawing *)
let rec test_loop_menu t (cmd_list : command list ) color =
  moveto ((size_x()/2)-100) (size_y()/2);
  set_text_size 500;
  draw_string "Press c to change color and then Press s to Start.";
  let ev = wait_next_event [Key_pressed] in

  if ev.keypressed
  then match ev.key with
    | 's'  -> clear_graph();  (* change bottom line to t *)
      action_list cmd_list t;
      test_loop_quit t;
    | 'q'  -> close_graph();
    | 'c'  -> set_color colors.((color + 1) mod 8);
      test_loop_menu t cmd_list ((color +1) mod 8)
    |  _   -> test_loop_menu t cmd_list color
;;

(* call_turtle : command list -> unit
    Create a test window and draws *)
let call_turtle (cmd_list : command list ) =
  open_graph " 1200x800";
  set_color shocking_pink;
  let new_cmd_list = re_dimension cmd_list in
  let t = start_point new_cmd_list in
  test_loop_menu t new_cmd_list 0
;;

exception Not_exists

(* at :  'a list -> int -> 'a
    Gets the element at position i from a list *)
let rec at list i =
  match list with
  | [] -> raise Not_exists
  | x :: y -> if i == 0 then x else at y (i-1)
;;


(* test_gui : unit -> unit
    FUNCTION TESTING GUI
    We use predefined commands to draw something *)
let test_gui() =
  let t = {location = {x = 300.; y = 200.; a = 0}; save = None} in
  let cmd_list = [Store; Line 100; Line 100; Store; Move 200; Store;
                  Turn 90; Line 1000; Restore; Turn (-30); Line 100;
                  Restore; Line 100; Restore; Turn 180; Line 100]
  in
  let re_size_test = max_size cmd_list in

  print_float re_size_test.x;
  print_string "\n";
  print_float re_size_test.y;
  print_string "\n";
  call_turtle cmd_list;
;;
