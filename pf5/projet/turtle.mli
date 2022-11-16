
(** Turtle graphical commands *)
type command =
  | Line of int      (** advance turtle while drawing *)
  | Move of int      (** advance without drawing *)
  | Turn of int      (** turn turtle by n degrees *)
  | Store            (** save the current position of the turtle *)
  | Restore          (** restore the last saved position not yet restored *)

(** Position and angle of the turtle *)
type position = {
  x: float;        (** position x *)
  y: float;        (** position y *)
  a: int;          (** angle of the direction *)
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

val re_dimension : command list -> command list
val start_point : command list -> turtle
val action_list : command list -> turtle -> unit
val re_dimension : command list -> command list
val at : 'a list -> int -> 'a
val colors : int array
val call_turtle : command list -> unit
val test_gui : unit -> unit
