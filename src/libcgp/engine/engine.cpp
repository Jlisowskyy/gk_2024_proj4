#include <libcgp/engine/engine.hpp>

#include <libcgp/mgr/settings_mgr.hpp>
#include <libcgp/utils/macros.hpp>
#include <libcgp/window/window.hpp>

LibGcp::EngineBase::~EngineBase() { TRACE("EngineBase::~EngineBase()"); }

void LibGcp::EngineBase::Init(const Scene &scene) noexcept
{
    TRACE("EngineBase::Init()");

    /* connect with settings */
    SettingsMgr::GetInstance().AddListener(Setting::kCameraType, OnCameraTypeChanged_);
    SettingsMgr::GetInstance().AddListener(Setting::kCurrentWordTime, OnWordTimeChanged_);

    /* Adjust camera type based on default settings - intentional cast */
    OnCameraTypeChanged_(SettingsMgr::GetInstance().GetSetting<uint64_t>(Setting::kCameraType));

    /* Add reaction on perspective change */
    SettingsMgr::GetInstance().AddListener(Setting::kFov, OnPerspectiveChanged_);
    SettingsMgr::GetInstance().AddListener(Setting::kNear, OnPerspectiveChanged_);
    SettingsMgr::GetInstance().AddListener(Setting::kFar, OnPerspectiveChanged_);

    /* init flowing TEMP object */
    flowing_camera_.position = glm::vec3{};
    flowing_camera_.front    = glm::vec3(0.0f, 0.0f, -1.0f);
    flowing_camera_.up       = glm::vec3(0.0f, 1.0f, 0.0f);

    /* load initial scene */
    ReloadScene(scene);

    /* TODO: load global lights from scene */
    global_light_.LoadLights({
        // sun
        GlobalLightSpec{
                        .light_info =
                        {
                        .ambient  = {0.5F, 0.5F, 0.5F},
                        .diffuse  = {0.8F, 0.8F, 0.8F},
                        .specular = {1.0F, 1.0F, 1.0F},
                        }, .is_moving = true,
                        .rise_time = WordTime::ConvertToSeconds(6, 0, 0),
                        .down_time = WordTime::ConvertToSeconds(22, 30, 0),
                        .angle     = 0.25,
                        },
        // Moon
        GlobalLightSpec{
                        .light_info =
                        {
                        .ambient  = {0.2F, 0.2F, 0.2F},
                        .diffuse  = {0.4F, 0.4F, 0.4F},
                        .specular = {0.05F, 0.05F, 0.05F},
                        }, .is_moving = true,
                        .rise_time = WordTime::ConvertToSeconds(21, 0, 0),
                        .down_time = WordTime::ConvertToSeconds(7,  0, 0),
                        .angle     = 0.6,
                        }
    });

    /* initialize g-buffer */
    g_buffer_.PrepareBuffers();

    /* load shaders */
    geometry_pass_shader_ = ResourceMgr::GetInstance().GetShader({
        .paths           = {"g_buffer", "g_buffer"},
        .type            = ResourceType::kShader,
        .load_type       = LoadType::kMemory,
        .is_serializable = false,
    });

    lighting_pass_shader_ = ResourceMgr::GetInstance().GetShader({
        .paths           = {"deferred_shading", "deferred_shading"},
        .type            = ResourceType::kShader,
        .load_type       = LoadType::kMemory,
        .is_serializable = false,
    });
}

void LibGcp::EngineBase::Draw()
{
    /* geometry pass */
    geometry_pass_shader_->Activate();
    g_buffer_.BindForWriting();
    Engine::GetInstance().GetView().PrepareViewMatrices(*geometry_pass_shader_);
    ObjectMgr::GetInstance().DrawStaticObjects(*geometry_pass_shader_, RenderPass::kGeometry);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* lighting pass */
    lighting_pass_shader_->Activate();
    g_buffer_.BindForReading(*lighting_pass_shader_);

    lighting_pass_shader_->SetVec3Unsafe("un_view_pos", view_.GetBindObject().position);
    light_mgr_.PrepareLights(*lighting_pass_shader_);
    global_light_.PrepareLights(*lighting_pass_shader_);

    ObjectMgr::GetInstance().DrawStaticObjects(*lighting_pass_shader_, RenderPass::kLighting);
}

void LibGcp::EngineBase::ProcessProgress(const uint64_t delta)
{
    ProcessInput_(delta);

    if (SettingsMgr::GetInstance().GetSetting<Setting::kClockTicking, bool>()) {
        word_time_.UpdateTime(delta);
        ProcessDynamicObjects_(delta);
    }

    if (SettingsMgr::GetInstance().GetSetting<Setting::kCameraType, CameraType>() == CameraType::kFree) {
        ProcessFreeCameraMovement_(delta);
    }

    view_.UpdateCameraPosition();

    /* Reset keys input for next frame */
    keys_.fill(0);
}

