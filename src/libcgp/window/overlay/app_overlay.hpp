#ifndef WINDOW_OVERLAY_APP_OVERLAY_HPP_
#define WINDOW_OVERLAY_APP_OVERLAY_HPP_

#include <libcgp/defines.hpp>

LIBGCP_DECL_START_
class AppOverlay
{
    public:
    // ------------------------------
    // Object creation
    // ------------------------------

    // ------------------------------
    // Class interaction
    // ------------------------------

    void Draw() {}

    NDSCRD FAST_CALL bool IsOwningMouse() const { return false; }

    // ------------------------------
    // Implementation methods
    // ------------------------------

    protected:
    // ------------------------------
    // Class fields
    // ------------------------------
};

LIBGCP_DECL_END_

#endif  // WINDOW_OVERLAY_APP_OVERLAY_HPP_
