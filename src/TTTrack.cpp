//
//  TTTrack.cpp
//  CocoaSimulator
//
//  Created by Jean-Luc Béchennec on 20/07/2017.
//  Copyright © 2017 Pierre Molinaro. All rights reserved.
//

#include "TTTrack.h"
#include "AWContext.h"
#include "AW-settings.h"
#include "TTConfig.h"

//#define DEBUG_TRACK
//#define TRACK_GRID
#define TRACK_TAG

#include <stdio.h>

//=============================================================================
AWRect computeTileRelativeFrame(const AWPoint & inOrigin,
                                const AWInt inWidth,
                                const AWInt inHeight)
{
  AWRect r (AWPoint (inOrigin.x * TILE_PIXEL_GRID, inOrigin.y * TILE_PIXEL_GRID),
            AWSize (inWidth * TILE_PIXEL_GRID, inHeight * TILE_PIXEL_GRID)) ;
  return r ;
}

//—————————————————————————————————————————————————————————————————————————————
static AWRect computeTileRelativeFrameWithMargin (const AWPoint & inOrigin,
                                                  const AWInt inWidth,
                                                  const AWInt inHeight)
{
  AWRect r (AWPoint (inOrigin.x * TILE_PIXEL_GRID - DIAGONAL_TRACK_WIDTH / 2, inOrigin.y * TILE_PIXEL_GRID - DIAGONAL_TRACK_WIDTH / 2),
            AWSize (inWidth * TILE_PIXEL_GRID + 2 * (DIAGONAL_TRACK_WIDTH / 2), inHeight * TILE_PIXEL_GRID + 2 * (DIAGONAL_TRACK_WIDTH / 2))) ;
  return r ;
}

//=============================================================================
TTView::TTView (const AWPoint & inOrigin,
                const AWInt inSizeX,
                const AWInt inSizeY,
                const String & inTitle) :
AWView (AWRect(inOrigin, AWSize(inSizeX * TILE_PIXEL_GRID, inSizeY * TILE_PIXEL_GRID)), TTBackColor),
mTitle (inTitle)
{
}

//—————————————————————————————————————————————————————————————————————————————
bool TTView::isOpaque() const
{
  return true ;
}

//—————————————————————————————————————————————————————————————————————————————
void TTView::drawInRegion ( const AWRegion & inDrawRegion ) const
{
  AWRect r = absoluteFrame() ;
  AWContext::setColor(backColor()) ;
  r.fillRectInRegion(inDrawRegion) ;
#ifdef TRACK_GRID
  AWContext::setColor(trackGridColor) ;
  AWRect h = AWRect::horizontalLine(r.origin.x, r.origin.y, r.size.width);
  for (int coord = 0; coord < r.size.height; coord += TILE_PIXEL_GRID) {
    h.fillRectInRegion(inDrawRegion) ;
    h.translateBy(0, TILE_PIXEL_GRID) ;
  }
  AWRect v = AWRect::verticalLine(r.origin.x, r.origin.y, r.size.height);
  for (int coord = 0; coord < r.size.width; coord += TILE_PIXEL_GRID) {
    v.fillRectInRegion(inDrawRegion) ;
    v.translateBy(TILE_PIXEL_GRID, 0) ;
  }
#endif
  AWContext::setColor(awkTextColor) ;
  awkDefaultFont.drawStringInRegion(r.origin.x + 10, r.origin.y + 10, mTitle, inDrawRegion) ;
}


//=============================================================================
TTTile::TTTile(const AWPoint & inOrigin) :
AWView(computeTileRelativeFrame( inOrigin, 1, 1), TTBackColor)
{
}

//—————————————————————————————————————————————————————————————————————————————
TTTile::TTTile(const AWPoint & inOrigin,
               const AWInt inWidth,
               const AWInt inHeight) :
AWView(computeTileRelativeFrame( inOrigin, inWidth, inHeight), TTBackColor)
{
}

//—————————————————————————————————————————————————————————————————————————————
TTTile::TTTile(const AWRect & inFrame) :
AWView(inFrame, TTBackColor)
{
}

//=============================================================================
TTTrack::TTTrack (const AWPoint & inOrigin,
                  const AWInt inWidth,
                  const AWInt inHeight,
                  const bool inIsReverted) :
TTTile (inOrigin, inWidth, inHeight),
mIsReverted (inIsReverted),
mDirection (kForward)
{
}

//=============================================================================
TTTrack::TTTrack (const AWRect & inRect,
                  const bool inIsReverted) :
TTTile (inRect),
mIsReverted (inIsReverted),
mDirection (kForward)
{
}

//-----------------------------------------------------------------------------
void TTTrack::drawFrame ( const AWRegion & inDrawRegion ) const
{
  AWContext::setColor( AWColor::black() ) ;
  absoluteFrame().frameRectInRegion( inDrawRegion ) ;
}

//—————————————————————————————————————————————————————————————————————————————
bool TTTrack::isOpaque (void) const
{
  return false ;
}

//=============================================================================
TTBlock::TTBlock (const AWPoint & inOrigin,
                  const AWInt inLength,
                  const BlockInOutShape inInShape,
                  const BlockInOutShape inOutShape,
                  const bool inIsReverted) :
TTTrack ( computeTileRelativeFrameWithMargin(inOrigin, inLength, 1), inIsReverted ),
mInShape(inInShape),
mOutShape(inOutShape),
mDashed (false)
{
}

