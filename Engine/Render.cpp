#include "Render.hpp"

//----------------------------------------------------------------------------//
// Image
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
Image::Image(void)
{
	m_data.SetPivot(arctic::Vec2Si32(0, 0));
}
//----------------------------------------------------------------------------//
void Image::Load(const char* _filename)
{
	m_data.Load(gResources->MakePath(_filename, "tga"));
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Sprite
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
Sprite::Sprite(void)
{
}
//----------------------------------------------------------------------------//
Sprite::~Sprite(void)
{
}
//----------------------------------------------------------------------------//
void Sprite::SetFramesCount(uint _count)
{
	m_frames.resize(_count);
}
//----------------------------------------------------------------------------//
void Sprite::Load(const char* _filename)
{
	Json _desc;
	_desc.Load(gResources->MakePath(_filename, "json").c_str());
	
	// images
	{
		m_images.clear();

		const Json& _images = _desc["Images"];
		for (uint i = 0; i < _images.Size(); ++i)
			m_images.push_back(gResources->GetResource<Image>(_images[i]));
	}

	// frames
	{
		//String type = _desc[]
	}

}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// SpriteRenderer
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void SpriteRenderer::Draw(const Vector2& _camera)
{
	const Transform& _t = m_entity->GetTransform();
	m_data.SetPivot(arctic::Vec2Si32(0, 0)); // temp

	m_data.Draw((int)(_t.e + _camera.x), int(_t.f + _camera.y), _t.Angle(), _t.Scale());
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// RenderWorld
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void RenderWorld::Register(void)
{
	// resources
	Object::Register<Image>();

	// componenets
	//Object::Register<RenderComponent>();
	Object::Register<SpriteRenderer>();

	// system
	Object::Register<RenderWorld>();
	AddDefaultSystem(TypeName, RENDER_PRIORITY);
}
//----------------------------------------------------------------------------//
void RenderWorld::_Start(Scene* _scene)
{
}
//----------------------------------------------------------------------------//
void RenderWorld::_Stop(void)
{
}
//----------------------------------------------------------------------------//
void RenderWorld::_AddComponent(Component* _component)
{

	if (_component->IsTypeOf<RenderComponent>())
	{
		RenderComponent* _child = static_cast<RenderComponent*>(_component);

		_child->AddRef();
		LL_LINK(m_first, _child, m_prevRenderComponent, m_nextRenderComponent);
		//_child->Start();
	}
}
//----------------------------------------------------------------------------//
void RenderWorld::_RemoveComponent(Component* _component)
{
	if (_component->IsTypeOf<RenderComponent>())
	{
		RenderComponent* _child = static_cast<RenderComponent*>(_component);

		//_child->Destroy();
		LL_UNLINK(m_first, _child, m_prevRenderComponent, m_nextRenderComponent);
		_child->Release();
	}
}
//----------------------------------------------------------------------------//
void RenderWorld::_ProcessFrame(void)
{
	Vector2 _camera = { 0, 0 };

	// clear screen
	{
		auto _bb = arctic::easy::GetEngine()->GetBackbuffer();
		memset(_bb.RawData(), 0, _bb.Width() * _bb.Height() * 4);
	}

	m_visibleSet.clear();
	for (RenderComponent* i = m_first; i; i = i->m_nextRenderComponent)
	{
		//TODO: check visibility

		if (i->IsEnabled()) // && i->IsVisible())
			m_visibleSet.push_back(i);
	}

	std::sort(m_visibleSet.begin(), m_visibleSet.end(),
		[](RenderComponent* _lhs, RenderComponent* _rhs) 
	{
		// TODO: use priority and layers

		int _lhsDepth = _lhs->GetEntity()->GetDepth();
		int _rhsDepth = _rhs->GetEntity()->GetDepth();
		return _lhsDepth < _rhsDepth;
	});

	for (auto i : m_visibleSet)
		i->Draw(_camera);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
