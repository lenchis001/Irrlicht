#include "CSceneNodeAnimatorCameraEditor.h"

#include"ICameraSceneNode.h"

#define _USE_MATH_DEFINES
#include "math.h"

#define ROTATION_DELTA 0.005
#define TETA_OFFSET_BORDER 0.1

namespace irr::scene {
	void CSceneNodeAnimatorCameraEditor::_updateTargetPositionData()
	{
		_targetPosition.X = _radius * sin(_teta) * cos(_fi);
		_targetPosition.Z = _radius * sin(_teta) * sin(_fi);
		_targetPosition.Y = _radius * cos(_teta);

		_targetPosition += _cameraPosition;
	}

	void CSceneNodeAnimatorCameraEditor::setFi(float offset)
	{
		_fi -= offset;
	}

	void CSceneNodeAnimatorCameraEditor::setTeta(float offset)
	{
		float offsetTeta = _teta + offset;
		if (offsetTeta < (M_PI - TETA_OFFSET_BORDER) && offsetTeta >(0 + TETA_OFFSET_BORDER)) {
			_teta = offsetTeta;
		}
	}

	CSceneNodeAnimatorCameraEditor::CSceneNodeAnimatorCameraEditor()
	{
		_fi = 0;
		_teta = M_PI_2;
		_radius = 2;
		_moveMode = NO_MOVE;

		_updateTargetPositionData();
	}

	CSceneNodeAnimatorCameraEditor::~CSceneNodeAnimatorCameraEditor() {}

	bool CSceneNodeAnimatorCameraEditor::isEventReceiverEnabled() const { return true; }

	bool CSceneNodeAnimatorCameraEditor::OnEvent(const irr::SEvent& event)
	{
		if (event.EventType == irr::EEVENT_TYPE::EET_MOUSE_INPUT_EVENT) {
			switch (event.MouseInput.Event) {
			case irr::EMOUSE_INPUT_EVENT::EMIE_LMOUSE_PRESSED_DOWN:
				_oldCursorPosition = irr::core::vector2di(event.MouseInput.X, event.MouseInput.Y);
				break;
			case irr::EMOUSE_INPUT_EVENT::EMIE_MOUSE_MOVED:
				if (event.MouseInput.isLeftPressed()) {
					setFi((_oldCursorPosition.X - event.MouseInput.X) * ROTATION_DELTA);
					setTeta((_oldCursorPosition.Y - event.MouseInput.Y) * ROTATION_DELTA);

					_updateTargetPositionData();

					_oldCursorPosition = irr::core::vector2di(event.MouseInput.X, event.MouseInput.Y);
				}
				break;
			default:
				return false;
			}
		}
		else if (event.EventType == irr::EEVENT_TYPE::EET_KEY_INPUT_EVENT) {
			switch (event.KeyInput.Char) {
			case irr::EKEY_CODE::KEY_SHIFT: {
				if (event.KeyInput.PressedDown) {
					_radius *= 2;
				}
				else {
					_radius /= 2;
				}
				break;
			}
			case irr::EKEY_CODE::KEY_KEY_W: {
				if (event.KeyInput.PressedDown)
					_moveMode = FORWARD;
				else
					_moveMode = NO_MOVE;
				break;
			}
			case irr::EKEY_CODE::KEY_KEY_S: {
				if (event.KeyInput.PressedDown)
					_moveMode = BACKWARD;
				else
					_moveMode = NO_MOVE;
				break;
			}
			default:
				break;
			}
		}

		return true;
	}

	void CSceneNodeAnimatorCameraEditor::animateNode(ISceneNode* node, irr::u32 timeMs)
	{
		switch (_moveMode)
		{
		case irr::scene::CSceneNodeAnimatorCameraEditor::FORWARD: {
			auto delta = _targetPosition - _cameraPosition;
			_cameraPosition = _targetPosition;

			_targetPosition += delta;
		}
			break;
		case irr::scene::CSceneNodeAnimatorCameraEditor::BACKWARD: {
			auto delta = _targetPosition - _cameraPosition;
			_targetPosition = _cameraPosition;

			_cameraPosition -= delta;
		}
			break;
		default:
			break;
		}

		ICameraSceneNode* camera = static_cast<ICameraSceneNode*>(node);

		camera->setPosition(_cameraPosition);
		camera->setTarget(_targetPosition);
	}

	ISceneNodeAnimator* CSceneNodeAnimatorCameraEditor::createClone(
		ISceneNode* node, ISceneManager* newManager)
	{
		return new CSceneNodeAnimatorCameraEditor();
	}
}