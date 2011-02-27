signature CVSL =
sig
    (* args: frame type and wait boolean
     * return unit *)
    val displayAll : int -> bool -> unit

    (* args: filename, frame type and id
     * returns unit *)
    val saveImage : string -> int -> int -> unit

    (* args: name, extension, type
     * return unit *)
    val saveAllImages : string -> string -> int -> unit

    (* args: histogram id
     * return unit *)
    val showHistogram : int -> unit

    (* args: unit
     * return unit *)
    val showAllHistograms : unit -> unit

    (* args: frame type and id
     * return unit *)
    val displayImage : int -> int -> unit

    (* args: frame type
     * return unit *)
    val displayAllImages : int -> unit

    (* args: frame type
     * return unit *)
    val displayVideo : int -> unit

    val getIds : int -> int vector

	(* RZ: exposing low-level images for ICCV *)
	val getImage: int -> int -> (char vector * int * int * int)
	val showImage: char vector * int * int * int -> unit
end;
