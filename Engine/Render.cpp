#include "Render.hpp"
#include "Time.hpp"

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
	LOG("Load Image \"%s\"", m_name.c_str());

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
Image* Sprite::GetImageByName(const String& _name)
{
	auto _it = m_aliases.find(_name);
	return _it != m_aliases.end() ? m_images[_it->second] : nullptr;
}
//----------------------------------------------------------------------------//
Sprite::Animation* Sprite::GetAnimation(const String& _name)
{
	auto _it = m_animations.find(_name);
	return _it != m_animations.end() ? &(_it->second) : nullptr;
}
//----------------------------------------------------------------------------//
void Sprite::Load(const char* _filename)
{
	LOG("Load Sprite \"%s\"", m_name.c_str());

	Json _desc;
	_desc.Load(gResources->MakePath(_filename, "json").c_str());
	
	Vector2 _pivot;
	{
		_pivot.x = _desc["Pivot"][0];
		_pivot.y = _desc["Pivot"][1];
	}

	// images
	{
		m_images.clear();

		const Json& _images = _desc["Images"];
		for (uint i = 0; i < _images.Size(); ++i)
		{
			ImagePtr _img = gResources->GetResource<Image>(_images[i]);
			_img->GetData().SetPivot(arctic::Vec2Si32((int)(_img->GetData().Width() * _pivot.x), (int)(_img->GetData().Height() * _pivot.y)));
			m_images.push_back(gResources->GetResource<Image>(_images[i]));
		}
	}

	// aliases
	{
		m_aliases.clear();
		const Json& _aliases = _desc["Aliases"];
		for (auto i = _aliases.Begin(); i != _aliases.End(); ++i)
		{
			if (i->second.AsInt() > (int)m_images.size())
			{
				LOG("Error: Wrong alias \"%s\"", i->first.c_str());
			}
			else
			{
				m_aliases[i->first] = i->second;
			}
		}
	}

	// animations
	{
		m_animations.clear();
		const Json& _anims = _desc["Animations"];
		for (auto i = _anims.Begin(); i != _anims.End(); ++i)
		{
			Animation _anim;
			_anim.start = i->second["Start"];
			_anim.end = i->second["End"];
			_anim.fps = i->second["FrameRate"];
			
			String _modeStr = i->second["Mode"];
			if (_modeStr == "Loop")
				_anim.mode = AnimMode::Loop;
			else if (_modeStr == "PingPong")
				_anim.mode = AnimMode::PingPong;
			else //if (_modeStr == "Default" || _modeStr == "Once")
				_anim.mode = AnimMode::Once;

			if(_anim.start > m_images.size() || _anim.end > m_images.size())
			{
				LOG("Error: Wrong animation \"%s\"", i->first.c_str());
			}
			else
			{
				m_animations[i->first] = _anim;
			}
		}
	}
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// SpriteRenderer
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void SpriteRenderer::SetSprite(Sprite* _sprite)
{
	m_sprite = _sprite;
	m_play = false;
	m_currentFrame = 0;
}
//----------------------------------------------------------------------------//
void SpriteRenderer::SetSprite(const String& _name)
{
	SetSprite(gResources->GetResource<Sprite>(_name));
}
//----------------------------------------------------------------------------//
void SpriteRenderer::Play(const String& _name, AnimMode _mode)
{
	Sprite::Animation* _anim = m_sprite ? m_sprite->GetAnimation(_name) : nullptr;
	if (_anim)
	{
		m_anim = *_anim;
		m_play = true;

		if (_mode != AnimMode::Default)
			m_anim.mode = _mode;
	}
}
//----------------------------------------------------------------------------//
void SpriteRenderer::Update(void)
{
	if (m_play)
	{
		m_animTime += gTime->Delta() * m_animSpeed;
		uint _count = Max(m_anim.start, m_anim.end) - Min(m_anim.start, m_anim.end);
		float _time = m_animTime * (m_anim.fps / _count);

		switch (m_anim.mode)
		{

		case AnimMode::PingPong:
		{
			_time -= floorf(_time);
			if ((int)(_time) & 0x1)
				_time = 1 - _time;

		} break;

		case AnimMode::Loop:
		{
			_time -= floorf(_time);

		} break;

		default:
		case AnimMode::Default:
		case AnimMode::Once:
		{
			if (_time >= 1)
			{
				m_play = false;
				_time = 1;
			}
		} break;
		}

		m_currentFrame = (int)Mix((float)m_anim.start, (float)m_anim.end, _time);
	}
}
//----------------------------------------------------------------------------//
void SpriteRenderer::Draw(const Vector2& _camera)
{
	if (!m_sprite)
		return;

	const Transform& _t = m_entity->GetTransform();
	Image* _img = m_sprite->GetImageByIndex(m_currentFrame);

	_img->GetData().Draw((int)(_t.e + _camera.x), int(_t.f + _camera.y), _t.Angle(), _t.Scale());

#if 1 // DEBUG DRAW
	arctic::Rgba _clr = arctic::Rgba(0xff, 0, 0, 0xff);
	Rect _r;
	_r.FromSizePivot({ (float)_img->GetData().Width(), (float)_img->GetData().Height() }, { (float)_img->GetData().Pivot().x, (float)_img->GetData().Pivot().y });

	Vector2 _lt = { _r.left, _r.top };
	Vector2 _lb = { _r.left, _r.bottom };
	Vector2 _rb = { _r.right, _r.bottom };
	Vector2 _rt = { _r.right, _r.top };

	_lt = _camera + _t * _lt;
	_lb = _camera + _t * _lb;
	_rb = _camera + _t * _rb;
	_rt = _camera + _t * _rt;

	arctic::easy::DrawLine(arctic::Vec2Si32(_lt.x, _lt.y), arctic::Vec2Si32(_lb.x, _lb.y), _clr);
	arctic::easy::DrawLine(arctic::Vec2Si32(_lb.x, _lb.y), arctic::Vec2Si32(_rb.x, _rb.y), _clr);
	arctic::easy::DrawLine(arctic::Vec2Si32(_rb.x, _rb.y), arctic::Vec2Si32(_rt.x, _rt.y), _clr);
	arctic::easy::DrawLine(arctic::Vec2Si32(_rt.x, _rt.y), arctic::Vec2Si32(_lt.x, _lt.y), _clr);
#endif
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
	Object::Register<Sprite>();

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
void RenderWorld::_Update(void)
{
	for (RenderComponent* i = m_first; i; i = i->m_nextRenderComponent)
	{
		if (i->IsEnabled())
		{
			i->Update();
			//TODO: check visibility
			//i->SetVisibility(i->CheckVisibility(m_view));
		}
	}
}
//----------------------------------------------------------------------------//
void RenderWorld::_PostUpdate(void)
{
}
//----------------------------------------------------------------------------//
void RenderWorld::_Render(void)
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
		i->Update();		

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
