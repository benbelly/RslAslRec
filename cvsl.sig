signature CVSL =
sig
    (* take type
     * return width, height, data-type, size (in bytes) *)
    val getFrameInfo : int -> int * int * int * int
    (* take type
     * return color, fourcc, fps *)
    val getVideoInfo : int -> bool * int * real
    (* take type and id
     * return image vector, width, height, data-type *)
    val getImage : int -> int -> char vector * int * int * int
    (* take type and id,
     * return unit *)
    val getAndShow : int -> int -> bool -> unit
    (* take type
     * return ids *)
    val getIds : int -> int vector
    (* take type and wait boolean
     * return unit *)
    val displayAll : int -> bool -> unit
    (* take filename, type, and id
     * returns unit *)
    val saveImage : string -> int -> int -> unit
    (* take name, extension, type
     * return unit *)
    val saveAllImages : string -> string -> int -> unit
    (* take id
     * return unit *)
    val showHistogram : int -> unit
    (* take unit
     * return unit *)
    val showAllHistograms : unit -> unit
    (* take name and type
     * return unit *)
    val saveVideo : string -> int -> unit
    (* take type and id
     * return unit *)
    val displayImage : int -> int -> unit
    (* take type
     * return unit *)
    val displayAllImages : int -> unit
    (* take type
     * return unit *)
    val displayVideo : int -> unit
end;
