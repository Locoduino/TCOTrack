//
//  TTSignal.hpp
//  CocoaSimulator
//
//  Created by Jean-Luc Béchennec on 28/08/2017.
//  Copyright © 2017 Pierre Molinaro. All rights reserved.
//

#ifndef TTSignal_h
#define TTSignal_h

#include "TTTrack.h"

//-----------------------------------------------------------------------------
class TTSignal : public TTTile
{
  public : TTSignal (const AWPoint &inLocation, const bool inDirection) ;
  
  private : bool mDirection ;
  public : bool direction () const { return mDirection ; }

  //--- Tell the view is opaque or not
  public : virtual bool isOpaque (void) const ;
};

//-----------------------------------------------------------------------------
typedef enum { kTTSemaphore, kTTRalentissement, kTTVoieLibre } TTSemaphoreState;

//-----------------------------------------------------------------------------
class TTSemaphoreF : public TTSignal
{
  public : TTSemaphoreF (const AWPoint &inLocation, const bool inDirection) ;
  
  private : TTSemaphoreState mState ;
  public : TTSemaphoreState state () const { return mState ; }
  
  public : void setState (const TTSemaphoreState inState) ;
  
  public : virtual void drawInRegion (const AWRegion & inDrawRegion) const ;
};

#endif /* TTSignal_h */
