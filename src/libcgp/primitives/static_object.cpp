#include <libcgp/primitives/static_object.hpp>

std::atomic<uint64_t> LibGcp::StaticObject::id_counter_{0};

void LibGcp::StaticObject::ProcessDelta(const uint64_t delta)
{
    const float delta_f = static_cast<float>(delta) / 1e+6;
    move_pos_           = std::fmodf(move_pos_ + delta_f * speed_, 2.0F * M_PI);
    TRACE("move_pos_: " << move_pos_);

    /* based on move_pos calculate x and z */
    position_.position.x = radius_ * cos(move_pos_);
    position_.position.z = radius_ * sin(move_pos_);

    /* Compute angle to always face the center */
    position_.rotation.y = atan2(-position_.position.x, -position_.position.z);
}
