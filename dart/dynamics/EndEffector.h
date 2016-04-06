/*
 * Copyright (c) 2015, Georgia Tech Research Corporation
 * All rights reserved.
 *
 * Author(s): Michael X. Grey <mxgrey@gatech.edu>
 *
 * Georgia Tech Graphics Lab and Humanoid Robotics Lab
 *
 * Directed by Prof. C. Karen Liu and Prof. Mike Stilman
 * <karenliu@cc.gatech.edu> <mstilman@cc.gatech.edu>
 *
 * This file is provided under the following "BSD-style" License:
 *   Redistribution and use in source and binary forms, with or
 *   without modification, are permitted provided that the following
 *   conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *   USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *   POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DART_DYNAMICS_ENDEFFECTOR_H_
#define DART_DYNAMICS_ENDEFFECTOR_H_

#include "dart/common/Aspect.h"
#include "dart/common/SpecializedForAspect.h"
#include "dart/common/AspectWithVersion.h"
#include "dart/dynamics/FixedJacobianNode.h"
#include "dart/dynamics/CompositeNode.h"

namespace dart {
namespace dynamics {

class BodyNode;
class Skeleton;
class EndEffector;
class Support;

namespace detail {

//==============================================================================
struct SupportStateData
{
  inline SupportStateData(bool active = false) : mActive(active) { }

  bool mActive;
};

//==============================================================================
struct SupportPropertiesData
{
  math::SupportGeometry mGeometry;
};

void SupportUpdate(Support* support);

using EndEffectorCompositeBase = CompositeNode<
    common::CompositeJoiner<
        FixedJacobianNode,
        common::SpecializedForAspect<Support>
    >
>;

} // namespace detail

//==============================================================================
class Support final :
    public common::AspectWithStateAndVersionedProperties<
        Support,
        detail::SupportStateData,
        detail::SupportPropertiesData,
        EndEffector,
        &detail::SupportUpdate>
{
public:

//  DART_COMMON_ASPECT_STATE_PROPERTY_CONSTRUCTORS( Support, &detail::SupportUpdate, &detail::SupportUpdate )
  DART_COMMON_ASPECT_STATE_PROPERTY_CONSTRUCTORS(Support)

  /// Set/Get the support geometry for this EndEffector. The SupportGeometry
  /// represents points in the EndEffector frame that can be used for contact
  /// when solving balancing or manipulation constraints.
  DART_COMMON_SET_GET_ASPECT_PROPERTY(math::SupportGeometry, Geometry)
  // void setGeometry(const math::SupportGeometry&);
  // const math::SupportGeometry& getGeometry() const;

  /// Pass in true if this EndEffector should be used to support the robot, like
  /// a foot
  void setActive(bool _supporting = true);

  /// Get whether this EndEffector is currently being used for support
  bool isActive() const;

};

//==============================================================================
class EndEffector final : public detail::EndEffectorCompositeBase
{
public:

  friend class Skeleton;
  friend class BodyNode;

  struct UniqueProperties
  {
    /// Name of this EndEffector
    std::string mName;

    /// The relative transform will be set to this whenever
    /// resetRelativeTransform() is called
    Eigen::Isometry3d mDefaultTransform;

    /// Default constructor
    UniqueProperties(
        const Eigen::Isometry3d& _defaultTransform =
            Eigen::Isometry3d::Identity());
  };

  struct PropertiesData : UniqueProperties
  {
    PropertiesData(
        const UniqueProperties& _effectorProperties = UniqueProperties(),
        const common::Composite::Properties& _compositeProperties =
            common::Composite::Properties());

    /// The properties of the EndEffector's Aspects
    common::Composite::Properties mCompositeProperties;
  };

  using Properties = Node::MakeProperties<PropertiesData>;

  /// Destructor
  virtual ~EndEffector() = default;

  //----------------------------------------------------------------------------
  /// \{ \name Structural Properties
  //----------------------------------------------------------------------------

  /// Set the Properties of this EndEffector. If _useNow is true, the current
  /// Transform will be set to the new default transform.
  void setProperties(const PropertiesData& _properties, bool _useNow=false);

  /// Set the Properties of this EndEffector. If _useNow is true, the current
  /// Transform will be set to the new default transform.
  void setProperties(const UniqueProperties& _properties, bool _useNow=false);

  /// Get the Properties of this EndEffector
  PropertiesData getEndEffectorProperties() const;

  /// Copy the State and Properties of another EndEffector
  void copy(const EndEffector& _otherEndEffector);

  /// Copy the State and Properties of another EndEffector
  void copy(const EndEffector* _otherEndEffector);

  /// Copy the State and Properties of another EndEffector
  EndEffector& operator=(const EndEffector& _otherEndEffector);

  /// Set the default relative transform of this EndEffector. The relative
  /// transform of this EndEffector will be set to _newDefaultTf the next time
  /// resetRelativeTransform() is called. If _useNow is set to true, then
  /// resetRelativeTransform() will be called at the end of this function.
  void setDefaultRelativeTransform(const Eigen::Isometry3d& _newDefaultTf,
                                   bool _useNow);

  /// Set the current relative transform of this EndEffector to the default
  /// relative transform of this EndEffector. The default relative transform can
  /// be set with setDefaultRelativeTransform()
  void resetRelativeTransform();

  DART_BAKE_SPECIALIZED_ASPECT(Support)

  //----------------------------------------------------------------------------
  /// \{ \name Notifications
  //----------------------------------------------------------------------------

  // Documentation inherited
  virtual void notifyTransformUpdate() override;

  /// \}

protected:

  /// Constructor used by the Skeleton class
  explicit EndEffector(BodyNode* parent, const PropertiesData& properties);

  /// Create a clone of this BodyNode. This may only be called by the Skeleton
  /// class.
  virtual Node* cloneNode(BodyNode* _parent) const override;


  /// Properties of this EndEffector
  DEPRECATED(6.0)
  UniqueProperties mEndEffectorP;
};

} // namespace dynamics
} // namespace dart


#endif // DART_DYNAMICS_ENDEFFECTOR_H_
