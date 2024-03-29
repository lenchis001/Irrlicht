#pragma once

#include"vector2d.h"
#include"ISceneNodeAnimator.h"

namespace irr::scene {
	class CSceneNodeAnimatorCameraEditor: public ISceneNodeAnimator
	{
		enum MoveMode {
			NO_MOVE,
			FORWARD,
			BACKWARD
		};

	public:
		CSceneNodeAnimatorCameraEditor();
		~CSceneNodeAnimatorCameraEditor();

		bool isEventReceiverEnabled() const override;

		bool OnEvent(const SEvent& event) override;
		void animateNode(boost::shared_ptr<ISceneNode> node, u32 timeMs) override;
		boost::shared_ptr<ISceneNodeAnimator> createClone(boost::shared_ptr<ISceneNode> node,
			boost::shared_ptr<scene::ISceneManager> newManager) override;
	private:
		void _updateTargetPositionData();

		inline void setFi(float offset);
		inline void setTeta(float offset);

		float _fi, _teta, _radius;
		irr::core::vector2di _oldCursorPosition;
		irr::core::vector3df _targetPosition, _cameraPosition;

		MoveMode _moveMode;
	};
}
