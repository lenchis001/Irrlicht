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
		CSceneNodeAnimatorCameraThirdPerson(irr::gui::ICursorControl* cursorControl);
		~CSceneNodeAnimatorCameraThirdPerson();

		bool isEventReceiverEnabled() const override;

		bool OnEvent(const irr::SEvent& event) override;
		void animateNode(irr::scene::ISceneNode* node, irr::u32 timeMs) override;
		irr::scene::ISceneNodeAnimator* createClone(irr::scene::ISceneNode* node,
			irr::scene::ISceneManager* newManager) override;
	private:
		void updateTargetPositionData();

		void setFi(float offset);
		float getFi();

		void setTeta(float offset);
		float getTeta();

		void setRadius(short offset);
		float getRadius();

		irr::gui::ICursorControl* _cursorControl;

		float _fi, _teta, _radius;
		irr::core::vector3df _targetPosition;
	};
}