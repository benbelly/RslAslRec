
local
  (* cvsl support api *)
  val numFrames = _import "numFrames" : unit -> int;
  val getFrameIds = _import "getFrameIds" : int array -> unit;
  val getFrameInfoC = _import "getFrameInfoC" : int * int ref * int ref * int ref * int ref -> unit;
  val getVideoInfoC = _import "getVideoInfoC" : int * bool ref * int ref * real ref -> unit;
  val getFrame = _import "getFrame" : int * int * char array -> unit;

  (* cvsl api *)
  val showImageC = _import "showImageC" : char vector * int * int * int * bool -> unit;
  val saveImageC = _import "saveImageC" : char vector * int * int * int * char vector * int -> unit;
  val videoBeginC = _import "videoSaveBeginC" : int * int * int * real * bool * char vector * int -> unit;
  val saveVideoC = _import "saveVideoC" : char vector * int * int * int -> unit;
  val videoEndC = _import "videoSaveEndC" : unit -> unit;
  val showNormalHistogram = _import "showNormalHistogram" : char vector * int * int -> unit;

  (* Helper functions *)
  fun vecMax is : int = Vector.foldl Int.max 0 is;
  fun numWidth i : int = size (Int.toString i);
  fun maxWidth is : int = numWidth (vecMax is);
  fun padNum n p : string = StringCvt.padLeft #"0" p (Int.toString n);
in
  structure Cvsl =
      struct

      (* Get the image data, width, height, data-type *)
      fun getFrameInfo t : int * int * int * int =
        let val width = ref 0
            val height = ref 0
            val dt = ref 0
            val size = ref 0
            val _ = getFrameInfoC( t, width, height, dt, size )
        in (!width, !height, !dt, !size) end;

      fun getVideoInfo t : bool * int * real =
        let val color = ref false
            val fourcc = ref 0
            val fps = ref 0.0
            val _ = getVideoInfoC( t, color, fourcc, fps )
        in ( !color, !fourcc, !fps ) end;

      fun getImage t i : char vector * int * int * int =
        let val (width, height, dt, size) = getFrameInfo t
            val img : char array = Array.array( size, Char.chr( 0 ) )
            val _ = getFrame( i, t, img )
        in (Array.vector( img ), width, height, dt) end;

      fun getAndShow t i wait : unit =
        let val (img, w, h, dt) = getImage t i
        in showImageC( img, w, h, dt, wait ) end;

      fun getIds () : int vector =
        let val num = numFrames()
            val ids : int array = Array.array( num, 0 )
            val _ = getFrameIds( ids )
        in Array.vector( ids ) end;

      fun displayAll t wait =
        let val ids = getIds()
            val show = fn i => getAndShow t i wait
        in Vector.app show ids end;

      fun saveImage name t i : unit =
        let val (img, w, h, dt) = getImage t i
        in saveImageC( img, w, h, dt, name, (size name)) end;

      fun saveAllImages name ext t : unit =
        let val ids = getIds()
            val w = maxWidth ids
            val getName = fn i => name ^ (padNum i w) ^ "." ^ ext
            val save = fn i => saveImage (getName i) t i
        in Vector.app save ids end;

      fun showHistogram i : unit =
        let val (img, w, h, dt) = getImage 5 i (* need to pass 5 *)
        in showNormalHistogram( img, w, h ) end;

      fun showAllHistograms () =
        let val ids = getIds()
            val show = fn i => showHistogram i
        in Vector.app show ids end;

      fun saveVideo name t : unit =
        let val (color, fourcc, fps) = getVideoInfo t
            val (width, height, dt, _) = getFrameInfo t
            val ids = getIds()
            val save = fn i => let val (img, _, _, _) = getImage t i
                               in saveVideoC( img, width, height, dt ) end;
        in
          videoBeginC( width, height, fourcc, fps, color, name, (size name) );
          Vector.app save ids;
          videoEndC()
        end;

      fun displayImage t i = getAndShow t i true;
      fun displayAllImages t = displayAll t true;
      fun displayVideo t = displayAll t false;

      end;
end
