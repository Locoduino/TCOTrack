//
//  TTTrack.h
//  CocoaSimulator
//
//  Created by Jean-Luc Béchennec on 20/07/2017.
//  Copyright © 2017 Pierre Molinaro. All rights reserved.
//

#ifndef TTTrack_h
#define TTTrack_h

#include "AWView.h"

typedef enum { kMiddle, kTopDiagonal, kBottomDiagonal } BlockInOutShape ;
typedef enum { kStraightPosition, kMiddlePosition, kDiagonalPosition, kNoPosition } TTPointPosition ;
typedef enum { kCanicheUnknownPosition, kCanicheRightPosition, kCanicheMiddlePosition, kCanicheLeftPosition } TTCanichePointPosition ;
typedef enum { kNormal, kLarge, kLargeNoSync } TTDoublePointSize ;
  
static const bool kForward = false;
static const bool kBackward = true;
static const bool kLeftHand = false;
static const bool kRightHand = true;
static const bool kStraight = false ;
static const bool kDiagonal = true ;
static const bool kTop = false;
static const bool kBottom = true;
static const bool kSlipLeft = 0x1 << 0;
static const bool kSlipRight = 0x1 << 1;

//-----------------------------------------------------------------------------

AWRect computeTileRelativeFrame(const AWPoint & inOrigin,
                                const AWInt inWidth,
                                const AWInt inHeight) ;

//-----------------------------------------------------------------------------
class TTView : public AWView {
  
  public : TTView (const AWPoint & inOrigin, const AWInt inSizeX, const AWInt inSizeY, const String & inTitle = "") ;
  
  private : String mTitle ;
  
  //--- Tell the view is opaque or not
  public : virtual bool isOpaque (void) const ;
  
  public : virtual void drawInRegion ( const AWRegion & inDrawRegion ) const ;
};

//-----------------------------------------------------------------------------
class TTTile : public AWView {
  public : TTTile (const AWPoint & inLocation) ;
  public : TTTile (const AWPoint & inOrigin,
                   const AWInt inWidth,
                   const AWInt inHeight) ;
  public : TTTile (const AWRect & inFrame) ;
};

//-----------------------------------------------------------------------------
class TTTrack : public TTTile {
  public : TTTrack (const AWPoint & inOrigin,
                    const AWInt inWidth,
                    const AWInt inHeight,
                    const bool inIsReverted) ;
  
  public : TTTrack (const AWRect & inRect,
                    const bool inIsReverted) ;
  
  //--- Direction
  private : bool mIsReverted ;
  public : bool isReverted() const { return mIsReverted; }
  private : bool mDirection ;
  public : bool direction ()  const { return mDirection ;  }
  
  //--- Tell the view is opaque or not
  public : virtual bool isOpaque (void) const ;

  //--- Additionnal drawing fo Debug
  protected : void drawFrame ( const AWRegion & inDrawRegion ) const ;
  
};

//-----------------------------------------------------------------------------
class TTBlock : public TTTrack {
  public : TTBlock (const AWPoint & inOrigin,
                    const AWInt inLength,
                    const BlockInOutShape inInShape = kMiddle,
                    const BlockInOutShape inOutShape = kMiddle,
                    const bool inIsReverted = false) ;
  
  private : BlockInOutShape mInShape ;
  private : BlockInOutShape mOutShape ;
  private : bool mDashed ;
  public : void setDashed (const bool inDashed) { mDashed = inDashed ; setNeedsDisplay () ; }
  
  public : virtual void drawInRegion ( const AWRegion & inDrawRegion ) const ;
};

//-----------------------------------------------------------------------------
class TTTouchableTrack : public TTTrack {
  public : TTTouchableTrack (const AWPoint & inOrigin,
                             const AWInt inWidth,
                             const AWInt inHeight,
                             const bool inIsReverted) ;
  public : TTTouchableTrack (const AWRect & inRect,
                             const bool inIsReverted) ;
  
  private : bool mFeedback ;
  public : bool feedback() const { return mFeedback; }
  protected : void setFeedback( const bool inFeedback ) { mFeedback = inFeedback ; }

  public : virtual void touchDown (const AWPoint & inPoint) ;
  public : virtual void touchMove (const AWPoint & inPoint) ;
  public : virtual void touchUp (const AWPoint & inPoint) ;
  
  protected : virtual void toggle() ;
};

//-----------------------------------------------------------------------------
static const int kTTMaxNumberOfPoints = 64 ;

//-----------------------------------------------------------------------------
class TTAbstractPoint : public TTTouchableTrack
{
  public : TTAbstractPoint (const AWPoint & inOrigin,
                            const AWInt inWidth,
                            const AWInt inHeight,
                            const bool inIsReverted) ;
  public : TTAbstractPoint (const AWRect & inRect,
                            const bool inIsReverted) ;
  