void LibGcp::EngineBase::ReloadScene(const Scene &scene)
{
    /* remove all objects */
    ObjectMgr::GetInstance().GetStaticObjects().Clear();

    /* clean all resources */
    ResourceMgr::GetInstance().GetModels().Clear();
    ResourceMgr::GetInstance().GetShaders().Clear();
    ResourceMgr::GetInstance().GetTextures().Clear();

    /* load resources */
    ResourceMgr::GetInstance().LoadResourceFromScene(scene);

    /* load objects */
    ObjectMgr::GetInstance().LoadObjectsFromScene(scene);

    /* load lights */
    light_mgr_.LoadLightsFromScene(scene);

    /* Note that settings must be loaded after all objects as there may be some events to fire */
    /* ensure default are loaded */
    SettingsMgr::GetInstance().LoadDefaultSettings();

    /* load settings */
    for (const auto &[setting, obj] : scene.settings) {
        SettingsMgr::GetInstance().SetSetting<uint64_t>(setting, obj.GetRaw());
    }
}

void LibGcp::EngineBase::ProcessInput_(const uint64_t delta)
{
    /* first process camera change */
    if (keys_[GLFW_KEY_F1]) {
        SettingsMgr::GetInstance().SetSetting<Setting::kCameraType>(CameraType::kStatic);
    } else if (keys_[GLFW_KEY_F2]) {
        SettingsMgr::GetInstance().SetSetting<Setting::kCameraType>(CameraType::kFollow);
    } else if (keys_[GLFW_KEY_F3]) {
        SettingsMgr::GetInstance().SetSetting<Setting::kCameraType>(CameraType::kFree);
    } else if (keys_[GLFW_KEY_F4]) {
        SettingsMgr::GetInstance().SetSetting<Setting::kCameraType>(CameraType::kFirstPerson);
    } else if (keys_[GLFW_KEY_F5]) {
        SettingsMgr::GetInstance().SetSetting<Setting::kCameraType>(CameraType::kThirdPerson);
    }

    /* process overlay changes */
    if (keys_[GLFW_KEY_F12]) {
        Window::GetInstance().SwitchDebugOverlay();
    }

    /* process movement */
    const long is_clock_enabled = SettingsMgr::GetInstance().GetSetting<Setting::kClockTicking, bool>();
    ProcessUserMovement_(is_clock_enabled * delta);
}

void LibGcp::EngineBase::ProcessUserMovement_(UNUSED const uint64_t delta) {}

void LibGcp::EngineBase::ProcessFreeCameraMovement_(const uint64_t delta)
{
    const double free_cam_speed = SettingsMgr::GetInstance().GetSetting<Setting::kFreeCameraSpeed, double>();
    const double distance       = free_cam_speed * static_cast<double>(delta) / 1e+6;

    free_camera_.MoveFreeCamera(static_cast<float>(distance), keys_);
}

void LibGcp::EngineBase::ProcessDynamicObjects_(const uint64_t delta)
{
    /* TODO: temporary, bind object movement to camera movement later */
    const double radius = 30.0f;
    flow_count_ += static_cast<double>(delta) / 1e+6;
    const float camX = sin(flow_count_) * radius;
    const float camZ = cos(flow_count_) * radius;

    flowing_camera_.position = glm::vec3(camX, 0.0f, camZ);
    flowing_camera_.front    = -flowing_camera_.position;
}

void LibGcp::EngineBase::OnCameraTypeChanged_(const uint64_t new_value)
{
    TRACE("Camera type changed to " << new_value);

    /* TODO: Bind proper object here */
    switch (static_cast<CameraType>(new_value)) {
        case CameraType::kStatic: {
            Window::GetInstance().GetMouse().BindCamera(nullptr);
            Engine::GetInstance().view_.BindCameraWithObjet(&Engine::GetInstance().static_camera_);
        } break;
        case CameraType::kFollow: {
            Window::GetInstance().GetMouse().BindCamera(nullptr);
            Engine::GetInstance().view_.BindCameraWithObjet(&Engine::GetInstance().flowing_camera_);
        } break;
        case CameraType::kFree: {
            /* preserve old camera */
            Engine::GetInstance().free_camera_ = Engine::GetInstance().view_.GetBindObject();

            /* ensure yaw pitch is set and ready */
            Engine::GetInstance().free_camera_.ConvertVectorToYawPitch();

            /* bind camera to mouse */
            Window::GetInstance().GetMouse().BindCamera(&Engine::GetInstance().free_camera_);

            /* bind camera to view */
            Engine::GetInstance().view_.BindCameraWithObjet(&Engine::GetInstance().free_camera_);
        } break;
        case CameraType::kFirstPerson:
            NOT_IMPLEMENTED;
            break;
        case CameraType::kThirdPerson:
            NOT_IMPLEMENTED;
            break;
        default:
            R_ASSERT(false && "Unknown camera type")
    }
}

void LibGcp::EngineBase::OnWordTimeChanged_(const uint64_t new_value)
{
    Engine::GetInstance().global_light_.UpdatePosition(new_value);
}

void LibGcp::EngineBase::OnPerspectiveChanged_([[maybe_unused]] uint64_t new_value)
{
    Engine::GetInstance().view_.SyncProjectionMatrixWithSettings();
}
