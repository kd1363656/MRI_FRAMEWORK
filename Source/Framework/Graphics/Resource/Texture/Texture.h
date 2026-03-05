#pragma once

namespace FWK::Graphics
{
	class Texture final
	{
	public:

		explicit Texture(const TextureID a_id);
		Texture         ();
		~Texture        ();
		
		TextureID GetID() const { return m_id; }

		bool IsInvalid() const;

	private:

		TextureID m_id;
	};
}