  //--- static data and methods to handle the collection of points
  private : static TTAbstractPoint * sPointForIdentifier[kTTMaxNumberOfPoints] ;
  public : static void setActualPositionOfPoint (const uint8_t inPointId,
                                                 const TTPointPosition inPosition) ;
  public : static void setWishedPositionOfPoint (const uint8_t inPointId,
                                                 const TTPointPosition inPosition) ;
  public : static TTPointPosition actualPositionOfPoint (const uint8_t inPointId) ;
  public : static TTPointPosition wishedPositionOfPoint (const uint8_t inPointId) ;
  
  public : static void setCanicheActualPositionOfPoint(const uint8_t inPointId,
                                                       const TTCanichePointPosition inPosition) ;
  public : static void setCanicheWishedPositionOfPoint(const uint8_t inPointId,
                                                       const TTCanichePointPosition inPosition) ;
  public : static TTCanichePointPosition wishedCanichePositionOfPoint (const uint8_t inPointId) ;
  
  public : static void setActionOfPoint (const uint8_t inPointId, AWAction inAction) ;
  public : static void setActionOfAllPoints (AWAction inAction) ;
  
  //--- Register
  protected : void registerPointId (const uint8_t inPointId) ;
  
  //--- Caniche to TT position and TT to Caniche position
  private : virtual TTCanichePointPosition pointToCanichePosition (TTPointPosition inPos) = 0 ;
  private : virtual TTPointPosition canicheToPointPosition (TTCanichePointPosition inPos) = 0 ;
  
  //--- Pure virtual methods for points interface
  private : virtual void setActualPosition (const uint8_t inPointId,
                                           const TTPointPosition inPosition) = 0 ;
  private : virtual void setWishedPosition (const uint8_t inPointId,
                                           const TTPointPosition inPosition) = 0 ;
  private : virtual TTPointPosition actualPosition (const uint8_t inPointId) = 0 ;
  private : virtual TTPointPosition wishedPosition (const uint8_t inPointId) = 0 ;
  public : virtual TTPointPosition pendingWish (uint8_t & outPointId) = 0 ;
  public : TTCanichePointPosition pendingCanicheWish (uint8_t & outPointId) ;
};

//-----------------------------------------------------------------------------
class TTDoublePoint : public TTAbstractPoint {
  public : TTDoublePoint (const AWPoint & inOrigin,
                          const uint8_t inTopPointId,
                          const uint8_t inBottomPointId,
                          const bool inWay,
                          const TTDoublePointSize inSize = kNormal,
                          const bool inIsReverted = false) ;
  
  private : uint8_t mTopPointId ;
  private : uint8_t mBottomPointId ;
  private : bool mWay ;
  private : TTDoublePointSize mSize ;
  private : TTPointPosition mWishedTopPosition ;
  private : TTPointPosition mWishedBottomPosition ;
  private : TTPointPosition mActualTopPosition ;
  private : TTPointPosition mActualBottomPosition ;
  private : bool mPendingWishedTopPosition ;
  private : bool mPendingWishedBottomPosition ;

  //--- Virtual methods for points interface
  private : virtual void setActualPosition (const uint8_t inPointId,
                                           const TTPointPosition inPosition) ;
  private : virtual void setWishedPosition (const uint8_t inPointId,
                                           const TTPointPosition inPosition) ;
  private : virtual TTPointPosition actualPosition (const uint8_t inPointId) ;
  private : virtual TTPointPosition wishedPosition (const uint8_t inPointId) ;
  public : virtual TTPointPosition pendingWish (uint8_t & outPointId) ;
  
  //--- Caniche to TT position and TT to Caniche position
  private : virtual TTCanichePointPosition pointToCanichePosition (TTPointPosition inPos) ;
  private : virtual TTPointPosition canicheToPointPosition (TTCanichePointPosition inPos) ;
  
  //--- Drawing
  private : void drawTopStraight (const AWRegion & inDrawRegion,
                                  const AWColor inStraightColor ) const ;
  private : void drawBottomStraight (const AWRegion & inDrawRegion,
                                     const AWColor inStraightColor ) const ;
  private : void drawTopDiagonal (const AWRegion & inDrawRegion,
                                  const AWColor inDiagonalColor ) const ;
  private : void drawBottomDiagonal (const AWRegion & inDrawRegion,
                                     const AWColor inDiagonalColor ) const ;
  public : virtual void drawInRegion ( const AWRegion & inDrawRegion ) const ;
  
  private : void drawStraightLogo (AWRect &inButton, const AWRegion & inDrawRegion) const ;
  private : void drawDiagonalLogo (AWRect &inButton, const AWRegion & inDrawRegion) const ;

  //--- Touch
  private : uint8_t mTouchLocation ; // 0, 1, 2 or 3
  private : AWRect mFeedbackRect ;
  private : bool mTouchInProgress ;
  
  public : virtual void touchDown (const AWPoint & inPoint) ;
  public : virtual void touchMove (const AWPoint & inPoint) ;
  public : virtual void touchUp (const AWPoint & inPoint) ;
  
