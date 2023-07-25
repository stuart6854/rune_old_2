#include <rune/rune.hpp>

using namespace rune;

class ApplicationSandbox : public IApplication
{
public:
    void initialise(Engine& /*engine*/) override
    {
        m_primaryWindow = platform::create_window("Sandbox", 800, 600);

        // Compile Shader
        rhi::ShaderCompileDecl shaderCompileDecl{
            .SourceLanguage = rhi::ShaderSourceLanguage::GLSL,
            .SourceFilename = "shader.vert",
            .Stage = rhi::ShaderStage::Vertex,
            .EntryPointName = "main",
            .OutputRuntimeLanguage = rhi::ShaderRuntimeLanguage::SPIRV,
            .GenerateDebugInfo = true,
        };
        auto vertexByteCode = rhi::compile_shader(shaderCompileDecl);

        shaderCompileDecl.SourceFilename = "shader.frag";
        shaderCompileDecl.Source = "";
        shaderCompileDecl.Stage = rhi::ShaderStage::Pixel;
        auto fragmentByteCode = rhi::compile_shader(shaderCompileDecl);

        m_renderDevice = rhi::Device::create();

        rhi::SurfaceDecl surfaceDecl{
            .DisplayHandle = m_primaryWindow->native_display_handle(),
            .SurfaceHandle = m_primaryWindow->native_surface_handle(),
            .Size = m_primaryWindow->fb_size(),
        };
        m_renderSurface = rhi::Surface::create(m_renderDevice, surfaceDecl);
        rhi::RenderTargetDecl rtDecl{
            .Surface = m_renderSurface.get(),
            .ClearColorEnable = true,
            .ClearColor = { 0.8f, 0.1f, 0.1f, 1.0f },
        };
        m_renderTarget = rhi::RenderTarget::create(m_renderDevice, rtDecl);

        m_cmdList = rhi::CommandList::create(m_renderDevice);
        m_fence = rhi::Fence::create(m_renderDevice);

        rhi::ShaderProgramDecl spDecl{};
        spDecl.ShaderStages[u8(rhi::ShaderType::Vertex)] = { .ByteCode = vertexByteCode };
        spDecl.ShaderStages[u8(rhi::ShaderType::Pixel)] = { .ByteCode = fragmentByteCode };
        m_shaderProgram = rhi::ShaderProgram::create(m_renderDevice, spDecl);

        rhi::PipelineStateDecl psDecl{
            .ShaderProgram = m_shaderProgram,
            .Topology = rhi::PrimitiveTopology::TriangleList,
            .DepthStencil = {
                .DepthTestEnable = false,
                .DepthWriteEnable = false,
            },
            .Rasterizer = {},
        };
        m_pipelineState = rhi::PipelineState::create(m_renderDevice, psDecl);

        struct Vertex
        {
            glm::vec3 pos;
            glm::vec3 color;
        };
        std::vector<Vertex> vertices{
            { { 0, 0, 0 }, { 1, 0, 0 } },
            { { 0, 0, 0 }, { 0, 1, 0 } },
            { { 0, 0, 0 }, { 0, 0, 1 } },
        };
        m_vertexBuffer = rhi::Buffer::create_vertex(m_renderDevice, sizeof(Vertex) * vertices.size());

        RUNE_CLIENT_INFO("Sandbox initialised.");
    }

    void shutdown(Engine& /*engine*/) override
    {
        m_cmdList = nullptr;
        m_renderDevice = nullptr;

        platform::destroy_window(m_primaryWindow);
        m_primaryWindow = nullptr;

        RUNE_CLIENT_INFO("Sandbox shutdown.");
    }

    void update(Engine& engine) override
    {
        m_renderSurface->acquire_next_image();

        m_cmdList->reset();
        m_cmdList->begin();
        m_cmdList->transition_state(m_renderSurface->current_image(), rhi::ResourceState::Undefined, rhi::ResourceState::RenderTarget);
        m_cmdList->begin_render_pass(m_renderTarget.get());
        m_cmdList->bind_pipeline_state(m_pipelineState.get());
        m_cmdList->set_viewport(0, 0, m_primaryWindow->fb_size().x, m_primaryWindow->fb_size().y);
        m_cmdList->set_scissor(0, 0, m_primaryWindow->fb_size().x, m_primaryWindow->fb_size().y);
        m_cmdList->bind_vertex_buffers(0, { m_vertexBuffer.get() }, { 0 });
        m_cmdList->draw(3, 1, 0, 0);
        m_cmdList->end_renderr_pass();
        m_cmdList->transition_state(m_renderSurface->current_image(), rhi::ResourceState::RenderTarget, rhi::ResourceState::Present);
        m_cmdList->end();

        m_renderDevice->submit({ m_cmdList.get() }, m_fence.get(), ++m_fenceValue);
        m_fence->wait(m_fenceValue);

        m_renderSurface->present();

        if (m_primaryWindow->close_requested())
            engine.request_shutdown();
    }

private:
    platform::WindowPtr m_primaryWindow{};
    Shared<rhi::Device> m_renderDevice{};
    Owned<rhi::Surface> m_renderSurface{};
    Owned<rhi::RenderTarget> m_renderTarget{};
    Owned<rhi::CommandList> m_cmdList{};
    Owned<rhi::Fence> m_fence{};
    u64 m_fenceValue{};
    Shared<rhi::ShaderProgram> m_shaderProgram{};
    Owned<rhi::PipelineState> m_pipelineState{};
    Owned<rhi::Buffer> m_vertexBuffer{};
};

int main()
{
    EngineConfig config{};
    config.logOutputDir = "./logs";

    Engine engine{};
    ApplicationSandbox app{};

    bool success = engine.run(&app, config);
    if (!success)
    {
        return 1;
    }

    return 0;
}