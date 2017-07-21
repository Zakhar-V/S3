#include "Render.hpp"
#include "Time.hpp"

//----------------------------------------------------------------------------//
// AnimMode
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
String AnimModeToString(AnimMode _code)
{
	switch (_code)
	{
	case AnimMode::Once:
		return "Once";
	case AnimMode::PingPong:
		return "PingPong";
	case AnimMode::Loop:
		return "Loop";
	}
	return "Default";
}
//----------------------------------------------------------------------------//
AnimMode AnimModeFromString(const String& _name)
{
	AnimMode _mode = AnimMode::Default;
	if (_name == "Loop")
		_mode = AnimMode::Loop;
	else if (_name == "PingPong")
		_mode = AnimMode::PingPong;
	else if (_name == "Once")
		_mode = AnimMode::Once;

	return _mode;
}
//----------------------------------------------------------------------------//

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
	
	// _pivot
	m_pivot = _desc["Pivot"];

	// images
	{
		m_images.clear();

		const Json& _images = _desc["Images"];
		for (uint i = 0; i < _images.Size(); ++i)
		{
			ImagePtr _img = gResources->GetResource<Image>(_images[i]);
			_img->GetData().SetPivot(arctic::Vec2Si32((int)(_img->GetData().Width() * m_pivot.x), (int)(_img->GetData().Height() * m_pivot.y)));
			m_images.push_back(gResources->GetResource<Image>(_images[i]));
		}
	}

	// scale and size
	{
		ASSERT(m_images.size() > 0);
		int _targetHeight = _desc["Height"];
		if (!_targetHeight)
			_targetHeight = m_images[0]->GetData().Height();
		m_scale = (float)_targetHeight / (float)m_images[0]->GetData().Height();
		m_size.y = (float)_targetHeight;
		m_size.x = m_images[0]->GetData().Width() * m_scale;
		m_pivot *= m_size;
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
			_anim.name = i->first;
			_anim.start = i->second["Start"];
			_anim.end = i->second["End"];
			_anim.fps = i->second["FrameRate"];
			
			_anim.mode = AnimModeFromString(i->second["Mode"]);
			if (_anim.mode == AnimMode::Default)
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
void SpriteRenderer::SetAnimationSpeed(float _speed)
{
	m_animSpeed = _speed;
}
//----------------------------------------------------------------------------//
void SpriteRenderer::SetAnimationMode(AnimMode _mode)
{
	m_animMode = _mode;
}
//----------------------------------------------------------------------------//
void SpriteRenderer::Play(bool _play)
{
	m_play = _play;
}
//----------------------------------------------------------------------------//
void SpriteRenderer::Stop(bool _stop)
{
	m_animTime = 0;
	Play(_stop);
}
//----------------------------------------------------------------------------//
void SpriteRenderer::Update(void)
{
	if (m_play)
	{
		m_animTime += gTime->Delta() * m_animSpeed;
		uint _count = Max(m_anim.start, m_anim.end) - Min(m_anim.start, m_anim.end);
		if (_count == 0)
			_count = 1;
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
void SpriteRenderer::Draw(const Vector2& _camera, float _zoom)
{
	if (!m_sprite)
		return;

	const Transform& _t = m_entity->GetTransform();
	Image* _img = m_sprite->GetImageByIndex(m_currentFrame);

	gRenderer->DrawSprite(m_sprite, m_currentFrame, 0, m_entity->GetTransform(), m_color);
}
//----------------------------------------------------------------------------//
void SpriteRenderer::Clone(Component* _src)
{
	RenderComponent::Clone(_src);

	if (_src->IsTypeOf<SpriteRenderer>())
	{
		SpriteRenderer* _sp = static_cast<SpriteRenderer*>(_src);

		m_sprite = _sp->m_sprite;
		m_play = false;

		//TODO: animation state
	}
}
//----------------------------------------------------------------------------//
Json SpriteRenderer::Serialize(void)
{
	Json _dst = RenderComponent::Serialize();
	_dst["Sprite"] = m_sprite ? m_sprite->GetName() : nullptr;
	_dst["Animation"] = m_anim.name;
	_dst["Speed"] = m_animSpeed;
	_dst["Time"] = m_animTime;
	_dst["Play"] = m_play;

	return _dst;
}
//----------------------------------------------------------------------------//
void SpriteRenderer::Deserialize(const Json& _src, class ObjectSolver* _context)
{
	RenderComponent::Deserialize(_src, _context);

	// temp

	SetSprite(_src["Sprite"]);
	//SetAnimation

	SetAnimationSpeed(_src["Speed"]);
	m_animTime = _src["Time"];
	Play(_src["Animation"].AsString());
	Play(_src["Play"].AsBool());
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Camera
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
const Vector2& Camera::Position(void)
{
	return m_entity->GetTransform().Pos();
}
//----------------------------------------------------------------------------//
float Camera::Zoom(void)
{
	return m_entity->GetTransform().Scale();
}
//----------------------------------------------------------------------------//
void Camera::Clone(Component* _src)
{
	RenderComponent::Clone(_src);

	if (_src->IsTypeOf<Camera>())
	{
		Camera* _camera = static_cast<Camera*>(_src);

		//...
	}
}
//----------------------------------------------------------------------------//
Json Camera::Serialize(void)
{
	Json _dst = RenderComponent::Serialize();

	//...

	return _dst;
}
//----------------------------------------------------------------------------//
void Camera::Deserialize(const Json& _src, class ObjectSolver* _context)
{
	RenderComponent::Deserialize(_src, _context);
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
	Object::Register<Camera>();
	Object::Register<SpriteRenderer>();

	// system
	Object::Register<RenderWorld>();
	AddDefaultSystem(TypeName, RENDER_PRIORITY);
}
//----------------------------------------------------------------------------//
void RenderWorld::SetCamera(Camera* _camera)
{
	if (_camera && _camera->GetScene() == m_scene)
		m_camera = _camera;
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
		
		if (_component->IsTypeOf<Camera>() && !m_camera)
			m_camera = static_cast<Camera*>(_component);
	}
}
//----------------------------------------------------------------------------//
void RenderWorld::_RemoveComponent(Component* _component)
{
	if (_component->IsTypeOf<RenderComponent>())
	{
		RenderComponent* _child = static_cast<RenderComponent*>(_component);

		if (_component->IsTypeOf<Camera>() && _component == m_camera)
			m_camera = nullptr;

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
	//m_cameraPos = { 0, 0 };
	//m_cameraZoom = 1;
	if (m_camera && m_camera->IsEnabled())
	{
		m_cameraPos = m_camera->GetEntity()->GetTransform().Pos();
		m_cameraZoom = m_camera->GetEntity()->GetTransform().Scale();
	}

	gRenderer->SetCamera(m_cameraPos, m_cameraZoom);

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
		i->Draw(m_cameraPos);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
//Renderer
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
Renderer::Renderer(void)
{
	_GetBackBuffer();
}
//----------------------------------------------------------------------------//
Renderer::~Renderer(void)
{

}
//----------------------------------------------------------------------------//
bool Renderer::OnEvent(int _type, void* _arg)
{
	switch (_type)
	{
	case SystemEvent::BeginFrame:
		_GetBackBuffer();
		break;

	case SystemEvent::Render:
	{

		// clear screen
		{
			uint* i = (uint*)m_backBuffer;
			uint* _end = i + m_screenWidth * m_screenHeight;
			while (i < _end)
				*i++ = m_backgroundColor.rgba;
		}

	} break;

	}
	return false;
}
//----------------------------------------------------------------------------//
void Renderer::SetScreenSize(uint _width, uint _height)
{
	arctic::easy::ResizeScreen(_width, _height);
	_GetBackBuffer();
}
//----------------------------------------------------------------------------//
void Renderer::SetCamera(const Vector2& _pos, float _zoom)
{	
	m_cameraPos = _pos;
	m_cameraZoom = _zoom;
}
//----------------------------------------------------------------------------//
void Renderer::DrawSprite(Sprite* _sprite, uint _frame, uint _targetHeight, const Transform& _transform, const Color4ub& _color)
{

	Image* _img = _sprite->GetImageByIndex(_frame);

	int _srcWidth = _img->GetData().Width();
	int _srcHeight = _img->GetData().Height();
	//_img->GetData().SetPivot(arctic::Vec2Si32((int)_pivot.x * _srcWidth, (int)_pivot.y * _srcHeight));

	float _imgScale = 1;
	if (_targetHeight)
	{
		_imgScale =  (float)_targetHeight / (float)_srcHeight;
	}
	else
	{
		_imgScale = _sprite->GetScale();
	}
	float _scale = _imgScale * _transform.Scale();

	float _angle = _transform.Angle();
	if (fabsf(_angle) < .1e-3f)
	{
		int _width = (int)(_srcWidth * _scale);
		int _height = (int)(_srcHeight * _scale);

		_img->GetData().Draw(
			(int)(_transform.e * m_cameraZoom - m_cameraPos.x), (int)(_transform.f * m_cameraZoom - m_cameraPos.y), (int)(_width * m_cameraZoom), (int)(_height * m_cameraZoom), // dst
			0, 0, _srcWidth, _srcHeight); //src
	}
	else
	{
		_img->GetData().Draw((int)(_transform.e * m_cameraZoom - m_cameraPos.x), (int)(_transform.f * m_cameraZoom - m_cameraPos.y), -_angle, _scale * m_cameraZoom);
	}

	if (m_debugDraw & DebugDraw::Bound)
	{
		arctic::Rgba _clr = arctic::Rgba(0xff, 0, 0, 0xff);
		Rect _r;
		_r.FromSizePivot(_sprite->Size(), _img->GetData().Pivot());

		Vector2 _lt = { _r.left, _r.top };
		Vector2 _lb = { _r.left, _r.bottom };
		Vector2 _rb = { _r.right, _r.bottom };
		Vector2 _rt = { _r.right, _r.top };

		_lt = _transform * _lt * m_cameraZoom - m_cameraPos;
		_lb = _transform * _lb * m_cameraZoom - m_cameraPos;
		_rb = _transform * _rb * m_cameraZoom - m_cameraPos;
		_rt = _transform * _rt * m_cameraZoom - m_cameraPos;

		arctic::easy::DrawLine(_lt, _lb, _clr);
		arctic::easy::DrawLine(_lb, _rb, _clr);
		arctic::easy::DrawLine(_rb, _rt, _clr);
		arctic::easy::DrawLine(_rt, _lt, _clr);

		DrawCross(_transform.Pos(), 10, _clr);
	}
}
//----------------------------------------------------------------------------//
void Renderer::DrawLine(const Vector2& _start, const Vector2& _end, const Color4ub& _color)
{
	arctic::easy::DrawLine(_start * m_cameraZoom - m_cameraPos, _end * m_cameraZoom - m_cameraPos, _color);
}
//----------------------------------------------------------------------------//
void Renderer::DrawWireCircle(const Vector2& _center, float _radius, const Color4ub& _color)
{
	const uint _segments = 32;

	float _invS = 1.0f / _segments;
	float _sAngle = 2 * PI * _invS;

	Vector2 _current, _prev = Vector2(0, 1) * _radius + _center;
	for (int i = 0; i <= _segments; i++)
	{
		float _a = i * _sAngle;
		_current.x = sinf(_a) * _radius + _center.x;
		_current.y = cosf(_a) * _radius + _center.y;
		DrawLine(_prev, _current, _color);
		_prev = _current;
	}
}
//----------------------------------------------------------------------------//
void Renderer::DrawCross(const Vector2& _center, float _size, const Color4ub& _color)
{
	_size *= .5f;
	DrawLine(_center - Vector2(_size, 0), _center + Vector2(_size, 0), _color);
	DrawLine(_center - Vector2(0, _size), _center + Vector2(0, _size), _color);
}
//----------------------------------------------------------------------------//
void Renderer::_GetBackBuffer(void)
{
	auto _bb = arctic::easy::GetEngine()->GetBackbuffer();
	m_backBuffer = reinterpret_cast<Color4ub*>(_bb.RawData());
	m_screenWidth = _bb.Width();
	m_screenHeight = _bb.Height();
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
