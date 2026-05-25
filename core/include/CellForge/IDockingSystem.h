

namespace CellForge{
    
        class IDockingSystem
    {
    public:
        virtual ~IDockingSystem() = default;

        virtual void AddDockPanel(
            const std::string& name,
            void* nativeWidget,
            const std::string& area) = 0;

        virtual void RemoveDockPanel(const std::string& name) = 0;

        virtual void ShowDockPanel(const std::string& name) = 0;
        virtual void HideDockPanel(const std::string& name) = 0;
    };
}