//-----------------------------------------------------------------------------
void TTBlock::drawInRegion ( const AWRegion & inDrawRegion ) const
{
  AWRect trackRect = absoluteFrame() ;
#ifdef DEBUG_TRACK
  drawFrame( inDrawRegion ) ;
#endif
  trackRect.inset(TILE_PIXEL_GRID / 2 + 3, (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 3) ;
  AWContext::setColor( activeTrackColor ) ;
  if (mDashed) {
    AWRect dash = trackRect ;
    dash.size.width = dash.size.height * 3 ;
    while (dash.origin.x < trackRect.origin.x + trackRect.size.width) {
      if (dash.origin.x + dash.size.width > trackRect.origin.x + trackRect.size.width) {
        dash.size.width = trackRect.origin.x + trackRect.size.width - dash.origin.x ;
      }
      dash.fillRectInRegion (inDrawRegion) ;
      dash.origin.x += dash.size.height * 5 ;
    }
  }
  else {
    trackRect.fillRectInRegion( inDrawRegion ) ;
  }
  AWInt xOffset ;
  AWInt yOffset ;
  AWInt initialYOffset ;

  trackRect = absoluteFrame() ;
  trackRect.inset(3, (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 3) ;
  switch (mInShape) {
    case kMiddle :
      trackRect.origin.x += isReverted() ? trackRect.size.width - TILE_PIXEL_GRID / 2 : 0 ;
      trackRect.size.width = TILE_PIXEL_GRID / 2 ;
      trackRect.fillRectInRegion(inDrawRegion) ;
      break ;
    case kTopDiagonal :
      trackRect.origin.x += isReverted() ? trackRect.size.width - TILE_PIXEL_GRID / 2 - 1 : TILE_PIXEL_GRID / 2 ;
      trackRect.origin.y += isReverted() ? -1 : 0 ;
      xOffset = isReverted() ? 1 : -1 ;
      yOffset = isReverted() ? -1 : 1 ;
      initialYOffset = isReverted() ? 0 : -1 ;
      trackRect.size.width = 1 ;
      trackRect.size.height = DIAGONAL_TRACK_WIDTH - 1 ;
      for (AWInt x = 0; x <= TILE_PIXEL_GRID / 2; x++) {
        trackRect.fillRectInRegion (inDrawRegion) ;
        trackRect.size.height = DIAGONAL_TRACK_WIDTH ;
        trackRect.translateBy(xOffset, yOffset + initialYOffset) ;
        initialYOffset = 0 ;
      }
      break ;
    case kBottomDiagonal :
      trackRect.origin.x += isReverted() ? trackRect.size.width - TILE_PIXEL_GRID / 2 - 1 : TILE_PIXEL_GRID / 2 ;
      trackRect.origin.y += isReverted() ? 0 : -1 ;
      xOffset = isReverted() ? 1 : -1 ;
      yOffset = isReverted() ? 1 : -1 ;
      initialYOffset = isReverted() ? -1 : 0 ;
      trackRect.size.width = 1 ;
      trackRect.size.height = DIAGONAL_TRACK_WIDTH - 1 ;
      for (AWInt x = 0; x <= TILE_PIXEL_GRID / 2; x++) {
        trackRect.fillRectInRegion (inDrawRegion) ;
        trackRect.size.height = DIAGONAL_TRACK_WIDTH ;
        trackRect.translateBy(xOffset, yOffset + initialYOffset) ;
        initialYOffset = 0 ;
      }
      break ;
  }

  trackRect = absoluteFrame() ;
  trackRect.inset(3, (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 3) ;
  switch (mOutShape) {
    case kMiddle :
      trackRect.origin.x += isReverted() ? 0 : trackRect.size.width - TILE_PIXEL_GRID / 2 ;
      trackRect.size.width = TILE_PIXEL_GRID / 2 ;
      trackRect.fillRectInRegion(inDrawRegion) ;
      break ;
    case kTopDiagonal :
      trackRect.origin.x += isReverted() ? TILE_PIXEL_GRID / 2 : trackRect.size.width - TILE_PIXEL_GRID / 2 - 1;
      trackRect.origin.y += isReverted() ? -1 : 0 ;
      xOffset = isReverted() ? -1 : 1 ;
      yOffset = isReverted() ? -1 : 1 ;
      initialYOffset = isReverted() ? 0 : -1 ;
      trackRect.size.width = 1 ;
      trackRect.size.height = DIAGONAL_TRACK_WIDTH - 1 ;
      for (int x = 0; x <= TILE_PIXEL_GRID / 2; x++) {
        trackRect.fillRectInRegion (inDrawRegion) ;
        trackRect.size.height = DIAGONAL_TRACK_WIDTH ;
        trackRect.translateBy(xOffset, yOffset + initialYOffset) ;
        initialYOffset = 0 ;
      }
      break ;
    case kBottomDiagonal :
      trackRect.origin.x += isReverted() ? TILE_PIXEL_GRID / 2 : trackRect.size.width - TILE_PIXEL_GRID / 2 - 1;
      trackRect.origin.y += isReverted() ? 0 : -1 ;
      xOffset = isReverted() ? -1 : 1 ;
      yOffset = isReverted() ? 1 : -1 ;
      initialYOffset = isReverted() ? -1 : 0 ;
      trackRect.size.width = 1 ;
      trackRect.size.height = DIAGONAL_TRACK_WIDTH - 1 ;
      for (int x = 0; x <= TILE_PIXEL_GRID / 2; x++) {
        trackRect.fillRectInRegion (inDrawRegion) ;
        trackRect.size.height = DIAGONAL_TRACK_WIDTH ;
        trackRect.translateBy(xOffset, yOffset + initialYOffset) ;
        initialYOffset = 0 ;
      }
      break ;
  }
}

//=============================================================================
TTTouchableTrack::TTTouchableTrack(const AWPoint & inOrigin,
                                   const AWInt inWidth,
                                   const AWInt inHeight,
                                   const bool inIsReverted) :
TTTrack(inOrigin, inWidth, inHeight, inIsReverted),
mFeedback(false)
{
}

//=============================================================================
TTTouchableTrack::TTTouchableTrack(const AWRect & inRect,
                                   const bool inIsReverted) :
TTTrack(inRect, inIsReverted),
mFeedback(false)
{
}

//=============================================================================
TTAbstractPoint::TTAbstractPoint (const AWPoint & inOrigin,
                                  const AWInt inWidth,
                                  const AWInt inHeight,
                                  const bool inIsReverted) :
TTTouchableTrack (inOrigin, inWidth, inHeight, inIsReverted)
{
}

//-----------------------------------------------------------------------------
TTAbstractPoint::TTAbstractPoint (const AWRect & inRect,
                                  const bool inIsReverted) :
TTTouchableTrack (inRect, inIsReverted)
{
}

//-----------------------------------------------------------------------------
 TTAbstractPoint * TTAbstractPoint::sPointForIdentifier[kTTMaxNumberOfPoints] = { NULL } ;

//-----------------------------------------------------------------------------
void TTAbstractPoint::setActualPositionOfPoint (const uint8_t inPointId,
                                                const TTPointPosition inPosition)
{
  if (inPointId < kTTMaxNumberOfPoints && sPointForIdentifier[inPointId] != NULL) {
    sPointForIdentifier[inPointId]->setActualPosition(inPointId, inPosition) ;
  }
}

//-----------------------------------------------------------------------------
void TTAbstractPoint::setWishedPositionOfPoint (const uint8_t inPointId,
                                                const TTPointPosition inPosition)
{
  if (inPointId < kTTMaxNumberOfPoints && sPointForIdentifier[inPointId] != NULL) {
    sPointForIdentifier[inPointId]->setWishedPosition(inPointId, inPosition) ;
  }
}

//-----------------------------------------------------------------------------
TTPointPosition TTAbstractPoint::actualPositionOfPoint (const uint8_t inPointId)
{
  if (inPointId < kTTMaxNumberOfPoints && sPointForIdentifier[inPointId] != NULL) {
    return sPointForIdentifier[inPointId]->actualPosition(inPointId) ;
  }
  else {
    return kNoPosition ;
  }
}

//-----------------------------------------------------------------------------
TTPointPosition TTAbstractPoint::wishedPositionOfPoint (const uint8_t inPointId)
{
  if (inPointId < kTTMaxNumberOfPoints && sPointForIdentifier[inPointId] != NULL) {
    return sPointForIdentifier[inPointId]->wishedPosition(inPointId) ;
  }
  else {
    return kNoPosition ;
  }
}

//-----------------------------------------------------------------------------
void TTAbstractPoint::setCanicheActualPositionOfPoint(const uint8_t inPointId,
                                                      const TTCanichePointPosition inPosition)
{
  if (inPointId < kTTMaxNumberOfPoints && sPointForIdentifier[inPointId] != NULL) {
    TTPointPosition ttPosition = sPointForIdentifier[inPointId]->canicheToPointPosition (inPosition) ;
    sPointForIdentifier[inPointId]->setActualPosition(inPointId, ttPosition) ;
  }
}

//-----------------------------------------------------------------------------
void TTAbstractPoint::setCanicheWishedPositionOfPoint(const uint8_t inPointId,
                                                      const TTCanichePointPosition inPosition)
{
  if (inPointId < kTTMaxNumberOfPoints && sPointForIdentifier[inPointId] != NULL) {
    TTPointPosition ttPosition = sPointForIdentifier[inPointId]->canicheToPointPosition (inPosition) ;
    sPointForIdentifier[inPointId]->setWishedPosition(inPointId, ttPosition) ;
  }
}

//-----------------------------------------------------------------------------
TTCanichePointPosition TTAbstractPoint::wishedCanichePositionOfPoint (const uint8_t inPointId)
{
  if (inPointId < kTTMaxNumberOfPoints && sPointForIdentifier[inPointId] != NULL) {
    TTPointPosition pos = sPointForIdentifier[inPointId]->wishedPosition(inPointId) ;
    return sPointForIdentifier[inPointId]->pointToCanichePosition (pos) ;
  }
  else {
    return kCanicheUnknownPosition ;
  }

}

//-----------------------------------------------------------------------------
void TTAbstractPoint::setActionOfPoint (const uint8_t inPointId, AWAction inAction)
{
  if (inPointId < kTTMaxNumberOfPoints && sPointForIdentifier[inPointId] != NULL) {
    sPointForIdentifier[inPointId]->setAction(inAction) ;
  }
}

//-----------------------------------------------------------------------------
void TTAbstractPoint::setActionOfAllPoints (AWAction inAction)
{
  for (uint8_t i = 0 ; i < kTTMaxNumberOfPoints ; i++) {
    setActionOfPoint(i, inAction) ;
  }
}

//-----------------------------------------------------------------------------
void TTAbstractPoint::registerPointId (const uint8_t inPointId)
{
  if (inPointId < kTTMaxNumberOfPoints) {
    sPointForIdentifier[inPointId] = this ;
  }
}

//-----------------------------------------------------------------------------
TTCanichePointPosition TTAbstractPoint::pendingCanicheWish (uint8_t & outPointId)
{
  TTPointPosition pos = pendingWish (outPointId) ;
  return pointToCanichePosition (pos) ;
}

//=============================================================================
TTPoint::TTPoint (const AWPoint & inOrigin,
                  const uint8_t inPointId,
                  const bool inWay,
                  const bool inInDiagonal,
                  const bool inOutDiagonal,
                  const bool inIsReverted) :
TTAbstractPoint (computeTileRelativeFrameWithMargin (inOrigin, 2, 2), inIsReverted),
mPointId (inPointId),
mWay (inWay),
mInDiagonal (inInDiagonal),
mOutDiagonal (inOutDiagonal),
mWishedPosition (kStraightPosition),
mActualPosition (kStraightPosition),
mPendingWishedPosition (false),
mToggled (false)
{
  registerPointId (inPointId) ;
}

//-----------------------------------------------------------------------------
void TTPoint::setActualPosition (const uint8_t inPointId,
                                 const TTPointPosition inPosition)
{
  if (mPointId == inPointId && mActualPosition != inPosition) {
    mActualPosition = inPosition ;
    if (mActualPosition == mWishedPosition) mPendingWishedPosition = false ;
    setNeedsDisplay() ;
  }
}

//-----------------------------------------------------------------------------
void TTPoint::setWishedPosition (const uint8_t inPointId,
                                 const TTPointPosition inPosition)
{
  if (mPointId == inPointId &&
      mWishedPosition != inPosition &&
      (inPosition == kStraightPosition ||
       inPosition == kDiagonalPosition))
  {
    mWishedPosition = inPosition ;
    mToggled = true ;
    sendAction () ;
    setNeedsDisplay() ;
  }
}

//-----------------------------------------------------------------------------
TTPointPosition TTPoint::actualPosition (const uint8_t inPointId)
{
  if (mPointId == inPointId) {
    return mActualPosition ;
  }
  else {
    return kNoPosition ;
   }
}

//-----------------------------------------------------------------------------
TTPointPosition TTPoint::wishedPosition (const uint8_t inPointId)
{
  if (mPointId == inPointId) {
    return mWishedPosition ;
  }
  else {
    return kNoPosition ;
  }
}

//-----------------------------------------------------------------------------
TTPointPosition TTPoint::pendingWish (uint8_t & outPointId)
{
  if (mToggled) {
    mToggled = false ;
    outPointId = mPointId ;
    return mWishedPosition ;
  }
  else {
    return kNoPosition ;
  }
}

//-----------------------------------------------------------------------------
TTCanichePointPosition TTPoint::pointToCanichePosition (TTPointPosition inPos)
{
  switch (inPos) {
    case kDiagonalPosition:
      return mWay == kRightHand ? kCanicheRightPosition : kCanicheLeftPosition ;
      break ;
    case kStraightPosition:
      return mWay == kRightHand ? kCanicheLeftPosition : kCanicheRightPosition ;
      break ;
    case kMiddlePosition:
      return kCanicheMiddlePosition ;
      break ;
    case kNoPosition:
      return kCanicheUnknownPosition ;
      break ;
    default:
      return kCanicheUnknownPosition ;
      break ;
  }
}

//-----------------------------------------------------------------------------
TTPointPosition TTPoint::canicheToPointPosition (TTCanichePointPosition inPos)
{
  switch (inPos) {
    case kCanicheLeftPosition:
      return mWay == kRightHand ? kStraightPosition : kDiagonalPosition ;
      break ;
    case kCanicheRightPosition:
      return mWay == kRightHand ? kDiagonalPosition : kStraightPosition ;
      break ;
    case kCanicheMiddlePosition:
      return kMiddlePosition ;
      break ;
    case kCanicheUnknownPosition:
      return kNoPosition ;
      break ;
    default:
      return kNoPosition ;
      break ;
  }
}

//-----------------------------------------------------------------------------
void TTPoint::drawStraight (const AWRegion & inDrawRegion, const AWColor inStraightColor ) const
{
  //--- Draw the straight track
  AWContext::setColor(inStraightColor) ;
  AWRect r = absoluteFrame() ;
  r.size.height = STRAIGHT_TRACK_WIDTH ;
  r.size.width -= (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 8 ;
  r.origin.x += 3 ;
  if (! isReverted()) r.origin.x += (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 2 ;
  if ((mWay == kRightHand) ^ isReverted()) {
    r.origin.y += TILE_PIXEL_GRID + (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 3 ;
  }
  else {
    r.origin.y += (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 3 ;
  }
  r.fillRectInRegion(inDrawRegion) ;
}

//-----------------------------------------------------------------------------
void TTPoint::drawDiagonal (const AWRegion & inDrawRegion, const AWColor inDiagonalColor ) const
{
  //--- Draw the diagonal track
  int yOffset ;
  AWRect r = absoluteFrame () ;
  int height = r.size.height ;
  r.size.width = 1 ;
  r.origin.x += (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 5 ;
  if (mWay == kRightHand) {
    yOffset = -1 ;
    r.origin.y += height - 9 - (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 ;
  }
  else {
    yOffset = 1 ;
    r.origin.y += (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 3;
  }

  AWContext::setColor( inDiagonalColor ) ;
  r.size.height = DIAGONAL_TRACK_WIDTH - 1;
  r.fillRectInRegion (inDrawRegion) ;
  r.size.height++ ;
  r.origin.x++ ;
  r.origin.y += (yOffset == -1) ? yOffset : 0 ;

  for (int x = 1 ; x < height - 6 - TILE_PIXEL_GRID ; x++) {
    r.fillRectInRegion (inDrawRegion) ;
    r.origin.x++ ;
    r.origin.y += yOffset ;
  }

  r.size.height-- ;
  r.origin.y -= (yOffset == -1) ? yOffset : 0 ;
  r.fillRectInRegion (inDrawRegion) ;
}

//-----------------------------------------------------------------------------
void TTPoint::drawInRegion ( const AWRegion & inDrawRegion ) const
{
  const AWRect trackRect = absoluteFrame() ;
  AWRect buttonRect = trackRect;
  buttonRect.inset(3, 3);
  AWContext::setColor (feedback() ? feedbackColor : buttonBackColor) ;
  buttonRect.fillRoundRectInRegion (4, inDrawRegion) ;
  AWContext::setColor( buttonFrameColor ) ;
  buttonRect.frameRoundRectInRegion (4, inDrawRegion) ;
#ifdef DEBUG_TRACK
  drawFrame( inDrawRegion ) ;
#endif

  AWColor diagonalColor;
  AWColor straightColor;

  switch (mActualPosition) {
    case kMiddlePosition:
      if (mWishedPosition == kStraightPosition) {
        straightColor = mPendingWishedPosition ? wishedPosColor : inactiveTrackColor ;
        diagonalColor = inactiveTrackColor ;
        drawDiagonal(inDrawRegion, diagonalColor);
        drawStraight(inDrawRegion, straightColor);
      }
      else { /* kDiagonalPosition */
        straightColor = inactiveTrackColor ;
        diagonalColor = mPendingWishedPosition ? wishedPosColor : inactiveTrackColor ;
        drawStraight(inDrawRegion, straightColor);
        drawDiagonal(inDrawRegion, diagonalColor);
      }
      break ;
    case kDiagonalPosition:
      diagonalColor = activeTrackColor ;
      straightColor = (mWishedPosition == kStraightPosition) ? mPendingWishedPosition ? wishedPosColor : inactiveTrackColor : inactiveTrackColor ;
      drawStraight(inDrawRegion, straightColor);
      drawDiagonal(inDrawRegion, diagonalColor);
      break ;
    case kStraightPosition:
      straightColor = activeTrackColor ;
      diagonalColor = (mWishedPosition == kDiagonalPosition) ? mPendingWishedPosition ? wishedPosColor : inactiveTrackColor : inactiveTrackColor ;
      drawDiagonal(inDrawRegion, diagonalColor);
      drawStraight(inDrawRegion, straightColor);
      break ;
    default:
      break ;
  }

  int yOffset ;

  //--- Draw the input
  AWContext::setColor(activeTrackColor) ;
  AWRect r = trackRect ;
  r.origin.x += 3 ;
  if ( isReverted() ) r.origin.x += TILE_PIXEL_GRID + (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 2 ;
  if (mInDiagonal == kStraight) {
    r.size.height = STRAIGHT_TRACK_WIDTH ;
    r.size.width = (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 3 ;
    if ((mWay == kRightHand) ^ isReverted()) {
      r.origin.y += TILE_PIXEL_GRID + (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 3;
    }
    else {
      r.origin.y += (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 3;
    }
    r.fillRectInRegion(inDrawRegion) ;
  }
  else {
    r.size.height = DIAGONAL_TRACK_WIDTH ;
    r.size.width = 1 ;
    if (mWay == kRightHand) {
      r.origin.y += isReverted() ? TILE_PIXEL_GRID / 3 + 3 : 2 * TILE_PIXEL_GRID - 1;
      yOffset = -1;
    }
    else {
      r.origin.y += isReverted() ? TILE_PIXEL_GRID + TILE_PIXEL_GRID / 3 + 3 : 0 ;
      yOffset = 1;
    }
    for (int x = 0; x < (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 3; x++) {
      r.fillRectInRegion (inDrawRegion) ;
      r.origin.x++ ;
      r.origin.y += yOffset ;
    }
  }

  //--- Draw the output
  AWContext::setColor(diagonalColor);
  r = trackRect ;
  r.origin.x += 3 ;
  if ( ! isReverted() ) r.origin.x += TILE_PIXEL_GRID + (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 2 ;
  if (mOutDiagonal == kStraight) {
    r.size.height = STRAIGHT_TRACK_WIDTH ;
    r.size.width = (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 2 ;
    if ((mWay == kRightHand) ^ isReverted()) {
      r.origin.y += (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 3;
    }
    else {
      r.origin.y += TILE_PIXEL_GRID + (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 3;
    }
    r.fillRectInRegion(inDrawRegion) ;
  }
  else {
    r.size.height = DIAGONAL_TRACK_WIDTH ;
    r.size.width = 1 ;
    if (mWay == kRightHand) {
      r.origin.y += isReverted() ? 2 * TILE_PIXEL_GRID - 1 : (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 2 ;
      yOffset = -1;
    }
    else {
      r.origin.y += isReverted() ? 0 : TILE_PIXEL_GRID + (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 2 ;
      yOffset = 1;
    }
    for (int x = 0; x <= (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 3; x++) {
      r.fillRectInRegion (inDrawRegion) ;
      r.origin.x++ ;
      r.origin.y += yOffset ;
    }
  }

#ifdef TRACK_TAG
  r = trackRect ;
  r.inset (3, 3) ;
  AWContext::setColor(awkTextColor) ;
  String text (mPointId) ;
  r.origin.x += isReverted() ? r.size.width - 5 - kTTTagFont.stringLength (text) : 5 ;
  r.origin.y += (mWay == kLeftHand) ^ isReverted() ? r.size.height - 5 - kTTTagFont.ascent() : 5 ;
  kTTTagFont.drawStringInRegion(r.origin.x, r.origin.y, text, inDrawRegion) ;
#endif
}

//-----------------------------------------------------------------------------
void TTPoint::toggle ()
{
  if (mWishedPosition == kStraightPosition) {
    mWishedPosition = kDiagonalPosition ;
  }
  else if (mWishedPosition == kDiagonalPosition) {
    mWishedPosition = kStraightPosition ;
  }
  mToggled = true ;
  mPendingWishedPosition = true ;
  setNeedsDisplay() ;
  sendAction() ;
}

//-----------------------------------------------------------------------------
void TTTouchableTrack::touchDown (const AWPoint & inPoint)
{
  AWRect r = absoluteFrame();
  r.inset(3, 3);
  if (r.containsPoint(inPoint)) {
    setFeedback(true) ;
    setNeedsDisplay() ;
  }
}

//-----------------------------------------------------------------------------
void TTTouchableTrack::touchMove (const AWPoint & inPoint)
{
  AWRect r = absoluteFrame();
  r.inset(3, 3);
  if (r.containsPoint(inPoint)) {
    if (! feedback()) {
      setFeedback(true) ;
      setNeedsDisplay() ;
    }
  }
  else {
    if (feedback()) {
      setFeedback(false) ;
      setNeedsDisplay() ;
    }
  }
}

//-----------------------------------------------------------------------------
void TTTouchableTrack::touchUp (const AWPoint & inPoint)
{
  AWRect r = absoluteFrame();
  r.inset(3, 3);
  if (r.containsPoint(inPoint)) {
    setFeedback(false) ;
    toggle() ;
  }
}

//-----------------------------------------------------------------------------
void TTTouchableTrack::toggle ()
{
}

//=============================================================================
TTDoublePoint::TTDoublePoint (const AWPoint & inOrigin,
                              const uint8_t inTopPointId,
                              const uint8_t inBottomPointId,
                              const bool inWay,
                              const TTDoublePointSize inSize,
                              const bool inIsReverted) :
TTAbstractPoint (computeTileRelativeFrameWithMargin(inOrigin, inSize == kNormal ? 2 : 3, inSize == kNormal ? 2 : 3), inIsReverted),
mTopPointId (inTopPointId),
mBottomPointId (inBottomPointId),
mWay (inWay),
mSize (inSize),
mWishedTopPosition (kStraightPosition),
mWishedBottomPosition (kStraightPosition),
mActualTopPosition (kStraightPosition),
mActualBottomPosition (kStraightPosition),
mPendingWishedTopPosition (false),
mPendingWishedBottomPosition (false),
mTouchInProgress (false),
mTopToggled (false),
mBottomToggled (false)
{
  registerPointId (inTopPointId) ;
  registerPointId (inBottomPointId) ;
}

//-----------------------------------------------------------------------------

void TTDoublePoint::setActualPosition(const uint8_t inPointId,
                                      const TTPointPosition inPosition)
{
  if (mTopPointId == inPointId && mActualTopPosition != inPosition) {
    mActualTopPosition = inPosition ;
    if (mActualTopPosition == mWishedTopPosition) mPendingWishedTopPosition = false ;
    setNeedsDisplay() ;
  }
  else if (mBottomPointId == inPointId && mActualBottomPosition != inPosition) {
    mActualBottomPosition = inPosition ;
    if (mActualBottomPosition == mWishedBottomPosition) mPendingWishedBottomPosition = false ;
    setNeedsDisplay() ;
  }
}

//-----------------------------------------------------------------------------

void TTDoublePoint::setWishedPosition(const uint8_t inPointId,
                                      const TTPointPosition inPosition)
{
  if (mTopPointId == inPointId &&
      mWishedTopPosition != inPosition &&
      (inPosition == kStraightPosition ||
       inPosition == kDiagonalPosition))
  {
    mWishedTopPosition = inPosition ;
    mPendingWishedTopPosition = true ;
    mTopToggled = true ;
    sendAction () ;
    setNeedsDisplay () ;
  }
  else if (mBottomPointId == inPointId &&
      mWishedBottomPosition != inPosition &&
      (inPosition == kStraightPosition ||
       inPosition == kDiagonalPosition))
  {
    mWishedBottomPosition = inPosition ;
    mPendingWishedBottomPosition = true ;
    mBottomToggled = true ;
    sendAction () ;
    setNeedsDisplay() ;
  }
}

//-----------------------------------------------------------------------------
TTPointPosition TTDoublePoint::actualPosition (const uint8_t inPointId)
{
  if (mTopPointId == inPointId) {
    return mActualTopPosition ;
  }
  else if (mBottomPointId == inPointId) {
    return mActualBottomPosition ;
  }
  else {
    return kNoPosition ;
  }
}

//-----------------------------------------------------------------------------
TTPointPosition TTDoublePoint::wishedPosition (const uint8_t inPointId)
{
  if (mTopPointId == inPointId) {
    return mWishedTopPosition ;
  }
  else if (mBottomPointId == inPointId) {
    return mWishedBottomPosition ;
  }
  else {
    return kNoPosition ;
  }
}

//-----------------------------------------------------------------------------
TTPointPosition TTDoublePoint::pendingWish (uint8_t & outPointId)
{
  if (mTopToggled) {
    outPointId = mTopPointId ;
    mTopToggled = false ;
    return mWishedTopPosition ;
  }
  else if (mBottomToggled) {
    outPointId = mBottomPointId ;
    mBottomToggled = false ;
    return mWishedBottomPosition ;
  }
  else {
    return kNoPosition ;
  }
}

//-----------------------------------------------------------------------------
TTCanichePointPosition TTDoublePoint::pointToCanichePosition (TTPointPosition inPos)
{
  switch (inPos) {
    case kDiagonalPosition:
      return mWay == kRightHand ? kCanicheRightPosition : kCanicheLeftPosition ;
      break ;
    case kStraightPosition:
      return mWay == kRightHand ? kCanicheLeftPosition : kCanicheRightPosition ;
      break ;
    case kMiddlePosition:
      return kCanicheMiddlePosition ;
      break ;
    case kNoPosition:
      return kCanicheUnknownPosition ;
      break ;
    default:
      return kCanicheUnknownPosition ;
      break ;
  }
}

//-----------------------------------------------------------------------------
TTPointPosition TTDoublePoint::canicheToPointPosition (TTCanichePointPosition inPos)
{
  switch (inPos) {
    case kCanicheLeftPosition:
      return mWay == kRightHand ? kStraightPosition : kDiagonalPosition ;
      break ;
    case kCanicheRightPosition:
      return mWay == kRightHand ? kDiagonalPosition : kStraightPosition ;
      break ;
    case kCanicheMiddlePosition:
      return kMiddlePosition ;
      break ;
    case kCanicheUnknownPosition:
      return kNoPosition ;
      break ;
    default:
      return kNoPosition ;
      break ;
  }
}

//-----------------------------------------------------------------------------
void TTDoublePoint::drawTopStraight (const AWRegion & inDrawRegion,
                                     const AWColor inStraightColor ) const
{
  //--- Draw the top straight track
  AWContext::setColor (inStraightColor) ;
  AWRect r = absoluteFrame() ;
  int height = r.size.height ;
  r.size.height = STRAIGHT_TRACK_WIDTH ;
  r.size.width -= (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 8 ;
  r.origin.x += 3 ;
  r.origin.y += height - (TILE_PIXEL_GRID + STRAIGHT_TRACK_WIDTH) / 2 - 3 ;
  if (mWay == kRightHand) {
    r.origin.x += TILE_PIXEL_GRID / 2 ;
  }
  r.fillRectInRegion (inDrawRegion) ;
}

//-----------------------------------------------------------------------------
void TTDoublePoint::drawBottomStraight (const AWRegion & inDrawRegion,
                                        const AWColor inStraightColor ) const
{
  //--- Draw the straight track
  AWContext::setColor (inStraightColor) ;
  AWRect r = absoluteFrame() ;
  r.size.height = STRAIGHT_TRACK_WIDTH ;
  r.size.width -= (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 8 ;
  r.origin.x += 3 ;
  r.origin.y += (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 3 ;
  if (mWay == kLeftHand) {
    r.origin.x += TILE_PIXEL_GRID / 2 ;
  }
  r.fillRectInRegion (inDrawRegion) ;
}

//-----------------------------------------------------------------------------
void TTDoublePoint::drawTopDiagonal (const AWRegion & inDrawRegion,
                                     const AWColor inDiagonalColor) const
{
  //--- Draw the diagonal track
  int yOffset ;
  AWRect r = absoluteFrame () ;
  int height = r.size.height ;
  r.size.width = 1 ;
  r.origin.x += (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 5 ;
  if (mWay == kRightHand) {
    yOffset = -1 ;
    r.origin.y += height - 9 - (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 ;
  }
  else {
    yOffset = 1 ;
    r.origin.x += (height - 4 - TILE_PIXEL_GRID) / 2 - (height % 2);
    r.origin.y += (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 3 + (height - 6 - TILE_PIXEL_GRID) / 2  - (height % 2);
  }

  AWContext::setColor (inDiagonalColor) ;
  r.size.height = DIAGONAL_TRACK_WIDTH ;

  if (mWay == kRightHand) {
    r.size.height-- ;
    r.fillRectInRegion (inDrawRegion) ;
    r.size.height++ ;
    r.origin.x++ ;
    r.origin.y += (yOffset == -1) ? yOffset : 0 ;
  }

  for (int x = 0 ; x < (height - 6 - TILE_PIXEL_GRID) / 2 ; x++) {
    r.fillRectInRegion (inDrawRegion) ;
    r.origin.x++ ;
    r.origin.y += yOffset ;
  }

  if (mWay == kLeftHand) {
    r.size.height-- ;
    r.origin.y -= (yOffset == -1) ? yOffset : 0 ;
    r.fillRectInRegion (inDrawRegion) ;
  }
}

//-----------------------------------------------------------------------------
void TTDoublePoint::drawBottomDiagonal (const AWRegion & inDrawRegion,
                                        const AWColor inDiagonalColor ) const
{
  //--- Draw the diagonal track
  int yOffset ;
  AWRect r = absoluteFrame () ;
  int height = r.size.height ;
  r.size.width = 1 ;
  r.origin.x += (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 5 ;
  if (mWay == kRightHand) {
    yOffset = -1 ;
    r.origin.x += (height - 4 - TILE_PIXEL_GRID) / 2 ;
    r.origin.y += (height - 8) / 2 ;
  }
  else {
    yOffset = 1 ;
    r.origin.y += (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 + 3 ;
  }

  AWContext::setColor (inDiagonalColor) ;
  r.size.height = DIAGONAL_TRACK_WIDTH ;

  if (mWay == kLeftHand) {
    r.size.height-- ;
    r.fillRectInRegion (inDrawRegion) ;
    r.size.height++ ;
    r.origin.x++ ;
    r.origin.y += (yOffset == -1) ? yOffset : 0 ;
  }

  int limit = (height - 6 - TILE_PIXEL_GRID) / 2 - (height % 2);
  for (int x = 0 ; x < limit ; x++) {
    r.fillRectInRegion (inDrawRegion) ;
    r.origin.x++ ;
    r.origin.y += yOffset ;
  }

  if (mWay == kRightHand) {
    r.size.height-- ;
    r.origin.y -= (yOffset == -1) ? yOffset : 0 ;
    r.fillRectInRegion (inDrawRegion) ;
  }
}

//-----------------------------------------------------------------------------
void TTDoublePoint::drawStraightLogo (AWRect &inButton, const AWRegion & inDrawRegion) const
{
  AWRect r = inButton ;
  r.inset (12, 17) ;
  r.translateBy (0, -5) ;
  AWColor currentColor = AWContext::color () ;
  AWContext::setColor (AWColor::gray ()) ;
  r.fillRectInRegion (inDrawRegion) ;
  AWContext::setColor (currentColor) ;
}

//-----------------------------------------------------------------------------
void TTDoublePoint::drawDiagonalLogo (AWRect &inButton, const AWRegion & inDrawRegion) const
{
  AWRect r = inButton ;
  r.inset (13, 0) ;
  r.size.width = 1 ;
  r.size.height = 4 ;
  r.origin.y += 17 ;
  AWInt yOffset = (mWay == kRightHand) ? -1 : 1 ;
  if (mWay == kRightHand) r.origin.y += 11 ;
  AWColor currentColor = AWContext::color () ;
  AWContext::setColor (AWColor::gray ()) ;
  for (int i = 0 ; i < 11 ; i++) {
    r.fillRectInRegion (inDrawRegion) ;
    r.translateBy (1, yOffset) ;
  }
  AWContext::setColor (currentColor) ;
}

//-----------------------------------------------------------------------------
void TTDoublePoint::drawInRegion ( const AWRegion & inDrawRegion ) const
{
  AWRect trackRect = absoluteFrame() ;
  trackRect.inset(3, 3);
  AWContext::setColor (feedback () ? feedbackColor : buttonBackColor) ;
  if (mSize == kLargeNoSync) {
    if (mTouchInProgress) {
      mFeedbackRect.fillRoundRectInRegion (4, inDrawRegion) ;
    }
    else {
      AWRect subButtonRect = trackRect ;
      subButtonRect.size.width /= 2 ;
      subButtonRect.size.height /= 2 ;
      subButtonRect.fillRoundRectInRegion(4, inDrawRegion) ;
      subButtonRect.origin.x += subButtonRect.size.width ;
      subButtonRect.fillRoundRectInRegion(4, inDrawRegion) ;
      subButtonRect.origin.y += subButtonRect.size.height ;
      subButtonRect.fillRoundRectInRegion(4, inDrawRegion) ;
      subButtonRect.origin.x -= subButtonRect.size.width ;
      subButtonRect.fillRoundRectInRegion(4, inDrawRegion) ;
    }
  }
  else {
    trackRect.fillRoundRectInRegion (4, inDrawRegion) ;
  }
  AWContext::setColor (buttonFrameColor) ;
  if (mSize == kLargeNoSync) {
    AWRect subButtonRect = trackRect ;
    subButtonRect.size.width /= 2 ;
    subButtonRect.size.height /= 2 ;
    subButtonRect.frameRoundRectInRegion(4, inDrawRegion) ;
    if (mWay == kRightHand) drawDiagonalLogo (subButtonRect, inDrawRegion) ;
    subButtonRect.origin.x += subButtonRect.size.width ;
    subButtonRect.frameRoundRectInRegion(4, inDrawRegion) ;
    if (mWay == kLeftHand) drawDiagonalLogo (subButtonRect, inDrawRegion) ;
    subButtonRect.origin.y += subButtonRect.size.height ;
    subButtonRect.frameRoundRectInRegion(4, inDrawRegion) ;
    if (mWay == kRightHand) drawStraightLogo (subButtonRect, inDrawRegion) ;
    subButtonRect.origin.x -= subButtonRect.size.width ;
    subButtonRect.frameRoundRectInRegion(4, inDrawRegion) ;
    if (mWay == kLeftHand) drawStraightLogo (subButtonRect, inDrawRegion) ;
  }
  else {
    trackRect.frameRoundRectInRegion (4, inDrawRegion) ;
  }
#ifdef DEBUG_TRACK
  drawFrame (inDrawRegion) ;
#endif

  switch (mActualTopPosition) {
    case kMiddlePosition:
      if (mWishedTopPosition == kStraightPosition) {
        drawTopDiagonal (inDrawRegion, inactiveTrackColor) ;
        drawTopStraight (inDrawRegion, mPendingWishedTopPosition ? wishedPosColor : inactiveTrackColor) ;
      }
      else { /* kDiagonalPosition */
        drawTopStraight (inDrawRegion, inactiveTrackColor) ;
        drawTopDiagonal (inDrawRegion, mPendingWishedTopPosition ? wishedPosColor : inactiveTrackColor) ;
      }
      break ;
    case kStraightPosition:
      drawTopDiagonal (inDrawRegion, mWishedTopPosition == kStraightPosition ? inactiveTrackColor : mPendingWishedTopPosition ? wishedPosColor : inactiveTrackColor) ;
      drawTopStraight (inDrawRegion, activeTrackColor) ;
      break ;
    case kDiagonalPosition:
      drawTopStraight (inDrawRegion, mWishedTopPosition == kDiagonalPosition ? inactiveTrackColor : mPendingWishedTopPosition ? wishedPosColor : inactiveTrackColor) ;
      drawTopDiagonal (inDrawRegion, activeTrackColor) ;
      break ;
    default:
      break ;
  }

  switch (mActualBottomPosition) {
    case kMiddlePosition:
      if (mWishedBottomPosition == kStraightPosition) {
        drawBottomDiagonal (inDrawRegion, inactiveTrackColor) ;
        drawBottomStraight (inDrawRegion, mPendingWishedBottomPosition ? wishedPosColor : inactiveTrackColor) ;
      }
      else { /* kDiagonalPosition */
        drawBottomStraight (inDrawRegion, inactiveTrackColor) ;
        drawBottomDiagonal (inDrawRegion, mPendingWishedBottomPosition ? wishedPosColor : inactiveTrackColor) ;
      }
      break ;
    case kStraightPosition:
      drawBottomDiagonal (inDrawRegion, mWishedBottomPosition == kStraightPosition ? inactiveTrackColor : mPendingWishedBottomPosition ? wishedPosColor : inactiveTrackColor) ;
      drawBottomStraight (inDrawRegion, activeTrackColor) ;
      break ;
    case kDiagonalPosition:
      drawBottomStraight (inDrawRegion, mWishedBottomPosition == kDiagonalPosition ? inactiveTrackColor : mPendingWishedBottomPosition ? wishedPosColor : inactiveTrackColor) ;
      drawBottomDiagonal (inDrawRegion, activeTrackColor) ;
      break ;
    default:
      break ;
  }

  AWContext::setColor( activeTrackColor ) ;
  AWRect r = trackRect ;
  int height = r.size.height ;
  r.size.width = TILE_PIXEL_GRID / 2;
  r.size.height = STRAIGHT_TRACK_WIDTH ;
  r.origin.y += (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 ;
  if (mWay == kRightHand) {
    r.origin.x += height - TILE_PIXEL_GRID / 2 ;
    r.fillRectInRegion (inDrawRegion) ;
    r.origin.x -= height - TILE_PIXEL_GRID / 2 ;
    r.origin.y += height - TILE_PIXEL_GRID ;
    r.fillRectInRegion (inDrawRegion) ;
  }
  else {
    r.fillRectInRegion (inDrawRegion) ;
    r.origin.x += height - TILE_PIXEL_GRID / 2 ;
    r.origin.y += height - TILE_PIXEL_GRID ;
    r.fillRectInRegion (inDrawRegion) ;
  }

#ifdef TRACK_TAG
  r = trackRect ;
  AWContext::setColor(awkTextColor) ;
  String textBottom (mBottomPointId) ;
  String textTop (mTopPointId) ;
  AWInt xBottom = r.origin.x + (mWay == kRightHand ? r.size.width - 5 - kTTTagFont.stringLength(textBottom) : 5 ) ;
  AWInt xTop = r.origin.x + (mWay == kLeftHand ? r.size.width - 5 - kTTTagFont.stringLength(textTop) : 5 ) ;
  kTTTagFont.drawStringInRegion(xBottom, r.origin.y + 22, textBottom, inDrawRegion) ;
  kTTTagFont.drawStringInRegion(xTop, r.origin.y + r.size.height - 22 - awkDefaultFont.ascent (), textTop, inDrawRegion) ;
#endif
}

static const uint8_t kBottomLeft = 0 ;
static const uint8_t kBottomRight = 1 ;
static const uint8_t kTopLeft = 2 ;
static const uint8_t kTopRight = 3 ;

//-----------------------------------------------------------------------------
void TTDoublePoint::touchDown (const AWPoint & inPoint)
{
  AWRect r = absoluteFrame ();
  r.inset(3, 3);
  if (mSize != kLargeNoSync) {
    if (r.containsPoint(inPoint)) {
      setFeedback (true) ;
      setNeedsDisplay() ;
    }
  }
  else {
    if (r.containsPoint(inPoint)) {
      AWPoint normalizedPoint = inPoint ;
      normalizedPoint.x -= r.origin.x ;
      normalizedPoint.y -= r.origin.y ;
      normalizedPoint.x = (2 * normalizedPoint.x) / r.size.width ;
      normalizedPoint.y = (2 * normalizedPoint.y) / r.size.height ;
      mTouchLocation = 2 * (uint8_t)normalizedPoint.y + (uint8_t)normalizedPoint.x ;
      r.size.width /= 2 ;
      r.size.height /= 2 ;
      r.translateBy(normalizedPoint.x * r.size.width, normalizedPoint.y * r.size.height) ;
      mFeedbackRect = r ;
      mTouchInProgress = true ;
      setFeedback (true) ;
      setNeedsDisplayInRect(r) ;
    }
  }
}

//-----------------------------------------------------------------------------
void TTDoublePoint::touchMove (const AWPoint & inPoint)
{
  AWRect r ;
  if (mSize == kLargeNoSync) {
    r = mFeedbackRect ;
  }
  else {
    r = absoluteFrame () ;
    r.inset (3, 3) ;
  }
  if (r.containsPoint (inPoint)) {
    if (! feedback ()) {
      setFeedback (true) ;
      setNeedsDisplayInRect(r) ;
    }
  }
  else {
    if (feedback ()) {
      setFeedback (false) ;
      setNeedsDisplayInRect(r) ;
    }
  }
}

//-----------------------------------------------------------------------------
void TTDoublePoint::touchUp (const AWPoint & inPoint)
{
  AWRect r ;
  if (mSize == kLargeNoSync) {
    r = mFeedbackRect ;
  }
  else {
    r = absoluteFrame();
    r.inset(3, 3);
  }
  if (r.containsPoint(inPoint)) {
    setFeedback(false) ;
    mTouchInProgress = false ;
    if (mSize == kLargeNoSync) {
      switch (mTouchLocation) {
        case kBottomLeft:
          if (mWay == kLeftHand) {
            toggleBottom () ;
          }
          else {
            setDiagonal () ;
          }
          break ;
        case kBottomRight:
          if (mWay == kRightHand) {
            toggleBottom() ;
          }
          else {
            setDiagonal () ;
          }
          break ;
        case kTopLeft:
          if (mWay == kRightHand) {
            toggleTop() ;
          }
          else {
            setStraight () ;
          }
          break ;
        case kTopRight:
          if (mWay == kLeftHand) {
            toggleTop() ;
          }
          else {
            setStraight () ;
          }
          break ;
      }
    }
    else {
      toggle() ;
    }
  }
}

//-----------------------------------------------------------------------------
void TTDoublePoint::toggle ()
{
  if (mWishedTopPosition == kStraightPosition) {
    mWishedTopPosition = kDiagonalPosition ;
  }
  else if (mWishedTopPosition == kDiagonalPosition) {
    mWishedTopPosition = kStraightPosition ;
  }
  if (mWishedBottomPosition == kStraightPosition) {
    mWishedBottomPosition = kDiagonalPosition ;
  }
  else if (mWishedBottomPosition == kDiagonalPosition) {
    mWishedBottomPosition = kStraightPosition ;
  }
  mTopToggled = true ;
  mBottomToggled = true ;
  mPendingWishedTopPosition = true ;
  mPendingWishedBottomPosition = true ;
  setNeedsDisplay () ;
  sendAction () ;
}

//-----------------------------------------------------------------------------
void TTDoublePoint::toggleTop ()
{
  if (mWishedTopPosition == kStraightPosition) {
    mWishedTopPosition = kDiagonalPosition ;
  }
  else if (mWishedTopPosition == kDiagonalPosition) {
    mWishedTopPosition = kStraightPosition ;
  }
  mTopToggled = true ;
  mPendingWishedTopPosition = true ;
  setNeedsDisplay () ;
  sendAction () ;
}

//-----------------------------------------------------------------------------
void TTDoublePoint::toggleBottom ()
{
  if (mWishedBottomPosition == kStraightPosition) {
    mWishedBottomPosition = kDiagonalPosition ;
  }
  else if (mWishedBottomPosition == kDiagonalPosition) {
    mWishedBottomPosition = kStraightPosition ;
  }
  mBottomToggled = true ;
  mPendingWishedBottomPosition = true ;
  setNeedsDisplay () ;
  sendAction () ;
}

//-----------------------------------------------------------------------------
void TTDoublePoint::setStraight ()
{
  mWishedTopPosition = mWishedBottomPosition = kStraightPosition ;
  mPendingWishedTopPosition = mPendingWishedBottomPosition = true ;
  mTopToggled = true ;
  mBottomToggled = true ;
  setNeedsDisplay () ;
  sendAction () ;
}

//-----------------------------------------------------------------------------
void TTDoublePoint::setDiagonal ()
{
  mWishedTopPosition = mWishedBottomPosition = kDiagonalPosition ;
  mPendingWishedTopPosition = mPendingWishedBottomPosition = true ;
  mTopToggled = true ;
  mBottomToggled = true ;
  setNeedsDisplay () ;
  sendAction () ;
}

//=============================================================================
TTSlip::TTSlip (const AWPoint & inOrigin,
                const int  inKind,
                const bool inWay,
                const bool inIsReverted) :
TTTrack ( inOrigin, 3, 3, inIsReverted ),
mKind (inKind),
mWay (inWay)
{
}

//-----------------------------------------------------------------------------
void TTSlip::drawInRegion ( const AWRegion & inDrawRegion ) const
{
  AWRect trackRect = absoluteFrame() ;
  AWContext::setColor ( AWColor::lightGray() ) ;
  AWRect touchableZone = trackRect ;
  touchableZone.size.width /= 2 ;
  touchableZone.frameRoundRectInRegion (4, inDrawRegion) ;
  touchableZone.translateBy(touchableZone.size.width, 0) ;
  touchableZone.frameRoundRectInRegion (4, inDrawRegion) ;
#ifdef DEBUG_TRACK
  drawFrame( inDrawRegion ) ;
#endif
  AWContext::setColor( activeTrackColor ) ;
  trackRect.inset(0, 20) ;
  trackRect.fillRectInRegion(inDrawRegion) ;
  AWRect r = absoluteFrame() ;
  AWPoint origin = r.origin ;
  r.size.height = TILE_PIXEL_GRID / 3 + 1 ;
  int yOffset ;
  if (mWay == kLeftHand) {
    r.origin.y += TILE_PIXEL_GRID / 3 ;
    yOffset = 1 ;
  }
  else {
    r.origin.y += 2 * TILE_PIXEL_GRID + TILE_PIXEL_GRID / 3 - 1;
    yOffset = -1 ;
  }
  for (int x = 0; x < TILE_PIXEL_GRID * 3 ; x += 3) {
    r.size.width = 2 ;
    r.fillRectInRegion (inDrawRegion) ;
    r.origin.x += 2 ;
    r.origin.y += yOffset ;
    r.size.width = 1 ;
    r.fillRectInRegion (inDrawRegion) ;
    r.origin.x += 1 ;
    r.origin.y += yOffset ;
  }
  AWContext::setColor( AWColor::black() ) ;
  AWPoint p1 (15, 10);  p1.translateBy (origin) ;
  AWPoint p2 (24, 16); p2.translateBy (origin) ;
  AWPoint p3 (34, 16); p3.translateBy (origin) ;
  AWPoint p4 (10, 28);  p4.translateBy (origin) ;
  AWPoint p5 (21, 28); p5.translateBy (origin) ;
  AWPoint p6 (30, 34); p6.translateBy (origin) ;

  if (isReverted ()) {
    if (mKind & kSlipLeft) {
      p1.strokeLineInRegion (p2, inDrawRegion) ;
      p2.strokeLineInRegion (p3, inDrawRegion) ;
    }
    if (mKind & kSlipRight) {
      p4.strokeLineInRegion (p5, inDrawRegion) ;
      p5.strokeLineInRegion (p6, inDrawRegion) ;
    }
  }
  else {
    if (mKind & kSlipLeft) {
      p4.strokeLineInRegion (p5, inDrawRegion) ;
      p5.strokeLineInRegion (p6, inDrawRegion) ;
    }
    if (mKind & kSlipRight) {
      p1.strokeLineInRegion (p2, inDrawRegion) ;
      p2.strokeLineInRegion (p3, inDrawRegion) ;
    }
  }
}

//=============================================================================
TTDiag::TTDiag (const AWPoint & inOrigin,
                const AWInt inSize,
                const bool inWay,
                const bool inIsReverted) :
  TTTrack (computeTileRelativeFrameWithMargin(inOrigin, inSize, inSize), inIsReverted),
  mWay (inWay)
{
}

//-----------------------------------------------------------------------------
void TTDiag::drawInRegion ( const AWRegion & inDrawRegion ) const
{
  AWRect r = absoluteFrame() ;
#ifdef DEBUG_TRACK
  drawFrame( inDrawRegion ) ;
#endif
  AWContext::setColor (activeTrackColor) ;
  int count = r.size.width ;
  int yOffset ;
  if (mWay == kLeftHand) {
    r.origin.y -= DIAGONAL_TRACK_WIDTH / 2 ;
    yOffset = 1 ;
  }
  else {
    r.origin.y += r.size.height - DIAGONAL_TRACK_WIDTH / 2 - 1 ;
    yOffset = -1 ;
  }
  r.size.height = DIAGONAL_TRACK_WIDTH ;
  r.size.width = 1 ;
  for (int x = 0; x < count ; x++) {
    r.fillRectInRegion (inDrawRegion) ;
    r.origin.x += 1 ;
    r.origin.y += yOffset ;
  }
}

//=============================================================================
TTArrow::TTArrow (const AWPoint & inOrigin,
                  const bool inIsReverted) :
TTTrack ( inOrigin, 1, 1, inIsReverted )
{
}

//-----------------------------------------------------------------------------
void TTArrow::drawInRegion ( const AWRegion & inDrawRegion ) const
{
  AWRect r = absoluteFrame() ;
//  AWContext::setColor ( AWColor::white() ) ;
//  r.fillRectInRegion (inDrawRegion) ;
#ifdef DEBUG_TRACK
  drawFrame( inDrawRegion ) ;
#endif
  AWContext::setColor( activeTrackColor ) ;
  int xOffset;
  r.inset(0, 2) ;
  if (isReverted()) {
    xOffset = -1;
    r.origin.x += TILE_PIXEL_GRID - 4 ;
  }
  else {
    xOffset = 1;
    r.origin.x += 3 ;
  }
  for (int x = 0; x < TILE_PIXEL_GRID / 2 ; x++) {
    r.size.width = 1 ;
    r.fillRectInRegion (inDrawRegion) ;
    r.origin.x += xOffset ;
    r.origin.y += 1 ;
    r.size.height -= 2 ;
  }
  r = absoluteFrame ();
  r.origin.y += (TILE_PIXEL_GRID - STRAIGHT_TRACK_WIDTH) / 2 ;
  r.size.height = STRAIGHT_TRACK_WIDTH ;
  r.size.width = 3;
  if (isReverted()) r.origin.x += TILE_PIXEL_GRID - 3;
  r.fillRectInRegion (inDrawRegion) ;
}

//=============================================================================
TTBuffer::TTBuffer (const AWPoint & inOrigin,
                  const bool inIsReverted) :
TTTrack (inOrigin, 1, 1, inIsReverted)
{
}

//-----------------------------------------------------------------------------
void TTBuffer::drawInRegion ( const AWRegion & inDrawRegion ) const
{
  AWRect r = absoluteFrame () ;
#ifdef DEBUG_TRACK
  drawFrame (inDrawRegion) ;
#endif
  AWContext::setColor( activeTrackColor ) ;
  r.inset(0, 3);
  r.size.width = 5;
  r.origin.x += isReverted () ? TILE_PIXEL_GRID - 5 : 0;
  r.fillRectInRegion (inDrawRegion) ;
  r.origin.x += isReverted () ? 3 : 0;
  r.origin.y += 2;
  r.size.width = 2;
  r.size.height -= 4;
  AWContext::setColor (TTBackColor) ;
  r.fillRectInRegion (inDrawRegion) ;
}

//=============================================================================
static AWRect computeDecouplerRelativeFrame(const AWPoint & inOrigin)
{
  AWRect r ( AWPoint (inOrigin.x * TILE_PIXEL_GRID - TILE_PIXEL_GRID / 4,
                      inOrigin.y * TILE_PIXEL_GRID - TILE_PIXEL_GRID / 4),
            AWSize (TILE_PIXEL_GRID + TILE_PIXEL_GRID / 2, TILE_PIXEL_GRID + TILE_PIXEL_GRID / 2) ) ;
  return r ;
}

//=============================================================================
TTDecoupler::TTDecoupler (const AWPoint & inOrigin, const uint8_t inDecouplerId) :
TTTouchableTrack (computeDecouplerRelativeFrame (inOrigin), false),
mPosition (false),
mDecouplerId (inDecouplerId)
{
  registerDecouplerId (inDecouplerId) ;
}

//-----------------------------------------------------------------------------
void TTDecoupler::registerDecouplerId (const uint8_t inDecouplerId)
{
  if (inDecouplerId < kTTMaxNumberOfDecouplers) {
    sDecouplerForIdentifier[inDecouplerId] = this ;
  }
}

//-----------------------------------------------------------------------------
TTDecoupler *TTDecoupler::sDecouplerForIdentifier[kTTMaxNumberOfDecouplers] = { NULL } ;

//-----------------------------------------------------------------------------
bool TTDecoupler::positionOfDecoupler (const uint8_t inDecouplerId)
{
  if (inDecouplerId < kTTMaxNumberOfDecouplers && sDecouplerForIdentifier[inDecouplerId] != NULL) {
    return sDecouplerForIdentifier[inDecouplerId]->position () ;
  }
  else {
    return false ;
  }
}

//-----------------------------------------------------------------------------
void TTDecoupler::setActionOfDecoupler (const uint8_t inDecouplerId,
                                        AWAction inAction)
{
  if (inDecouplerId < kTTMaxNumberOfDecouplers && sDecouplerForIdentifier[inDecouplerId] != NULL) {
    sDecouplerForIdentifier[inDecouplerId]->setAction (inAction) ;
  }
}

//-----------------------------------------------------------------------------
void TTDecoupler::setActionOfAllDecouplers (AWAction inAction)
{
  for (uint8_t i = 0 ; i < kTTMaxNumberOfDecouplers ; i++) {
    setActionOfDecoupler (i, inAction) ;
  }
}

//-----------------------------------------------------------------------------
void TTDecoupler::drawInRegion (const AWRegion & inDrawRegion) const
{
  AWRect r = absoluteFrame () ;
  AWContext::setColor (feedback () ? feedbackColor : buttonBackColor) ;
  r.fillRoundRectInRegion (4, inDrawRegion) ;
  AWContext::setColor( buttonFrameColor ) ;
  r.frameRoundRectInRegion (4, inDrawRegion) ;
#ifdef DEBUG_TRACK
  drawFrame (inDrawRegion) ;
#endif

  AWContext::setColor (activeTrackColor);
  r.size.height = STRAIGHT_TRACK_WIDTH ;
  r.origin.y += TILE_PIXEL_GRID / 2 + TILE_PIXEL_GRID / 4 - STRAIGHT_TRACK_WIDTH / 2 ;
  r.fillRectInRegion (inDrawRegion) ;

  r = absoluteFrame() ;
  AWContext::setColor (feedback () ? feedbackColor : buttonBackColor);
  r.origin.x += r.size.width / 2 - STRAIGHT_TRACK_WIDTH -  STRAIGHT_TRACK_WIDTH / 2 ;
  r.size.width = 3 * STRAIGHT_TRACK_WIDTH ;
  r.origin.y += r.size.height / 2 - STRAIGHT_TRACK_WIDTH -  STRAIGHT_TRACK_WIDTH / 2 ;
  r.size.height = 3 * STRAIGHT_TRACK_WIDTH ;
  r.fillOvalInRegion (inDrawRegion) ;

  AWContext::setColor (mPosition ? decoupleColor : activeTrackColor) ;
  r.inset (2,2);
  r.fillOvalInRegion (inDrawRegion) ;

#ifdef TRACK_TAG
  r = absoluteFrame() ;
  AWContext::setColor(awkTextColor) ;
  String text (mDecouplerId) ;
  r.origin.x += 3 ;
  r.origin.y += 3 ;
  kTTTagFont.drawStringInRegion(r.origin.x, r.origin.y, text, inDrawRegion) ;
#endif
}

//-----------------------------------------------------------------------------
void TTDecoupler::toggle ()
{
  mPosition = ! mPosition ;
  setNeedsDisplay () ;
  sendAction () ;
}

