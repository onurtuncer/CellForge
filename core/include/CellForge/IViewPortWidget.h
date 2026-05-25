

namespace CellForge
{
    class Event;

    class IViewportWidget
    {
    public:
        virtual ~IViewportWidget() = default;

        virtual void Show() = 0;
        virtual void Hide() = 0;
        virtual void Resize(int width, int height) = 0;
        virtual void Redraw() = 0;

        virtual void* NativeHandle() = 0;

        virtual void SetEventCallback(
            std::function<void(Event&)> callback) = 0;
    };
}