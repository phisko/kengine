#pragma once

#include "Point.hpp"

namespace putils { class BaseModule; }

namespace kengine {
    namespace packets {
        namespace ScreenSize {
            struct ScreenSizeQuery {
                putils::BaseModule * sender;
            };

			struct TileSizeQuery {
                putils::BaseModule * sender;
			};

			struct GridSizeQuery {
                putils::BaseModule * sender;
			};

            struct Response {
				putils::Point2d size;
            };
        }
    }
}