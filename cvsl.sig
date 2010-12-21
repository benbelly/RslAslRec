signature CVSL =
sig
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
