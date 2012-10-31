/**
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
**/


#ifndef UTILITY_H__
#define UTILITY_H__


template<typename ForwardIteratorA, typename ForwardIteratorB,
         typename Inserter, typename BinaryOp>
void Zip( ForwardIteratorA abegin, ForwardIteratorA aend,
          ForwardIteratorB bbegin, ForwardIteratorB bend,
          Inserter ins, BinaryOp op ) {
    for( ; ( abegin != aend ) && ( bbegin != bend ); ++abegin, ++bbegin ) {
        *ins = op( *abegin, *bbegin );
        ++ins;
    }
}

//Copy_if was dropped from the standard library by accident.
template<typename In, typename Out, typename Pred>
Out Copy_if(In first, In last, Out res, Pred Pr)
{
  while (first != last)
  {
    if (Pr(*first))
      *res++ = *first;
    ++first;
  }
  return res;
}

template<typename ForwardIteratorA, typename ForwardIteratorB,
         typename BinaryOp>
void TwoFor( ForwardIteratorA abegin, ForwardIteratorA aend,
             ForwardIteratorB bbegin, ForwardIteratorB bend,
             BinaryOp op ) {
    // Is there a do nothing inserter that would allow zip to work?
    for( ; ( abegin != aend ) && ( bbegin != bend ); ++abegin, ++bbegin ) {
        op( *abegin, *bbegin );
    }
}

#endif
