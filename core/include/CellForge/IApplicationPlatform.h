

namespace CellForge{

    class IApplicationPlatform
    {
    public:
        virtual ~IApplicationPlatform() = default;

        virtual void Initialize(int argc, char** argv) = 0;
        virtual int Run() = 0;
        virtual void Shutdown() = 0;

        virtual std::shared_ptr<IWindow> CreateMainWindow(
            const std::string& title,
            int width,
            int height) = 0;

        virtual std::shared_ptr<IViewportWidget> CreateViewportWidget() = 0;

        virtual std::shared_ptr<IMenuBar> CreateMenuBar(
            std::shared_ptr<IWindow> window) = 0;

        virtual std::shared_ptr<IDockingSystem> CreateDockingSystem(
            std::shared_ptr<IWindow> window) = 0;
    };
} 



}

