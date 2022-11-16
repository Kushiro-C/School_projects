open Graphics
open Unix
open Ast

exception OutOfBounds

type position = {
  x:      float;  (* position x *)
  y:      float;  (* position y *)
  a:      int;    (* angle of the direction *)
  draw:   bool;   (* HautPinceau=true BasPinceau=false *)
  width:  int;    (* ChangeEpaisseur *)
}

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

(* draw : int -> int -> position -> unit
    draw line with animation *)
let rec draw i j pos =
  sleep 0.0001;

  if j < i then
    let _ = fill_circle
      (int_of_float (pos.x +. (float_of_int j *. cos(degr_of_rad pos.a))))
      (int_of_float (pos.y +. (float_of_int j *. sin(degr_of_rad pos.a))))
      (pos.width)
    in

    draw i (j+1) pos
;;

(* line : int -> position -> position
    draw line and update the position with new line *)
let line i pos =
  let newPos = {
    x = (pos.x +. (float_of_int i *. cos(degr_of_rad pos.a)));
    y = (pos.y +. (float_of_int i *. sin(degr_of_rad pos.a)));
    a = pos.a;
    draw = pos.draw;
    width = pos.width
  } in
  if newPos.x < 0. || newPos.x > 1200. || newPos.y < 0. || newPos.y > 800.
  then raise OutOfBounds
  else
    let _ = draw i 0 pos in
    newPos
;;

(* move : int -> position -> position
    move in window and position *)
let move i pos =
  let l = {
    x = (pos.x +. (float_of_int i *. cos(degr_of_rad pos.a)));
    y = (pos.y +. (float_of_int i *. sin(degr_of_rad pos.a)));
    a = pos.a;
    draw = pos.draw;
    width = pos.width
  } in
  moveto (int_of_float l.x) (int_of_float l.y);
  l
;;

exception UnexpectedVar
exception UnexpectedColor

(* getExpressionNumber : expression -> int
    Returns the Number value of an expression
    Usage in execAction
    Raise UnexpectedVar exception if the expr is not a Number *)
let getExpressionNumber = function
  | Nombre n -> n
  | _ -> raise UnexpectedVar

(* setColor : string -> unit
    Sets the Graoh Drawing color of the given string
    Usage in execAction
    Raise UnexpectedColor if the color is not
    a predefined color of Graphics *)
let setColor color =
  if compare color "noir" == 0
  then set_color black
  else if compare color "blanc" == 0
  then set_color white
  else if compare color "rouge" == 0
  then set_color red
  else if compare color "vert" == 0
  then set_color green
  else if compare color "bleu" == 0
  then set_color blue
  else if compare color "jaune" == 0
  then set_color yellow
  else if compare color "cyan" == 0
  then set_color cyan
  else if compare color "magenta" == 0
  then set_color magenta
  else raise UnexpectedColor

(* takeAction : position -> instruction -> position
    Executes an instruction and return new position data
    Usage in doInstructions
    Raise UnexpectedVar if the instruction is Ident *)
let execAction pos = function
  | Avance e -> if pos.draw == false
              then move (getExpressionNumber e) pos
              else line (getExpressionNumber e) pos
  | Tourne e ->
              {
                x = pos.x; y = pos.y;
                a = ((pos.a + (getExpressionNumber e)) mod 360);
                draw = pos.draw;
                width = pos.width
              }
  | BasPinceau ->
              {
                x = pos.x; y = pos.y; a = pos.a;
                draw = true;
                width = pos.width
              }
  | HautPinceau ->
              {
                x = pos.x; y = pos.y; a = pos.a;
                draw = false;
                width = pos.width
              }
  | ChangeEpaisseur e ->
              {
                x = pos.x; y = pos.y; a = pos.a;
                draw = pos.draw;
                width = (getExpressionNumber e)
              }
  | ChangeCouleur s -> setColor s; pos
  | _ -> raise UnexpectedVar

(* doInstructions : position -> instruction list -> unit
    Takes instructions and a starting position
    then execute all actions in a graph *)
let rec doInstructions pos instr_list =
  match instr_list with
  | [] -> ()
  | h::t -> let newPos = execAction pos h in
          doInstructions newPos t
;;

(* waitQuit : unit -> unit
   Loop after the graph is displayed
   We wait for the user to press Q to quit *)
let rec waitQuit () =
  let ev = wait_next_event [Key_pressed] in
  if ev.keypressed
  then match ev.key with
    | 'q' -> close_graph()
    | _ -> waitQuit ()
;;

(* create_graph : instruction list -> unit
    Creates a window and executes the instructions *)
let create_graph instr_list =
  open_graph " 1200x800";
  set_color black;
  let pos = {
    (* Le sujet demande (0,0,90) mais on met (5,5,90)
      pour des raisons de meilleure visibilité *)
    x = 5.; y = 5.; a = 90;
    draw = false;
    width = 2
  } in

  doInstructions pos instr_list;
  waitQuit ()
;;