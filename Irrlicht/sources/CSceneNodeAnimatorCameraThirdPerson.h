#pragma once

#include"ISceneNodeAnimator.h"

namespace irr::gui
{
	class ICursorControl;
}

namespace irr::scene {
	class CSceneNodeAnimatorCameraThirdPerson: public ISceneNodeAnimator
	{
	public:
		CSceneNodeAnimatorCameraThirdPerson(boost::shared_ptr<irr::gui::ICursorControl> cursorControl);
		~CSceneNodeAnimatorCameraThirdPerson();

		bool isEventReceiverEnabled() const override;

		bool OnEvent(const irr::SEvent& event) override;
		void animateNode(boost::shared_ptr<irr::scene::ISceneNode> node, irr::u32 timeMs) override;
		boost::shared_ptr<irr::scene::ISceneNodeAnimator> createClone(boost::shared_ptr<irr::scene::ISceneNode> node,
			boost::shared_ptr<irr::scene::ISceneManager> newManager) override;
	private:
		void updateTargetPositionData();

		void setFi(float offset);
		float getFi();

		void setTeta(float offset);
		float getTeta();

		void setRadius(short offset);
		float getRadius();

		boost::shared_ptr<irr::gui::ICursorControl> _cursorControl;

		float _fi, _teta, _radius;
		irr::core::vector3df _targetPosition;
	};
}