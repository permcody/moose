/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef SETS_INTERSECT_H
#define SETS_INTERSECT_H

// Helper function that simply detects when two sorted ranges
// intersect without actually building the intersection.  Originally
// from FeatureFloodCount.h. The iterators don't need to come from the
// same type of container, they just need to be forward iterators.
template <class InputIterator1, class InputIterator2>
inline bool
sets_intersect(InputIterator1 first1,
               InputIterator1 last1,
               InputIterator2 first2,
               InputIterator2 last2)
{
  while (first1 != last1 && first2 != last2)
  {
    if (*first1 == *first2)
      return true;

    if (*first1 < *first2)
      ++first1;
    else if (*first2 < *first1)
      ++first2;
  }
  return false;
}

#endif
