#pragma once
#include "GameObject.h"

namespace DE
{
	//TODO: move 
	class Camera : public GameObject
	{
	public:
		explicit Camera(float nearPlaneDistance = 0.01f, float farPlaneDistance = 1000.0f, float fieldOfView = DirectX::XM_PIDIV4);

		DirectX::XMFLOAT3	GetPosition() const noexcept { return m_position; }
		//DirectX::XMVECTOR	GetPositionAsSimd() const;
		DirectX::XMFLOAT3	GetDirection() const noexcept { return m_direction; }
		//DirectX::XMVECTOR	GetDirectionAsSimd() const;
		DirectX::XMFLOAT3	GetUpVector() const noexcept { return m_upVector; }
		//DirectX::XMVECTOR	GetUpVectorAsSimd() const;
		float				GetNearPlaneDistance() const noexcept { return m_nearPlaneDistance; }
		void				SetNearPlaneDistance(float newDistance);
		float				GetFarPlaneDistnace() const noexcept { return m_farPlaneDistance; };
		void				SetFarPlaneDistance(float newDistance);
		float				GetFieldOfView() const noexcept { return m_fieldOfView; }
		void				SetFieldOfView(float newField);
		float				GetAspectRatio() const noexcept { return m_aspectRatio; }
		void				SetAspectRatio(float newRatio);
		void				SetAspectRatio(int numerator, int denominator) { SetAspectRatio(float(numerator) / float(denominator)); }
		
	private:
		DirectX::XMFLOAT4X4	m_viewMatrix{};
		DirectX::XMFLOAT4X4 m_projectionMatrix{};
		DirectX::XMFLOAT3	m_position{};
		DirectX::XMFLOAT3	m_direction{};
		DirectX::XMFLOAT3	m_upVector{};
		float				m_nearPlaneDistance{};
		float				m_farPlaneDistance{};
		float				m_fieldOfView{};
		float				m_aspectRatio{};
	};
}
