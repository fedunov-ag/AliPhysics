/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/
#include "AliEmcalContainer.h"
#include "AliEmcalIterableContainer.h"

/// \cond CLASSIMP
ClassImp(AliEmcalIterableContainer)
/// \endcond

/**
 * Default (I/O) constructor
 */
AliEmcalIterableContainer::AliEmcalIterableContainer():
TObject(),
fkContainer(NULL),
fAcceptIndices(),
fUseAccepted(kFALSE)
{

}

/**
 * Standard constructor, to be used by the users. Specifying the type of iteration (all vs. accepted).
 * In case the iterator runs over accepted object, an index map is build inside the constructor.
 * @param[in] cont EMCAL container to iterate over
 * @param[in] useAccept If true accepted objects are used in the iteration, otherwise all objects
 */
AliEmcalIterableContainer::AliEmcalIterableContainer(const AliEmcalContainer *cont, bool useAccept):
TObject(),
fkContainer(cont),
fAcceptIndices(),
fUseAccepted(useAccept)
{
  if(fUseAccepted) BuildAcceptIndices();
}

/**
 * Copy constructor. Initializing all parameters from the reference. As the
 * container is not owner over its input container only pointers are copied.
 * @param[in] ref Reference for the copy
 */
AliEmcalIterableContainer::AliEmcalIterableContainer(const AliEmcalIterableContainer &ref):
TObject(ref),
fkContainer(ref.fkContainer),
fAcceptIndices(ref.fAcceptIndices),
fUseAccepted(ref.fUseAccepted)
{

}

/**
 * Assignment operator. As the container is not owner over the input container only
 * pointers are copied
 * @param[in] ref Reference for assignment
 * @return Object after assignment
 */
AliEmcalIterableContainer &AliEmcalIterableContainer::operator=(const AliEmcalIterableContainer &ref){
  TObject::operator=(ref);
  if(this != &ref){
    fkContainer = ref.fkContainer;
    fAcceptIndices = ref.fAcceptIndices;
    fUseAccepted = ref.fUseAccepted;
  }
  return *this;
}

/**
 * Return the number of objects to iterate over (depending on whether the
 * iterator loops over all or only accepted objects)
 * @return Number of iterable objects in container
 */
int AliEmcalIterableContainer::GetEntries() const {
  return fUseAccepted ? fAcceptIndices.GetSize() : fkContainer->GetNEntries();
}

/**
 * Array index operator. Returns object of the container at the
 * position provided by the parameter index. The operator
 * distinguishes between all and accepted objects:
 * - If accepted was specified in the constructor the index
 *   refers to the nth accepted object, neglecting rejected
 *   objects in between. For this it relies on its internal
 *   index map.
 * - If accepted was not specified in the constructor the
 *   index refers to the nth object inside the container,
 *   based on all objects including rejected objects. The
 *   index map is not needed in this case.
 * @param[in] index Index of the object inside the container to access
 * @return The object at the given index (NULL if the index is out of range)
 */
TObject *AliEmcalIterableContainer::operator[](int index) const {
  if(index < 0 || index >= GetEntries()) return NULL;
  const AliEmcalContainer &contref = *fkContainer;
  return fUseAccepted ? contref[fAcceptIndices[index]] : contref[index];
}

/**
 * Build list of accepted indices inside the container.
 * For this all objects inside the container are checked
 * for being accepted or not.
 */
void AliEmcalIterableContainer::BuildAcceptIndices(){
  fAcceptIndices.Set(fkContainer->GetNAcceptEntries());
  int acceptCounter = 0;
  for(int index = 0; index < fkContainer->GetNEntries(); index++){
    UInt_t rejectionReason = 0;
    if(fkContainer->AcceptObject(index, rejectionReason)) fAcceptIndices[acceptCounter++] = index;
  }
}

///////////////////////////////////////////////////////////////////////
/// Content of class AliEmcalIterableContainer::Iterator            ///
///////////////////////////////////////////////////////////////////////

/**
 * Constructor of the iterator. Setting underlying data, starting
 * position of the iterator, and direction.
 *
 * Iterators should be constructed by the iterable container via
 * the functions begin, end, rbegin and rend. Direct use of the
 * constructor by the users is discouraged.
 * @param[in] cont EMCAL container to iterate over
 * @param[in] currentpos starting position for the iteration
 * @param[in] forward Direction of the iteration. If true, the iteration is
 * performed in forward direction, otherwise in backward direction.
 */
AliEmcalIterableContainer::iterator::iterator(const AliEmcalIterableContainer *cont, int currentpos, bool forward):
fkData(cont),
fCurrent(currentpos),
fForward(forward)
{

}

/**
 * Copy constructor. Only pointers are copied as the
 * iterator does not own its container.
 * @param[in] ref Reference for the copy
 */
AliEmcalIterableContainer::iterator::iterator(const AliEmcalIterableContainer::iterator &ref):
     fkData(ref.fkData),
     fCurrent(ref.fCurrent),
     fForward(ref.fForward)
{

}

/**
 * Assignment operator. Only pointers are copied as the
 * iterator does not own its container.
 * @param[in] ref Reference for the assignment
 * @return This iterator after assignment
 */
AliEmcalIterableContainer::iterator &AliEmcalIterableContainer::iterator::operator=(const AliEmcalIterableContainer::iterator &ref){
  if(this != &ref){
    fkData = ref.fkData;
    fCurrent = ref.fCurrent;
    fForward = ref.fForward;
  }
  return *this;
}

/**
 * Comparison operator for unequalness. Comparison is performed based on the position inside the container
 * @param[in] ref Reference for comparison
 * @return True if the position does not match, false otherwise
 */
bool AliEmcalIterableContainer::iterator::operator!=(const AliEmcalIterableContainer::iterator &ref) const{
  return fCurrent != ref.fCurrent;
}

/**
 * Prefix increment operator. Incrementing / decrementing position of the
 * iterator based on the direction.
 * @return Status of the operator before incrementation.
 */
AliEmcalIterableContainer::iterator &AliEmcalIterableContainer::iterator::operator++(){
  if(fForward) fCurrent++;
  else fCurrent--;
  return *this;
}

/**
 * Prefix decrement operator. Decrementing / incrementing the position of the
 * iterator based on the direction.
 * @return Status of the iterator before decrementation.
 */
AliEmcalIterableContainer::iterator &AliEmcalIterableContainer::iterator::operator--(){
  if(fForward) fCurrent--;
  else fCurrent++;
  return *this;
}

/**
 * Postfix increment operator. Incrementing / decrementing the position
 * of the  iterator based on the direction. This operator requires a
 * copy of itself.
 * @param[in] index Not used
 * @return State of the iterator before incrementation
 */
AliEmcalIterableContainer::iterator AliEmcalIterableContainer::iterator::operator++(int index){
  AliEmcalIterableContainer::iterator tmp(*this);
  operator++();
  return tmp;
}

/**
 * Postfix decrement operator. Decrementing / incrementing the position
 * of the iterator based on the direction. This operator requires a copy
 * of itself.
 * @param[in] index Not used
 * @return State of the iterator before decrementation.
 */
AliEmcalIterableContainer::iterator AliEmcalIterableContainer::iterator::operator--(int index){
  AliEmcalIterableContainer::iterator tmp(*this);
  operator--();
  return tmp;
}

/**
 * Access operator. Providing access to the
 * object at the position of the iterator.
 * @return Object at the position of the iterator (NULL if the iterator is out of range)
 */
TObject *AliEmcalIterableContainer::iterator::operator*() const {
  return (*fkData)[fCurrent];
}