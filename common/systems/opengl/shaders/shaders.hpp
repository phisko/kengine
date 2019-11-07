#pragma once

namespace kengine::Shaders::src {
	namespace ApplyTransparency {
		extern const char * frag;
	}

	namespace ProjViewModel {
		extern const char * vert;
	}
	namespace Quad {
		extern const char * vert;
	}

	namespace CSM {
		extern const char * frag;
	}
	namespace ShadowMap {
		extern const char * frag;
	}
	namespace ShadowCube {
		extern const char * frag;
	}

	namespace DepthCube {
		extern const char * geom;
		extern const char * frag;
	}

	namespace DirLight {
		extern const char * frag;
		namespace GetDirection {
			extern const char * frag;
		}
	}
	namespace PointLight {
		extern const char * frag;
		namespace GetDirection {
			extern const char * frag;
		}
	}
	namespace SpotLight {
		extern const char * frag;
	}

	namespace Color {
		extern const char * frag;
	}

	namespace GodRays {
		extern const char * frag;
	}
}
