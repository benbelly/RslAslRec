
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