  //--- Internal toggle methods and state attributes
  private : bool mTopToggled ;
  private : bool mBottomToggled ;
  
  private : void toggleTop () ;
  private : void toggleBottom () ;
  private : void setStraight () ;
  private : void setDiagonal () ;
  protected : virtual void toggle () ;
  
};

//-----------------------------------------------------------------------------
class TTPoint : public TTAbstractPoint {
  public : TTPoint (const AWPoint & inOrigin,
                    const uint8_t inPointId,
                    const bool inWay,
                    const bool inInDiagonal,
                    const bool inoutDiagonal,
                    const bool inIsReverted = false) ;
  
  private : uint8_t mPointId ;
  private : bool mWay ;
  private : bool mInDiagonal ;
  private : bool mOutDiagonal ;
  private : TTPointPosition mWishedPosition ;
  private : TTPointPosition mActualPosition ;
  private : bool mPendingWishedPosition ;
  
  //--- Virtual methods for points interface
  private : virtual void setActualPosition (const uint8_t inPointId,
                                           const TTPointPosition inPosition) ;
  private : virtual void setWishedPosition (const uint8_t inPointId,
                                           const TTPointPosition inPosition) ;
  private : virtual TTPointPosition actualPosition (const uint8_t inPointId) ;
  private : virtual TTPointPosition wishedPosition (const uint8_t inPointId) ;
  public : virtual TTPointPosition pendingWish (uint8_t & outPointId) ;

  //--- Caniche to TT position and TT to Caniche position
  private : virtual TTCanichePointPosition pointToCanichePosition (TTPointPosition inPos) ;
  private : virtual TTPointPosition canicheToPointPosition (TTCanichePointPosition inPos) ;
  
  //--- Drawing
  private : void drawStraight (const AWRegion & inDrawRegion,
                               const AWColor inStraightColor ) const ;
  private : void drawDiagonal (const AWRegion & inDrawRegion,
                               const AWColor inDiagonalColor ) const ;
  public : virtual void drawInRegion ( const AWRegion & inDrawRegion ) const ;

  //--- Internal toggle methods and state attributes
  private : bool mToggled ;
  
  protected : virtual void toggle() ;
};

//-----------------------------------------------------------------------------
class TTSlip : public TTTrack {
  public : TTSlip (const AWPoint & inOrigin,
                   const int  inKind,
                   const bool inWay = kLeftHand,
                   const bool inIsReverted = false) ;
  
  private : bool mKind ;
  private : bool mWay ;
  
  public : virtual void drawInRegion ( const AWRegion & inDrawRegion ) const ;
};

//-----------------------------------------------------------------------------
class TTDiag : public TTTrack {
  public : TTDiag (const AWPoint & inOrigin,
                   const AWInt inSize,
                   const bool inWay = kLeftHand,
                   const bool inIsReverted = false) ;
  
  private : bool mWay ;
  
  public : virtual void drawInRegion ( const AWRegion & inDrawRegion ) const ;
};

//-----------------------------------------------------------------------------
class TTArrow : public TTTrack {
  public : TTArrow (const AWPoint & inOrigin,
                    const bool inIsReverted = false) ;
  
  public : virtual void drawInRegion ( const AWRegion & inDrawRegion ) const ;
};

//-----------------------------------------------------------------------------
class TTBuffer : public TTTrack {
  public : TTBuffer (const AWPoint & inOrigin,
                     const bool inIsReverted = false) ;
  
  public : virtual void drawInRegion ( const AWRegion & inDrawRegion ) const ;
};

//-----------------------------------------------------------------------------
static const int kTTMaxNumberOfDecouplers = 64 ;

//-----------------------------------------------------------------------------
class TTDecoupler : public TTTouchableTrack {
  public : TTDecoupler (const AWPoint & inOrigin, const uint8_t inDecouplerId) ;
  
  private : bool mPosition ;
  private : uint8_t mDecouplerId ;
  public : bool position () const { return mPosition ; }
  public : uint8_t identifier () const { return mDecouplerId ; }
  
  //--- static data and methods to handle the collection of points
  private : static TTDecoupler *sDecouplerForIdentifier[kTTMaxNumberOfDecouplers] ;
  public : static bool positionOfDecoupler (const uint8_t inDecouplerId) ;
  public : static void setActionOfDecoupler (const uint8_t inDecouplerId, AWAction inAction) ;
  public : static void setActionOfAllDecouplers (AWAction inAction) ;
  
  //--- Register
  protected : void registerDecouplerId (const uint8_t inDecouplerId) ;
  
  //--- Draw
  public : virtual void drawInRegion ( const AWRegion & inDrawRegion ) const ;

  protected : virtual void toggle() ;
};

//-----------------------------------------------------------------------------
//class TTDecouplerButton

#endif /* TTTrack_h */
