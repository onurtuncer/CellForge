

namespace CellForge{

    class Entity;

    class IViewportBackend
{
public:
    virtual ~IViewportBackend() = default;

    virtual void Display(Entity entity) = 0;
    virtual void Hide(Entity entity) = 0;
    virtual void UpdateTransform(Entity entity) = 0;
    virtual std::optional<Entity> Pick(int x, int y) = 0;
    virtual void FitAll() = 0;
    virtual void Redraw() = 0;
};



}

