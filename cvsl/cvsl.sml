(**
    Copyright 2011 Ben Holm

    This file is part of RslAslRec.

    RslAslRec is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RslAslRec is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RslAslRec.  If not, see <http://www.gnu.org/licenses/>.
**)


local
  (* cvsl support api *)
  val numFramesC    = _import "numFramesC" : int -> int;
  val getFrameIdsC  = _import "getFrameIdsC" : int * int array -> unit;
  val getFrameInfoC = _import "getFrameInfoC" : int * int ref * int ref * int ref * int ref -> unit;
  val getFrameC     = _import "getFrameC" : int * int * char array -> unit;

  (* cvsl api *)
  val showImageC      = _import "showImageC" : char vector * int * int * int * bool -> unit;
  val saveImageC      = _import "saveImageC" : char vector * int * int * int * char vector * int -> unit;
  val showNormalHistC = _import "showNormalHistogramC" : char vector * int * int -> unit;

  (* Helper functions *)
  fun vecMax is : int = Vector.foldl Int.max 0 is;
  fun numWidth i : int = size (Int.toString i);
  fun maxWidth is : int = numWidth (vecMax is);
  fun padNum n p : string = StringCvt.padLeft #"0" p (Int.toString n);
  structure OpenCV_cvsl =
      struct

      (* Get the image data, width, height, data-type *)
      fun getFrameInfo t : int * int * int * int =
        let val width = ref 0
            val height = ref 0
            val dt = ref 0
            val size = ref 0
            val _ = getFrameInfoC( t, width, height, dt, size )
        in (!width, !height, !dt, !size) end;

      fun getImage t i : char vector * int * int * int =
        let val (width, height, dt, size) = getFrameInfo t
            val img : char array = Array.array( size, Char.chr( 0 ) )
            val _ = getFrameC( i, t, img )
        in (Array.vector( img ), width, height, dt) end;

      fun getAndShow t i wait : unit =
        let val (img, w, h, dt) = getImage t i
        in showImageC( img, w, h, dt, wait ) end;

      fun getIds t : int vector =
        let val num = numFramesC(t)
            val ids : int array = Array.array( num, 0 )
            val _ = getFrameIdsC( t, ids )
        in Array.vector( ids ) end;

      fun displayAll t wait =
        let val ids = getIds t
            val show = fn i => getAndShow t i wait
        in Vector.app show ids end;

      fun saveImage name t i : unit =
        let val (img, w, h, dt) = getImage t i
        in saveImageC( img, w, h, dt, name, (size name)) end;

      fun saveAllImages name ext t : unit =
        let val ids = getIds t
            val w = maxWidth ids
            val getName = fn i => name ^ (padNum i w) ^ "." ^ ext
            val save = fn i => saveImage (getName i) t i
        in Vector.app save ids end;

      fun showHistogram i : unit =
        let val (img, w, h, dt) = getImage 5 i (* need to pass 5 *)
        in showNormalHistC( img, w, h ) end;

      fun showAllHistograms () =
        let val ids = getIds 5
        in Vector.app showHistogram ids end;

      fun displayImage t i = getAndShow t i true;
      fun displayAllImages t = displayAll t true;
      fun displayVideo t = displayAll t false;

      (* RZ: ICCV addition *)
      fun showImage (img, w, h, dt ) =
        showImageC( img, w, h, dt, true)

      end;
in
  structure Cvsl :> CVSL = OpenCV_cvsl;
end
