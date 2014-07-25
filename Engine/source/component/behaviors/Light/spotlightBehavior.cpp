//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------
#include "platform/platform.h"
#include "console/consoleTypes.h"
#include "component/behaviors/Light/spotLightBehavior.h"
#include "core/util/safeDelete.h"
#include "console/consoleTypes.h"
#include "console/consoleObject.h"
#include "core/stream/bitStream.h"
#include "console/engineAPI.h"
#include "sim/netConnection.h"
#include "gfx/gfxDrawUtil.h"

//////////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////////
SpotLightBehavior::SpotLightBehavior()
{
   mNetFlags.set(Ghostable | ScopeAlways);
}

SpotLightBehavior::~SpotLightBehavior()
{
   for(S32 i = 0;i < mFields.size();++i)
   {
      BehaviorField &field = mFields[i];
      SAFE_DELETE_ARRAY(field.mFieldDescription);
   }

   SAFE_DELETE_ARRAY(mDescription);
}

IMPLEMENT_CO_NETOBJECT_V1(SpotLightBehavior);

//////////////////////////////////////////////////////////////////////////
BehaviorInstance *SpotLightBehavior::createInstance()
{
   SpotLightBehaviorInstance *instance = new SpotLightBehaviorInstance(this);

   setupFields( instance );

   if(instance->registerObject())
      return instance;

   delete instance;
   return NULL;
}

bool SpotLightBehavior::onAdd()
{
   if(! Parent::onAdd())
      return false;

   return true;
}

void SpotLightBehavior::onRemove()
{
   Parent::onRemove();
}
void SpotLightBehavior::initPersistFields()
{
   Parent::initPersistFields();
}

U32 SpotLightBehavior::packUpdate(NetConnection *con, U32 mask, BitStream *stream)
{
   U32 retMask = Parent::packUpdate(con, mask, stream);
   return retMask;
}

void SpotLightBehavior::unpackUpdate(NetConnection *con, BitStream *stream)
{
   Parent::unpackUpdate(con, stream);
}

//==========================================================================================
//==========================================================================================
SpotLightBehaviorInstance::SpotLightBehaviorInstance( BehaviorTemplate *btemplate ) 
{
   mRange = 10.0f;
   mInnerConeAngle = 40.0f;
   mOuterConeAngle = 45.0f;

   mLight->setType( LightInfo::Spot );

   mTemplate = btemplate;
   mBehaviorOwner = NULL;

   mNetFlags.set(Ghostable);
}

SpotLightBehaviorInstance::~SpotLightBehaviorInstance()
{
}
IMPLEMENT_CO_NETOBJECT_V1(SpotLightBehaviorInstance);

bool SpotLightBehaviorInstance::onAdd()
{
   if(! Parent::onAdd())
      return false;

   return true;
}

void SpotLightBehaviorInstance::onRemove()
{
   Parent::onRemove();
}

//This is mostly a catch for situations where the behavior is re-added to the object and the like and we may need to force an update to the behavior
void SpotLightBehaviorInstance::onBehaviorAdd()
{
   Parent::onBehaviorAdd();
}

void SpotLightBehaviorInstance::onBehaviorRemove()
{
   Parent::onBehaviorRemove();
}


void SpotLightBehaviorInstance::initPersistFields()
{
   addGroup( "Light" );

   addField( "range", TypeF32, Offset( mRange, SpotLightBehaviorInstance ) );
   addField( "innerAngle", TypeF32, Offset( mInnerConeAngle, SpotLightBehaviorInstance ) );
   addField( "outerAngle", TypeF32, Offset( mOuterConeAngle, SpotLightBehaviorInstance ) );

   endGroup( "Light" );

   // We do the parent fields at the end so that
   // they show up that way in the inspector.
   Parent::initPersistFields();

   // Remove the scale field... it's already 
   // defined by the range and angle.
   removeField( "scale" );
}

U32 SpotLightBehaviorInstance::packUpdate(NetConnection *con, U32 mask, BitStream *stream)
{
   if ( stream->writeFlag( mask & UpdateMask ) )
   {
      stream->write( mRange );
      stream->write( mInnerConeAngle );
      stream->write( mOuterConeAngle );
   }

   return Parent::packUpdate( con, mask, stream );
}

void SpotLightBehaviorInstance::unpackUpdate(NetConnection *con, BitStream *stream)
{
   if ( stream->readFlag() ) // UpdateMask
   {   
      stream->read( &mRange );
      stream->read( &mInnerConeAngle );
      stream->read( &mOuterConeAngle );
   }

   Parent::unpackUpdate( con, stream );
}
void SpotLightBehaviorInstance::setScale( const VectorF &scale )
{
   // The y coord is the spotlight range.
   mRange = getMax( scale.y, 0.05f );

   // Use the average of the x and z to get a radius.  This
   // is the best method i've found to make the manipulation
   // from the WorldEditor gizmo to feel right.
   F32 radius = mClampF( ( scale.x + scale.z ) * 0.5f, 0.05f, mRange );
   mOuterConeAngle = mRadToDeg( mAsin( radius / mRange ) ) * 2.0f;

   // Make sure the inner angle is less than the outer.
   //
   // TODO: Maybe we should make the inner angle a scale
   // and not an absolute angle?
   //
   mInnerConeAngle = getMin( mInnerConeAngle, mOuterConeAngle );

   // We changed a bunch of our settings 
   // so notify the client.
   setMaskBits( UpdateMask );

   // Let the parent do the final scale.
   //TODO: Parent::setScale( VectorF( radius, mRange, radius ) );
}
void SpotLightBehaviorInstance::_conformLights()
{
   /*mLight->setTransform( mBehaviorOwner->getTransform() );

   mRange = getMax( mRange, 0.05f );
   mLight->setRange( mRange );

   mLight->setColor( mColor );
   mLight->setBrightness( mBrightness );
   mLight->setCastShadows( mCastShadows );
   mLight->setPriority( mPriority );

   mOuterConeAngle = getMax( 0.01f, mOuterConeAngle );
   mInnerConeAngle = getMin( mInnerConeAngle, mOuterConeAngle );

   mLight->setInnerConeAngle( mInnerConeAngle );
   mLight->setOuterConeAngle( mOuterConeAngle );

   // Update the bounds and scale to fit our spotlight.
   F32 radius = mRange * mSin( mDegToRad( mOuterConeAngle ) * 0.5f );
   mObjBox.minExtents.set( -1, 0, -1 );
   mObjBox.maxExtents.set( 1, 1, 1 );
   mObjScale.set( radius, mRange, radius );

   // Skip our transform... it just dirties mask bits.
   Parent::setTransform( mObjToWorld );*/
}
void SpotLightBehaviorInstance::_renderViz( SceneRenderState *state )
{
   GFXDrawUtil *draw = GFX->getDrawUtil();

   GFXStateBlockDesc desc;
   desc.setZReadWrite( true, false );
   desc.setCullMode( GFXCullNone );
   desc.setBlend( true );

   // Base the color on the light color.
   ColorI color( mColor );
   color.alpha = 16;

   F32 radius = mRange * mSin( mDegToRad( mOuterConeAngle * 0.5f ) );
   draw->drawCone(   desc, 
      mBehaviorOwner->getPosition() + ( mBehaviorOwner->getTransform().getForwardVector() * mRange ),
      mBehaviorOwner->getPosition(),
      radius,
      color );
}