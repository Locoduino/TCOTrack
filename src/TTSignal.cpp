//
//  TTSignal.cpp
//  CocoaSimulator
//
//  Created by Jean-Luc Béchennec on 28/08/2017.
//  Copyright © 2017 Pierre Molinaro. All rights reserved.
//

#include "TTSignal.h"
#include "TTConfig.h"
#include "AWContext.h"
#include "AWLine.h"

//=============================================================================
TTSignal::TTSignal (const AWPoint &inLocation, const bool inDirection) :
TTTile (inLocation),
mDirection (inDirection)
{
}

//-----------------------------------------------------------------------------
bool TTSignal::isOpaque() const
{
  return false ;
}

//=============================================================================
TTSemaphoreF::TTSemaphoreF (const AWPoint &inLocation, const bool inDirection) :
TTSignal (inLocation, inDirection),
mState (kTTSemaphore)
{
}

//-----------------------------------------------------------------------------
void TTSemaphoreF::setState (const TTSemaphoreState inState)
{
  mState = inState ;
  setNeedsDisplay () ;
}

//-----------------------------------------------------------------------------
void TTSemaphoreF::drawInRegion (const AWRegion & inDrawRegion) const
{
  AWRect r = absoluteFrame () ;
    r.size.width = TILE_PIXEL_GRID - 2 ;
    r.size.height = (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 - 1;
  if (! direction ()) {
    r.origin.y += (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + STRAIGHT_TRACK_WIDTH + 1;
    r.origin.x += 2 ;
  }
  AWContext::setColor (AWColor::black()) ;
  r.fillRoundRectInRegion ( TILE_PIXEL_GRID / 8, inDrawRegion) ;
  if (direction()) {
    const AWPoint p1 (r.origin.x + 2 * r.size.width / 3, r.origin.y + r.size.height) ;
    const AWPoint p2 (p1.x, p1.y + STRAIGHT_TRACK_WIDTH / 2) ;
    p1.strokeLineInRegion (p2, inDrawRegion) ;
    AWRect light = r ;
    light.inset (TILE_PIXEL_GRID / 10, TILE_PIXEL_GRID / 10) ;
    light.size.width = light.size.height ;
    AWContext::setColor (mState == kTTVoieLibre ? AWColor::green() : AWColor::gray()) ;
    light.fillOvalInRegion(inDrawRegion) ;
    light.origin.x += light.size.width + 2 ;
    AWContext::setColor (mState == kTTSemaphore ? AWColor::red() : AWColor::gray()) ;
    light.fillOvalInRegion(inDrawRegion) ;
    light.origin.x += light.size.width + 2;
    AWContext::setColor (mState == kTTRalentissement ? AWColor::yellow() : AWColor::gray()) ;
    light.fillOvalInRegion(inDrawRegion) ;
  }
  else {
    const AWPoint p1 (r.origin.x + r.size.width / 3, r.origin.y) ;
    const AWPoint p2 (p1.x, p1.y - STRAIGHT_TRACK_WIDTH / 2 - 1) ;
    p1.strokeLineInRegion (p2, inDrawRegion) ;
    AWRect light = r ;
    light.inset (TILE_PIXEL_GRID / 10, TILE_PIXEL_GRID / 10) ;
    light.size.width = light.size.height ;
    AWContext::setColor (mState == kTTRalentissement ? AWColor::green() : AWColor::gray()) ;
    light.fillOvalInRegion(inDrawRegion) ;
    light.origin.x += light.size.width + 2 ;
    AWContext::setColor (mState == kTTSemaphore ? AWColor::red() : AWColor::gray()) ;
    light.fillOvalInRegion(inDrawRegion) ;
    light.origin.x += light.size.width + 2;
    AWContext::setColor (mState == kTTVoieLibre ? AWColor::yellow() : AWColor::gray()) ;
    light.fillOvalInRegion(inDrawRegion) ;
  }
}
